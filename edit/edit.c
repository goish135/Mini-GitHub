#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	char *name;
	float gpa;
}student;

int main()
{
	FILE *originalFile = fopen("./data/cp.txt","r");
	FILE *newFile = fopen("./data/cp_temp.txt","w");

	int studentCount;
	fscanf(originalFile,"%d",&studentCount);
	
	student *passingStudents = malloc(sizeof(student)*studentCount);
	int passingCount=0;
	for(int i=0;i<studentCount;i++)
	{
		int nameLen;
		fscanf(originalFile,"%d",&nameLen);
		char *name = malloc(sizeof(char)*(nameLen+1));
		name[nameLen] = '\0';
		float gpa;
		fscanf(originalFile,"%s %f",name,&gpa);
		if(gpa>=65.0)
		{
			passingStudents[passingCount].name = "PINU";
			passingStudents[passingCount].gpa = gpa;
			passingCount++;
		}
	}
	fprintf(newFile,"%d\n",passingCount);
	for(int i=0;i<passingCount;i++)
	{
		fprintf(newFile,"%ld %s %.2f\n",strlen(passingStudents[i].name),passingStudents[i].name,passingStudents[i].gpa);
	}
	


	fclose(originalFile);
	fclose(newFile);
	remove("./data/cp.txt");
	rename("./data/cp_temp.txt","./data/cp.txt");
	return 0;
}
