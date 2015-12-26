#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

#include <event2/event.h>

#include "options_parser.h"
#include "config.h"
#include "logging.h"

namespace webhttpd {
	/* 全局变量 */
	OptionsParser* opt_parser;
	// TODO:ConfigParser* conf_parser;
	// TODO:日志记录器

	struct event_base *base;

	std::vector<int> child_fds; // 存储master进程用来与各worker进程通信的unix域套接字
	int child_index; // master进程将向child_fds[child_index]写入新的连接套接字，传递给对应的worker子进程 
	
	int parent_fd; // 在worker子进程中存储的是用来与master进程通信的unix域套接字，在master进程中为-1
}

using namespace webhttpd;

int main(int argc, char** argv) {
	std::string conf_file_name = "conf/conf.json"; // 默认的配置文件
	
	/* 处理命令行选项 */
	opt_parser = OptionsParser::GetInstance();
	opt_parser->Parse(argc, argv);

	if("" != opt_parser->GetOptionVal('h')) {
		opt_parser->Help();
		return 0;
	}
	if("" != opt_parser->GetOptionVal('c'))
		conf_file_name = opt_parser->GetOptionVal('c');
	std::cout << "configuration file: " << conf_file_name << std::endl;

	/* 加载配置文件 */
	ConfigParser conf_parser(conf_file_name);
	std::cout << "load " << conf_file_name << " successfully." << std::endl;

	/* 初始化日志模块 */
	std::string log_level = conf_parser.GetStringItem("log| level");
	int log_maxsize = conf_parser.GetIntItem("log| maxsize");
	int log_backup = conf_parser.GetIntItem("log| backup");
	std::string logfile_runtime_name = conf_parser.GetStringItem("log| runtime_log");

	Logging logger_master("master", logfile_runtime_name, log_maxsize, log_backup);
	logger_master.SetLevel(log_level);

	logger_master.Debug("This is a debug message");
	logger_master.Info("This is a info message");
	logger_master.Warn("This is a warn message");
	//logger_master.Error("This is a error message");


	// TODO: 信号处理
	// TODO: 创建多进程，基于libevent来处理http请求
	/*
	int worker_num = conf_parser.GetIntItem("common| worker_processes");
	logger_master.Info("webhttp will start xxx worker process"); // TODO: 修改接口

	for(int i = 0; i < worker_num; ++i) {
		int sockfd[2];
		if(0 != socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd)) {
			logger_master.Error(""); // TODO
			return -1;
		}
		pid_t pid = fork();
		// TODO



	}
	*/

	delete opt_parser;

	return 0;
}
