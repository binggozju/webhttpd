#include "logging.h"

Logging::Logging() {
	logger_name_ = "";
	logfile_name_ = "";
	max_file_size_ = 0;
	max_backup_ = 0;
}

Logging::Logging(const string& logger_name, const string& logfile, int max_size, int backup)
	: logger_name_(logger_name), 
	  logfile_name_(logfile),
	  max_file_size_(max_size),
	  max_backup_(backup) {
	
	SharedAppenderPtr appender(new RollingFileAppender(logfile, max_size*1024*1024, backup));
	appender->setName("log file");

	string pattern = "%D{%Y/%m/%d %H:%M:%S} - %m%n";
	auto_ptr<Layout> layout(new PatternLayout(pattern));

	appender->setLayout(layout);

	/* 设置日志记录器 */
	logger_ = Logger::getInstance(logger_name);
	logger_.addAppender(appender);
	logger_.setLogLevel(INFO_LOG_LEVEL);
}

Logging::~Logging() {
}

void Logging::SetLogger(const string& logger_name, const string& logfile, int max_size, int backup) {
	logger_name_ = logger_name; 
	logfile_name_= logfile;
	max_file_size_ = max_size;
	max_backup_ = backup;
	
	SharedAppenderPtr appender(new RollingFileAppender(logfile, max_size*1024*1024, backup));
	appender->setName("log file");

	string pattern = "%D{%Y/%m/%d %H:%M:%S} - %m%n";
	auto_ptr<Layout> layout(new PatternLayout(pattern));

	appender->setLayout(layout);

	/* 设置日志记录器 */
	logger_ = Logger::getInstance(logger_name);
	logger_.addAppender(appender);
	logger_.setLogLevel(INFO_LOG_LEVEL);
}

void Logging::Debug(const string& msg) {
	if("" == logger_name_) {
		fprintf(stderr, "has not set logger\n");
		return;
	}
	LOG4CPLUS_DEBUG(logger_, msg);
}

void Logging::Info(const string& msg) {
	if("" == logger_name_) {
		fprintf(stderr, "has not set logger\n");
		return;
	}
	LOG4CPLUS_INFO(logger_, msg);
}

void Logging::Warn(const string& msg) {
	if("" == logger_name_) {
		fprintf(stderr, "has not set logger\n");
		return;
	}
	LOG4CPLUS_WARN(logger_, msg);
}

void Logging::Error(const string& msg) {
	if("" == logger_name_) {
		fprintf(stderr, "has not set logger\n");
		return;
	}
	LOG4CPLUS_ERROR(logger_, msg);
}

void Logging::SetLevel(const string& level) {
	if("" == logger_name_) {
		fprintf(stderr, "has not set logger\n");
		return;
	}

	if("debug" == level)
		logger_.setLogLevel(DEBUG_LOG_LEVEL);
	else if("info" == level)
		logger_.setLogLevel(INFO_LOG_LEVEL);
	else if("warn" == level)
		logger_.setLogLevel(WARN_LOG_LEVEL);
	else if("error" == level)
		logger_.setLogLevel(ERROR_LOG_LEVEL);
	else
		logger_.setLogLevel(INFO_LOG_LEVEL);
}
