#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 1000
#define PERMISSIONS 0644

//Globals
char *Input;
char *Args[SIZE];
char *listCmds[SIZE];
char *exportArgs[SIZE];
char currentDir[SIZE];
char directory[SIZE];

typedef struct job {
    char *Name;
    int Index;
    int Status;
    int pid;
}job;


int JobsNum;

job Jobs[SIZE];

// This stores the fg job -> needed for CtrlC & Z command
job fgJob;

// For storing Foreground processes
int FPID[SIZE];
int FPNums;

void red () {
    printf("\033[1;31m");
}

void reset() {
    printf("\033[0m");
}
//////////////////////////////////////////////////////TOKENIZER//////////////////////////////////////////////////

void tokenizer(char *token[], char *s, char *delimParameter,  int *total){
    int index = 0;
    token[0] = strtok(s, delimParameter);
    while(token[index]!=NULL){
        token[++index] = strtok(NULL, delimParameter);
    }
        // Returns the total no. of commands
    *total = index;
}
//////////////////////////////////////////////////////CURRENT_DIRECTORY//////////////////////////////////////////////////

void getCurrentDir() {
    if(getcwd(currentDir, SIZE) == NULL) {
        perror("");
        exit(0);
    }
    return;
}
//////////////////////////////////////////////////////CHECK_REDIRECTIONAL//////////////////////////////////////////////////

int checkRedirection(int ArgsNum, char *Args[]) {
   for (int i = 0; i < ArgsNum; i++) {
       if (strcmp(Args[i], "<") == 0)
           return 1;
       else if(strcmp(Args[i], ">") == 0)
           return 1;
       else if(strcmp(Args[i], ">>") == 0 )
           return 1;
   }
   return 0;
}
//////////////////////////////////////////////////////REDIRECTIONAL//////////////////////////////////////////////////

int redirectionHandler(int totalArgsInEachCommand, char *listOfArgs[]) {
    
    char inputFile[10000], outputFile[10000];
    int position1 = 0, position2 = 0, position3 = 0;
    int inputDup, outputDup;
    int pos = 100;
    int inputfd, outputfd;

    for(int i = 0; i < totalArgsInEachCommand; i++) {
        if(strcmp(listOfArgs[i], "<") == 0) position1 = i;
        if(strcmp(listOfArgs[i], ">") == 0) position2 = i;
        if(strcmp(listOfArgs[i], ">>") == 0) position3 = i;
    }

    if(position1 != 0) {
        
        strcpy(inputFile, listOfArgs[position1 + 1]);
        pos = position1;
        listOfArgs[pos] = NULL;
        struct stat tmp;
        if(stat(inputFile, &tmp) < 0) {
            perror("");
            return -1;
        }

        inputDup = dup(STDIN_FILENO);
        if (inputDup < 0) {
            perror("Dup ");
            return -1;
        }

        inputfd = open(inputFile, O_RDONLY, PERMISSIONS);
        if (inputfd < 0) {
            printf("File doesn't exist!\n");
            return -1;
        }   
        if (dup2(inputfd, STDIN_FILENO)) {
            perror("Dup2 ");
            return -1;
        }
    }
    
    if(position2 != 0 || position3 != 0) {
        
        if(position2 + position3 <= pos)
            pos = position2 + position3;
        strcpy(outputFile, listOfArgs[position2 + position3 + 1]);
        listOfArgs[pos] = NULL;
        outputDup = dup(STDOUT_FILENO);
        if (outputDup < 0) {
            perror("Dup ");
            return -1;
        }

        if(position2 != 0)
            outputfd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, PERMISSIONS);
        else
            outputfd = open(outputFile, O_WRONLY | O_CREAT | O_APPEND, PERMISSIONS);

        if (outputfd < 0) {
            printf("File doesn't exist!\n");
            return -1;
        }  

        if(dup2(outputfd, STDOUT_FILENO) < 0)
        {
            perror("Dup2 ");
            return -1;
        }
    }

    pid_t pidValue;
    pidValue = fork();
    if(pidValue < 0) {
        close(outputfd);
        perror("Fork ");
        return -1;
    }
    if(pidValue == 0) {
        if(execvp(listOfArgs[0], listOfArgs) < 0) {
            perror("Execvp ");
            return -1;
        }
    }
    else {
        wait(NULL);
        dup2(inputDup, STDIN_FILENO);
        dup2(outputDup, STDOUT_FILENO);
    }   
    return 0;
}
//////////////////////////////////////////////////////CHECK_PIPE//////////////////////////////////////////////////

