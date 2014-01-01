PROGRAMMING ASSIGNMENT - 1                                                                    CSCI 5273 (Network Systems)
==========================================================================================================================
UDP Socket Programming                                                                  University of Colorado at Boulder
-------------------------------------------------------------------------------------------------------------------------
### PURPOSE

1. An appropriate understanding of C language.
2. An understanding of tools (compilers in Linux, shell,etc).
3. Introduction to the socket programming interface.

In this assignment, you will build two programs in C, one for the client which will simply send a command and the other for a server which will send a suitable response back to the client.

### PROGRAM REQUIREMENTS

#### The client:

1. The client must take two command line arguments: an IP address of the machine on which the server application is     
   running, and the port the server application is using. [The IP address can be obtained using hostname -i . Type man
   hostname at the shell prompt for more information on how to use the hostname command.]

2. It should prompt the user to type any of the following commands
    get [file_name]
    put [file_name]
    ls
    exit

3. It must then send the command to the server.

4. Then it must wait for the server's response. Once the server responds, it should print appropriate messages, if any, on
   the standard output.

#### The server:

1. The server must take one command line argument: a port number for the server to use. You should select port #'s > 5000.

2. It should wait for a UDP connection.

3. Depending on the commands received, the server responds to the client's request in the following manner :

    “get [file_name]”: The server transmits the requested file to the client (use files of small size in order of 2 to 5      KB for transfer like any jpeg file).
    
    “put [file_name]”: The server receives the transmitted file by the client and stores it locally (use files of small       size in order of 2 to 5 KB for transfer like any jpeg file).
    
    “ls”: The server should search all the files it has in its local directory and send a list of all these files to the      client.
    
    “exit” : The server should exit gracefully.

For any other commands, the server should simply repeat the command back to the client with no modification, stating that the given command was not understood.

Notes:

Both the client and the server should be in a loop. For the purpose of this assignment, the client can be terminated by typing Ctrl-C.

The client side should contain at least three files with their names hard-coded as foo1, foo2, and foo3.

#### What To Turn In:

You should test your programs thoroughly so that they do not crash or hang. Your code must be organized and clear with comments/explanations for your functions and arguments.  Please turn in one tar file containing two .c source files one readme.txt file and one Makefile. The Makefile must compile both client.c and server.c into their respective executables.  The readme.txt file must explain what you have done and how to run it. The documentation does not have to be long, but does have to be very clear.   The code that you turn in for this programming assignment must be your own original work and must compile and run on the CSEL machines. Upload the tar file containing the following files via the moodle.

Client file named “client.c”

Server file named “server.c”

makefile – compiles “client.c” and “server.c” into “client” and “server” executables.

README.txt file

#### Getting Started:

(Please read as this may answer some of your questions)

Download the udp_example.tar file located on the Moodle.  You can access the files from the main course page under the link "Example tar file for Programming Assignment #1".  To extract files from udp_example.tar on a Linux machine, type 'tar -xvf udp_example.tar'. This should create a directory named “udp” which contains several files including “udp_client.c” and “udp_server.c” and testing files. These files contain a pseudo-code and not the actual implementation. You will have to modify both programs accordingly. After modifying the programs, following commands should be used to compile the programs.

To compile the files type:

gcc udp_client.c -o client
gcc udp_server.c -o server
To run the programs after compiling:

Run the server by typing : ./server [port_number]
Then run the client by typing : ./client [ip_address] [port_number]
The IP address is the server's IP address. The port number is the server's port number. You are informing the client where the server is located. You can test your application locally by running both the client and the server on the same machine. In this case, you can type localhost in place of the IP address.  Feel free to use these files as a baseline for your design. You will have to modify parts of the code, especially for the server where parsing of the message is required.

#### Header Files:

Examine the code in the provided programs. The list of header files that need to be introduced in the socket programming are listed below.

sys/socket.h : The header file socket.h includes a number of definitions of structures needed for sockets.

netinet/in.h : The header file in.h contains constants and structures needed for internet domain addresses.

arpa/inet.h : The header file contains definitions for internet operations.

netdb.h : This header file contains definitions for network database operations.

#### Functions:

A brief explanation of some of the functions used in the code is provided here. However, for in depth understanding of the functions, please read the manpages of the functions.

socket() : The input parameters of the function lets you determine which type of socket you want in your application. It may be a TCP or UDP socket. The function returns a socket descriptor which can prove helpful later system calls or -1 on error. A quick look at the function:

sockfd = socket(PF_INET,SOCK_DGRAM,0);

sockfd is a UDP socket.

bind() : Once we have our socket ready, we have to associate it with a port number on the local machine. It will return -1 on error. When calling bind function, it should be noted that ports below 1024 are reserved. Any port number above 1024 and less than 65535 can be used. A quick reference:

bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr);

sendto(): This function is used to send the data. Since it is used in unconnected datagrams, the input paramater of this function includes the destination address.It returns the number of bytes sent on success and -1 on error.

ssize_t sendto( int sockfd, void *buff, size_t nbytes, int flags, const struct sockaddr* to, socklen_t addrlen);

“buff” is the address of the data (nbytes long).

“to” is the address of a sockaddr containing the destination address.

recvfrom() : This function is used to receive the data from an unconnected datagram socket. The input paramters contain the address of the originating machine. It returns the number of bytes received on success and -1 on error

ssize_t recvfrom( int sockfd, void *buff, size_t nbytes, int flags, struct sockaddr* from, socklen_t *fromaddrlen);

Note: It will be beneficial if you read the Beej’s Socket Programming guide (provided as a reference in this assignment) before starting the actual assignment.

#### Non-blocking client (not required)

Also, on the client side, it may be beneficial for you to configure a non-blocking socket, so the client does not lock up waiting for a message from the server in recvfrom(). If the socket receive buffer is empty for a blocking UDP socket, the calling application is put to sleep until a UDP datagram arrives. If a non-blocking UDP socket cannot return a complete datagram, then it returns immediately with an error status of EWOULDBLOCK, which you should check for. To set a socket as non-blocking, use fcntl(). A typical line might look like:

fcntl(sockfd, F_SETFL, O_NONBLOCK). Do a man for fcntl for more information. It will be easier to use a default blocking socket on the server side.
