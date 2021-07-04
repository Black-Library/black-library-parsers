CFLAGS = -c -g -Wall --std=c++11 -lstdc++fs

all: Test

Test: test.cpp
	g++ -o Test test.cpp -std=c++11 -O3 -I/usr/include/python3.6m/ -lpython3.6m -lcrypt -lpthread -ldl -lutil -lrt -lm -lstdc++fs

clean:
	rm -f *.o Test
