#include "config.h"
#include "utils.h"

ConfigParser::ConfigParser() {
	conf_file_ = "";
}

ConfigParser::ConfigParser(const string& conf_file): conf_file_(conf_file) {
	if(0 != LoadConfFile(conf_file))
		conf_file_ = "";
}
 
ConfigParser::~ConfigParser() {

}

int ConfigParser::LoadConfFile(const string& conf_file) {
	if("" ==  conf_file) {
		fprintf(stderr, "error: there is no config file\n");
		return 1;
	}
	conf_file_ = conf_file;
	
	/* 读取配置文件 */
	string conf_data("");
	if(0 != ReadFileByWord(conf_file, conf_data)) {
		fprintf(stderr, "error: unable to open the configuration file\n");
		return 1;
	}
	fprintf(stdout, "conf: %s\n", conf_data.c_str());

	/* 解析配置文件 */
	if("" == conf_data) {
		fprintf(stderr, "error: the config file is empty\n");
		return 1;
	}
	if(doc_.Parse(conf_data.c_str()).HasParseError() || !doc_.IsObject()) {
		fprintf(stderr, "error: fail to parse the config file\n");
		return 1;
	}

	return 0;
}

string ConfigParser::GetStringItem(const string& item_name) {
	if("" == conf_file_) {
		fprintf(stderr, "has not load config file\n");
		return "";
	}
	if("" == item_name)
		return "";

	vector<string> vec = ParseItemName(item_name);
	Value* val = &doc_;
	for(vector<string>::size_type i = 0; i != vec.size(); ++i) {
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

int ConfigParser::GetIntItem(const string& item_name) {
	if("" == conf_file_) {
		fprintf(stderr, "has not load config file\n");
		return 0;
	}
	if("" == item_name)
		return 0;

	vector<string> vec = ParseItemName(item_name);
	Value* val = &doc_;
	for(vector<string>::size_type i = 0; i != vec.size(); ++i) {
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

vector<string> ConfigParser::ParseItemName(const string& item_name) {
	vector<string> vec;
	string::size_type p1 = 0;
	string::size_type p2 = item_name.find('|', p1);

	while(p2 != string::npos) {
		string name = item_name.substr(p1, p2-p1);
		vec.push_back(Trim(name));
		p1 = p2 + 1;
		p2 = item_name.find('|', p1);
	}

	string name = item_name.substr(p1);	
	vec.push_back(Trim(name));

	return vec;
}
