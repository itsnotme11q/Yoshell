#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

// for open() and dup2()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tokeniser.h"
#include "execMaster.h"
//#include "execute.h"

#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported

bool cmdHandled(char**);
void readFile(char**, int);


int argsLength(char **args) {
  int i = 0;
  while(args[i] != NULL) {
    i++;
  }
  return i;
}

bool isEmpty(char *cmd)
{
	if(strlen(cmd)==0)
	return 1;
	else
	return 0;
}

bool fileHasNextLine(char* line)
{
	size_t n = strlen(line);
	size_t x=0;
	char *ptr = line;
	//printf("Characters: ");
	while(x<n-2)
	{
		//printf("\'%c\', ",*ptr);
		++ptr;
		++x;
	}
	//printf("\'%c\' \n",*ptr);
	if(*ptr=='\\')
	return 1;
	else 
	return 0;
}

bool hasNextLine(char* line)
{
	size_t n = strlen(line);
	size_t x=0;
	char *ptr = line;
	//printf("Characters: ");
	while(x<n-1)
	{
		//printf("\'%c\', ",*ptr);
		++ptr;
		++x;
	}
	//printf("\'%c\' \n",*ptr);
	if(*ptr=='\\')
	return 1;
	else 
	return 0;
}

char* cmdSoFar(char* line, char* cmd)
{
	//printf("cleared 1\n");
	//strcat(cmd,line);
	size_t m = strlen(cmd);
	size_t n = strlen(line);
	char *cmd0 = (char *)malloc(sizeof(char)*(m+n));
	strcpy(cmd0,cmd);
	size_t i;
	for(i=m;i<(m+n) && *line!='\\';i++)
	{
		//printf("In loop\n");
		*(cmd0+i)=*line;
		//printf("i: %ld, *line: %c\n",i,*line);
		++line;
	}
	*(cmd0+i)='\0';
	return cmd0;
}

/*function to execute a non-inbuilt command*/
void loopCMD(char **args, int len)
{
	if(len<3)//lpc [i] arg1 arg2 ... arg_i ... arg_n where arg_i is the file
	{
		printf("Invalid loop command\n");
		return;
	}
	else
	{
		int filePos;
		if(sscanf(args[1],"%d",&filePos)==EOF)
		{
			printf("Invalid syntax\n");
			return;
		}
		if(filePos+3>len||filePos<=0)
		{
			printf("Invalid position of file: %d\n",filePos);
			return;
		}
		filePos+=2;
		FILE *fp;
		fp = fopen(args[filePos], "r");
		if (fp == NULL)
		{
			printf("File doesn't exist\n");
			return;
		}
		//printf("File exists\n");
		char *linef = NULL;
		size_t line_len;
		while(getline(&linef, &line_len, fp) != -1)
		{
			char **ftoks = tokeniseCMD(linef);	
			//printf("line: %s",linef);
			linef = NULL;
			
			int newargsize = len-3+numOfToks(ftoks);
			char **newargs = (char**)malloc(sizeof(char*)*(newargsize+1));
			printf("newargsize: %d...",newargsize);
			//newargs[0] = args[1];
			int j=2;
			int k=0;
			int i=0;
			/*for(i=0;i<newargsize;i++)
			{
				if(j!=filePos)
				{
					newargs[i]=args[j];
					
					//printf("%s ",newargs[i]);
					++j;
				}
				else
				{
					++j;
					for(int k=0;k<numOfToks(ftoks);k++)
					{
						newargs[i] = ftoks[k];
						//printf("%s ",newargs[i]);
						++i;	
					}
				}
			}*/
			//printf("filePos: %d, numOfToks(ftoks): %d, newargsize: %d\n",filePos,numOfToks(ftoks),newargsize);
			for(i=0;j<filePos;i++)
			{
				newargs[i]=args[j];
				++j;
			}
			++j;
			for(int k=0;k<numOfToks(ftoks);k++)
			{
				newargs[i]= ftoks[k];
				++i;
			}
			for(;i<newargsize;i++)
			{
				newargs[i]=args[j];
				++j;
			}
			newargs[i]=NULL;
			/*int m = 0;
			while(newargs[m]!=NULL)
			{
				printf("%s ",newargs[m]);
				++m;
			}*/
			cmd_exec(newargs);
			//for(int z=0;z<10000000;z++);//delay for reading next line
			printf("\n");
			free(newargs);
			free(ftoks);
				
		}
		fclose(fp);
	}
}

