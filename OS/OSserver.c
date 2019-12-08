#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdbool.h>
#include<sys/stat.h>

void dostuff(int);
int checkaccount(char*);
void newfile(char *filename, char *permission, char *account, int group);
void read_file(char *filename, char *content);
void write_file(char *filename, char *content, char *detail);
void changeper(char *filename, char *detail, char *account);
void information(char *filename, char *respon);
bool checkper(char *filename, int group, char *behave);
void closeuse(char *filename, char *behave);
bool checkusage(char *filename, char *behave);
void setuse(char *filename, char *behave);

//顯示錯誤訊息
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

bool quit = false;

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno,pid;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	if (argc < 2) 
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
	
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	
	while(1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
		if (newsockfd < 0) 
			error("ERROR on accept");
     		pid = fork();

		if(pid < 0)
			error("ERROR on fork");

		if(pid == 0 && !quit)
		{
			close(sockfd);
			dostuff(newsockfd);
			exit(0);
		}
     		else close(newsockfd);
	}
     	close(sockfd);
	return 0; 
}

void dostuff(int newsockfd)
{
	bool login = false;
	int usergroup = 0;
	char name[50];
	bzero(name, 50);
	while(1)
	{
		char content[2000];
		bzero(content, 2000);
		int n;
		char buffer[256];
		bzero(buffer, 256);
		char command_list[][20] = {"new", "read", "write", "change", "information", "quit"};
	    char command[3][100];
		for(int x = 0; x < 3; x++)
        {
            bzero(command[x], 100);
        }

		if(!login)
		{
	       	n = read(newsockfd, buffer, 255);
			printf("%s\n", buffer);
			usergroup  = checkaccount(buffer);
			//printf("%d", usergroup);
			if(usergroup == 1)
			{
				printf("%s is %s", buffer, "AOS_group");
				//n = write(newsockfd, "hello_AOS", 9);
				n = write(newsockfd, "success", 8);
				strcpy(name, buffer);
				login = true;
			}
			else if(usergroup == 2)
			{
				printf("%s is %s", buffer, "CSE_group");
				//n = write(newsockfd, "hello_CSE", 10);
				n = write(newsockfd, "success", 8);
				strcpy(name, buffer);
				login = true;
			}
			else if(usergroup == 3)
			{
				printf("%s is %s", buffer, "other_group");
				//n = write(newsockfd, "hello_other", 12);
				n = write(newsockfd, "success", 8);
				strcpy(name, buffer);
				login = true;
			}
			else
			{
				printf("%s", buffer);
				printf("unknown user");
				n = write(newsockfd, "unknown_user", 13);
			}
			//printf("%d", usergroup);
		}
		else if(login)
		{
			printf("get int\n");
			n = read(newsockfd, buffer, 255);
			printf("%s", buffer);
			int j = 0, z = 0;
			for(int i = 0; i < strlen(buffer); i++)
			{
				if(buffer[i] == ' ')
				{
					j = 0;
					z++;
				}
				else
				{
					command[z][j] = buffer[i];
					j++;
				}
			}
			
			printf("%d\n", strcmp(command[0], command_list[5]));
			
			for(int x = 0; x < 6; x++)
			{
				if(strcmp(command[0], command_list[x]) == 0)
				{
					//new
					if(x == 0)
					{
						newfile(command[1], command[2], name, usergroup);
						n = write(newsockfd, "new successed", 13);
					}
					//read
					else if(x == 1)
					{
						bzero(content, 2000);
						if(checkper(command[1], usergroup, "read") && checkusage(command[1], "read"))
						{
							setuse(command[1], "read");
							read_file(command[1], content);
							n = write(newsockfd, content, strlen(content));
						}
						else if(!checkper(command[1], usergroup, "read") && checkusage(command[1], "read"))
						{
							n = write(newsockfd, "no permission", 14);
						}
						else if(checkper(command[1], usergroup, "read") && !checkusage(command[1], "read"))
						{
							n = write(newsockfd, "File is being accessed", 23);
						}
						else
						{
							n = write(newsockfd, "no permission and being used", 29);
						}
						n = read(newsockfd, buffer, 255);
						closeuse(command[1], "read");
					
						printf("%s", buffer);
					}
					//write
					else if(x == 2)
					{
						bzero(content, 2000);
						if(checkper(command[1], usergroup, "write") && checkusage(command[1], "write"))
						{
							n = write(newsockfd, "ok", 2);
							setuse(command[1], "write");
							n = read(newsockfd, content, 2000);
							printf("%s", content);
							write_file(command[1], content, command[2]);
							closeuse(command[1], "write");
							printf("write_done");
						}
						else if(!checkper(command[1], usergroup, "write") && checkusage(command[1], "write"))
							n = write(newsockfd, "no permission", 14);
						else if(checkper(command[1], usergroup, "write") && !checkusage(command[1], "write"))
							n = write(newsockfd, "File is being accessed", 23);
						else
							n = write(newsockfd, "no permission and being used", 29);
						
						
					}
					//change
					else if(x == 3)
					{
						changeper(command[1], command[2], name);
						n = write(newsockfd, "change successed", 16);
					}
					//information
					else if(x == 4)
					{
						information(command[1], content);
						n = write(newsockfd, content, strlen(content));
					}
					//quit
					else if(x == 5)
					{
						printf("quit being");
						n = write(newsockfd, "Connection close", 17);
						quit = true;
						//break;
					}
				}
			}
			
		}
		
		if(quit)
			break;

    	if (n < 0) 
			error("ERROR writing to socket");


	}
}

