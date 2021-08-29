int numOfToks(char **toks);
void cmd_exec(char **toks);
void execPipedCMD(char **atomicCMD[], int numPipes, int cmdSize, int hasTrunc, int hasAppen, char *outfile);
