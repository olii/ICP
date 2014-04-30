FLAGS = -std=c++11 -g3 -pedantic -Wall -Wextra
LINKERF = -lboost_system-mt -lboost_thread-mt -lboost_serialization

#-O2 -static-libstdc++ -pthread
CXX = g++-4.8
.PHONY=clean blackhole

SERVEROBJ = server/server.o server/main.o server/player.o server/game.o server/manager.o
SHAREDOBJ = shared/serverinfo.o shared/command.o
CLIENTOBJ = client/main.o

DEPSFILES = $(wildcard server/*.c) $(wildcard server/*.h) $(wildcard client/*.c) $(wildcard client/*.h) $(wildcard shared/*.c) $(wildcard shared/*.h)

all: Makefile.deps server-bin client-bin
%.o: %.cpp
	$(CXX) $(FLAGS) -c $*.cpp -o $*.o
	
server-bin: $(SERVEROBJ) $(SHAREDOBJ)
	$(CXX) $(FLAGS) $(LINKERF) $^ -o $@
       
client-bin: $(CLIENTOBJ) $(SHAREDOBJ)
	$(CXX) $(FLAGS) $(LINKERF) $^ -o $@  
	
	
-include Makefile.deps
	
Makefile.deps: $(DEPSFILES)
	$(CXX) $(FLAGS) -MM ./client/*.cpp | sed -r 's|[A-Za-z]+\.o|client/&|g' > Makefile.deps
	$(CXX) $(FLAGS) -MM ./server/*.cpp | sed -r 's|[A-Za-z]+\.o|server/&|g' >> Makefile.deps
	$(CXX) $(FLAGS) -MM ./shared/*.cpp | sed -r 's|[A-Za-z]+\.o|shared/&|g' >> Makefile.deps


clean: 
	rm server-bin server/*.o client-bin client/*.o shared/*.o Makefile.deps
	
blackhole: