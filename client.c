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
#include <errno.h>
#include <string.h>

#define MAXBUFSIZE 5120

int main (int argc, char * argv[])
{

	int nbytes,l;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[MAXBUFSIZE];
	char cli_choice[10];					// Client's command choice
	char fname[100],length[100];						// File name		
	struct sockaddr_in remote;              //"Internet socket address structure"
	char * fsbuffer; long fsSize;
	socklen_t r;

	if (argc < 3)
	{
		printf("USAGE:  <server_ip:%s> <server_port:%s>\n",argv[1],argv[2]);
		exit(1);
	}

	/******************
	  Here we populate a sockaddr_in struct with
	  information regarding where we'd like to send our packet 
	  i.e the Server.
	 ******************/
	bzero(&remote,sizeof(remote));               //zero the struct
	remote.sin_family = AF_INET;                 //address family
	remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
	remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address

	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("unable to create socket");
	}

	/******************
	  sendto() sends immediately.  
	  it will report an error if the message fails to leave the computer
	  however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
	 ******************/
	 while(1)
	 {
	 	printf("PUTF - Put a file\n");
	 	printf("GETF - Get a file\n");
	 	printf("LIST - List all files\n");
	 	printf("EXIT - Exit the server\n");
	 	printf("Enter a command:");
	 	scanf("%s", cli_choice);
	 	
	 	if(strcmp(cli_choice,"PUTF")==0)
	 	{
			bzero(buffer,sizeof(buffer));
			strcpy(buffer, "PUTF");
			printf("Which file to put?");
			scanf("%s",fname);
			strcat(buffer, fname);
			// Sending command and filename
			nbytes = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&remote, sizeof(remote));
			FILE *fs = fopen(fname,"r");
			fseek (fs,0,SEEK_END);
			fsSize = ftell(fs);
			rewind(fs);
			// Sending size of file
			sprintf(length,"%ld", fsSize);
			nbytes = sendto(sock, length, strlen(length), 0, (struct sockaddr *)&remote, sizeof(remote));
			// Sending file
			fsbuffer = (char*) malloc (sizeof(char)*fsSize);
			bzero(fsbuffer,sizeof(fsbuffer));
			fread(fsbuffer,1,fsSize,fs);
			nbytes = sendto(sock, fsbuffer, sizeof(fsbuffer), 0, (struct sockaddr *)&remote, sizeof(remote));
			if(nbytes>0)
			{
				printf("File Sent\n");			
			}
			fclose(fs);
	 	}
	 	else if(strcmp(cli_choice,"GETF")==0)
	 	{
			bzero(buffer,sizeof(buffer));
			strcpy(buffer, "GETF");
			printf("Which file to get?");
			scanf("%s",fname);
			strcat(buffer, fname);
			// Sending filename and command
			nbytes = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&remote, sizeof(remote));
			// Receiving length
			nbytes = recvfrom(sock, length, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &r);
			l=atoi(length);
			//Receiving file
			bzero(buffer,sizeof(buffer));
			r = sizeof(remote);
			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &r);
			FILE *fr = fopen(fname,"w+");
			fwrite(buffer,sizeof(char),l,fr);
			fclose(fr);
			if(nbytes>0)
			{
				printf("File Received\n");			
			}
	 	}
	 	else if(strcmp(cli_choice,"LIST")==0)
	 	{
			bzero(buffer,sizeof(buffer));
			strcpy(buffer, "LIST");
			nbytes = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&remote, sizeof(remote));
			// Receiving ls result from the server
			bzero(buffer,sizeof(buffer));
			r = sizeof(remote);
			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &r);
			if(strcmp(buffer,"")==0)
			{
				printf("Server says no files\n");
			}
			else
			{
				printf("Server says: %s\n", buffer);
			}
	 	}
	 	else if(strcmp(cli_choice,"EXIT")==0)
	 	{
			bzero(buffer,sizeof(buffer));
			strcpy(buffer, "EXIT");
			nbytes = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&remote, sizeof(remote));
			exit(1);
	 	}
	 	else
	 	{
	 		bzero(buffer,sizeof(buffer));
	 		strcpy(buffer, cli_choice);
	 		nbytes = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&remote, sizeof(remote));
	 		bzero(buffer,sizeof(buffer));
			r = sizeof(remote);
			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *)&remote, &r);  
			printf("Server says %s\n", buffer);
	 	}
	 }

	close(sock);
}

