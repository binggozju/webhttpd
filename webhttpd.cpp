/*
 * a easy http server based on libevent.
 * 
 * author:	binggozju
 * date:	2015-12-29
 */

#include "webhttpd.h"

#include "options_parser.h"
#include "config.h"
#include "logging.h"

struct event_base *base;
struct evhttp *http;
struct evhttp_bound_socket *handle;
//string uri_root[512]; 

vector<int> child_fds; // 存储master进程用来与各worker进程通信的unix域套接字
int child_index; // master进程将向child_fds[child_index]写入新的连接套接字，传递给对应的worker子进程 
int parent_fd; // 在worker子进程中存储的是用来与master进程通信的unix域套接字，在master进程中为-1

OptionsParser* opt_parser;
ConfigParser conf_parser;
Logging logger;

char uri_root[512];

const struct table_entry {
	const char *extension;
	const char *content_type;
} content_type_table[] = {
	{ "txt", "text/plain" },
	{ "c", "text/plain" },
	{ "h", "text/plain" },
	{ "html", "text/html" },
	{ "htm", "text/htm" },
	{ "css", "text/css" },
	{ "gif", "image/gif" },
	{ "jpg", "image/jpeg" },
	{ "jpeg", "image/jpeg" },
	{ "png", "image/png" },
	{ "pdf", "application/pdf" },
	{ "ps", "application/postsript" },
	{ NULL, NULL },
};

/* Try to guess a good content-type for 'path' */
const char* guess_content_type(const char *path)
{
	const char *last_period, *extension;
	const struct table_entry *ent;
	last_period = strrchr(path, '.');
	if (!last_period || strchr(last_period, '/'))
		goto not_found; /* no exension */
	extension = last_period + 1;
	for (ent = &content_type_table[0]; ent->extension; ++ent) {
		if (!evutil_ascii_strcasecmp(ent->extension, extension))
			return ent->content_type;
	}

not_found:
	return "application/misc";
}

/* Callback used for the /dump URI, and for every non-GET request:
 * dumps all information to stdout and gives back a trivial 200 ok */
void dump_request_cb(struct evhttp_request *req, void *arg)
{
	const char *cmdtype;
	struct evkeyvalq *headers;
	struct evkeyval *header;
	struct evbuffer *buf;

	switch (evhttp_request_get_command(req)) {
	case EVHTTP_REQ_GET: cmdtype = "GET"; break;
	case EVHTTP_REQ_POST: cmdtype = "POST"; break;
	case EVHTTP_REQ_HEAD: cmdtype = "HEAD"; break;
	case EVHTTP_REQ_PUT: cmdtype = "PUT"; break;
	case EVHTTP_REQ_DELETE: cmdtype = "DELETE"; break;
	case EVHTTP_REQ_OPTIONS: cmdtype = "OPTIONS"; break;
	case EVHTTP_REQ_TRACE: cmdtype = "TRACE"; break;
	case EVHTTP_REQ_CONNECT: cmdtype = "CONNECT"; break;
	case EVHTTP_REQ_PATCH: cmdtype = "PATCH"; break;
	default: cmdtype = "unknown"; break;
	}

	printf("Received a %s request for %s\nHeaders:\n",
	    cmdtype, evhttp_request_get_uri(req));

	headers = evhttp_request_get_input_headers(req);
	for (header = headers->tqh_first; header;
	    header = header->next.tqe_next) {
		printf("  %s: %s\n", header->key, header->value);
	}

	buf = evhttp_request_get_input_buffer(req);
	puts("Input data: <<<");
	while (evbuffer_get_length(buf)) {
		int n;
		char cbuf[128];
		n = evbuffer_remove(buf, cbuf, sizeof(cbuf));
		if (n > 0)
			(void) fwrite(cbuf, 1, n, stdout);
	}
	puts(">>>");

	evhttp_send_reply(req, 200, "OK", NULL);
}

/* This callback gets invoked when we get any http request that doesn't match
 * any other callback.  Like any evhttp server callback, it has a simple job:
 * it must eventually call evhttp_send_error() or evhttp_send_reply().
 */
