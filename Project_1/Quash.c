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

#define SIZE 1000000
#define PERMISSIONS 0644
#define ll long long int

//Globals
char *inputGiven;
char *Args[SIZE];
char *listOfCommands[SIZE];
char *exportArgs[SIZE];
char *cdArgs[SIZE];
char currentDir[SIZE];
char pseudoHome[SIZE];
char directory[SIZE];
char lastCD[SIZE];
char copyOfInput[SIZE];

typedef struct job {
    char *jobsNames;
    int jobsIndex;
    int jobsStatus;
    int pid;
}job;


int totalNoOfJobs;

job myJobs[SIZE];

// This stores the sorted list of jobs for "jobs" command
job myJobsTemp[SIZE];

// This stores the fg job -> needed for CtrlC & Z command
job fgJob;

// For storing Foreground processes
int foreProcessesID[SIZE];
int noOfForeProcesses;

void tokenizer(char *token[], char *s, char *delimParameter,  int *total){
    int index = 0;
    token[0] = strtok(s, delimParameter);
    while(token[index]!=NULL){
        token[++index] = strtok(NULL, delimParameter);
    }
        // Returns the total no. of commands
    *total = index;
}

void getCurDir() {
    if(getcwd(currentDir, SIZE) == NULL) {
        perror("");
        exit(0);
    }
    return;
}

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

