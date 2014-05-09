FLAGS = -std=c++11 -g3 -pedantic -Wall -Wextra -fdiagnostics-color=always
# FLAGS = -std=c++11 -O2
LINKERF = -lboost_system -lboost_serialization -lboost_filesystem

#-O2 -static-libstdc++ -pthread
CXX = g++-4.9
.PHONY=clean blackhole

SERVEROBJ = server/server.o server/main.o server/player.o server/game.o server/manager.o
SHAREDOBJ = shared/serverinfo.o shared/command.o shared/map.o
CLIENTOBJ = client/network.o

DEPSFILES = $(wildcard server/*.cpp) $(wildcard server/*.h) $(wildcard client/*.cpp) $(wildcard client/*.h) $(wildcard shared/*.cpp) $(wildcard shared/*.h)

all: Makefile.deps server-bin client-bin-console client-bin-gui
%.o: %.cpp
	$(CXX) $(FLAGS) -c $*.cpp -o $*.o
	
server-bin: $(SERVEROBJ) $(SHAREDOBJ)
	$(CXX) $(FLAGS) $(LINKERF) -static-libstdc++ $^ -o $@
       
client-bin-console: client/console/main.o $(CLIENTOBJ) $(SHAREDOBJ)
	$(CXX) $(FLAGS) -pthread $(LINKERF) $^ -o $@  

client/gui/klient:
	cd client/gui/ && \
	qmake QMAKE_CXX=g++-4.9 -o Makefile klient.pro && \
	make -j 4
client-bin-gui: client/gui/klient
	cp client/gui/klient client-bin-gui
	
-include Makefile.deps
	
Makefile.deps: $(DEPSFILES)
	$(CXX) $(FLAGS) -MM ./client/*.cpp | sed -r 's|[A-Za-z]+\.o|client/&|g' > Makefile.deps
	$(CXX) $(FLAGS) -MM ./client/console/*.cpp | sed -r 's|[A-Za-z/]+\.o|client/console/&|g' > Makefile.deps
	$(CXX) $(FLAGS) -MM ./server/*.cpp | sed -r 's|[A-Za-z]+\.o|server/&|g' >> Makefile.deps
	$(CXX) $(FLAGS) -MM ./shared/*.cpp | sed -r 's|[A-Za-z]+\.o|shared/&|g' >> Makefile.deps


clean: 
	rm server-bin client-bin-console client-bin-gui
	rm server/*.o client/*.o client/console/*.o shared/*.o 
	cd client/gui/ && make distclean
	rm Makefile.deps
	
blackhole: