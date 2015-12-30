#ifndef WEBHTTPD_OPTIONS_PARSER_H_
#define WEBHTTPD_OPTIONS_PARSER_H_

#include "webhttpd_core.h"

/* 命令行选项 */
struct Option {
	char short_name_;
	string long_name_;
	string info_;

	Option(char _short, string _long, string _info):
		short_name_(_short),
		long_name_(_long),
		info_(_info) {}
};


/* 选项解析器，单例模式 */
class OptionsParser {
public:
	static OptionsParser* GetInstance();

	int Parse(int argc, char** argv);

	/* 获取命令行选项的值, opt为选项的单字母名称
	 * 函数返回选项的值，如果没有值则返回空串 
	 */
	string GetOptionVal(char opt);
	void Help() const;

	~OptionsParser();

private:
	OptionsParser();
	static OptionsParser* instance_;

	void AddNewOption(char short_opt, string long_opt, string info);
	/* 设置选项的值
	 * opt为选项的短名，如果选项没有值，则将选项的值置为"T" 
	 */
	void UpdateOptionVal(char opt, string val="T");		// short name
	/* 功能同上，不同在于opt为选项的长名 */
	void UpdateOptionVal(string opt, string val="T");	// long name
	/* 清空所有选项的值，都置为空串 */
	void DeleteOptionVal();

	map<char, string> options_kv_; // key为选项的短名（不带参数的选项值为T）
	vector<Option> options_;

	// No copying allowd
	OptionsParser(const OptionsParser&);
	OptionsParser& operator=(const OptionsParser&);
}; // class OptionsParser

#endif
