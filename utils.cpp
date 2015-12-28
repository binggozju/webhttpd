#include <string>

#include "utils.h"

std::string Trim(std::string& str) {
	if("" == str)
		return "";

	std::string::size_type p1 = str.find_first_not_of(' ');
	if(std::string::npos == p1)
		return "";

	std::string::size_type p2 = str.find_last_not_of(' ');
	
	return str.substr(p1, p2-p1+1);
}


int ReadFileByWord(std::string file_name, std::string& content) {
	if(CheckError(file_name))
		return 1;

	std::ifstream file_in(file_name.c_str());
	file_in >> content;

	std::string word;
	while(file_in >> word)
		content += " " + word;
	
	return 0;
}

int ReadFileByLine(std::string file_name, std::string& content) {
	if(CheckError(file_name))
		return 1;

	std::ifstream file_in(file_name.c_str());
	std::string line;
	content = "";
	while(getline(file_in, line))
		content += line + "\n";

	return 0;
}
