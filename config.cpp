#include <string>
#include <vector>
#include <stdexcept>

#include <libjson/libjson.h>

#include "config.h"
#include "utils.h"

namespace webhttpd {

ConfigParser::ConfigParser(const std::string& conf_file)
	: conf_file_(conf_file) {
	if(0 != LoadConfFile(conf_file))
		std::cout << "error: fail to create the ConfigParser object." << std::endl;
}
 
ConfigParser::~ConfigParser() {
}

int ConfigParser::LoadConfFile(const std::string& conf_file) {
	/* 读取配置文件 */
	std::string conf_data("");
	if(0 != ReadFileByWord(conf_file, conf_data)) {
		std::cout << "error: unable to open the configuration file." << std::endl;
		return -1;
	}
	//std::cout << conf_data << std::endl;

	/* 解析配置文件 */
	try {
		root_ = libjson::parse(conf_data);
	}
	catch(std::invalid_argument& e) {
		std::cout << "error: " << e.what() << std::endl;
		return -2;
	}

	return 0;
}

std::string ConfigParser::GetStringItem(const std::string& item_name) {
	JSONNode node = root_.duplicate(); // 不可以用JSONNode node = root_;会发生段错误

	std::vector<std::string> vec = ParseItemName(item_name);
	for(std::vector<std::string>::size_type i = 0; i != vec.size(); ++i) {
		json_string json_name = libjson::to_json_string(vec[i]);
		node = node[json_name].duplicate();
	}

	std::string val = libjson::to_std_string(node.as_string());
	return val;
}

int ConfigParser::GetIntItem(const std::string& item_name) {
	JSONNode node = root_.duplicate();

	std::vector<std::string> vec = ParseItemName(item_name);
	for(std::vector<std::string>::size_type i = 0; i != vec.size(); ++i) {
		json_string json_name = libjson::to_json_string(vec[i]);
		node = node[json_name].duplicate();
	}

	return node.as_int();
}

std::vector<std::string> ConfigParser::ParseItemName(const std::string& item_name) {
	std::vector<std::string> vec;
	std::string::size_type p1 = 0;
	std::string::size_type p2 = item_name.find('|', p1);

	while(p2 != std::string::npos) {
		std::string name = item_name.substr(p1, p2-p1);
		vec.push_back(Trim(name));
		p1 = p2 + 1;
		p2 = item_name.find('|', p1);
	}

	std::string name = item_name.substr(p1);	
	vec.push_back(Trim(name));

	return vec;
}

} // namespace
