#include <iostream>
#include <string>

#include "utils.h"
using namespace webhttpd;

int main(int argc, char** argv) {
	std::string str("   zhan ye bing    ");
	std::cout << str << std::endl << Trim(str) << std::endl;
	
	// ---------------------------------------------------------------
	std::string file_name("../conf/conf.json");
	std::string data("");

	if(0 != ReadFileByWord(file_name, data))
		std::cout << "error: unable to open the input file.\n";
	else
		std::cout << data << std::endl;

	std::cout << std::endl;

	if(0 != ReadFileByLine(file_name, data))
		std::cout << "error: unable to open the input file.\n";
	else
		std::cout << data << std::endl;

	return 0;
}
