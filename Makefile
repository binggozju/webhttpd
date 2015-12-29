VERSION = 0.1.0
APP = webhttp
TARGET = webhttpd.${VERSION}

INCLUDES = -I/usr/local/include -I/usr/include -I./include
LIBS = -L/usr/local/lib -L/usr/lib

CC = gcc
CXX = g++
CTAGS = ctags
CFLAGS = -g -Wall -O0 -c $(INCLUDES) 
LDFLAGS = -ljson -llog4cplus -lpthread -levent $(LIBS)

SRCDIR = .
SRCS = $(wildcard $(SRCDIR)/*.cc $(SRCDIR)/*.cpp)
OBJS = $(addsuffix .o, $(basename $(SRCS)))
HEADS = $(addsuffix .h, $(basename $(SRCS)))


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ $<

clean:
	rm -rf *.o $(TARGET)
install:
	cp $(TARGET) ./bin/
