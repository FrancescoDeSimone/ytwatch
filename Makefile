CC=g++
FLAG= -lcurl -lpthread

all: clean ytwatch

ytwatch:
	$(CC) $(FLAG) -Ofast main.cpp -o ytwatch

clean:
	rm ytwatch
