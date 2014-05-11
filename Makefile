.PHONY=build_binaries doxygen clean clean-build blackhole pack run

all: build_binaries


build_binaries: 
	cd src && make -j 4 && \
	cd .. && \
	cp src/server-bin bludiste2014-server && \
	cp src/client-bin-gui bludiste2014 && \
	cp src/client-bin-console bludiste2014-cli
	
	
doxygen:
	doxygen
	
pack:
	mkdir xmarti62
	cp -r examples ./xmarti62
	cp -r src ./xmarti62
	cp -r maps ./xmarti62
	cp Doxyfile ./xmarti62
	cp Makefile ./xmarti62
	cp README.txt ./xmarti62
	# tar -zcvf archive_name.tar.gz directory_to_compress
	tar --exclude=src/tools -czf xmarti62.tar.gz xmarti62
	rm -rf ./xmarti62
	
run:
	./bludiste2014-server 55555 &
	sleep 1
	./bludiste2014
	

clean: 
	cd src && make clean
	rm bludiste2014-server
	rm bludiste2014
	rm bludiste2014-cli
	rm -rf xmarti62
	
clean-build: 
	cd src && make clean

blackhole: