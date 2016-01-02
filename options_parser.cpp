#include "options_parser.h"

OptionsParser::OptionsParser() {
	AddNewOption('c', "conf", "specify the configuration file, ./conf/webhttpd.conf default.");
	AddNewOption('h', "help", "print the help infomation.");
}

OptionsParser::~OptionsParser() {	
}

void OptionsParser::AddNewOption(char short_opt, string long_opt, string info) {
	Option opt(short_opt, long_opt, info);
	options_.push_back(opt);
	options_kv_[short_opt] = "";
}

void OptionsParser::UpdateOptionVal(char opt, string val) {
	if(options_kv_.count(opt))
		options_kv_[opt] = val;
	else
		fprintf(stderr, "Error: -%c is an unvalid option\n", opt);
}

void OptionsParser::UpdateOptionVal(string opt, string val) {
	for(vector<Option>::size_type i = 0; i != options_.size(); ++i)
		if(options_[i].long_name_ == opt) {
			options_kv_[options_[i].short_name_] = val;
			return;
		}

	fprintf(stderr, "Error: --%s is an unvailid option\n", opt.c_str());
}

void OptionsParser::DeleteOptionVal() {
	map<char, string>::iterator it = options_kv_.begin();
	while(it != options_kv_.end()) {
		it->second = "";
		++it;
	}
}

// 每个option最多只有一个值（可调整代码逻辑以支持多个值）
int OptionsParser::Parse(int argc, char** argv) {
	DeleteOptionVal();

	int arg_index = 1;
	while(arg_index < argc) {
		string arg_cur(*(argv + arg_index));
		if(arg_cur[0] != '-') {
			arg_index++;
			continue;
		}

		if(arg_cur[1] != '-' && arg_cur.size() == 2) {		// short option name
			if(arg_index + 1 >= argc) { // current option is the last one
				UpdateOptionVal(arg_cur[1]);
				return 0;
			}
				
			string arg_next(*(argv + arg_index + 1));
			if(arg_next[0] != '-') { // this option has a value
				UpdateOptionVal(arg_cur[1], arg_next);
				arg_index += 2;
			}
			else {					// this option has no value
				UpdateOptionVal(arg_cur[1]);
				arg_index++;
			}
		}
		else if(arg_cur[1] == '-' && arg_cur.size() >= 4) { // long option name
			if(arg_index + 1 >= argc) { // current option is the last one
				UpdateOptionVal(arg_cur.substr(2));
				return 0;
			}
			
			string arg_next(*(argv + arg_index + 1));
			if(arg_next[0] != '-') { // this option has a value
				UpdateOptionVal(arg_cur.substr(2), arg_next);
				arg_index += 2;
			}
			else {					// this option has no value
				UpdateOptionVal(arg_cur.substr(2));
				arg_index++;		
			}
		}
		else {
			fprintf(stderr, "Error: %s is an unvalid option.\n", arg_cur.c_str());
			arg_index++;
		}
	} // while

	return 0;
}

string OptionsParser::GetOptionVal(char opt) {
	if(options_kv_.count(opt))
		return options_kv_[opt];
	else
		return "";
}

void OptionsParser::Help() const {
	fprintf(stdout, "[Usage]\n");
	for(vector<Option>::size_type i = 0; i != options_.size(); ++i)
		fprintf(stdout, "  -%c, --%s\n\t%s\n", options_[i].short_name_, options_[i].long_name_.c_str(), options_[i].info_.c_str());
}

OptionsParser* OptionsParser::instance_ = new OptionsParser();

OptionsParser* OptionsParser::GetInstance() {
	return instance_;
}