int checkPiping(char* Args[], int ArgsNum) {
	for (int i = 0; i < ArgsNum; i++) {
       if (strcmp(Args[i], "|") == 0)
          return 1;
  }
  return 0;
}
//////////////////////////////////////////////////////PIPE//////////////////////////////////////////////////

void piping(char *command, int ArgsNum) {

    //////////////////////////////////////////////////////////////////////////////  
    if (strchr(command, '&')) {
        //needs to be run at the background
    }
    //////////////////////////////////////////////////////////////////////////
    char* pipedCommand[100];
    int numPipeCommands = 0;
    tokenizer(pipedCommand, command, "|", &numPipeCommands);
    int fd[2];
    int originalInput = dup(STDIN_FILENO);
    int originalOutput = dup(STDOUT_FILENO);

    for (int i = 0; i  < numPipeCommands; i++) {
        int lenOfEachPipeCommand = 0;
        char *onePipeCommand[10000];
        tokenizer(onePipeCommand, pipedCommand[i], " \t", &lenOfEachPipeCommand);
        if (i == 0) {
            pipe(fd);
            dup2(fd[1], STDOUT_FILENO); 
            close(fd[1]);
        }
        else if(i == numPipeCommands - 1) {
            dup2(fd[0], STDIN_FILENO);
            dup2(originalOutput,1);
        }
        else {
            dup2(fd[0], STDIN_FILENO);
            pipe(fd);
            dup2(fd[1], STDOUT_FILENO); 
            close(fd[1]);
        }
        int pid_fork = fork();
        int stat;
        if(pid_fork == 0) {
            if(checkRedirection(lenOfEachPipeCommand, onePipeCommand) == 1) {
                redirectionHandler(lenOfEachPipeCommand, onePipeCommand);
            }
            else {
                execvp(onePipeCommand[0], onePipeCommand);
            }
            exit(0);
        }
        else {
            waitpid(pid_fork, &stat, WUNTRACED);
            dup2(originalInput, STDIN_FILENO);
            dup2(originalOutput, STDOUT_FILENO);
        }
    }
    return;
}

int comparator(const void *p, const void *q){
    return strcmp(((job*)p)->Name, ((job*)q)->Name);
}
//////////////////////////////////////////////////////FOREGROUND_CHILD//////////////////////////////////////////////////

