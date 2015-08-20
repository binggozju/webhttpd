#include <iostream>
#include <string>

#include "options_parser.h"
#include "config.h"
#include "logging.h"
using namespace webhttpd;

int main(int argc, char** argv) {
	std::string conf_file_name = "conf/conf.json"; // 默认的配置文件
	
	/* 处理命令行选项 */
	OptionsParser* opt_parser = OptionsParser::GetInstance();
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
	logger_master.Error("This is a error message");

	// TODO: 创建多进程，基于libevent来处理http请求




	delete opt_parser;

	return 0;
}
