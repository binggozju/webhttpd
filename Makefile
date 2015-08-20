all: webhttpd

webhttpd: webhttpd.cpp options_parser.cpp config.cpp utils.cpp logging.cpp
	g++ -O0 -g -Wall webhttpd.cpp options_parser.cpp config.cpp utils.cpp logging.cpp  -ljson -llog4cplus -lpthread -o webhttpd

clean:
	rm webhttpd
