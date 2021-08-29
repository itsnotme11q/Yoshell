#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <iostream>

#include "tokeniser.h"
#include "execMaster.h"
using namespace std;
int numOfToks(char **toks)
{
	int i = 0;
	while(toks[i]!=NULL)
	{
		i+=1;
	}
	return i;
}

bool isSpcl(char *tok)
{
	return (strcmp(tok,"|")==0 || strcmp(tok,">>")==0 || strcmp(tok, ">")==0);
}

int pipeCount(char **toks)
{
	int pc=0;//stores number of '|'
	int l = numOfToks(toks);
	for(int i=0;i<l;i++)
	{
		if(strcmp(toks[i],"|")==0)
		{
			++pc;
		}
	}
	return pc;
}


void cmd_exec(char **toks)
{
	pid_t pid, wpid;
	int stat,l= numOfToks(toks);
	int hasTrunc=0,hasAppen=0,hasPipe=0;
	for(int i=0;i<l;i++)
	{
		if(strcmp(toks[i],">")==0)
		{
			hasTrunc = i;
		}
		if(strcmp(toks[i],">>")==0)
		{
			hasAppen = i;
		}
		if(strcmp(toks[i],"|")==0)
		{
			hasPipe = i;
		}
	}
	//printf("In execCMD, hasAppen: %d\n",hasAppen);
	if(!hasTrunc && !hasAppen && !hasPipe)
	{
		if((pid=fork())<0)
			perror("Couldn't fork\n");
		else if(pid==0)
		{
			execvp(toks[0], toks);
			printf("\033[1;31m");
        	printf("Invalid command. Execute nbchelp for help on Non Built-in Commands.\n");
        	printf("\033[0m");
        	exit(0);
		}
		else
		{
			do 
			{
          		wpid = waitpid(pid, &stat, 0);
        	} while(!WIFEXITED(stat) && !WIFSIGNALED(stat));
		}
	}
	
	else if((!hasAppen||hasTrunc<hasAppen) && hasTrunc && !hasPipe)
	{
		
		if((pid=fork())<0)
			perror("Couldn't fork\n");
		else if(pid==0)
		{
			toks[hasTrunc] = NULL;
			
			int fd_out = open(toks[hasTrunc + 1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP |S_IWUSR);
			
			dup2(fd_out, 1);  // 1 equivalent to STDOUT_FILENO
		    close(fd_out);
		    printf("\n\n");
		    execvp(toks[0], toks);
		    //printf("\033[1;31m");
		    printf("Couldn't execute this command\n");
		    //printf("\033[0m");
		    exit(1);
		}
		else
		{
			do 
			{
          		wpid = waitpid(pid, &stat, 0);
        	} while(!WIFEXITED(stat) && !WIFSIGNALED(stat));
		}
	}
	
	else if((!hasTrunc||hasAppen<hasTrunc) && hasAppen && !hasPipe)
	{
		
		if((pid=fork())<0)
			perror("Couldn't fork\n");
		else if(pid==0)
		{
			toks[hasAppen] = NULL;
			
			int fd_out = open(toks[hasAppen + 1], O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			
			dup2(fd_out, 1);  // 1 equivalent to STDOUT_FILENO
		    close(fd_out);
		    printf("\n\n");
		    execvp(toks[0], toks);
		    //printf("\033[1;31m");
		    printf("Couldn't execute this command\n");
		    //printf("\033[0m");
		    exit(1);
		}
		else
		{
			do 
			{
          		wpid = waitpid(pid, &stat, 0);
        	} while(!WIFEXITED(stat) && !WIFSIGNALED(stat));
		}
	}
	
	else if(hasPipe)
	{
		int n = pipeCount(toks);
		char **atomicCMD[n+1];
		int i=0,tokNum=0,j;
		
		while(i<(n+1))
		{
			atomicCMD[i] = (char**)malloc(l*sizeof(char*));
			if(!atomicCMD[i])
			{
				printf("\033[1;31m");
				printf("Insufficient memory\n");
				printf("\033[0m");
				return;
			}
			
			j = 0;
			while(tokNum < l && !isSpcl(toks[tokNum]))
			{
				atomicCMD[i][j] = toks[tokNum];
				//printf("	%s ",atomicCMD[i][j]);
				++tokNum;
				++j;
			}
			atomicCMD[i][j] = NULL;
			++i;
			++tokNum;
			//printf("\n");
		}
		char *outfile=NULL;// = (char*)
		if(hasTrunc)
			outfile = toks[hasTrunc+1];
		if(hasAppen)
			outfile = toks[hasAppen+1];
		//cout<<"hasAppen before calling execPiped: "<<hasAppen<<"\n";
		execPipedCMD(atomicCMD,n,l,hasTrunc,hasAppen,outfile);
	}
	
	else
	{
		printf("\033[1;31m");
		printf("Invalid command. Execute nbchelp for help on Non Built-in Commands.");
		printf("\033[0m");
	}
	//return 1;
}

void execPipedCMD(char **atomicCMD[], int numPipes, int cmdSize, int hasTrunc, int hasAppen, char *outfile)
{
	int p_stat,n=numPipes+1;
	int pipefds[2*numPipes];
	for(int i = 0; i < 2*(numPipes); i++)
	{
		if(pipe(pipefds + i*2) < 0) {
			perror("Pipe error: ");
			return;
		}
	}
	
	for(int i = 0;i<n;i++)
	{
		pid_t pid,wpid;
		int stat;
		if((pid=fork())<0)
		{
			printf("\033[1;31m");
			printf("Fork error");
			printf("\033[0m");
			return;
		}
		else if(pid==0)
		{
			if(i<n-1)
			{
				if(dup2(pipefds[i*2+1],1)<0)
				{
					printf("\033[1;31m");
					printf("Dup2 error \n");
					printf("\033[0m");
					exit(1);
				}
			}
			if(i>0)
			{
				if(dup2(pipefds[(i-1)*2],0)<0)
				{
					printf("\033[1;31m");
					printf("Dup2 error \n");
					printf("\033[0m");
					exit(1);
				}
			}
			if((hasTrunc || hasAppen) && i==n-1)
			{
				if(hasAppen>cmdSize)hasAppen=0;
				if(hasTrunc>cmdSize)hasTrunc=0;
				//printf("In outfile portion hasTrunc: %d, hasAppen: %d, i: %d\n",hasTrunc,hasAppen,i);
				
				int outdes=0; //descriptor of output file to which execution result is to be appended or rewrote
				if(hasAppen && (!hasTrunc||hasAppen<hasTrunc))
				{
					outdes = open(outfile,O_APPEND | O_WRONLY | O_CREAT, S_IWGRP | S_IRGRP | S_IRUSR | S_IWUSR);//the file descriptor returns the file descriptor for a file that is to be truncated, in write-only mode, and to be created if does not exist. And the permission bits permits the group user and the creator of the file to read and write
					if(dup2(outdes,1)==-1)
					{
						printf("\033[1;31m");
						printf("Dup2 error at outfile (>>)\n");
						printf("\033[0m");
						exit(1);
					}
					else
					{
						close(outdes);
					}
				}
				else if(hasTrunc && (!hasAppen||hasTrunc<hasAppen))
				{
					outdes = open(outfile,O_TRUNC | O_WRONLY | O_CREAT, S_IWGRP | S_IRGRP | S_IRUSR | S_IWUSR);//the file descriptor returns the file descriptor for a file that is to be truncated, in write-only mode, and to be created if does not exist. And the permission bits permits the group user and the creator of the file to read and write
					if(dup2(outdes,1)==-1)
					{
						printf("\033[1;31m");
						printf("Dup2 error at outdfile (>)\n");
						printf("\033[0m");
						exit(1);
					}
					close(outdes);
				}
				
			}
			for(int j=0;j<2*numPipes;j++)
				close(pipefds[j]);
			execvp(atomicCMD[i][0], atomicCMD[i]);
			perror("Execution error");
			exit(0);
		}
		
	}
	
	for(int j=0;j<2*numPipes;j++)
		close(pipefds[j]);
	
	
}


