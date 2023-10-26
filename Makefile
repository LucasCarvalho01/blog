all:
	gcc -Wall -c common.c
	gcc -Wall -g server.c common.o game.c -o ./bin/server
	gcc -Wall -g client.c common.o -o ./bin/client

clean:
	rm common.o ./bin/client ./bin/server