//確認登入帳號
//從account中比對				
int checkaccount(char* account)
{
	FILE *fp;
	char ch;
	int response = 0;
	char name[20];
	char temp[20];
	bzero(name, 20);
	bzero(temp, 20);
	int i = 0;
	fp = fopen("account/account_list", "r");

	if(fp == NULL)
	{
		printf("Can't find account file");
		fclose(fp);
		return response;
	}
	else
	{
		while((ch=getc(fp)) != EOF)
		{
			if(ch == ' ')
			{
				i = 0;
				//printf("%s\n", name);
				if(strcmp(name, account) == 0)
				{
					//printf("%s\n", name);
					while((ch=getc(fp)) != '\n')
					{
						temp[i] = ch;
						//printf("%s\n", temp);
						i++;
					}
					if(strcmp(temp, "AOS_group") == 0)
					{
						fclose(fp);
						return 1;
					}
					else if(strcmp(temp, "CSE_group") == 0)
					{
						fclose(fp);
						return 2;
					}
					else if(strcmp(temp, "other_group") == 0)
					{
						fclose(fp);
						return 3;
					}	
				}
				else
				{
					//printf("%s", name);
					bzero(name, 20);
				}
			}
			else if(ch == '\n')
			{
				//printf("ss %s\n", name);
				bzero(name, 20);
				i = 0;
			}
			else
			{
				name[i] = ch;
				//printf("append %c, %d\n", ch, i);
				i++;
			}
			//printf("%c", ch);
		}
	}
	fclose(fp);
	return response;
}

void newfile(char *filename, char *permission, char *account, int group)
{
	//創建新檔案
	FILE *outfp;
	printf("%s %s %s %d\n", filename, permission, account, group);
	
	//增加file/資料夾位置
	char temp[50];
	bzero(temp, 50);
	strcpy(temp,"./file/");
	//printf("%s", temp);
	//strcpy(temp, "file/");
	int j = 0;
	for(int i = 7; i < (strlen(filename) + 7); i++)
	{
		temp[i] = filename[j];
		//printf("%s\n", temp);
		j++;
	}
	printf("%s\n", temp);
	
	//新增檔案
	outfp = fopen(temp, "w");
	//關閉fp
	fclose(outfp);	

	//修改filelist--filename, creator, group, date
	FILE *listfp;

	listfp = fopen("./file/file_list", "a");

	char filedetail[200];
	bzero(filedetail, 200);
	strcat(filedetail, filename);
	strcat(filedetail, " ");
	strcat(filedetail, permission);
	strcat(filedetail, " ");
	strcat(filedetail, account);
	strcat(filedetail, " ");
	if(group == 1)
		strcat(filedetail, "AOS_group");
	else if(group == 2)
		strcat(filedetail, "CSE_group");
	else if(group == 3)
		strcat(filedetail, "other_group");
	strcat(filedetail, " ");
	struct stat st;
	stat(temp, &st);
	struct tm * timeinfo = localtime(&st.st_ctime);
	strcat(filedetail, asctime(timeinfo));
	printf("%s\n", filedetail);

	fprintf(listfp, "%s", filedetail);
	fclose(listfp);

	//根據權限修改Capability
	FILE *AOSfp, *CSEfp, *otherfp;
	
	AOSfp = fopen("./capability/AOS_group", "a");
	CSEfp = fopen("./capability/CSE_group", "a");
	otherfp = fopen("./capability/other_group", "a");

	//permission 分段
	char permission_AOS[3], permission_CSE[3], permission_other[3];
	permission_AOS[2] = '\0';
	permission_CSE[2] = '\0';
	permission_other[2] ='\0';

	permission_AOS[0] = permission[0];
	permission_AOS[1] = permission[1];
	permission_CSE[0] = permission[2];
	permission_CSE[1] = permission[3];
	permission_other[0] = permission[4];
	permission_other[1] = permission[5];

	fprintf(AOSfp, "%s %s\n", filename, permission_AOS);
	fprintf(CSEfp, "%s %s\n", filename, permission_CSE);
	fprintf(otherfp, "%s %s\n", filename, permission_other);
	
	fclose(AOSfp);
	fclose(CSEfp);
	fclose(otherfp);
	
}

