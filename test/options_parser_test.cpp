#include <iostream>

#include "options_parser.h"
using namespace webhttpd;


int main(int argc, char** argv) {
	OptionsParser* opt_parser = OptionsParser::GetInstance();
	opt_parser->Parse(argc, argv);

	if("" != opt_parser->GetOptionVal('c') && "T" != opt_parser->GetOptionVal('c'))
		std::cout<<"the configuration file is: "<<opt_parser->GetOptionVal('c')<<std::endl;

	if("" != opt_parser->GetOptionVal('h'))
		opt_parser->Help();

	delete opt_parser;

	return 0;
}
