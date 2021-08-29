/*Non Bash Command handler*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <sys/wait.h>


// for open() and dup2()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "tokeniser.h"
//#include "parse.h"
//#include "execute.h"
#include "execMaster.h"
#include "NBC_handler.h"
bool isEmpty(char *cmd)
{
	if(strlen(cmd)==0 || strcmp(cmd,"\n")==0 || strcmp(cmd,"\t")==0 || strcmp(cmd,"\r")==0)
	return 1;
	else
	return 0;
}

void help()
{
	printf("\033[1;32m");
	printf("----> cf to change current folder.  Syntax: cf <path>\n\n");
	printf("----> exef to execute commands from a file with text commands. Syntax: exef <filename>\n\n");
	printf("----> lpc to loop a command with different arguments read from a file.\nSyntax: lpc [i] arg1 arg2 ... arg_i ... arg_n where arg_i is the file\n\n");
	printf("----> qt to quit\n\n");
	printf("\033[0m");
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

/* returns the new command after appending the part in the next line*/
char* cmdSoFar(char* line, char* cmd)
{
	size_t m = strlen(cmd);
	size_t n = strlen(line);
	char *cmd0 = (char *)malloc(sizeof(char)*(m+n));
	strcpy(cmd0,cmd);
	size_t i;
	for(i=m;i<(m+n) && *line!='\\';i++)
	{
		
		*(cmd0+i)=*line;
		++line;
	}
	*(cmd0+i)='\0';
	return cmd0;
}

void loopCMD(char **args, int len)
{
	if(len<3)//lpc [i] arg1 arg2 ... arg_i ... arg_n where arg_i is the file
	{
		printf("\033[1;31m");
		printf("Invalid loop command:\n1 integer for file position, atleast 1 bash command and 1 file-name was not found\n");
		printf("\033[0m");
		return;
	}
	else
	{
		int filePos;
		if(sscanf(args[1],"%d",&filePos)==EOF)
		{
			printf("\033[1;31m");
			printf("Invalid syntax\n");
			printf("\033[0m");
			return;
		}
		if(filePos+3>len||filePos<=0)
		{
			printf("\033[1;31m");
			printf("Invalid position of file: %d\n",filePos);
			printf("\033[0m");
			return;
		}
		filePos+=2;
		FILE *fp;
		pid_t pid, wpid;
		if((pid=fork())<0)
		{
			printf("\033[1;31m");
			printf("Fork error\n");
			printf("\033[0m");
			return;
		}
		else if(pid==0)
		{
			fp = fopen(args[filePos], "r");
			if (fp == NULL)
			{
				printf("\033[1;31m");
				printf("File doesn't exist\n");
				printf("\033[0m");
				return;
			}
			char *linef = NULL;
			size_t line_len;
			while(getline(&linef, &line_len, fp) != -1)
			{
				char **ftoks = tokeniseCMD(linef);	
				linef = NULL;
				if(ftoks[0]==NULL)
					continue;
				
				int newargsize = len-3+numOfToks(ftoks);
				char **newargs = (char**)malloc(sizeof(char*)*(newargsize+1));
				int j=2;
				int k=0;
				int i=0;
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
				if(!cmdHandled(newargs))
					cmd_exec(newargs);
				printf("\n");
				free(newargs);
				free(ftoks);
					
			}
			fclose(fp);
			exit(0);
		}
		else
		{
			int stat;
			do 
			{
			  wpid = waitpid(pid, &stat, 0);
			}while(!WIFEXITED(stat) && !WIFSIGNALED(stat));
		}
		
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
			exit(0);
		}
		
		else
		{
		
			char *linef = NULL;

			size_t line_len;
			while(getline(&linef, &line_len, fp) != -1)
			{		
				char *cmdf = NULL;
				cmdf = (char *)"";
				
				if(fileHasNextLine(linef))
				{
					bool fileEnd = 0;
					while(fileHasNextLine(linef))
					{
						if(strlen(linef)!=0)
							cmdf = cmdSoFar(linef,cmdf);
						linef = NULL;
						if(getline(&linef, &line_len, fp) == -1)
						{
							fileEnd=1;
							break;
						}
							
					}
					if(!fileEnd)
						cmdf = cmdSoFar(linef,cmdf);
				}
				else if(strlen(linef)!=0 && !fileHasNextLine(linef))
				{
					cmdf = cmdSoFar(linef,cmdf);
					linef = NULL;
				}
				if(isEmpty(cmdf))
				{
					linef = NULL;
					continue;
				}
					
				printf("--> command in file \'%s\' : %s\n",args[1],cmdf);
				char **argsf = tokeniseCMD(cmdf);
				if(argsf[0]==NULL)
				{
					linef = NULL;
					continue;
				}
				if(!cmdHandled(argsf))
					cmd_exec(argsf);
				usleep(5000);
				linef = NULL;
			}
			
			fclose(fp);
			exit(0);
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
	int len = numOfToks(args); //////////////////////////numOfToks is not defined here
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
	else if(strcmp(args[0],"cf")==0)
	{
		if(numOfToks(args)==2)
		{
			chdir(args[1]);
			return 1;
		}
		else
		{
			printf("\033[1;31m");
			printf("Location does not exist\n");
			printf("\033[0m");
			return 1;
		}
	}
	else if(strcmp(args[0],"nbchelp")==0)
	{
		help();
		return 1;
	}
	else if(strcmp(args[0],"qt")==0)
	{
		exit(0);
	}
	else
	return 0;
}
