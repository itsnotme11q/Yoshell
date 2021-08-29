//#define delim_str " \t\n\r"


char** tokeniseCMD(char *cmd);
bool allocated(char **tokens,int type);
/* ^^ function signature for a function that divides the command string passed as parameter 
	into several tokens that were separated by ' ' (space), '\r' (carriage return), '\t' (tab) or '\n' (new line)*/
