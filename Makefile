CC=g++
FLAG= -lcurl -lpthread
NAME=ytscraper

all: clean ytwatch


ytwatch: clean
	$(CC) -std=c++20 $(FLAG) -Ofast main.cpp -o $(NAME)

clean:
	rm -f $(NAME)
