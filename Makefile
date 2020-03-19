all: hive-fuse

clean: 
	rm -f hive-fuse

hive-fuse: hive-fuse.cpp
	g++ -Wall `pkg-config fuse3 --cflags --libs` -lgnutls -lopendht -c hive-fuse.cpp 
	gcc hive-fuse.o  -o hive-fuse `pkg-config fuse3 --cflags --libs` -lstdc++ -lopendht -lgnutls

start: all
	rm -rf hive-root
	mkdir hive-root
	./hive-fuse -f hive-root > out.txt &
	mount
	tail -f out.txt &

stop:
	( sudo umount hive-root ; echo "")
	( killall -q hive-fuse ; echo "")
	mount
	( killall -q tail ; echo "")