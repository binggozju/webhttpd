#!/bin/bash

#g++ -g -Wall -I .. -o options_parser_test.out options_parser_test.cpp ../options_parser.cpp ../options_parser.h

#g++ -g -Wall -I .. -o utils_test.out utils_test.cpp ../utils.cpp ../utils.h

g++ -g -Wall -I .. config_test.cpp ../config.cpp ../config.h ../utils.h  ../utils.cpp -ljson -o config_test.out

