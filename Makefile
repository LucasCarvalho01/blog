all:
	gcc -Wall -c common.c -o ./bin/common.o 
	gcc -Wall -c blog.c -o ./bin/blog.o
	gcc -Wall -g server.c common.o blog.o -o ./bin/server
	gcc -Wall -g client.c ./bin/common.o -o ./bin/client

clean:
	rm ./bin/*
