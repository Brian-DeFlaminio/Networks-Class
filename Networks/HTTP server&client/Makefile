CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

all: HTTPServer HTTPClient

HTTPServer: HTTPServer.c
	gcc -pthread -Werror HTTPServer.c -o HTTPServer

HTTPClient1: HTTPClient.c
	gcc -pthread -Werror HTTPClient.c -o HTTPClient

clean:
	rm -f HTTPServer HTTPClient
