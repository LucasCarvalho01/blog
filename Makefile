all:
	gcc -Wall -c common.c -o ./bin/common.o 
	gcc -Wall -c blog.c -o ./bin/blog.o
	gcc -Wall -g server.c blog.c ./bin/common.o  -lpthread -o server
	gcc -Wall -g client.c ./bin/common.o -o client

clean:
	rm ./bin/* server client