int fgChildHandler(int ArgsNum, char *repeatArgs[]) {

    setpgid(0, 0);
    repeatArgs[ArgsNum] = NULL;

    tcsetpgrp(STDIN_FILENO, getpgid(0));
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);

    int check_execvp = execvp(repeatArgs[0], repeatArgs);

    if(check_execvp < 0){
        printf("Invalid command!\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

//////////////////////////////////////////////////////FOREGROUND_PARENT//////////////////////////////////////////////////

void fgParentHandler(int pid, char *repeatArgs[], int ArgsNum) {
    char fgCommand[SIZE];
    strcpy(fgCommand, "");
    strcat(fgCommand, repeatArgs[0]);

    fgJob.pid = pid;
    fgJob.Name = malloc(strlen(repeatArgs[0]) * sizeof(char) + 2);
    strcpy(fgJob.Name, repeatArgs[0]);
    fgJob.Index = 0;

    int status;
    if (waitpid(pid, &status, WUNTRACED) < 0)
        printf("Invalid command");

    fgJob.pid = -1;

    tcsetpgrp(STDIN_FILENO, getpgid(0));

    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    if(WIFSTOPPED(status)){   
        int len = strlen(fgCommand);
        for (int i = 1; i < ArgsNum - 1; i++) {
            len += strlen(repeatArgs[i]);
        }

        Jobs[JobsNum].pid = pid;
        Jobs[JobsNum].Name = malloc(len * sizeof(char));

        Jobs[JobsNum].Status = 1;
        Jobs[JobsNum].Index = JobsNum;

        strcpy(Jobs[JobsNum].Name, repeatArgs[0]);
        for (int i = 1; i < ArgsNum-1; i++) {
            strcat(Jobs[JobsNum].Name, " ");
            strcat(Jobs[JobsNum].Name, Args[i]);
        }

        JobsNum++;
        FPID[FPNums] = pid;
        FPNums++;

        printf("Process %s with process ID [%d] suspended\n", fgCommand, (int)pid );
        return;
    }else{
        if (status == 1)
            printf("Process %s with process ID [%d] exited \n", fgCommand, (int)pid );
    }
}
//////////////////////////////////////////////////////FOREGROUND_PROCESS//////////////////////////////////////////////////

void foregroundProcess(int ArgsNum, char *repeatArgs[]) {

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    int pid = fork();

    if (pid == 0) {
        fgChildHandler(ArgsNum, repeatArgs);
    }
    else {
        fgParentHandler(pid, repeatArgs, ArgsNum);
        free(fgJob.Name);
    }
}
//////////////////////////////////////////////////////SIGCHILD//////////////////////////////////////////////////

void sigchildHandler() {
    pid_t pid;
    pid = waitpid(-1, NULL, WNOHANG);
    if (pid < 0) {
        return;
    }

    else {
        for(int i = 0; i < JobsNum; i++){
            if((int)pid == Jobs[i].pid) {
                if (Jobs[i].Status != -1) {
                    Jobs[i].Status = -1;
                    printf("Completed: [%d]   %d   %s \n",  JobsNum , Jobs[i].pid, Jobs[i].Name);

                    break;
                }
            }
        }
    }
    return;
}

//////////////////////////////////////////////////////BACKGROUND_PROCESS//////////////////////////////////////////////////
void backgroundProcess(int ArgsNum, char *Args[]) {
    
    int pid = fork();

    if(pid == 0) {
        setpgrp();
        Args[ArgsNum - 1] = NULL; //delete &
        execvp(Args[0], Args); 
        exit(EXIT_SUCCESS);
    }
    else {
        int len = strlen(Args[0]);
        for (int i = 1; i < ArgsNum - 1; i++) {
            len += strlen(Args[i]);
        }
        // Now store the process details in the Jobs Arrays
        // Store pid
        Jobs[JobsNum].pid = pid;

        // create a space in memory
        Jobs[JobsNum].Name = malloc(len * sizeof(char) + 2);


        strcpy(Jobs[JobsNum].Name, Args[0]);

        for (int i = 1; i < ArgsNum-1; i++) {
            strcat(Jobs[JobsNum].Name, " ");
            strcat(Jobs[JobsNum].Name, Args[i]);
        }
        // Set status of bg processes as 1
        Jobs[JobsNum].Status = 1;
        // Store index
        Jobs[JobsNum].Index = JobsNum;
        // Increase the total no of processes
        JobsNum++;
        fprintf(stderr, "Background job started: [%d] %d %s \n",JobsNum, Jobs[JobsNum-1].pid, Jobs[JobsNum-1].Name);
        return;
    }
}

//////////////////////////////////////////////////////JOBS//////////////////////////////////////////////////
void jobs(int ArgsNum, char *Args[]) {

    // Sort the array of structs
    qsort(Jobs, JobsNum, sizeof(Jobs[0]), comparator);
    for(int i = 0; i < JobsNum; i++){
        if (Jobs[i].Status == -1)
            continue;

        printf("[%d] ",Jobs[i].Index + 1);
        printf("Running ");
        printf("%d %s\n", Jobs[i].pid,  Jobs[i].Name);
    }
} 

//////////////////////////////////////////////////////CD//////////////////////////////////////////////////
void cd(int numArgs, char *commandArgument) {

    int check = 0;
    if (numArgs == 1)
        return;

    if(strchr(commandArgument, '$') != NULL)
    {
         check = 1;
    }
    if(check == 1) {
        if(chdir(getenv(strtok(strtok(commandArgument, "="), "$")))< 0) {
            perror("cd ");
            return;
        }

        getCurrentDir();
    }
    else {
        if (chdir(commandArgument) < 0) {
            perror("cd ");
            return;
        }

        getCurrentDir();
    }
    return;
}

//////////////////////////////////////////////////////EXPORT//////////////////////////////////////////////////
void export(char *commandArgument) {
	int cmds = 0;
	tokenizer(exportArgs, commandArgument, "=", &cmds);
    char* env = exportArgs[0];
    char* val = exportArgs[1];
    int check = 0;
    if(strchr(val, '$') != NULL)
    {
         check = 1;
    }

    if(check == 1) {
        if( (setenv(env, getenv(strtok(strtok(exportArgs[1], "$"), "$")), 1)) < 0) {
            perror("export ");
            return;
        }
    }
    else {
        setenv(env, val, 1);
    }
}

//////////////////////////////////////////////////////LS//////////////////////////////////////////////////
void ls(int numArgs, char *commandArgument) {

    if (numArgs == 2) {
        if (strcmp (commandArgument, "-a") == 0 ) {
            int pid = fork();
            if (pid == 0){
                execlp("/bin/ls", "ls", "-a", NULL);
            }
            else {
                wait(NULL);
            }
        }
        else if (strcmp (commandArgument, "-l") == 0 ) {
            int pid = fork();
            if (pid == 0){
                execlp("/bin/ls", "ls", "-l" , NULL);
            }
            else {
                wait(NULL);
            }
        }
    }
    if (numArgs == 1) {
        int pid = fork();
        if (pid == 0){
            execlp("/bin/ls", "ls", NULL);
        }
        else {
            wait(NULL);
        }
    }
}

//////////////////////////////////////////////////////ECHO//////////////////////////////////////////////////
void echo (int numArgs, char *commandArgument[]){

    if (numArgs == 1)
        return;

    char * tmp;
    if(strchr(commandArgument[1], '$') != NULL){
	    if (strchr(commandArgument[1], '/') != NULL){
		    tmp = strtok(commandArgument[1], " /");
		    tmp = strtok(NULL, " /");
		    printf("%s/%s\n", getenv(strtok(strtok(commandArgument[1], "$"), "$")), tmp);
	    } else{
		    printf("%s\n", getenv(strtok(strtok(commandArgument[1], "$"), "$")));
	    }
        return;
    }

    else { 
        if (numArgs > 1) {
            for(int i = 1; i < numArgs; i++) {
                if(strchr(commandArgument[i], '#')){
                    return;
                }
                printf("%s ", strtok(strtok(commandArgument[i], "\'\""), "\'\"")); 
            }          
        }
               return;
    }
}

void cmdHandler() {
   
    int CmdsNum = 0;
    tokenizer(listCmds, Input, "\n", &CmdsNum);
    // free(Input);
    // Always one
    for (int i = 0; i < CmdsNum; i++) {
    char tempStr[10000];
    strcpy(tempStr, listCmds[i]);
    int ArgsNum = 0;
    tokenizer(Args, listCmds[i], " \t", &ArgsNum);

    // Checking the background process (if at last we have &)
    if(strcmp(Args[ArgsNum - 1],"&") == 0){
	    backgroundProcess(ArgsNum, Args);
            return; 
    }
    // Check piping
    else if(checkPiping(Args, ArgsNum) == 1){ 
        
        piping(tempStr, ArgsNum);
    }
    
    else {
        // Check redirection
        if (checkRedirection(ArgsNum, Args) == 1) {
            
            redirectionHandler(ArgsNum, Args);
        }

        else {
		    // If the list is empty then simply return
            if (ArgsNum == 0 || Args[0] == NULL){
                // free(Input);
                return;
            }

            // Check for cd.
            else if(strcmp(Args[0], "cd") == 0) {
                cd(ArgsNum, Args[1]);
            }

            // Check for pwd
            else if(strcmp(Args[0], "pwd") == 0) {
                char myPwd[SIZE];
                if(getcwd(myPwd, SIZE) == NULL) {
                    perror("");
                    exit(0);
                }
                printf("%s\n", myPwd);
                return;
            }

            // Check for echo
            else if(strcmp(Args[0], "echo") == 0) {
                echo (ArgsNum, Args);
                printf("\n");
            }

		     // Check for jobs
            else if(strcmp(Args[0], "jobs") == 0) {
                jobs(ArgsNum, Args);
            }

            // Check for ls
            else if(strcmp(Args[0], "ls") == 0) {
                int tmp = ArgsNum;
                if (strchr(Args[ArgsNum-1], '#'))
                    tmp--;
                ls(tmp, Args[1]);
            }

            // Check for exit
            else if(strcmp(Args[0], "exit") == 0) {
                exit(0);
            }
		
            // Check for quit
            else if (strcmp(Args[0], "quit") == 0){
                exit(0);
            }
		  
            // Check fro export 
            else if(strcmp(Args[0], "export") == 0) {
                export(Args[1]);
            }
            // Check fro comments 
            else if( (strcmp(Args[0], "#") == 0) || (strchr(Args[0], '#'))) {
                return;
            }
            // Check fro comments 
            else if(strcmp(Args[0], "kill") == 0) {
                kill(atoi(Args[2]), atoi(Args[1]));
                return;
            }
            else {
                foregroundProcess(ArgsNum, Args);
            }
        }
    }
        // check if any child process terminated
        signal(SIGCHLD, sigchildHandler);
    }
    return;
}

void getInput(){
    Input = (char *)malloc(SIZE);
    fgets(Input, SIZE, stdin);
    if (strcmp(Input, "clear") == 0) {
        printf("\033[H\033[J");
    }
}

void print(){
    getCurrentDir();
   // printf("\n");
    red();
    printf("[QUASH]$   ");
    reset();
}


int main(){

	printf("Welcome to Quash.... \n \n");

    JobsNum = 0;
    FPNums = 0;
    while (1){

        signal(SIGCHLD, sigchildHandler);

        getCurrentDir();
        print();
        getInput();
        cmdHandler();   
        free(Input);
   }

   return(0);
}


