#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>


// for open(), dup() and dup2()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#define _GNU_SOURCE 

// for tee and splice error checking 
#include <limits.h>
#include <errno.h>

int main()
{
	pid_t pid, wpid, pipe_pid, pipe_wpid;
	int numPipes = 2,status;
	int pipefds[2*numPipes];
	
	for(int i = 0; i < 2*(numPipes); i++){
		if(pipe(pipefds + i*2) < 0) {
		    perror("pipe");
		    exit(EXIT_FAILURE);
		}
	}
	int j=0;
	if((pid=fork())<0)
	{
		printf("Fork error");
		exit(1);
	}
	else if(pid==0)
	{
		//int pipefd[2];
		if(dup2(pipefds[j*2+1],1)<0)
		{
			printf("Dup2 error \n");
			exit(0);
		}
		close(pipefds[0]);
		close(pipefds[1]);
		close(pipefds[2]);
		close(pipefds[3]);
		char *prog1[3];
            	prog1[0]=(char*)"ls";
            	prog1[1]=(char*)"-l";
            	prog1[2] = NULL;
            	//f1=1;
            	execvp(prog1[0],prog1);
            	perror("execvp of ls failed");
            	exit(1);
		
	}
	
	
	j+=1;
	if((pid=fork())<0)
	{
		printf("Fork error");
		exit(1);
	}
	else if(pid==0)
	{
		int pipefd[2];
		if(dup2(pipefds[(j-1)*2],0)<0)
		{
			printf("Dup2 error input\n");
			exit(0);
		}
		if(dup2(pipefds[j*2+1],1)<0)
		{
			printf("Dup2 error output\n");
			exit(0);
		}
		close(pipefds[0]);
		close(pipefds[1]);
		close(pipefds[2]);
		close(pipefds[3]);
		char *prog2[3];
	    	prog2[0]=(char*)"wc";
	    	prog2[1]=(char*)"-l";
	    	prog2[2] = NULL;
	    	//f2=1;
	    	execvp(prog2[0],prog2);
	    	perror("execvp of wc failed");
	    	exit(1);
	}
	
	
	j+=1;
	if((pid=fork())<0)
	{
		printf("Fork error");
		exit(1);
	}
	else if(pid==0)
	{
		int pipefd[2];
		if(dup2(pipefds[(j-1)*2],0)<0)
		{
			printf("Dup2 error input\n");
			exit(0);
		}
		close(pipefds[0]);
		close(pipefds[1]);
		close(pipefds[2]);
		close(pipefds[3]);
		char *prog3[3];
            	prog3[0]=(char*)"sort";
            	prog3[1]=NULL;
            	//printf("Executing sort\n");
            	execvp(prog3[0],prog3);
            	perror("execvp of sort failed");
            	exit(1);
	}
	
	
	for( int i = 0; i < 2*(numPipes); i++ ){
	    close( pipefds[i] );
	}
	
	//printf("j: %d",j);
}

/*int main(int argc, char** argv)
{
        int des_p[2];
        if(pipe(des_p) == -1) {
          perror("Pipe failed");
          exit(1);
        }

        if(fork() == 0)            //first fork
        {
            close(STDOUT_FILENO);  //closing stdout
            dup(des_p[1]);         //replacing stdout with pipe write 
            close(des_p[0]);       //closing pipe read
            close(des_p[1]);

            char *prog1[3];
            prog1[0]=(char*)"ls";
            prog1[1]=(char*)"-l";
            prog1[2] = NULL;
            execvp(prog1[0],prog1);
            perror("execvp of ls failed");
            exit(1);
        }

        if(fork() == 0)            //creating 2nd child
        {
            close(STDIN_FILENO);   //closing stdin
            close(STDOUT_FILENO);
            dup(des_p[0]);         //replacing stdin with pipe read
            //close(des_p[1]);       //closing pipe write
            /*close(des_p[0]);
            close(des_p[1]);

            
            char *prog3[3];
            prog3[0]=(char*)"sort";
            prog3[1]=NULL;
            execvp(prog3[0],prog3);
            
            
            perror("execvp of sort failed");
            exit(1);
        }
        
        if(fork() == 0)            //creating 2nd child
        {
            close(STDIN_FILENO);   //closing stdin
            dup(des_p[0]);         //replacing stdin with pipe read
            close(des_p[1]);       //closing pipe write
            close(des_p[0]);

            char *prog2[3];
            prog2[0]=(char*)"wc";
            prog2[1]=(char*)"-l";
            prog2[2] = NULL;
            execvp(prog2[0],prog2);
            perror("execvp of wc failed");
            exit(1);
        }

        close(des_p[0]);
        close(des_p[1]);
        wait(0);
        wait(0);
        return 0;
}*/
