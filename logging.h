#ifndef WEBHTTPD_LOGGING_H_
#define WEBHTTPD_LOGGING_H_

#include "webhttpd_core.h"

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
	Logging(const string& logger_name, const string& logfile, int max_size, int backup);
	~Logging();

	void SetLogger(const string& logger_name, const string& logfile, int max_size, int backup);

	void Debug(const string& msg);
	void Info(const string& msg);
	void Warn(const string& msg);
	void Error(const string& msg);
	//void Fatal(const string& msg);

	void SetLevel(const string& level);

	//Logging GetChild(const string& name);

private:
	Logger logger_;
	string logger_name_;  // Logging的名称

	string logfile_name_; // 日志文件的名称
	int level_;
	int max_file_size_; // 日志文件的最大尺寸，单位为MB，当达到最大则会转储
	int max_backup_; // 日志文件个数最多为max_backup_ + 1
};

#endif
