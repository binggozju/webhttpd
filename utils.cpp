#include "utils.h"

string Trim(string& str) {
	if("" == str)
		return "";

	string::size_type p1 = str.find_first_not_of(' ');
	if(string::npos == p1)
		return "";

	string::size_type p2 = str.find_last_not_of(' ');
	
	return str.substr(p1, p2-p1+1);
}


int ReadFileByWord(string file_name, string& content) {
	if(CheckError(file_name))
		return 1;

	ifstream file_in(file_name.c_str());
	file_in >> content;

	string word;
	while(file_in >> word)
		content += " " + word;
	
	return 0;
}

int ReadFileByLine(string file_name, string& content) {
	if(CheckError(file_name))
		return 1;

	ifstream file_in(file_name.c_str());
	string line;
	content = "";
	while(getline(file_in, line))
		content += line + "\n";

	return 0;
}
