#ifndef WEBHTTPD_LOGGING_H_
#define WEBHTTPD_LOGGING_H_

#include <string>

#include <log4cplus/logger.h>
using namespace log4cplus;

/* 日志级别 */
/* enum LogLevel {
	debug = 0,
	info,
	warn,
	error
}; */

/* 日志类，带转储功能 */
class Logging {
public:
	Logging();
	Logging(const std::string& logger_name, const std::string& logfile, int max_size, int backup);
	~Logging();

	SetLogger(const std::string& logger_name, const std::string& logfile, int max_size, int backup);

	void Debug(const std::string& msg);
	void Info(const std::string& msg);
	void Warn(const std::string& msg);
	void Error(const std::string& msg);
	//void Fatal(const std::string& msg);

	void SetLevel(const std::string& level);

	//Logging GetChild(const std::string& name);

private:
	Logger logger_;
	std::string logger_name_;  // Logging的名称

	std::string logfile_name_; // 日志文件的名称
	int level_;
	int max_file_size_; // 日志文件的最大尺寸，单位为MB，当达到最大则会转储
	int max_backup_; // 日志文件个数最多为max_backup_ + 1
};

#endif
