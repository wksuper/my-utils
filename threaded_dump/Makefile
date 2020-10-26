all:
	gcc -fPIC -c threaded_dump.c -I ../klogging -o threaded_dump.o
	gcc -shared threaded_dump.o -L ../klogging -lklogging -o libthreaded_dump.so
clean:
	rm -f threaded_dump.o libthreaded_dump.so
