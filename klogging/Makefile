all:
	gcc -fPIC -c KLogging.cpp -o KLogging.o
	gcc -shared KLogging.o -o libklogging.so
clean:
	rm -f KLogging.o libklogging.so
