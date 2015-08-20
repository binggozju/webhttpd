#include <string>
#include <memory>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
using namespace log4cplus;

#include "logging.h"

namespace webhttpd {

Logging::Logging(const std::string& logger_name, const std::string& logfile, int max_size, int backup)
	: logger_name_(logger_name), 
	  logfile_name_(logfile),
	  max_file_size_(max_size),
	  max_backup_(backup) {
	
	SharedAppenderPtr appender(new RollingFileAppender(logfile, max_size*1024*1024, backup));
	appender->setName("log file");

	std::string pattern = "%D{%Y/%m/%d %H:%M:%S} - %m%n";
	std::auto_ptr<Layout> layout(new PatternLayout(pattern));

	appender->setLayout(layout);

	/* 设置日志记录器 */
	logger_ = Logger::getInstance(logger_name);
	logger_.addAppender(appender);
	logger_.setLogLevel(INFO_LOG_LEVEL);
}

Logging::~Logging() {
}

void Logging::Debug(const std::string& msg) {
	LOG4CPLUS_DEBUG(logger_, msg);
}

void Logging::Info(const std::string& msg) {
	LOG4CPLUS_INFO(logger_, msg);
}

void Logging::Warn(const std::string& msg) {
	LOG4CPLUS_WARN(logger_, msg);
}

void Logging::Error(const std::string& msg) {
	LOG4CPLUS_ERROR(logger_, msg);
}

void Logging::SetLevel(const std::string& level) {
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

} // namespace webhttpd
