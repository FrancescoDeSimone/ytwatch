CC=g++
FLAG= -lcurl -lptread

all: ytwatch

ytwatch:
	$(CC) main.cpp $(FLAG) -Ofast 
