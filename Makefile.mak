FLAGS = -std=c++11 -pedantic -Wall -Wextra -O2 -static-libstdc++ -pthread
CXX = g++-4.8
.PHONY=clean blackhole

all: blackhole

server: server/server.o server/main.o
	$(CXX) $(FLAGS) server/server.o server/main.o -o server
    
server/server.o: server/server.cpp server/server.hpp
	$(CXX) $(FLAGS) -c server/server.cpp -o server/server.o
    
server/main.o: server/main.cpp server/server.hpp
	$(CXX) $(FLAGS) -c server/main.cpp -o server/main.o
clean: 
	rm *.o server client
blackhole:
	