void readFile(char **args, int len)
{

	
	pid_t fpid,wfpid;//file process id and wait file process id
	int filestat; //status of file process
	
	if((fpid = fork()) < 0)
	{
		perror("Fork error");
	}
	else if(fpid==0)
	{
		FILE *fp;
		fp = fopen(args[1], "r");
		if (fp == NULL)
		{
			printf("File doesn't exist\n");
			return;
		}
		
		else
		{
		
			char *linef = NULL;
		
			//linef = (char *)"";
			size_t line_len;
			//printf("	Step 1 done..\n");
			while(getline(&linef, &line_len, fp) != -1)
			{
				//printf("	Step 2 done..\n");
				//printf("Line in file: %s\n",linef);
				
				char *cmdf = NULL;
				cmdf = (char *)"";
				
				if(fileHasNextLine(linef))
				{
					bool fileEnd = 0;
					while(fileHasNextLine(linef))
					{
						if(strlen(linef)!=0)
							cmdf = cmdSoFar(linef,cmdf);
						//printf("	1..Command so far: %s (File)\n",cmdf);
						linef = NULL;
						if(getline(&linef, &line_len, fp) == -1)
						{
							fileEnd=1;
							break;
						}
							
					}
					if(!fileEnd)
						cmdf = cmdSoFar(linef,cmdf);
					//printf("	2..Command so far: %s (File)\n",cmdf);
				}
				else if(strlen(linef)!=0 && !fileHasNextLine(linef))
				{
					cmdf = cmdSoFar(linef,cmdf);
					//printf("	In second condition (File)\n");
					linef = NULL;
				}
				printf("	command in file \'%s\' : %s\n",args[1],cmdf);
				char **argsf = tokeniseCMD(cmdf);
				if(!cmdHandled(argsf))
				printf("		Command to be executed\n");
				linef = NULL;
				//linef = (char*)"";
				//printf("	Step 3 done..\n");
			}
		//printf("Nice\n");
			
			fclose(fp);
		}
	}
	else
	{
		do 
		{
		  wfpid = waitpid(fpid, &filestat, 0);
		}while(!WIFEXITED(filestat) && !WIFSIGNALED(filestat));
	}
		
	
}

bool cmdHandled(char **args)
{
	int len = argsLength(args); 
	if(strcmp(args[0],"exef")==0 /*&& len>1*/)
	{
		readFile(args,len);
		return 1;
	}
	else if(strcmp(args[0],"lpc")==0)
	{
		loopCMD(args,len);
		return 1;
	}
	else
	return 0;
}


int takeInput(char** str)
{
    char* buf;
    int stlen;
    buf = readline("\n>>> ");
    if ((stlen=strlen(buf)) != 0 && strcmp(buf,"\n")!=0 && strcmp(buf,"\r")!=0) {
        //add_history(buf);
        *str = (char*)malloc(sizeof(char)*stlen);
        //str = buf;
        strcpy(*str, buf);
        printf("line: %s",*str);
        return 0;//non-empty input
    } else {
    	printf("line not found\n");
        return 1;//input
    }
}

int takeNextLine(char** str)
{
	char* buf;
	int stlen;
	buf = readline("\n	> ");
	if ((stlen=strlen(buf)) != 0) {
		//add_history(buf);
		*str = (char*)malloc(sizeof(char)*stlen);
		//str = buf;
		strcpy(*str, buf);
		//printf("%s",*str);
		return 0;//non-empty input
	} else {
	return 1;//input
	}
}

void passArray(int *a)
{
	for(int i=0;i<3;i++)
	{
		printf("%d",a[i]);
	}
}


int main()
{
	char *line=NULL;
	line = (char *)"";
	//printf("Yeah: %s\n",line);
	//int n = takeInput(&line);
	//printf("%s\n",line);
	char *cmd=NULL;
	cmd = (char*)"";
	//int n = takeInput(&line);
	
	int t=3;
	while(1)
	{
		
		int n = takeInput(&line);
		if(n==0)
		{
			if(hasNextLine(line))
			{
				while(hasNextLine(line))
				{
					cmd = cmdSoFar(line,cmd);
					//printf("1..Command so far: %s\n",cmd);
					//cmd = my_strcat(cmd,line);
					//free(line);
					line = NULL;
					line = (char*)"";
					n=takeNextLine(&line);
					if(n==1)
						break;
				}
				cmd = cmdSoFar(line,cmd);
				//printf("2..Command so far: %s\n",cmd);
			}
			else 
			{
				cmd = cmdSoFar(line,cmd);
				//printf("In second condition\n");
				line = NULL;
				line = (char*)"";
				//cmd = my_strcat(cmd,line);
			}
		}
		/*if(hasNextLine(line) && n==0)
		{
			while(hasNextLine(line))
			{
				cmd = cmdSoFar(line,cmd);
				//printf("1..Command so far: %s\n",cmd);
				//cmd = my_strcat(cmd,line);
				//free(line);
				line = NULL;
				line = (char*)"";
				n=takeNextLine(&line);
				if(n==1)
					break;
			}
			cmd = cmdSoFar(line,cmd);
			//printf("2..Command so far: %s\n",cmd);
		}
		else if(n==0 && !hasNextLine(line))
		{
			cmd = cmdSoFar(line,cmd);
			//printf("In second condition\n");
			line = NULL;
			line = (char*)"";
			//cmd = my_strcat(cmd,line);
		}*/
		else
		{
			printf("Empty command\n");
			line = NULL;
			line = (char*)"";
			cmd = NULL;
			cmd = (char*)"";
			continue;
		}
		//continue;
		printf("command: %s, n=%d\n",cmd,n);
		if(isEmpty(cmd))
		printf("Empty command\n");
		else
		{
			char **args = tokeniseCMD(cmd);
			if(cmdHandled(args))
			{
				printf("Non inbuilt command\n");
			}
		}
		cmd = NULL;
		cmd = (char*)"";
	}
	
	
	
	//int a[] = {1,2,3,4};
	//passArray(a+1);
}
