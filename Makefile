.PHONY=build_binaries

all: build_binaries


build_binaries: 
	cd src && make -j 4 && \
	cd .. && \
	cp src/server-bin bludiste2014-server && \
	cp src/client-bin-gui bludiste2014 && \
	cp src/client-bin-console bludiste2014-cli

clean: 
	cd src && make clean
	rm bludiste2014-server
	rm bludiste2014
	rm bludiste2014-cli
	
clean-build: 
	cd src && make clean

blackhole: