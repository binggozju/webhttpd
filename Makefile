#all: webhttpd
#webhttpd: webhttpd.cpp options_parser.cpp config.cpp utils.cpp logging.cpp
#	g++ -O0 -g -Wall webhttpd.cpp options_parser.cpp config.cpp utils.cpp logging.cpp  -ljson -llog4cplus -lpthread -o webhttpd
#clean:
#	rm webhttpd

TARGET = webhttpd

INCLUDES = -I/usr/local/include
LIBS = -L/usr/local/lib

CXX = g++
CFLAGS = -g -Wall -O0 -c $(INCLUDES) 
LDFLAGS = -ljson -llog4cplus -lpthread -levent $(LIBS)

SRCDIR = .
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(addsuffix .o, $(basename $(SRCS)))
HEADS = $(addsuffix .h, $(basename $(SRCS)))


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(TARGET)
