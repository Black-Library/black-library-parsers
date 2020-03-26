CFLAGS = -c -g -Wall --std=c++11
CURLLIBS = -I/usr/local/include -L/user/local/lib -lcurl
XMLLIBS = -I/usr/include/libxml2 -lxml2
LIBS = $(CURLLIBS) $(XMLLIBS)

SRC = src/
INC = include/

all: Tester

Tester: $(SRC)Tester.cpp ParserAO3.o
	g++ -g -Wall --std=c++11 -o Tester $(SRC)Tester.cpp *.o -lm $(LIBS)

ParserAO3.o: $(SRC)ParserAO3.cpp Parser.o $(INC)ParserAO3.h
	g++ $(CFLAGS) $(SRC)ParserAO3.cpp $(LIBS)

Parser.o: $(SRC)Parser.cpp SourceInformation.o $(INC)Parser.h
	g++ $(CFLAGS) $(SRC)Parser.cpp $(LIBS)

SourceInformation.o: $(SRC)SourceInformation.cpp $(INC)SourceInformation.h
	g++ $(CFLAGS) $(SRC)SourceInformation.cpp

clean:
	rm -f *.o Tester
