#include<netinet/in.h>                         // for sockaddr_in  
#include<sys/types.h>                          // for socket  
#include<sys/socket.h>                         // for socket  
#include<stdio.h>                              // for printf  
#include<stdlib.h>                             // for exit  
#include<string.h>                             // for bzero  
#define HELLO_WORLD_SERVER_PORT       6666  
#define BUFFER_SIZE                   1024  
#define FILE_NAME_MAX_SIZE            512
#include <arpa/inet.h>
#include <unistd.h> // for close
int main(int argc, char **argv)  
{  
if (argc != 2)  
{  
printf("Usage: ./%s ServerIPAddress\n", argv[0]);  
exit(1);  
}  
// 設定一個socket地址結構client_addr, 代表客戶機的internet地址和埠  
struct sockaddr_in client_addr;  
bzero(&client_addr, sizeof(client_addr));  
client_addr.sin_family = AF_INET; // internet協議族  
client_addr.sin_addr.s_addr = htons(INADDR_ANY); // INADDR_ANY表示自動獲取本機地址  
client_addr.sin_port = htons(0); // auto allocated, 讓系統自動分配一個空閒埠  
// 建立用於internet的流協議(TCP)型別socket，用client_socket代表客戶端socket  
int client_socket = socket(AF_INET, SOCK_STREAM, 0);  
if (client_socket < 0)  
{  
printf("Create Socket Failed!\n");  
exit(1);  
}  
// 把客戶端的socket和客戶端的socket地址結構繫結   
if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))  
{  
printf("Client Bind Port Failed!\n");  
exit(1);  
}  
// 設定一個socket地址結構server_addr,代表伺服器的internet地址和埠  
struct sockaddr_in  server_addr;  
bzero(&server_addr, sizeof(server_addr));  
server_addr.sin_family = AF_INET;  
// 伺服器的IP地址來自程式的引數   
if (inet_aton(argv[1], &server_addr.sin_addr) == 0)  
{  
printf("Server IP Address Error!\n");  
exit(1);  
}  
server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);  
socklen_t server_addr_length = sizeof(server_addr);  
// 向伺服器發起連線請求，連線成功後client_socket代表客戶端和伺服器端的一個socket連線  
if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)  
{  
printf("Can Not Connect To %s!\n", argv[1]);  
exit(1);  
}
char file_name[FILE_NAME_MAX_SIZE+1];
bzero(file_name,sizeof(file_name));

char account[FILE_NAME_MAX_SIZE +1];  
bzero(account, sizeof(account));
char pw[FILE_NAME_MAX_SIZE+1];
bzero(pw,sizeof(pw));
char ap[FILE_NAME_MAX_SIZE+1];
bzero(ap,sizeof(ap));

printf("Type account:");  
scanf("%[^\n]",account);
printf("x%sx\n",account);
strcpy(ap,account);
strcat(ap," ");
getchar();
printf("Type password:");
scanf("%[^\n]",pw);
printf("x%sx\n",pw);
strcat(ap,pw);
printf(">>>%s$\n",ap);

/*
// process token
int cnt = 0;
char *delim = " ";
char *pch;
pch = strtok(file_name,delim);
cnt++;
while(pch!=NULL)
{
	if(cnt==2)
	{
		strcpy(file_name,pch);
	}
	pch = strtok(NULL,delim);
	cnt++;
}
*/

//gets(file_name);
char buffer[BUFFER_SIZE];  
bzero(buffer, sizeof(buffer));
char tmp[BUFFER_SIZE];
bzero(tmp,sizeof(tmp));

strcpy(buffer,ap);
//strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));
  
// 向伺服器傳送buffer中的資料，此時buffer中存放的是客戶端需要接收的檔案的名字  
send(client_socket, buffer, BUFFER_SIZE, 0);
bzero(buffer,sizeof(buffer));

