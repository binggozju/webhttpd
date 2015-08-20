#include <iostream>
#include <string>
using namespace std;

#include "config.h"
using namespace webhttpd;

int main() {
	string conf_file_name("../conf/conf.json");
	ConfigParser conf_parser(conf_file_name);

	string str_workers("common | worker_processes");
	int workers = conf_parser.GetIntItem(str_workers);
	string str_log_level("log | level");
	string log_level = conf_parser.GetStringItem(str_log_level);

	cout << "worker_processes: " << workers << endl;
	cout << "log's level: " << log_level << endl;

	return 0;
}