void information(char *filename, char *respon)
{
	FILE *fp;
	char ch;
	char temp[50];
	bzero(temp, 50);
	strcpy(temp,"./file/");
	int j = 0;
	for(int i = 7; i < (strlen(filename) + 7); i++)
	{
		temp[i] = filename[j];
		//printf("%s\n", temp);
		j++;
	}

	char detail[200];
	bzero(detail, 200);
	fp = fopen("./file/file_list", "r");
	int i = 0;
	bool end = false;
	while(1)
	{
		ch = getc(fp);
		if(ch == ' ')
		{
			if(strcmp(detail, filename) == 0)
			{
				detail[i] = ch;
				i++;
				while(1)
				{
					ch = getc(fp);
					if(ch == '\n' || ch == EOF)
					{
						struct stat st;
						stat(temp, &st);
						int size = st.st_size;
						strcpy(respon, detail);
						char size_str[20];
						sprintf(size_str, " %d", size);
						strcat(respon, size_str);
						end = true;
						break;
					}
					else
						detail[i] = ch;
					i++;
				}
			}
			else
			{
				while(1)
				{
					ch = getc(fp);
					if(ch == '\n' || ch == EOF)
					{
						//end = true;
						bzero(detail, 200);
						break;
					}
				}
				i = 0;
			}
		}
		else if(ch == EOF)
		{
			fclose(fp);
			strcpy(respon, "Can't find file");
			break;
		}
		else
		{
			detail[i] = ch;
			//printf("%s", detail);
			i++;
		}
        //printf("%c",ch);
		if(end)
		{
			fclose(fp);
			break;
		}
	}
}

