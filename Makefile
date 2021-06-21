CC=g++
FLAG= -lcurl -lpthread

all: ytwatch

ytwatch:
	$(CC) $(FLAG) -Ofast main.cpp -o ytwatch
