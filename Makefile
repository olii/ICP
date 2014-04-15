FLAGS = -std=c++11 -g3 -pedantic -Wall -Wextra -lboost_system-mt -lboost_thread-mt
#-O2 -static-libstdc++ -pthread
CXX = g++-4.8
.PHONY=clean blackhole

all: blackhole

server-bin: server/server.o server/main.o
	$(CXX) $(FLAGS) server/server.o server/main.o -o server-bin
    
server/server.o: server/server.cpp server/server.h
	$(CXX) $(FLAGS) -c server/server.cpp -o server/server.o
    
server/main.o: server/main.cpp server/server.h
	$(CXX) $(FLAGS) -c server/main.cpp -o server/main.o
clean: 
	rm server-bin server/*.o
blackhole:
	