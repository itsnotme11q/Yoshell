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
#include "NBC_handler.h"
#include "execMaster.h"

#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported

bool cmdHandled(char**);
void readFile(char**, int);

int takeInput(char** str)
{
    char* buf;
    int stlen;
    buf = readline("\u20B9> ");
    if ((stlen=strlen(buf)) != 0) {
        //add_history(buf);
        *str = (char*)malloc(sizeof(char)*stlen);
        //str = buf;
        strcpy(*str, buf);
        return 0;//non-empty input
    } else {
        return 1;//input
    }
}

int takeNextLine(char** str)
{
	char* buf;
	int stlen;
	buf = readline("\u20B9>>> ");
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


bool hasNextLine(char* line)
{
	size_t n = strlen(line);
	size_t x=0;
	char *ptr = line;
	while(x<n-1)
	{
		++ptr;
		++x;
	}
	if(*ptr=='\\')
	return 1;
	else 
	return 0;
}


void currPath()
{
	char cwd[1024];
	printf("\033[1;36;40m");
	getcwd(cwd, sizeof(cwd));
	printf("%s\033[0m\n", cwd);
	//printf("\033[0m");
}

void printInit()
{
	
	printf("\033[1;33;40m\n");
	printf("----------------------------------------------------------\n");
	printf("########           ####                 #######\n");
	printf("##########         ####              ############\n");
	printf("###    ####        ####             #####     ####\n");
	printf("###    ####        ####             #####   \n");
	printf("##########         ####              #######\n");
	printf("########           ####                 #######\n");
	printf("##########         ####                     #####\n");
	printf("###    ####        ####             #####    #####\n");
	printf("###    ####        ####             #####    #####\n");
	printf("##########         #########          ###########\n");
	printf("########           #########            ######\n");
	printf("------------------------------------------------------------\033[0m\n");
	printf("\033[1;34m");
	printf(".o000  .o000 00000 .o0o. .o0o. .o0o. 00000\n");
	printf("00     00    00       00 o   o o   o 00\n");
	printf("0      .oo.  0000    00  0   0 0   0  00o.\n");
	printf("00        00 00    00    0   0 0   0     0\n");
	printf("°o000  0o00° 00000 00000 °o0o° °o0o° 0000°\n\n");
	printf("   00       .o000 .o0o. 00.  .0 0000o.\n");
	printf("   00       00    0   0 000  00 00  00\n");
	printf("   00  000  0     0   0 00 \\/ 0 00000°\n");
	printf("   0/       00    0   0 00    0 00\n");
	printf("000°        °o000 °o0o° 00    0 00\n\n");
	printf("\033[1;37;44m Soham Bhattacharyya, 19BCE1199 \033[0m\n");
	printf("\033[1;37;44m Pranjal Singh, 19BCE1207       \033[0m\n\n");
}



int main() {
    
	//FILE *fp;
	char *line=NULL;//[MAXCOM];
	line = (char*)"";
	size_t linesize;

	//char **args;

	/*int i,j,k;

	int beginflag = 0;  
	int endflag = 0;

	int commentbegin = 0;
	int commentend = 0;*/

	char *cmd=NULL;
	cmd = (char*)"";

	//int t=4;
	printInit();
	while(1) {       

		currPath();
		
		int n = takeInput(&line);
		if(n==0)
		{
			if(hasNextLine(line))
			{
				while(hasNextLine(line))
				{
					cmd = cmdSoFar(line,cmd);
					//cmd = my_strcat(cmd,line);
					line = NULL;
					line = (char*)"";
					n=takeNextLine(&line);
					if(n==1)
						break;
				}
				cmd = cmdSoFar(line,cmd);
			}
			else
			{
				cmd = cmdSoFar(line,cmd);
				line = NULL;
				line = (char*)"";
				//cmd = my_strcat(cmd,line);
			}
		}
		else
		continue;
		
		if(isEmpty(cmd))
			continue;	
		add_history(cmd);
		//printf("command: %s\n",cmd);
		/*if(takeInput(&line)==1)
			continue;*/
		//args = parseLine(cmd);
		char **toks = tokeniseCMD(cmd);
		
		
		//printf("args: %s\n",args[0]);
		//printf("%s",args[0]);
		//if(strcmp(args[1],"exit")==0)
			//break;
			
		// printf("Calling 'execute' with the following as args: ");
		// for(j = 0; args[j]!=NULL; j++) printf("%s ", args[j]); 
		// printf("\n");
		
		
		if(!cmdHandled(toks))///////////////////////////////check if command is handled 
			cmd_exec(toks);
			//execute(args);
		
		printf("\n");
		free(toks);
		cmd = NULL;
		cmd = (char*)"";
		usleep(5000); //sleeps for 5000 microseconds

	}

	return 0;
}   