/*
char str[] = "./client_side/";
strcat(str,file_name);
FILE *fp = fopen(str, "w");
if (fp == NULL)  
{  
printf("File:\t%s Can Not Open To Write!\n", file_name);  
exit(1);  
}  
// 從伺服器端接收資料到buffer中   
bzero(buffer, sizeof(buffer));  
int length = 0;
int empty =1;
while(length = recv(client_socket, buffer, BUFFER_SIZE, 0))  
{
//printf("buffer %s\n",buffer);      	
empty = 0;	
if (length < 0)  
{  
printf("Recieve Data From Server %s Failed!\n", argv[1]);  
break;  
}


int write_length = fwrite(buffer, sizeof(char), length, fp);  
if (write_length < length)  
{  
printf("File:\t%s Write Failed!\n", file_name);  
break;  
}  
bzero(buffer, BUFFER_SIZE);  
} // end recv 
if(empty==1) { printf("File not on Server\n"); remove(str);}
if(empty==0){
printf("Recieve File:\t %s From Server[%s] Finished!\n", str, argv[1]);
}
// 傳輸完畢，關閉socket   
fclose(fp);
*/
printf("Wait for Server...\n");
recv(client_socket,buffer,BUFFER_SIZE,0);
printf("Receive from Server:%s\n",buffer);
int status = 0;
if(strcmp(buffer,"Pass!")==0)
{
	status = 1;
}
bzero(buffer,sizeof(buffer));
if(status==1)
{
	getchar();
	printf("Type command:");
	scanf("%[^\n]",buffer);
	strcpy(tmp,buffer);
	char action[BUFFER_SIZE];
	bzero(action,BUFFER_SIZE);

	// process token
	int cnt = 0;
	char *delim = " ";
	char *pch;
	pch = strtok(buffer,delim);
	cnt++;
	while(pch!=NULL)
	{
		if(cnt==1)
		{
			strcpy(action,pch);
		}
        	if(cnt==2)
        	{
                	strcpy(file_name,pch);
        	}
        	pch = strtok(NULL,delim);
        	cnt++;
	}
//	printf("testqq:%s\n",tmp);
	send(client_socket,tmp,BUFFER_SIZE,0);
	bzero(buffer,sizeof(buffer));
	bzero(tmp,sizeof(tmp));
	if(strcmp(action,"Write")==0)
	{
		recv(client_socket,buffer,BUFFER_SIZE,0);
		printf("recv:%s\n",buffer);
		bzero(buffer,sizeof(buffer));
		// write //
		char str[] = "./client_side/";
		strcat(str,file_name);
		FILE *fp = fopen(str,"r");
		if(fp==NULL)
		{
			printf("File not found\n");
		}
		else
		{
			bzero(buffer,BUFFER_SIZE);
			int file_block_length = 0;
			while( (file_block_length = fread(buffer,sizeof(char),BUFFER_SIZE,fp))>0)
			{
				printf("block length=%d\n",file_block_length);
				if(send(client_socket,buffer,file_block_length,0)<0)
				{
					printf("Send error.\n");
					break;
				}
				bzero(buffer,sizeof(buffer));
			}
			fclose(fp);
			printf("Success Write!\n");
		}
	}	
	else if(strcmp(action,"Read")==0)
	{
       		 
			char str[] = "./client_side/";
			strcat(str,file_name);
			printf("testqq\n");
			printf("fn:%s\n",str);
			FILE *fp = fopen(str, "w");
			if (fp == NULL)
			{
				printf("File:\t%s Can Not Open To Write!\n", file_name);
				exit(1);
			}
			// 從伺服器端接收資料到buffer中
			bzero(buffer, sizeof(buffer));
			int length = 0;
			int empty =1;
			while(length = recv(client_socket, buffer, BUFFER_SIZE, 0))
			{
				//printf("buffer %s\n",buffer);
				empty = 0;
				if (length < 0)
				{
					printf("Recieve Data From Server %s Failed!\n", argv[1]);
					break;
			}
			int write_length = fwrite(buffer, sizeof(char), length, fp);
			if (write_length < length)
			{
				printf("File:\t%s Write Failed!\n", file_name);
				break;
			}
			bzero(buffer, BUFFER_SIZE);
			} // end recv
			if(empty==1) { printf("File not on Server\n"); remove(str);}
			if(empty==0){
				printf("Recieve File:\t %s From Server[%s] Finished!\n", str, argv[1]);
			}
			// 傳輸完畢，關閉socket
			fclose(fp);
	
	}
}
close(client_socket);  
return 0;  
}  
