#ifndef WEBHTTPD_CONFIG_H_
#define WEBHTTPD_CONFIG_H_

#include <string>
#include <vector>

#include <libjson/libjson.h>

namespace webhttpd {

class ConfigParser {
public:
	explicit ConfigParser(const std::string& conf_file);
	~ConfigParser();

	/* item_name: 配置项的路径及名称，例如"worker_processes"的item_name为"common|worker_processes"
	 * val: 存储配置项的值
	 */
	std::string GetStringItem(const std::string& item_name);
	int GetIntItem(const std::string& item_name);

private:
	std::string conf_file_;
	JSONNode root_;

	int LoadConfFile(const std::string& conf_file);

	/* 解析配置项的路径，如对于"common|worker_processes"，会将common和worker_processes分别解析出来并存储到vector中 */
	std::vector<std::string> ParseItemName(const std::string& item_name);
};

} // namespace webhttpd

#endif