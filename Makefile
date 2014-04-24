FLAGS = -std=c++11 -g3 -pedantic -Wall -Wextra -lboost_system-mt -lboost_thread-mt
#-O2 -static-libstdc++ -pthread
CXX = g++-4.8
.PHONY=clean blackhole

all: blackhole

server-bin: server/server.o server/main.o server/player.o server/message.o
	$(CXX) $(FLAGS) server/server.o server/main.o server/player.o server/message.o -o server-bin
    
server/server.o: server/server.cpp server/server.h server/player.h
	$(CXX) $(FLAGS) -c server/server.cpp -o server/server.o
    
server/main.o: server/main.cpp server/server.h server/player.h
	$(CXX) $(FLAGS) -c server/main.cpp -o server/main.o
    
server/player.o: server/player.cpp server/player.h server/message.h
	$(CXX) $(FLAGS) -c server/player.cpp -o server/player.o

server/message.o: server/message.cpp server/message.h
	$(CXX) $(FLAGS) -c server/message.cpp -o server/message.o

    
clean: 
	rm server-bin server/*.o
blackhole:
	