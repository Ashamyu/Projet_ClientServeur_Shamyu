#include <stdlib.h> // for basic memmory allocation and deallocation
#include <stdio.h> // for file read and write
#include <netdb.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#define MAX 100
#define PORT 8080 
#define SA struct sockaddr
#define FIFO_FILE_1  "/tmp/client_to_server_fifo"
#define FIFO_FILE_2  "/tmp/server_to_client_fifo"

void sentFile(int sockfd, char *mystring)
{ 
	char buff[MAX]; // for read operation from file and used to sent operation 
 
	// create file 
	FILE *fp;
 	fp=fopen(mystring,"r");  // open file uses both stdio and stdin header files
	// file should be present at the program directory
	if( fp == NULL )
	{
 		printf("Error IN Opening File .. \n");
  		return ;
 	}
 
 	while ( fgets(buff,MAX,fp) != NULL ) // fgets reads upto MAX character or EOF 
  	write(sockfd,buff,sizeof(buff));  // sent the file data to stream
 
 	fclose (fp);       // close the file 
 
 	printf("File Sent successfully !!! \n"); 
}

void list(int sockfd)
{
	struct dirent *de;  // Pointer for directory entry 
  
    	// opendir() returns a pointer of DIR type.  
	

	char cwd[PATH_MAX];
  	if (getcwd(cwd, sizeof(cwd)) != NULL) 
	{
		
   		DIR *dr = opendir(cwd); 
  
    		if (dr == NULL)  // opendir returns NULL if couldn't open directory 
    		{ 
        		printf("Could not open current directory" ); 
    		}

		char buff1[MAX]; 

    		while ((de = readdir(dr)) != NULL)
		{
            		write(sockfd,de->d_name,sizeof(buff1));
  		}
    		closedir(dr);

	} 
	else 
	{
       		perror("getcwd() error");
   	}
   
}

void deletefile(int sockfd, char *mystring1) 
{
   	if (remove(mystring1) == 0)
	{
		char output[1000]= "1"; 
		printf("Deleted successfully \n");
		write(sockfd,output,sizeof(output));
	} 
   	else
	{
		char output[1000]= "2"; 
      		printf("Unable to delete the file \n");
		write(sockfd,output,sizeof(output));
	}
} 


int main() 
{
 
	int client_to_server;
    	int server_to_client;   	
	char buf[BUFSIZ];
	char buf1[1000];
	char buf2[1000];
	
    	/* create the FIFO (named pipe) */
    	mkfifo(FIFO_FILE_1, 0666);
    	mkfifo(FIFO_FILE_2, 0666);

	int sockfd, connfd, len;     // create socket file descriptor 
 	struct sockaddr_in servaddr, cli;   // create structure object of sockaddr_in for client and server// socket create and verification 
 	sockfd = socket(AF_INET, SOCK_STREAM, 0);    // creating a TCP socket ( SOCK_STREAM )
 
 	if (sockfd == -1) 
	{ 
  	printf("socket creation failed...\n"); 
  	exit(0); 
 	} 
 	else
  	printf("Socket successfully created..\n"); 
 
 
 	bzero(&servaddr, sizeof(servaddr));// assign IP, PORT 
 	servaddr.sin_family = AF_INET;     // specifies address family with IPv4 Protocol 
 	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // binds to any address
 	servaddr.sin_port = htons(PORT);     // binds to PORT specified// Binding newly created socket to given IP and verification 
 	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) 
	{ 
		printf("socket bind failed...\n"); 
  		exit(0); 
 	} 
 	else
  		printf("Socket successfully binded...\n");// Now server is ready to listen and verification 
 	
	if ((listen(sockfd, 5)) != 0) 
	{ 
  		printf("Listen failed...\n"); 
  		exit(0); 
 	} 
 	else
  		printf("Server listening...\n"); 
 
 	len = sizeof(cli); // Accept the data packet from client and verification 
 	connfd = accept(sockfd, (SA*)&cli, &len);  // accepts connection from socket
 
 	if (connfd < 0) 
	{ 
  		printf("server acccept failed...\n"); 
  		exit(0); 
 	} 
 	else
  		printf("server acccept the client...\n"); // Function for chatting between client and server 
	
	
	client_to_server = open(FIFO_FILE_1, O_RDONLY);
       	server_to_client = open(FIFO_FILE_2, O_WRONLY);

        read(client_to_server, buf, BUFSIZ);

	int temp;
	temp = *buf; //storing the decimal value pointed in the variable temp
	// char 1 = 49, 2 = 50, 3 = 51
	
	printf("\n");

	if (temp == 49)
	{
		read(client_to_server, buf1, 1000);
		printf("File to be transfered : %s \n", buf1);
		
		sentFile(connfd,buf1); // Calling the function
	
	}
	
	
	if (temp == 50) //Function to Display the content of the server directory
	{

		list(connfd);//Calling the function list to display all the files 
	}


	if (temp == 51)
	{
		read(client_to_server, buf2, 1000);
		printf("File to be deleted : %s \n", buf2);

		deletefile(connfd, buf2); //Calling the delete file function
		
	}
	

	memset(buf, 0, sizeof(buf));

	close(client_to_server);
    	close(server_to_client);

	unlink(FIFO_FILE_1);
   	unlink(FIFO_FILE_2);

	
 
}