void send_document_cb(struct evhttp_request *req, void *arg)
{
	struct evbuffer *evb = NULL;
	const char *docroot = (const char*)arg;
	const char *uri = evhttp_request_get_uri(req);
	struct evhttp_uri *decoded = NULL;
	const char *path;
	char *decoded_path;
	char *whole_path = NULL;
	size_t len;
	int fd = -1;
	struct stat st;

	if (evhttp_request_get_command(req) != EVHTTP_REQ_GET) {
		dump_request_cb(req, arg);
		return;
	}

	printf("Got a GET request for <%s>\n",  uri);

	/* Decode the URI */
	decoded = evhttp_uri_parse(uri);
	if (!decoded) {
		printf("It's not a good URI. Sending BADREQUEST\n");
		evhttp_send_error(req, HTTP_BADREQUEST, 0);
		return;
	}

	/* Let's see what path the user asked for. */
	path = evhttp_uri_get_path(decoded);
	if (!path) path = "/";

	/* We need to decode it, to see what path the user really wanted. */
	decoded_path = evhttp_uridecode(path, 0, NULL);
	if (decoded_path == NULL)
		goto err;
	/* Don't allow any ".."s in the path, to avoid exposing stuff outside
	 * of the docroot.  This test is both overzealous and underzealous:
	 * it forbids aceptable paths like "/this/one..here", but it doesn't
	 * do anything to prevent symlink following." */
	if (strstr(decoded_path, ".."))
		goto err;

	len = strlen(decoded_path)+strlen(docroot)+2;
	if (!(whole_path = (char*)malloc(len))) {
		perror("malloc");
		goto err;
	}
	evutil_snprintf(whole_path, len, "%s/%s", docroot, decoded_path);

	if (stat(whole_path, &st)<0) {
		goto err;
	}

	/* This holds the content we're sending. */
	evb = evbuffer_new();

	if (S_ISDIR(st.st_mode)) {	// 如果用户请求的是一个目录
		/* If it's a directory, read the comments and make a little
		 * index page */
		DIR *d;
		struct dirent *ent;
		const char *trailing_slash = "";

		if (!strlen(path) || path[strlen(path)-1] != '/')
			trailing_slash = "/";

		if (!(d = opendir(whole_path)))
			goto err;

		evbuffer_add_printf(evb, "<html>\n <head>\n"
		    "  <title>%s</title>\n"
		    "  <base href='%s%s%s'>\n"
		    " </head>\n"
		    " <body>\n"
		    "  <h1>%s</h1>\n"
		    "  <ul>\n",
		    decoded_path, /* XXX html-escape this. */
		    uri_root, path, /* XXX html-escape this? */
		    trailing_slash,
		    decoded_path /* XXX html-escape this */);
		while ((ent = readdir(d))) {
			const char *name = ent->d_name;
			evbuffer_add_printf(evb,
			    "    <li><a href=\"%s\">%s</a>\n",
			    name, name);/* XXX escape this */
		}
		evbuffer_add_printf(evb, "</ul></body></html>\n");
		closedir(d);
		evhttp_add_header(evhttp_request_get_output_headers(req),
		    "Content-Type", "text/html");
	} else {	// 如果用户请求的是一个文件
		/* Otherwise it's a file; add it to the buffer to get
		 * sent via sendfile */
		const char *type = guess_content_type(decoded_path);
		if ((fd = open(whole_path, O_RDONLY)) < 0) {
			perror("open");
			goto err;
		}

		if (fstat(fd, &st)<0) {
			/* Make sure the length still matches, now that we
			 * opened the file :/ */
			perror("fstat");
			goto err;
		}
		evhttp_add_header(evhttp_request_get_output_headers(req),
		    "Content-Type", type);
		evbuffer_add_file(evb, fd, 0, st.st_size);
	}

	evhttp_send_reply(req, 200, "OK", evb); // 发送用户指定的文件
	goto done;
err:
	evhttp_send_error(req, 404, "Document was not found");
	if (fd>=0)
		close(fd);
done:
	if (decoded)
		evhttp_uri_free(decoded);
	if (decoded_path)
		free(decoded_path);
	if (whole_path)
		free(whole_path);
	if (evb)
		evbuffer_free(evb);
}

void syntax(void)
{
	fprintf(stdout, "Syntax: http-server <docroot>\n");
}

