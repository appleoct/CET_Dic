CC=g++

all:
	$(CC)  demo.cpp `mysql_config --libs`
