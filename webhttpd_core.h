#ifndef WEBHTTPD_CORE_H_
#define WEBHTTPD_CORE_H_

#define HTTPD_OK			0
#define HTTPD_ERROR			-1

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <memory>
using namespace std;

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>

#include "rapidjson/document.h"
using namespace rapidjson;

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h> 
#include <log4cplus/fileappender.h> 
#include <log4cplus/layout.h> 
using namespace log4cplus;

#endif
