#include <stdio.h>
#include <stdlib.h>
#define MAX 1024
int main()
{
	FILE *fp;
	fp = fopen("./data/cp.txt","r");

	char buffer[MAX];
	int line = 0;
	while(fgets(buffer,MAX,fp)!=NULL)
	{
	    printf("%s\n",buffer);
	    line++;
	}
	printf("行數:%d\n",line);
	return 0;
}
