#include<netinet/in.h>   
#include<sys/types.h>   
#include<sys/socket.h>   
#include<stdio.h>   
#include<stdlib.h>   
#include<string.h>
#include <unistd.h> // for close 
#include <arpa/inet.h>
#include <time.h>   
#define HELLO_WORLD_SERVER_PORT    6666 
#define LENGTH_OF_LISTEN_QUEUE     20  
#define BUFFER_SIZE                1024  
#define FILE_NAME_MAX_SIZE         512

typedef struct
{
	char per[BUFFER_SIZE];
	char owner[BUFFER_SIZE];
	char group[BUFFER_SIZE];
	char file_size[BUFFER_SIZE];
	char month[BUFFER_SIZE];
	char day[BUFFER_SIZE];
	char year[BUFFER_SIZE];
	char filename[BUFFER_SIZE];
}cp[100];
int status = 0; // 紀錄是否登入 

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
        socklen_t length = sizeof(client_addr);  
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
        char file_name[BUFFER_SIZE];  
        bzero(file_name, sizeof(file_name));
        printf("Receive from client %s\n",buffer);
        // strncpy(file_name, buffer,  strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));

        char account[BUFFER_SIZE];
        bzero(account,sizeof(account));

        char pw[BUFFER_SIZE];
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
            
            pch = strtok(NULL,delim);
            cnt++;
        }
        bzero(buffer,sizeof(buffer));
        // Login //
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
            printf("Command:%s\n",buffer);
            char action[BUFFER_SIZE];
            bzero(action,sizeof(action));
            char per[BUFFER_SIZE]; // permission rwx
            bzero(per,sizeof(per));

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
                    if(strcmp(pch,"o")==0) strcpy(per,"w"); // overwrite
                    if(strcmp(pch,"a")==0) strcpy(per,"a"); // append
                    if(strlen(pch)==9)
                    {      
                        strcpy(per,pch);		
                    }
                }
                
                pch = strtok(NULL,delim);
                cnt++;
            }

            if(strcmp(action,"Write")==0)
            {
                printf("send ok...");
                char msg[]="ok";
                send(new_server_socket,msg,sizeof(msg),0);
                char dir[BUFFER_SIZE] = "./server_side/";
                strcat(dir,file_name);
                FILE *fp = fopen(dir,per);
                if(fp==NULL)
                {
                    printf("Open Error.\n");
                    exit(1);
                }
		
                int length = 0;
                int empty = 1;
                bzero(buffer,sizeof(buffer));
                while(length=recv(new_server_socket,buffer,BUFFER_SIZE,0))
                {
                    empty = 0;
                    /*
                    if(length<0) // I think it always not run this if 
                    {
                        printf("Receive error.\n");
                        break;
                    }
                    */
                    int write_length = fwrite(buffer,sizeof(char),length,fp);
                    if(write_length<length)
                    {
                        printf("Write Error.\n");
                        break;
                    }
                    bzero(buffer,BUFFER_SIZE);
                }
                if(empty==1) { printf("File is not exist\n"); remove(dir);}
                if(empty==0) 
                {
                    printf("Receive file:%s from client",file_name);
                }
                fclose(fp);
            }
            else if(strcmp(action,"Read")==0)
            {		
                char tmp[BUFFER_SIZE] = "./server_side/";
                strcat(tmp,file_name);
                
                FILE *fp = fopen(tmp, "r");  
                if (fp == NULL)  
                {  
                    printf("File:%s Not Found!\n", tmp);  
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
                    printf("File:%s Transfer Finished!\n", tmp);  
                }
		
            }
            else if(strcmp(action,"Create")==0)
            {
                // 傳送 "TYPE" => 觸發 client 開始下階段傳送 => 並等待client傳資料來
                bzero(buffer,sizeof(buffer));
                strcpy(buffer,"TYPE");
                send(new_server_socket,buffer,sizeof(buffer),0);
                bzero(buffer,sizeof(buffer));
                
                
                char dir[BUFFER_SIZE] = "./server_side/";  
                int len = 0;
                len = recv(new_server_socket,buffer,BUFFER_SIZE,0); // buffer -> context
                
                if(len<0) {printf("Receive Error.\n");}
                else
                {
                    strcat(dir,file_name);
                    FILE *fp;
                    fp = fopen(dir,"w");

                    if(strcmp(buffer,"")==0)
                    {
                        printf("NO CONTEXT\n");
                    }
                    else
                    {    
                        fprintf(fp,"%s",buffer);
                        fclose(fp);
                    }    
                }

                time_t T = time(NULL);
                struct tm tm = *localtime(&T);
                
                printf("%s %s %s %d %d/%d/%d %s\n",per,account,pw,len,tm.tm_mon+1,tm.tm_mday,tm.tm_year+1900,file_name);
		FILE *fp_cp = fopen("./data/cp.txt","a");
		if(fp_cp==NULL)
		{
			printf("Open Failure.");
		}
		else
		{
			fprintf(fp_cp,"%s %s %s %d %d/%d/%d %s\n",per,account,pw,len,tm.tm_mon+1,tm.tm_mday,tm.tm_year+1900,file_name);
		}
		fclose(fp_cp);


            }
            else if(strcmp(action,"Changemode")==0)
	    {
		    FILE *fp_cg;
		    fp_cg = fopen("./data/cp.txt","r");
		    FILE *fp_new = fopen("./data/cp_temp.txt","w");

		    char buf[BUFFER_SIZE];
		    int line = 0;
		    cp *save_cp;
		    bzero(buf,sizeof(buf));
            cp cpList;
		    while(fgets(buf,sizeof(buf),fp_cg)!=NULL)
		    {
			char temp_buf[BUFFER_SIZE];
		        char per_[BUFFER_SIZE];
	        	char owner[BUFFER_SIZE];
		        char gp[BUFFER_SIZE];
		        int fs; // file size
		  	int mon;
			int day;
			int year;
			char fn[BUFFER_SIZE];
			//cp cp_list[BUFFER_SIZE];
              //          cp cpList;
			printf("buffer %s",buf);
			//fscanf(fp_cg,"%s",per_);
		//	buf[strlen(buf)-1] = '\0';
			printf("len:%ld\n",strlen(buf));
		//	printf("bless:%s\n",per_);
		//	fscanf(fp_cg,"%s %s %s %d %d %d %d %s",per_,owner,gp,&fs,&mon,&day,&year,fn);
			// printf("check ...per:%s\n",per_);
		//	printf("buf:%s\n",buf);
	//
	//strcpy(per_,buf);
	//strcpy(per_,"A B C");
			strcpy(temp_buf,buf);
			int ct = 0;
			char *delim=" ";
			char *pch;
			pch = strtok(temp_buf,delim);
			ct++;
			//printf("pch:%s\n",pch);
			while(pch!=NULL)
			{
				if(ct==1) strcpy(cpList[line].per,pch);
				if(ct==6) {
					strcpy(fn,pch);
					fn[strlen(fn)-1] = '\0';
					if(strcmp(fn,file_name)==0)
					{
						printf("bingo!\n");
						strcpy(cpList[line].per,per);
					}
				}	
				printf("pch:%s\n",pch);
				
				pch = strtok(NULL,delim);
				ct++;
			}
			printf(">>%s\n",cpList[line].per);
			printf(">>>%s\n",fn);
			line++;
		//	printf("per_:%s\n",per_);
			/*
			int cnt = 0;
			char *delim = " ";
			char *pch;
			pch = strtok(per_,delim);
			cnt++;
			while(*pch!='\n')
			{
				if(cnt==1)
				{
					printf("only per:%s\n",pch);
				}
				cnt++;
				pch = strtok(NULL,delim);
			}*/
		//	bzero(buf,sizeof(buf));
			/*
			if(strcmp(file_name,fn)==0)
			{
				save_cp[line].per =  per;
				save_cp[line].owner = owner;
				save_cp[line].group = gp;
				save_cp[line].file_size = fs;
				save_cp[line].month = mon;
			        save_cp[line].day = day;
				save_cp[line].year = year;
				save_cp[line].filename = fn;
			}
			else{
                                save_cp[line].per =  per_;
                                save_cp[line].owner = owner;
                                save_cp[line].group = gp;
                                save_cp[line].file_size = fs;
                                save_cp[line].month = mon;
                                save_cp[line].day = day;
                                save_cp[line].year = year;
                                save_cp[line].filename = fn;
			}
			printf("buffer:%s\n",buf);
			line++;
		    }
		    printf("line數:%d\n",line);
		    for(int i=0;i<line;i++)
		    {
		    fprintf(fp_new,"%s %s %s %d %d %d %d %s",save_cp[i].per,save_cp[i].owner,save_cp[i].group,save_cp[i].file_size,save_cp[i].month,save_cp[i].day,save_cp[i].year,save_cp[i].filename);}

		    fclose(fp_cg);
		    fclose(fp_new);
		    remove("./data/cp.txt");
		    rename("./data/cp_temp.txt","./data/cp.txt");
	    }	   */
		}
       // FILE *fp_new=fopen("./data/cp_go.txt","w");

        for(int i=0;i<line;i++)
        {
            fprintf(fp_new,"%s\n",cpList[i].per);
        }
        fclose(fp_new);
	fclose(fp_cg);
	remove("./data/cp.txt");
	 rename("./data/cp_temp.txt","./data/cp.txt");  
        
	}	
    } // End if Status == 1 : 登入狀態        	
	close(new_server_socket);
  
    } // end while  
    close(server_socket);  
    return 0;  
}  
