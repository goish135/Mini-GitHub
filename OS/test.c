#include<time.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<stdbool.h>

void newfile(char *filename, char *permission, char *account, int group);
void read_file(char *filename, char *content);
void write_file(char *filename, char *content, char *detail);
void changeper(char *filename, char *detail, char *account);
void information(char *filename, char *respon);
bool checkper(char *filename, int group, char *behave);
void closeuse(char *filename, char *behave);

int main() {
	char input[20];
	char command_list[][20] = {"new", "read", "write", "change", "information"};
	char command[3][100];
	char respon[50];
	char content[2000];
	char ch;
	int i = 0, j = 0, z = 0;
	while(i < 100)
	{
		for(int x = 0; x < 3; x++)
		{
			bzero(command[x], 100);
		}		
		printf("Enter your command:");
		j = 0;
		z = 0;
		//讀入指令遇到空格就換
		while(1)
		{
			ch = getchar();
			if(ch == '\n')
			{
				break;
			}
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
		printf("%s %s %s\n", command[0], command[1], command[2]);
		if(strcmp(command[0], "quit") == 0)
		{
			break;
		}
		for(int x = 0; x < 5; x++)
		{
			if(strcmp(command[0], command_list[x]) == 0)
			{
				if(x == 0)
					newfile(command[1], command[2], "JIAN", 1);
				else if(x == 1)
				{
					bzero(content, 2000);
					read_file(command[1], content);
					printf("%s", content);
				}
				else if(x == 2)
				{
					bzero(content, 2000);
					if(checkper(command[1], 1, "read"))
						printf("ok\n");
					write_file(command[1], "test input", command[2]);
				}
				else if(x == 3)
				{
					changeper(command[1], command[2], "JIAN");
				}
				else if(x == 4)
				{
					information(command[1], respon);
					printf("%s\n", respon);
				}
			}
		}
 		i++;
	}	
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
	char permission_AOS[2], permission_CSE[2], permission_other[2];
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
	
	flistfp = fopen("./file/file_list", "r+");
	AOS_groupfp = fopen("./capability/AOS_group", "r+");
	CSE_groupfp = fopen("./capability/CSE_group", "r+");
	other_groupfp = fopen("./capability/other_group", "r+");

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
	fprintf(flistfp,"%s", buffer);
	
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
	fprintf(AOS_groupfp, "%s", buffer);

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
	fprintf(CSE_groupfp,"%s", buffer);

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
	fprintf(other_groupfp,"%s", buffer);

	fclose(flistfp);
	fclose(AOS_groupfp);
	fclose(CSE_groupfp);
	fclose(other_groupfp);
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
        fp = fopen("usage", "r+");

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

		
        fprintf(fp, "%s", new_input);
        fclose(fp);

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
	
	if(current == 0)
		return true;
	else if(current == 1 && require == 1)
		return true;
	else
		return false;
		
}
