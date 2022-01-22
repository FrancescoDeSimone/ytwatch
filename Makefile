CC=clang++
FLAG= -lcurl -lpthread -lfmt
NAME=ytscraper

all: clean ytwatch


ytwatch: clean
	$(CC) -std=c++20 $(FLAG) -Ofast main.cpp -o $(NAME)

clean:
	rm -f $(NAME)
