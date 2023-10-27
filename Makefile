all:
	gcc -Wall -c common.c ./bin/common.o 
	gcc -Wall -c blog.c ./bin/blog.o
	gcc -Wall -g server.c common.o blog.o -o ./bin/server
	gcc -Wall -g client.c common.o -o ./bin/client

clean:
	rm common.o blog.o ./bin/client ./bin/server
