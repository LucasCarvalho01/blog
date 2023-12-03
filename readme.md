# Blog in C using POSIX and threads

This project is a blog, written in C, using POSIX library to make the communication between the hosts and the server using sockets.
It also uses pthreads library to permit simultaneous users accessing the blog system.

It permits clients to subscribe in topics, aswell usubscribe to them. Clients may also publish new posts in those topics, and check which topics exist in order to subscribe in them.

The network layer is built upon TCP protocol. It also implements both ipv4 and ipv6 protocols.

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

#### Check the existing topics in the blog:  
> _list topics_   
It will prompt a list of topics that were previously created.

To subscribe to an existing topic, or create one and after that subscribe in it:  
_subscribe <topic_name>_ In the server console it will be prompted that the given client was subscribed to a specific topic.

To 