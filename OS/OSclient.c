#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdbool.h>


void write_file(char *filename, char *content, char *detail);
void read_file(char *filename, char *content);
void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	if (argc < 3) 
	{
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
	
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) 
		error("ERROR opening socket");
    	server = gethostbyname(argv[1]);

	if (server == NULL) 
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");
	printf("Welcome to the System\n");
	printf("Connection Success \n");

	bool login = false;
    
	while(1)
	{
		char ch;
		char command[3][100];
		char all_command[300];
		char content[1000];
		bool quit = false;
		bzero(content, 1000);
		bzero(all_command, 300);
		for(int x = 0; x < 3; x++)
		{
			bzero(command[x], 100);
		}
		int j = 0, z = 0;
		char account[25];
		if(!login)
		{
			bzero(buffer,256);
			printf("Enter your account:");
			scanf("%s", account);
			n = write(sockfd, account, strlen(account));
			//n = read(sockfd, buffer, 255);
			printf("%s\n", account);
			n = read(sockfd, buffer, 255);
			printf("%s\n", buffer);
			if(strcmp(buffer, "success") == 0)
				login = true;
			ch = getchar();
		}
		else if (!quit)
		{
			bzero(buffer,256);
			printf("Enter your command: ");
			while(1)
			{
				ch = getchar();
				if(ch == '\n')
					break;
				else if(ch == ' ')
				{
					z++;
					j = 0;
				}
				else
				{
					command[z][j] = ch;
					j++;
				}
			}
			strcpy(all_command, command[0]);
			strcat(all_command, " ");
			strcat(all_command, command[1]);
			strcat(all_command, " ");
			strcat(all_command, command[2]);
			//printf("%s", all_command);
			//write
			if(strcmp(command[0], "write") == 0)
			{	
				n = write(sockfd, all_command, strlen(all_command));
				n = read(sockfd, content, 1000);
				//printf("%s", content);
				if(strcmp(content, "ok") == 0)
				{
					z = 0;
					bzero(content, 1000);
					read_file(command[1], content);
					printf("%s", content);
					n = write(sockfd, content, strlen(content));
				}
				else
					printf("%s\n", content);
				
			}
			//read
			else if(strcmp(command[0], "read") == 0)
			{
				n = write(sockfd, all_command, strlen(all_command));
				n = read(sockfd, content, 1000);
				printf("%s\nenter to continue:", content);
				getchar();
				n = write(sockfd, "ok", 2);
				write_file(command[1], content, "o");
			}
			//new
			else if(strcmp(command[0], "new") == 0)
			{
				n = write(sockfd, all_command, strlen(all_command));
				n = read(sockfd, content, 1000);
				printf("%s\n", content);
			}
			//information
			else if(strcmp(command[0], "information") == 0)
			{
				n = write(sockfd, all_command, strlen(all_command));
				n = read(sockfd, content, 1000);
				printf("%s\n", content);
			}
			//changeper
			else if(strcmp(command[0], "change") == 0)
			{
				n = write(sockfd, all_command, strlen(all_command));
				n = read(sockfd, content, 1000);
				printf("%s\n", content);
			}
			//quit
			else if(strcmp(command[0], "quit") == 0)
			{
				n = write(sockfd, all_command, strlen(all_command));
				n = read(sockfd, content, 1000);
				quit = true;
				printf("%s\n", content);
				break;
			}
			else
			{
				printf("Unknown command\n");
			}
			//n = read(sockfd, buffer, 255);
		}
		else if(quit)
			break;
    		//n = read(sockfd,buffer,255);
		if (n < 0) 
		{
			error("ERROR reading from socket"); 
			break;
		}	
		/*printf("here %s\n",buffer);
		bzero(buffer, 255);*/
	}
	close(sockfd);
	return 0;
}

//讀檔
void read_file(char *filename, char *content)
{
	char temp[50];
	char ch;
	bzero(temp, 50);
	strcat(temp, filename);
	FILE *fp;

	fp = fopen(temp, "r");

	int i = 0;
	while((ch = getc(fp)) != EOF)
	{
		printf("check\n");
		content[i] = ch;
		i++;
	}

	fclose(fp);
}

//寫檔
void write_file(char *filename, char *content, char *detail)
{
	char temp[50];
	char ch;
	bzero(temp, 50);
	strcat(temp, filename);
	FILE *fp;

	if(detail[0] == 'o')
	{
		fp = fopen(temp, "w");
		fprintf(fp, "%s", content);
	}
	else if(detail[0] == 'a')
	{
		fp = fopen(temp, "a");
		fprintf(fp, "%s", content);
	}

	fclose(fp);

}
