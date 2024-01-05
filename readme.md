# Blog in C using POSIX and threads

This project is a blog, written in C, using POSIX library to make the communication between the hosts and the server using sockets.
It also uses pthreads library to permit simultaneous users accessing the blog system.

It permits clients to subscribe in topics, aswell usubscribe to them. Clients may also publish new posts in those topics, and check which topics exist in order to subscribe in them.

The network layer is built upon TCP protocol. It also implements both ipv4 and ipv6 protocols.

## Running

To run the blog, firstly build all the source code. You can do this simply using the makefile by entering:

```
make
```

Then, first run the server. You can choose to use ipv4 or ipv6 as mentioned later in this documentation. For example, run:

```
./bin/server v4 51511
```

And for each user, run the client binary:
```
./bin/client 127.0.0.1 51511
```

## Server

The server is a program that listens to clients connection and handles the users' commands. 

When a new connection is established, the server instatiates a new thread to that client, and properly store the threads to permit correct communication between clients.

To run the server, enter the command:

```
./bin/server <ip_version> <port>
```
-  _ip_version_ may be v4 or v6. 
- _port_ is the chosen one to permit communication with running clients programs.

## Clients

The client is a program that receives commands from the user, handles it, and send the proper message to server.

When client program is first ran, it gets an ID from server and stores it. After that, a thread is instatiated to permit send and receive messages to/from server. 

To run the client program(s), enter the command:
```
./bin/client <ip_adress> <port> 
```
- _ip_address_ is the IP address used by POSIX library to proper send/receive messages related to this client. When running locally, you may enter:  
  _127.0.0.1_ -> localhost address in ipv4  
  _::1_ -> localhost address in ipv6
- _port_ value prompted when running server program

## Commands

_list topics_   
It will prompt a list of topics that were previously created.

_subscribe <topic_name>_  
To subscribe to an existing topic, or create one and after that subscribe in it:  
In the server console it will be prompted that the given client was subscribed to a specific topic.

_unsubscribe <topic_name>_  
Command to unsubscribe the user from a topic.

_pubish in <topic_name>_   
In order to publish a new post in a specific topic. After hitting enter, the console will get the input related to the content of the post.

_exit_  
Command to finish the execution of the client program. The related user ID will be freed, and all the infos related to subscription will be reseted to that ID.