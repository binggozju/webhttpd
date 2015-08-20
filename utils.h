#ifndef WEBHTTPD_UTILS_H_
#define WEBHTTPD_UTILS_H_

#include <string>
#include <fstream>
#include <iostream>

namespace webhttpd {

/* 字符串辅助函数 */
extern std::string Trim(std::string& str); // 去除字符串两端的空格

/* 文件读写函数 */
extern int ReadFileByWord(std::string file_name, std::string& content); // 逐词读取（单词之间以空格分开）
extern int ReadFileByLine(std::string file_name, std::string& content); // 逐行读取（行与行之间以回车换行分开）
inline int CheckError(std::string file_name) {  // 检查文件打开是否成功
	std::ifstream file_in(file_name.c_str());
	if(!file_in)
		return 1;
	
	return 0;
}






} // namespace

#endif
