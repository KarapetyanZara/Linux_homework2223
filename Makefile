CC=g++
CFLAGS=-Wall -Wextra -pedantic -std=c++11


all: client server


client: client.cpp
	$(CC) $(CFLAGS) -o client client.cpp -lrt


server: server.cpp
	$(CC) $(CFLAGS) -o server server.cpp -lrt


clean:
	rm -f client server
