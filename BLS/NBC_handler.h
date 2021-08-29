bool isEmpty(char *cmd);
bool fileHasNextLine(char* line);
char* cmdSoFar(char* line, char* cmd);
void readFile(char **args, int len);// for exef
bool cmdHandled(char **args);
void loopCMD(char **args, int len);//for lpc