// This is the function for redirection
int redirectionHandler(ll totalArgsInEachCommand, char *listOfArgs[]) {
    
    char inputFile[10000], outputFile[10000];
    int position1 = 0, position2 = 0, position3 = 0;
    int inputDup, outputDup;
    int pos = 100;
    int inputfd, outputfd;

    // Check for the first positions of all the <, >, >>.
    for(int i = 0; i < totalArgsInEachCommand; i++) {
        if(strcmp(listOfArgs[i], "<") == 0) position1 = i;
        if(strcmp(listOfArgs[i], ">") == 0) position2 = i;
        if(strcmp(listOfArgs[i], ">>") == 0) position3 = i;
    }

    // Case - 1
    if(position1 != 0) {
        
        strcpy(inputFile, listOfArgs[position1 + 1]);
        pos = position1;
        listOfArgs[pos] = NULL;
        struct stat tmp;
        if(stat(inputFile, &tmp) < 0) {
            perror("");
            return -1;
        }

        // Storing the input file
        inputDup = dup(STDIN_FILENO);
        if (inputDup < 0) {
            perror("Dup ");
            return -1;
        }

        // Open the input file  
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
    
    // Case - 2
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

    // Forking the process
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

int checkPiping(char* Args[], int ArgsNum) {
	for (int i = 0; i < ArgsNum; i++) {
       if (strcmp(Args[i], "|") == 0)
          return 1;
  }
  return 0;
}

void piping(char *command, char* Args[], long long ArgsNum) {
    
   char* pipedCommand[10000];
   int numPipeCommands = 0;
        // Split commands the on basis of |
   tokenizer(pipedCommand, command, "|", &numPipeCommands);
        // File descriptor
   int fd[2];
        // Storing the original Input & output filedescriptors
   int originalInput = dup(STDIN_FILENO);
   int originalOutput = dup(STDOUT_FILENO);

        // Calling on each command
   for (int i = 0; i  < numPipeCommands; i++) {
       int lenOfEachPipeCommand = 0;
       char *spaceSepPipedCommand[10000];
            // Tokenizing each command on basis of space, tab and newline
       tokenizer(spaceSepPipedCommand, pipedCommand[i], " \t\n", &lenOfEachPipeCommand);
            // Case - 1
       if (i == 0) {
          pipe(fd);
          dup2(fd[1], STDOUT_FILENO); 
          close(fd[1]);
      }
        // Case - 2
      else if(i == numPipeCommands - 1) {
          dup2(fd[0], STDIN_FILENO);
          dup2(originalOutput,1);
      }
        // Case - 3
      else {
          dup2(fd[0], STDIN_FILENO);
          pipe(fd);
          dup2(fd[1], STDOUT_FILENO); 
          close(fd[1]);
      }
            // Fork the process
      int returnedFork = fork();
      int stat;
      if(returnedFork == 0) {
          if(checkRedirection(lenOfEachPipeCommand, spaceSepPipedCommand) == 1) {
              redirectionHandler(lenOfEachPipeCommand, spaceSepPipedCommand);
          }
          else {
              execvp(spaceSepPipedCommand[0], spaceSepPipedCommand);
          }
          exit(0);
      }
      else {
          waitpid(returnedFork, &stat, WUNTRACED);
          dup2(originalInput, STDIN_FILENO);
          dup2(originalOutput, STDOUT_FILENO);
      }
  }
  return;
}
// This comapres the 2 structs of jobs array
int comparator(const void *p, const void *q){
    return strcmp(((job*)p)->jobsNames, ((job*)q)->jobsNames);
}

// When the child is called
int fgChildHandler(int ArgsNum, char *repeatArgs[]) {
    // Make the child process as the leader of the new group of processes.
    // Setting the process ID of child = Process ID of group
    setpgid(0, 0);

    // Removing the last '&' as we need the name
    repeatArgs[ArgsNum] = NULL;

    // For passing the control to signal
    tcsetpgrp(STDIN_FILENO, getpgid(0));

    // Since the child has become the leader, so now give the control back to the
    // default signals. SIGINT - Ctrl + C(Interrupt handler)
    signal(SIGINT, SIG_DFL);

    // Causes the system to set the default signal handler for the given signal
    // Ctrl + Z.
    signal(SIGTSTP, SIG_DFL);

    // Overlay's a process that has been created by a call to the fork function.
    // Execute the files.
    int check_execvp = execvp(repeatArgs[0], repeatArgs);

    // Check for the errors.
    if(check_execvp < 0){
        printf("Invalid command!\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}


void fgParentHandler(int pid, char *repeatArgs[], int ArgsNum) {
    // When the parent is called
    char fgCommand[SIZE];
    strcpy(fgCommand, "");
    strcat(fgCommand, repeatArgs[0]);

    fgJob.pid = pid;
    fgJob.jobsNames = malloc(strlen(repeatArgs[0]) * sizeof(char));
    strcpy(fgJob.jobsNames, repeatArgs[0]);
    fgJob.jobsIndex = 0;

    int status;
    // waits for child process(fg)
    // Here WUNTRACED uis used as the status is sent back even if the child is stopped by ctrlZ (i.e. not killed)
    if (waitpid(pid, &status, WUNTRACED) < 0)
        printf("Invalid command");

    fgJob.pid = -1;


    // For passing the control to signal
    tcsetpgrp(STDIN_FILENO, getpgid(0));


    // Setting the default behaviours.
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    if(WIFSTOPPED(status)){   // if child is stopped(not killed) by ctrl-z, we can check that by this macro!
        int len = strlen(fgCommand);
        for (int i = 1; i < ArgsNum - 1; i++) {
            len += strlen(repeatArgs[i]);
        }

        // push created child to jobs as its suspended in background!
        myJobs[totalNoOfJobs].pid = pid;
        myJobs[totalNoOfJobs].jobsNames = malloc(len * sizeof(char));
        // strcpy(myJobs[totalNoOfJobs].jobsNames, fgCommand);
        myJobs[totalNoOfJobs].jobsStatus = 1;
        myJobs[totalNoOfJobs].jobsIndex = totalNoOfJobs;

        // Store in the temp array as well
        myJobsTemp[totalNoOfJobs].pid = pid;
        myJobsTemp[totalNoOfJobs].jobsNames = malloc(len * sizeof(char));
        // strcpy(myJobsTemp[totalNoOfJobs].jobsNames, fgCommand);
        myJobsTemp[totalNoOfJobs].jobsStatus = 1;
        myJobsTemp[totalNoOfJobs].jobsIndex = totalNoOfJobs;

        strcpy(myJobs[totalNoOfJobs].jobsNames, repeatArgs[0]);
        strcpy(myJobsTemp[totalNoOfJobs].jobsNames, repeatArgs[0]);
        // Copy the commandName in the processesNames array
        for (int i = 1; i < ArgsNum-1; i++) {
            strcat(myJobs[totalNoOfJobs].jobsNames, " ");
            strcat(myJobsTemp[totalNoOfJobs].jobsNames, " ");
            strcat(myJobs[totalNoOfJobs].jobsNames, Args[i]);
            strcat(myJobsTemp[totalNoOfJobs].jobsNames, Args[i]);
        }

        // Now increase the total no. of jobs present
        totalNoOfJobs++;


        // For the + thing.
        foreProcessesID[noOfForeProcesses] = pid;
        noOfForeProcesses++;

        printf("Process %s with process ID [%d] suspended\n", fgCommand, (int)pid );
        return;
    }else{
        if (status == 1)
            printf("Process %s with process ID [%d] exited \n", fgCommand, (int)pid );
    }

}

void foregroundProcess(int ArgsNum, char *repeatArgs[]) {

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    // Forking child process
    int pid = fork();

    // Sending the child process to background
    if (pid == 0) {
        fgChildHandler(ArgsNum, repeatArgs);
    }

    // Means PID > 0, i.e. the parent's process
    else {
        fgParentHandler(pid, repeatArgs, ArgsNum);
    }
}


// This is the signal controller for CTRL+C and CTRL+Z
void signalControl(int signal){
    if(fgJob.pid > 0)
    {
        myJobs[totalNoOfJobs].pid = fgJob.pid;
        strcpy(myJobs[totalNoOfJobs].jobsNames,fgJob.jobsNames);
        myJobs[totalNoOfJobs].jobsIndex = totalNoOfJobs;
        myJobs[totalNoOfJobs].jobsStatus = 1;

        myJobsTemp[totalNoOfJobs].pid = fgJob.pid;
        strcpy(myJobsTemp[totalNoOfJobs].jobsNames, fgJob.jobsNames);
        myJobsTemp[totalNoOfJobs].jobsIndex = totalNoOfJobs;
        myJobsTemp[totalNoOfJobs].jobsStatus = 1;

        totalNoOfJobs++;

        kill(fgJob.pid , signal);
        fgJob.pid = -1;
        printf("\n");
    }
    else
    {
        printf("\nNo foreground process found\n");
        printf("Press [ENTER] to continue\n");
    }
}

void sigchildHandler() {
    pid_t pid;
    pid = waitpid(-1, NULL, WNOHANG);
    // Check if its a valid process
    if (pid < 0) {
        printf("Invalid process ID\n");
        return;
    }

    else {
        for(int i = 0; i < totalNoOfJobs; i++){
            if((int)pid == myJobs[i].pid) {
                // check if its a background process.
                if (myJobs[i].jobsStatus != -1) {
                    myJobs[i].jobsStatus = -1;
                    printf("Completed: [%d]   %d   %s \n",  totalNoOfJobs , myJobs[i].pid, myJobs[i].jobsNames);
                    break;
                }
            }
        }
    }
    return;
}

//////////////////////////////////////////////////////BACKGROUND_PROCESS//////////////////////////////////////////////////
void backgroundProcess(long long ArgsNum, char *Args[]) {

    int pid = fork();

    if(pid == 0) {
        setpgrp();
        Args[ArgsNum - 1] = NULL;
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
        myJobs[totalNoOfJobs].pid = pid;

        // create a space in memory
        myJobs[totalNoOfJobs].jobsNames = malloc(len * sizeof(char));


        strcpy(myJobs[totalNoOfJobs].jobsNames, Args[0]);

        for (int i = 1; i < ArgsNum-1; i++) {
            strcat(myJobs[totalNoOfJobs].jobsNames, " ");
            strcat(myJobs[totalNoOfJobs].jobsNames, Args[i]);
        }
        // Set status of bg processes as 1
        myJobs[totalNoOfJobs].jobsStatus = 1;
        // Store index
        myJobs[totalNoOfJobs].jobsIndex = totalNoOfJobs;
        // Increase the total no of processes
        totalNoOfJobs++;
        fprintf(stderr, "Background job started: [%d] %d %s \n",totalNoOfJobs, myJobs[totalNoOfJobs-1].pid, myJobs[totalNoOfJobs-1].jobsNames);
        return;
    }
}

//////////////////////////////////////////////////////JOBS//////////////////////////////////////////////////
void jobs(int ArgsNum, char *Args[]) {

    // Sort the array of structs
    qsort(myJobs, totalNoOfJobs, sizeof(myJobs[0]), comparator);
    for(int i = 0; i < totalNoOfJobs; i++){
        if (myJobs[i].jobsStatus == -1)
            continue;

        printf("[%d] ",myJobs[i].jobsIndex + 1);
        printf("Running ");
        printf("%d %s\n", myJobs[i].pid,  myJobs[i].jobsNames);
    }
} 

//////////////////////////////////////////////////////CD//////////////////////////////////////////////////
void cd(int numArgs, char *commandArgument) {
    int check = 0;
    if(strchr(commandArgument, '$') != NULL)
    {
         check = 1;
    }
    if(check == 1) {
        if(chdir(getenv(strtok(strtok(commandArgument, "="), "$")))< 0) {
            perror("cd ");
            return;
        }
        strcpy(lastCD, currentDir);
        getCurDir();
    }
    else {
        if (chdir(commandArgument) < 0) {
            perror("cd ");
            return;
        }
        strcpy(lastCD, currentDir);
        getCurDir();
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
        if( (setenv(env, getenv(strtok(strtok(commandArgument, "$"), "$")),1))< 0) {
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

    if(strchr(commandArgument[1], '$') != NULL){
        printf("%s\n", getenv(strtok(strtok(commandArgument[1], "$"), "$")));
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

void commandHandler() {
   
    int CmdsNum = 0;
    tokenizer(listOfCommands, inputGiven, "\n", &CmdsNum);
    //tokenizer(listOfCommands, inputGiven, "#", &CmdsNum);

    for (int i = 0; i < CmdsNum; i++) {
        char tempStr[10000];
        strcpy(tempStr, listOfCommands[i]);
        int ArgsNum = 0;
        tokenizer(Args, listOfCommands[i], " \t", &ArgsNum);
        // Check piping
        if(checkPiping(Args, ArgsNum) == 1){ //tokenize pipe commands
            
            piping(tempStr, Args, ArgsNum);
        }
        
        else {
            // Check redirection
            int check = checkRedirection(ArgsNum, Args);
            if (check == 1) {
                
                redirectionHandler(ArgsNum, Args);
            }

            else {
    		    // If the list is empty then simply return
                if (ArgsNum == 0 || Args[0] == NULL){
                    return;
                }

                // Checking the background process (if at last we have &)
                else if(strcmp(Args[ArgsNum - 1],"&") == 0){
                    backgroundProcess(ArgsNum, Args);
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
                    printf("%d\n", ArgsNum);
                    ls(ArgsNum, Args[1]);
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
                    kill(atoi(Args[1]), atoi(Args[2]));
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

void getQuashInput(){
    inputGiven = (char *)malloc(SIZE);
    fgets(inputGiven, SIZE, stdin);
    if (strcmp(inputGiven, "clear") == 0) {
        printf("\033[H\033[J");
    }
}

/*void updateRelativePathToMove() {
    // If the paths are not equal, then we aren't at the home directory.
    if (strcmp(currentDir, pseudoHome) != 0) {
        // This is the first non-matched index
        int nonMatchedIndex = 0;
        int index = 0;
        for (index; pseudoHome[index]; index++)
        {
            if (pseudoHome[index] != currentDir[index])
            {
                nonMatchedIndex = index;
                break;
            }
        }
        // If the value of the first non-matched index = 0 then we have to copy the currentpath to directoty.
        if (nonMatchedIndex != 0) {
            strcpy(directory, currentDir);
        }
        else {
            // Else found the relative path
            directory[0] = '~';
            int p, j;
            for (p = 1, j = index; currentDir[j]; p++, j++)
            {
                directory[p] = currentDir[j];
            }
            directory[p] = '\0';
        }
    }
}*/

void printPrompt(){
    getCurDir();
    printf(" [QUASH]$ > ");
}

void getPseudoHome() {
    getCurDir();
    strcpy(pseudoHome, currentDir);
}

int main(){

	printf("Welcome to Quash.... \n \n");
	
	getPseudoHome();
	strcpy(lastCD, pseudoHome);
    totalNoOfJobs = 0;
    noOfForeProcesses = 0;
    while (1){

		// Check if any child process terminated
        signal(SIGCHLD, sigchildHandler);
        //signal(SIGINT, signalControl);
        //signal(SIGTSTP, signalControl);

        getCurDir();
        printPrompt();
        getQuashInput();
        commandHandler();

   }
   return(0);
}


