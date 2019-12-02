#include<netinet/in.h>   
#include<sys/types.h>   
#include<sys/socket.h>   
#include<stdio.h>   
#include<stdlib.h>   
#include<string.h>   
#define HELLO_WORLD_SERVER_PORT    6666 
#define LENGTH_OF_LISTEN_QUEUE     20  
#define BUFFER_SIZE                1024  
#define FILE_NAME_MAX_SIZE         512
#include <unistd.h> // for close 
#include <arpa/inet.h>

int status = 0;

int main(int argc, char **argv)  
{  
// set socket's address information   
// 設定一個socket地址結構server_addr,代表伺服器internet的地址和埠  
struct sockaddr_in   server_addr;  
bzero(&server_addr, sizeof(server_addr));  
server_addr.sin_family = AF_INET;  
server_addr.sin_addr.s_addr = htons(INADDR_ANY);  
server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);  
// create a stream socket   
// 建立用於internet的流協議(TCP)socket，用server_socket代表伺服器向客戶端提供服務的介面  
int server_socket = socket(PF_INET, SOCK_STREAM, 0);  
if (server_socket < 0)  
{  
printf("Create Socket Failed!\n");  
exit(1);  
}  
// 把socket和socket地址結構繫結   
if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))  
{  
printf("Server Bind Port: %d Failed!\n", HELLO_WORLD_SERVER_PORT);  
exit(1);  
}  
// server_socket用於監聽   
if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))  
{  
printf("Server Listen Failed!\n");  
exit(1);  
}  
// 伺服器端一直執行用以持續為客戶端提供服務   
while(1)  
{  
// 定義客戶端的socket地址結構client_addr，當收到來自客戶端的請求後，呼叫accept  
// 接受此請求，同時將client端的地址和埠等資訊寫入client_addr中  
struct sockaddr_in client_addr;  
socklen_t          length = sizeof(client_addr);  
// 接受一個從client端到達server端的連線請求,將客戶端的資訊儲存在client_addr中  
// 如果沒有連線請求，則一直等待直到有連線請求為止，這是accept函式的特性，可以  
// 用select()來實現超時檢測   
// accpet返回一個新的socket,這個socket用來與此次連線到server的client進行通訊  
// 這裡的new_server_socket代表了這個通訊通道  
int new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
fprintf(stderr,"Connection established with %s:%d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
if (new_server_socket < 0)  
{  
printf("Server Accept Failed!\n");  
break;  
}  
char buffer[BUFFER_SIZE];  
bzero(buffer, sizeof(buffer));  
length = recv(new_server_socket, buffer, BUFFER_SIZE, 0);  
if (length < 0)  
{  
printf("Server Recieve Data Failed!\n");  
break;  
}  
char file_name[FILE_NAME_MAX_SIZE+1];  
bzero(file_name, sizeof(file_name));
printf("Receive from client %s\n",buffer);
strncpy(file_name, buffer,  
strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));

char account[FILE_NAME_MAX_SIZE+1];
bzero(account,sizeof(account));

char pw[FILE_NAME_MAX_SIZE+1];
bzero(pw,sizeof(account));

// process strtok //
int cnt = 0;
char *delim =" ";
char *pch;
pch = strtok(buffer,delim);
cnt++;
while(pch!=NULL)
{
	if(cnt==1)
	{
		printf("account:%s\n",pch);
		strcpy(account,pch);
	}
	if(cnt==2)
	{
		printf("pw:%s\n",pch);
		strcpy(pw,pch);
	}
	printf("token:%s\n",pch);
	pch = strtok(NULL,delim);
	cnt++;
}
bzero(buffer,sizeof(buffer));

if(strcmp(account,"sundar")==0)
{
	if(strcmp(pw,"m083040017")==0)
	{
		strcpy(buffer,"Pass!");
		send(new_server_socket,buffer,sizeof(buffer),0);
		status = 1;
	}
	else
	{
		strcpy(buffer,"Reject!!");
		send(new_server_socket,buffer,sizeof(buffer),0);
		status = 0;
	}
}
else
{
	strcpy(buffer,"Reject!!");
	send(new_server_socket,buffer,sizeof(buffer),0);
	status = 0;
}
bzero(buffer,sizeof(buffer));
if(status==1)
{
	printf("Receive command...\n");
	recv(new_server_socket,buffer,BUFFER_SIZE,0);
	printf("%s\n",buffer);
	char action[FILE_NAME_MAX_SIZE+1];
	bzero(action,sizeof(action));
	char ch[2];

        // file name 
	// process strtok //
	int cnt = 0;
	char *delim =" ";
	char *pch;
	pch = strtok(buffer,delim);
	cnt++;

	while(pch!=NULL)
	{
		
       	       	if(cnt==1)
        	{
                	printf("action:%s\n",pch);
 			strcpy(action,pch);               	
        	}
		
        	if(cnt==2)
       		{
			printf("filename:%s\n",pch);
                	strcpy(file_name,pch);
        	}
		if(cnt==3)
		{
			printf("per:%s\n",pch);
			strcpy(ch,pch);
		}
        	printf("token:%s\n",pch);
        	pch = strtok(NULL,delim);
        	cnt++;
	}
	printf("ch:%s\n",ch);
	if(strcmp(action,"Write")==0)
	{
		printf("send ok...");
		char msg[]="ok";
		send(new_server_socket,msg,sizeof(msg),0);
		char dir[] = "./server_side/";
		strcat(dir,file_name);
		//FILE *fp;
		/*
		if(strcmp(ch,"a")==0)
		{
			*fp = fopen(dir,"a"); // append
		}
		else if(strcmp(ch,"o")==0)
		{
			*fp = fopen(dir,"w"); 
		}
		*/
		FILE *fp = fopen(dir,ch);

		if(fp==NULL)
		{
			printf("Open  error.\n");
			exit(1);
		}
		bzero(buffer,sizeof(buffer));
		int length = 0;
		int empty = 1;
		while(length=recv(new_server_socket,buffer,BUFFER_SIZE,0))
		{
			empty = 0;
			if(length<0)
			{
				printf("Receive error.\n");
				break;
			}
			int write_length = fwrite(buffer,sizeof(char),length,fp);
			if(write_length<length)
			{
				printf("Write error.");
				break;
			}
			bzero(buffer,BUFFER_SIZE);
		}
		if(empty==1) { printf("File is not exist\n"); remove(dir);}
		if(empty==0) 
		{
			printf("receive file:%s from client",file_name);
		}
		fclose(fp);
	}	
        /*
	char tmp[] = "./server_side/";
	strcat(tmp,file_name);
	printf("%s\n",tmp);
	FILE *fp = fopen(tmp, "r");  
	if (fp == NULL)  
	{  
		printf("File:\t%s Not Found!\n", tmp);  
	}  
	else  
	{  
		bzero(buffer, BUFFER_SIZE);  
		int file_block_length = 0;  
		while( (file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)  
		{  
			printf("file_block_length = %d\n", file_block_length);  
			// 傳送buffer中的字串到new_server_socket,實際上就是傳送給客戶端  
			if (send(new_server_socket, buffer, file_block_length, 0) < 0)  
			{  
				printf("Send File:\t%s Failed!\n", file_name);  
				break;  
			}  
			bzero(buffer, sizeof(buffer));  
		}  
		fclose(fp);  
		printf("File:\t%s Transfer Finished!\n", tmp);  
	}
	*/
}      	
	close(new_server_socket);
 // tranfer file 
} // end while?  
close(server_socket);  
return 0;  
}  
