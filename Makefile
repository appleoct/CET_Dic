CC=g++
all:
	$(CC) -std=c++11 server.cpp -I/usr/local/include/redis/  -lhiredis