int main(int argc, char** argv) {
	string conf_file_name = "./conf/webhttpd.json";

	// parse command option
	opt_parser = OptionsParser::GetInstance();
	opt_parser->Parse(argc, argv);
	if("" != opt_parser->GetOptionVal('h')) {
		opt_parser->Help();
		return HTTPD_OK;
	}
	if("" != opt_parser->GetOptionVal('c'))
		conf_file_name = opt_parser->GetOptionVal('c');
	fprintf(stderr, "configuration file: %s\n", conf_file_name.c_str());


	// load configuration file
	if(0 != conf_parser.LoadConfFile(conf_file_name)) 
		return HTTPD_ERROR;
	fprintf(stderr, "load %s successfully\n", conf_file_name.c_str());


	// initialize the logging module
	string log_level = conf_parser.GetStringItem("log| level");
	fprintf(stdout, "log level: %s\n", log_level.c_str());
	int log_maxsize = conf_parser.GetIntItem("log| maxsize");
	int log_backup = conf_parser.GetIntItem("log| backup");
	string logfile_runtime_name = conf_parser.GetStringItem("log| runtime_log");
	string logfile_access_name = conf_parser.GetStringItem("log| access_log");
	
	logger.SetLogger("webhttpd", logfile_runtime_name, log_maxsize, log_backup);
	logger.SetLevel(log_level);
	fprintf(stdout, "initialize logger\n");

	// TODO
	logger.Debug("this is the first log of webhttpd");
	//logger.Info("This is a info message");
	//logger.Warn("This is a warn message");

	// TODO: 信号处理
	// TODO: 创建多进程，基于libevent来处理http请求
	/*
	int worker_num = conf_parser.GetIntItem("common| worker_processes");
	logger.Info("webhttp will start xxx worker process"); // TODO: 修改接口

	for(int i = 0; i < worker_num; ++i) {
		int sockfd[2];
		if(0 != socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd)) {
			logger_master.Error(""); // TODO
			return -1;
		}
		pid_t pid = fork();
		// TODO
	}

	delete opt_parser;
	*/

	// ===================================================================

	int port = 0;
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return HTTPD_ERROR;

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "couldn't create an event_base, exiting\n");
		return HTTPD_ERROR;
	}

	/* Create a new evhttp object to handle requests. */
	http = evhttp_new(base);
	if (!http) {
		fprintf(stderr, "couldn't create evhttp, exiting\n");
		return HTTPD_ERROR;
	}

	/* The /dump URI will dump all requests to stdout and say 200 ok. */
	evhttp_set_cb(http, "/dump", dump_request_cb, NULL);

	/* We want to accept arbitrary requests, so we need to set a "generic"
	 * cb.  We can also add callbacks for specific paths. */
	string html_path = conf_parser.GetStringItem("server| www");
	evhttp_set_gencb(http, send_document_cb, (char*)html_path.c_str());

	/* Now we tell the evhttp what port to listen on */
	port = conf_parser.GetIntItem("server| listen");
	handle = evhttp_bind_socket_with_handle(http, "0.0.0.0", port);
	if (!handle) {
		fprintf(stderr, "couldn't bind to port %d. Exiting.\n", port);
		return HTTPD_ERROR;
	}

	//{
		/* Extract and display the address we're listening on. */
		struct sockaddr_storage ss;
		evutil_socket_t fd;
		ev_socklen_t socklen = sizeof(ss);
		char addrbuf[128];
		void *inaddr;
		const char *addr;
		int got_port = -1;
		fd = evhttp_bound_socket_get_fd(handle);
		memset(&ss, 0, sizeof(ss));
		if (getsockname(fd, (struct sockaddr *)&ss, &socklen)) {
			perror("getsockname() failed");
			return HTTPD_ERROR;
		}
		if (ss.ss_family == AF_INET) {
			got_port = ntohs(((struct sockaddr_in*)&ss)->sin_port);
			inaddr = &((struct sockaddr_in*)&ss)->sin_addr;
		} else if (ss.ss_family == AF_INET6) {
			got_port = ntohs(((struct sockaddr_in6*)&ss)->sin6_port);
			inaddr = &((struct sockaddr_in6*)&ss)->sin6_addr;
		} else {
			fprintf(stderr, "Weird address family %d\n",
			    ss.ss_family);
			return HTTPD_ERROR;
		}
		addr = evutil_inet_ntop(ss.ss_family, inaddr, addrbuf,
		    sizeof(addrbuf));
		if (addr) {
			printf("Listening on %s:%d\n", addr, got_port);
			evutil_snprintf(uri_root, sizeof(uri_root),
			    "http://%s:%d",addr,got_port);
		} else {
			fprintf(stderr, "evutil_inet_ntop failed\n");
			return HTTPD_ERROR;
		}
	//}

	event_base_dispatch(base);

	return HTTPD_OK;
}
