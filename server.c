#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
/* You will have to modify the program below */

#define MAXBUFSIZE 5120

int main (int argc, char * argv[] )
{


	int sock;                           	//This will be our socket
	struct sockaddr_in sin, remote;     	//"Internet socket address structure"
	unsigned int remote_length;       		//length of the sockaddr_in structure
	int nbytes,l;                        		//number of bytes we receive in our message
	char buffer[MAXBUFSIZE];             	//a buffer to store our received message
	socklen_t r;
	char fname[100],length[100];
	char * fsbuffer; long fsSize;
	if (argc != 2)
	{
		printf ("USAGE:  <port:%s>\n",argv[1]);
		exit(1);
	}

	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&sin,sizeof(sin));                    //zero the struct
	sin.sin_family = AF_INET;                   //address family
	sin.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
	sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine


	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET,SOCK_DGRAM,0)) < 0)
	{
		printf("unable to create socket");
	}


	/******************
	  Once we've created a socket, we must bind that socket to the 
	  local address and port we've supplied in the sockaddr_in struct
	 ******************/
	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		printf("unable to bind socket\n");
	}
		
	while(1)
	{
		bzero(buffer,sizeof(buffer));
		r = sizeof(remote);
		nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &r);
		strcpy(fname, buffer+4);
		if(strncmp(buffer,"PUTF",4)==0)
		{
			bzero(length,sizeof(length));
			// Receiving length
			nbytes = recvfrom(sock, length, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &r);
			l=atoi(length);
			bzero(buffer,sizeof(buffer));
			//Receiving file
			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &r);
			FILE *fr = fopen(fname,"w+");
			fwrite(buffer,sizeof(char),l,fr);
			fclose(fr);
		}
		else if(strncmp(buffer,"GETF",4)==0)
		{
			FILE *fs = fopen(fname,"r");
			fseek (fs,0,SEEK_END);
			fsSize = ftell(fs);
			rewind(fs);
			sprintf(length,"%ld", fsSize);
			//sending filesize
			nbytes = sendto(sock, length, strlen(length), 0, (struct sockaddr *)&remote, sizeof(remote));
			fsbuffer = (char*) malloc (sizeof(char)*fsSize);
			bzero(fsbuffer,sizeof(fsbuffer));
			fread(fsbuffer,1,fsSize,fs);
			// Sending file
			nbytes = sendto(sock, fsbuffer, sizeof(fsbuffer), 0, (struct sockaddr *)&remote, sizeof(remote));
			fclose(fs);
		}
		else if(strncmp(buffer,"LIST",4)==0)
		{
			FILE *fp = popen("ls","r");
			char lsString[1024];
			bzero(buffer,sizeof(buffer));
			while(fgets(lsString,1024,fp))
			{
				strcat(buffer,lsString);
			}
			nbytes = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&remote, sizeof(remote));
		}
		else if(strncmp(buffer,"EXIT",4)==0)
		{
			exit(1);
		}
		else
		{
			strcat(buffer," not understood");
			nbytes = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&remote, sizeof(remote));
		}
	}
	close(sock);
}
