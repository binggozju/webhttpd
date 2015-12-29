#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <string>
#include <map>

#include "options_parser.h"

OptionsParser::OptionsParser() {
	AddNewOption('c', "conf", "specify the configuration file.");
	AddNewOption('h', "help", "print the help infomation.");
}

OptionsParser::~OptionsParser() {	
}

void OptionsParser::AddNewOption(char short_opt, std::string long_opt, std::string info) {
	Option opt(short_opt, long_opt, info);
	options_.push_back(opt);
	options_kv_[short_opt] = "";
}

void OptionsParser::UpdateOptionVal(char opt, std::string val) {
	if(options_kv_.count(opt))
		options_kv_[opt] = val;
	else
		fprintf(stderr, "Error: -%c is an unvalid option\n", opt)
}

void OptionsParser::UpdateOptionVal(std::string opt, std::string val) {
	for(std::vector<Option>::size_type i = 0; i != options_.size(); ++i)
		if(options_[i].long_name_ == opt) {
			options_kv_[options_[i].short_name_] = val;
			return;
		}

	fprintf(stderr, "Error: --%s is an unvailid option\n", opt.c_str());
}

void OptionsParser::DeleteOptionVal() {
	std::map<char, std::string>::iterator it = options_kv_.begin();
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
		std::string arg_cur(*(argv + arg_index));
		if(arg_cur[0] != '-') {
			arg_index++;
			continue;
		}

		if(arg_cur[1] != '-' && arg_cur.size() == 2) {		// short option name
			if(arg_index + 1 >= argc) { // current option is the last one
				UpdateOptionVal(arg_cur[1]);
				return 0;
			}
				
			std::string arg_next(*(argv + arg_index + 1));
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
			
			std::string arg_next(*(argv + arg_index + 1));
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

std::string OptionsParser::GetOptionVal(char opt) {
	if(options_kv_.count(opt))
		return options_kv_[opt];
	else
		return "";
}

void OptionsParser::Help() const {
	std::cout<<"[Usage]\n";
	for(std::vector<Option>::size_type i = 0; i != options_.size(); ++i)
		fprintf(stdout, "-%c, --%s\n\t%s\n", options_[i].short_name_, options_[i].long_name_.c_str(), options_[i].info_.c_str())
}

OptionsParser* OptionsParser::instance_ = new OptionsParser();

OptionsParser* OptionsParser::GetInstance() {
	return instance_;
}
