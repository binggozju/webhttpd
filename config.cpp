#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include "config.h"
#include "utils.h"

ConfigParser::ConfigParser() {
	conf_file_ = "";
}

ConfigParser::ConfigParser(const std::string& conf_file): conf_file_(conf_file) {
	if(0 != LoadConfFile(conf_file))
		conf_file_ = "";
}
 
ConfigParser::~ConfigParser() {

}

int ConfigParser::LoadConfFile(const std::string& conf_file) {
	/* 读取配置文件 */
	std::string conf_data("");
	if(0 != ReadFileByWord(conf_file, conf_data)) {
		fprintf(stderr, "error: unable to open the configuration file\n");
		return 1;
	}

	/* 解析配置文件 */
	if("" == conf_data) {
		fprintf(stderr, "error: the config file is empty\n");
		return 2;
	}
	if(doc_.Parse(conf_data.c_str()).HasParseError() || !doc_.IsObject()) {
		fprintf(stderr, "error: fail to parse the config file\n");
		return 3;
	}

	return 0;
}

std::string ConfigParser::GetStringItem(const std::string& item_name) {
	if("" == conf_file_) {
		fprintf(stderr, "has not load config file\n");
		return "";
	}
	if("" == item_name)
		return "";

	std::vector<std::string> vec = ParseItemName(item_name);
	Value* val = &doc_;
	for(std::vector<std::string>::size_type i = 0; i != vec.size(); ++i) {
		if(val->HasMember(vec[i].c_str()))
			val = &((*val)[vec[i].c_str()]);
		else {
			fprintf(stderr, "%s is not element in config file\n", vec[i].c_str());
			return "";
		}
	}
	
	if(val->IsString())
		return val->GetString();
	else {
		return "";
	}
}

int ConfigParser::GetIntItem(const std::string& item_name) {
	if("" == conf_file_) {
		fprintf(stderr, "has not load config file\n");
		return 0;
	}
	if("" == item_name)
		return 0;

	std::vector<std::string> vec = ParseItemName(item_name);
	Value* val = &doc_;
	for(std::vector<std::string>::size_type i = 0; i != vec.size(); ++i) {
		if(val->HasMember(vec[i].c_str()))
			val = &((*val)[vec[i].c_str()]);
		else {
			fprintf(stderr, "%s is not element in config file\n", vec[i].c_str());
			return 0;
		}
	}
	
	if(val->IsInt())
		return val->GetInt();
	else {
		return 0;
	}
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
