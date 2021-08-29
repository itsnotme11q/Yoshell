#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokeniser.h"
#define delim_str " \t\n\r"
void allocate(char ***tokens,int allocType, int numOfToks)
{
	if(allocType==1)
	{
		*tokens = (char**)malloc(sizeof(char*)*numOfToks);
		if(!tokens) {
			// Insufficient space, so terminate and exit
			printf("Insufficient space for %d bytes... Terminating.\n",numOfToks);
			exit(EXIT_FAILURE);    
			//return false; 
		}
		//else
		//return true;
	}
	else
	{
		*tokens = (char**)realloc(tokens, sizeof(char*)*numOfToks);
		if(!tokens) {
			// Insufficient space, so terminate and exit
			printf("Insufficient space for %d bytes... Terminating.\n",numOfToks);
			exit(EXIT_FAILURE);
			//return false;     
		}
		//else
		//return true;
	}
}


char** tokeniseCMD(char *cmd) {
	int len = strlen(cmd);
	const int initBytes = 256;//maximum number of tokens that an array of tokens can initially store
	int numOfToks = initBytes; 
	char** tokens;
	
	allocate(&tokens,1,numOfToks);
	int i = 0;//index at which new token is to be created

	char* currToken;
	currToken = (char*)"";

    	currToken = strtok(cmd, delim_str);
	while(currToken != NULL) 
	{

		if(strcmp(currToken, "#") == 0) // anything post '#' is ignored as it marks the start of a comment
			break;
			    

		if(strlen(currToken)!=0 && strcmp("\n",currToken)!=0 && strcmp("\r",currToken)!=0 && strcmp("\t",currToken)!=0)
		tokens[i] = currToken;
		++i;

		if(i > numOfToks) //if number of tokens has gone beyond the number of characters assigned, reallocate tokens array
		{
			// reallocate memory for tokens
			numOfToks += initBytes;
			allocate(&tokens,2,numOfToks);
			

		}
		currToken = strtok(NULL, delim_str); //get next token
    }
    
    tokens[i] = NULL;

    return tokens;
}
