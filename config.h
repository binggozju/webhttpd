#ifndef WEBHTTPD_CONFIG_H_
#define WEBHTTPD_CONFIG_H_

#include "webhttpd_core.h"

class ConfigParser {
public:
	ConfigParser();
	explicit ConfigParser(const string& conf_file);
	~ConfigParser();

	int LoadConfFile(const string& conf_file);
	
	/* item_name: 配置项的路径及名称，例如"worker_processes"的item_name为"common|worker_processes"
	 * val: 存储配置项的值
	 */
	string GetStringItem(const string& item_name);
	int GetIntItem(const string& item_name);

private:
	string conf_file_;
	Document doc_;

	/* 解析配置项的路径，如对于"common|worker_processes"，会将common和worker_processes分别解析出来并存储到vector中 */
	vector<string> ParseItemName(const string& item_name);
};

#endif