void changeper(char *filename, char *detail, char *account)
{
	FILE *flistfp;
	FILE *AOS_groupfp;
	FILE *CSE_groupfp;
	FILE *other_groupfp;
	
	flistfp = fopen("./file/file_list", "r");
	AOS_groupfp = fopen("./capability/AOS_group", "r");
	CSE_groupfp = fopen("./capability/CSE_group", "r");
	other_groupfp = fopen("./capability/other_group", "r");

	FILE *wflistfp;
	FILE *wAOS_groupfp;
	FILE *wCSE_groupfp;
	FILE *wother_groupfp;


	char buffer[2000];
	char temp_filename[50];
	char ch;
	bzero(buffer, 2000);
	bzero(temp_filename, 50);
	int i = 0, j = 0;

	while(1)
	{
		ch = getc(flistfp);
		//printf("%c", ch);
		if(ch == ' ' || ch == '\n')
		{
			buffer[i] = ch;
			i++;
			//printf("%s %s\n", filename, temp_filename);
			if(strcmp(filename, temp_filename) == 0)
			{
				for(int x = 0; x < 6; x++)
				{
					ch = getc(flistfp);
				}
				strncat(buffer, detail, 6);
				i += 6;
				bzero(temp_filename, 50);
				//printf("fuck");
	
			}
			else
			{
				//printf("%s\n", temp_filename);
				bzero(temp_filename, 50);
				j = 0;
			}	
		}
		else if(ch == EOF)
			break;
		else
		{
			buffer[i] = ch;
			//printf("%s", buffer);
			temp_filename[j] = ch;
			i++;
			j++;
		}
		//printf("%c", buffer[i-1]);
	}
	fclose(flistfp);
	wflistfp = fopen("./file/file_list", "w");
	
	printf("%s", buffer);
	fprintf(wflistfp,"%s", buffer);
	
	//permission 分段
        char permission_AOS[2], permission_CSE[2], permission_other[2];
        permission_AOS[0] = detail[0];
        permission_AOS[1] = detail[1];
        permission_CSE[0] = detail[2];
        permission_CSE[1] = detail[3];
        permission_other[0] = detail[4];
        permission_other[1] = detail[5];
	
	bzero(buffer, 2000);
	bzero(temp_filename, 50);
	i = 0;
	j = 0;
	while(1)
	{
		ch = getc(AOS_groupfp);
		//printf("%c", ch);
		if(ch == ' ' || ch == '\n')
		{
			buffer[i] = ch;
			i++;
			if(strcmp(temp_filename, filename) == 0)
			{
				for(int x = 0;x < 2; x++)
				{
					ch = getc(AOS_groupfp);
				}
				strncat(buffer, permission_AOS,2);
				//printf("%s", buffer);
				i += 2;
				bzero(temp_filename, 50);
			}
			else
			{
				bzero(temp_filename, 50);
				j = 0;
			}
		}
		else if(ch == EOF)
			break;
		else
		{
			buffer[i] = ch;
			temp_filename[j] = ch;
			i++;
			j++;
		}
		//printf("%s", buffer);
	}	
	
	fclose(AOS_groupfp);	
	wAOS_groupfp = fopen("./capability/AOS_group", "w");
	
	fprintf(wAOS_groupfp, "%s", buffer);

	bzero(buffer, 2000);
	bzero(temp_filename, 50);
	i = 0;
	j = 0;
	while(1)
        {
                ch = getc(CSE_groupfp);
                if(ch == ' ' || ch == '\n')
                {
                        buffer[i] = ch;
                        i++;
                        if(strcmp(temp_filename, filename) == 0)
                        {
                                for(int x = 0;x < 2; x++)
                                {
                                        ch = getc(CSE_groupfp);
                                }
                                strncat(buffer, permission_CSE, 2);
                                i += 2;
                                bzero(temp_filename, 50);
                        }
                        else
                        {
                                bzero(temp_filename, 50);
                                j = 0;
                        }
                }
                else if(ch == EOF)
                        break;
                else
                {
                        buffer[i] = ch;
                        temp_filename[j] = ch;
                        i++;
                        j++;
		}
	}
	
	fclose(CSE_groupfp);
	wCSE_groupfp = fopen("./capability/CSE_group", "w");
	
	fprintf(wCSE_groupfp,"%s", buffer);

	bzero(buffer, 2000);
	bzero(temp_filename, 50);
	i = 0;
	j = 0;
	while(1)
        {
                ch = getc(other_groupfp);
                if(ch == ' ' || ch == '\n')
                {
                        buffer[i] = ch;
                        i++;
                        if(strcmp(temp_filename, filename) == 0)
                        {
                                for(int x = 0;x < 2; x++)
                                {
                                        ch = getc(other_groupfp);
                                }
                                strncat(buffer, permission_other, 2);
                                i += 2;
                                bzero(temp_filename, 50);
                        }
                        else
                        {
                                bzero(temp_filename, 50);
                                j = 0;
                        }
                }
                else if(ch == EOF)
                        break;
                else
                {
                        buffer[i] = ch;
                        temp_filename[j] = ch;
                        i++;
                        j++;
		}
	}
	
	fclose(other_groupfp);
	wother_groupfp = fopen("./capability/other_group", "w");
	fprintf(wother_groupfp,"%s", buffer);

	
	fclose(wflistfp);
	fclose(wAOS_groupfp);
	fclose(wCSE_groupfp);
	fclose(wother_groupfp);
}	
//讀檔
void read_file(char *filename, char *content)
{
	char temp[50];
	char ch;
	bzero(temp, 50);
	strcpy(temp, "./file/");
	strcat(temp, filename);
	FILE *fp;

	fp = fopen(temp, "r");

	int i = 0;
	while((ch = getc(fp)) != EOF)
	{
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
	strcpy(temp, "./file/");
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

//檢查檔案使用權限 (讀、寫)
bool checkper(char *filename, int group, char *behave)
{
	FILE *fp;
	if(group == 1)
		fp = fopen("./capability/AOS_group", "r");
	else if(group == 2)
		fp = fopen("./capability/CSE_group", "r");
	else if(group == 3)
		fp = fopen("./capability/other_group", "r");

	int temp;
	if(strcmp(behave, "read") == 0)
	{
		temp = 1;
	}
	else if(strcmp(behave, "write") == 0)
	{
		temp = 2;
	}

	char temp_filename[50];
	bzero(temp_filename, 50);
	int i = 0;
	int per = 0;
	char ch;
	while(1)
	{
		ch = getc(fp);

		if(ch == ' ' || ch == '\n')
		{
			if(strcmp(temp_filename, filename) == 0)
			{
				while(1)
				{
					ch = getc(fp);
					if(ch == '\n')
					{
						break;
					}
					else if(ch == 'r')
					{
						per += 1;
					}
					else if(ch == 'w')
					{
						per += 2;
					}
				}
			}
			else
			{
				bzero(temp_filename, 50);
				i = 0;
			}
		}
		else if(ch == EOF)
			break;
		else
		{
			temp_filename[i] = ch;
			i++;
		}
	}

	if(per == temp || per == 3)
		return true;
	else
		return false;
}

//新增使用戳記
void setuse(char *filename, char *behave)
{
	FILE *fp;
	fp = fopen("usage", "a");
	
	char input[100];
	bzero(input, 100);
	
	strcpy(input, filename);
	if(strcmp(behave, "read") == 0)
		strcat(input, " r");
	else if(strcmp(behave, "write") == 0)
		strcat(input, " w");

	fprintf(fp, "%s\n", input);
	fclose(fp);
}

//清除使用戳記
void closeuse(char *filename, char *behave)
{
	FILE *fp;
    fp = fopen("usage", "r");

        char input[2000];
        bzero(input, 2000);

	char temp[100];
	bzero(temp, 100);

	strcpy(temp, filename);
        if(strcmp(behave, "read") == 0)
                strcat(temp, " r");
        else if(strcmp(behave, "write") == 0)
                strcat(temp, " w");
	
	char ch;
	int i = 0;
	while(1)
	{
		ch = getc(fp);
		if(ch == EOF)
			break;
		input[i] = ch;
		i++;
	}
	
	int z = 0, line = 0;
	for(int j = 0; j < strlen(input); j++)
	{
		if(input[j] == temp[z])
		{
			z++;
			if(z == strlen(temp))
				break;
		}
		if(input[j] == '\n')
			line++;
	}

	char new_input[2000];
	bzero(new_input, 2000);
	int now_line = 0;
	i = 0;
	for(int j = 0; j < strlen(input); j++)
        {
		if(now_line != line)
		{
			new_input[i] = input[j];
			i++;
		}
                if(input[j] == '\n')
                        now_line++;
        }

	fclose(fp);
	fp = fopen("usage", "w");
		
        fprintf(fp, "%s", new_input);
        

}

bool checkusage(char *filename, char *behave)
{
	FILE *fp;
	fp = fopen("usage", "r");
	
	char buffer[2000];
	bzero(buffer, 2000);
	char temp_filename[50];
	bzero(temp_filename, 50);
	
	char ch;
	int i = 0;

	int require;
	if(strcmp(behave, "read") == 0)
		require = 1;
	else if(strcmp(behave, "write") == 0)
		require = 2;
	
	int current = 0;

	
	while(1)
	{
		ch = getc(fp);
		if(ch == ' ' || ch == '\n')
		{
			if(strcmp(temp_filename, filename) == 0)
			{
				ch = getc(fp);
				if(ch == 'r')
					current = 1;
				else if(ch == 'w')
					current = 2;
			}
			else
			{
				bzero(temp_filename, 50);
				i = 0;
			}
		}
		else if(ch == EOF)
		{
			break;
		}
		else
		{
			temp_filename[i] = ch;
			i++;
		}
	}

	printf("%d, %d", current, require);
	
	if(current == 0)
		return true;
	else if(current == 1 && require == 1)
		return true;
	else
		return false;
		
}
