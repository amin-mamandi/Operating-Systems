#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#define SIZE 1000000
#define PERMISSIONS 0644
#define ll long long int

//Globals
char *inputGiven;
char *listOfArgs[SIZE];
char *listOfCommands[SIZE];
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

void getCurrentDirectory() {
    if(getcwd(currentDir, SIZE) == NULL) {
        perror("");
        exit(0);
    }
    return;
}

void getPseudoHome() {
    getCurrentDirectory();
    strcpy(pseudoHome, currentDir);
}

int ifAddTelda() {
     if (strcmp(currentDir, pseudoHome) == 0) {
         strcpy(directory, "~");
     }
}

int checkRedirection(int totalArgsInEachCommand, char *listOfArgs[]) {
         for (int i = 0; i < totalArgsInEachCommand; i++) {
                 if (strcmp(listOfArgs[i], "<") == 0)
                         return 1;
                 else if(strcmp(listOfArgs[i], ">") == 0)
                         return 1;
                 else if(strcmp(listOfArgs[i], ">>") == 0 )
                         return 1;
         }
         return 0;
}

void updateRelativePathToMove() {
    
    // Add ~ if needed.
    ifAddTelda();

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
}

void printPrompt(){
	strcpy(directory, " ");
	getCurrentDirectory();
	updateRelativePathToMove();
	printf(" %s > ", directory);
}

void getQuashInput(){
	inputGiven = (char *)malloc(SIZE);

    // Checking memory allocation error
    if(inputGiven == NULL){
        printf("Oops! Memory Error!\n");
        return;
    }

    // Taking Input
    char * checkError = fgets(inputGiven, SIZE, stdin);

    //int checkError = gets(inputGiven);
    if (checkError < 0) {
        printf("Oops! Input Error!\n");
        return;
    } 
    // Condition for CtrlD
    else if (checkError == 0) {
        printf("\n");
        exit(0);
    }
    

    // Checking the clear condition
    if (strcmp(inputGiven, "clear") == 0) {
        printf("\033[H\033[J");
    }
    strcpy(copyOfInput, inputGiven);
}

void tokenizer(char *token[], char *s, char *delimParameter,  int *total){
	int index = 0;
    	token[0] = strtok(s, delimParameter);
    	while(token[index]!=NULL){
        	token[++index] = strtok(NULL, delimParameter);
    	}
    	// Returns the total no. of commands
    	*total = index;
}

int checkPiping(char* listOfArgs[], int totalArgsInEachCommand) {
	for (int i = 0; i < totalArgsInEachCommand; i++) {
        	if (strcmp(listOfArgs[i], "|") == 0)
            		return 1;
    	}
    	return 0;
}

// This comapres the 2 structs of jobs array
int comparator(const void *p, const void *q)
{
    return strcmp(((job*)p)->jobsNames, ((job*)q)->jobsNames);
}

// This is the function for the command - jobs
void jobs(int totalArgsInEachCommand, char *listOfArgs[]) {

    if(totalArgsInEachCommand > 2) {
        printf("Too many arguments!\n");
    }
    else {

        // Sort the array of structs
        qsort(myJobsTemp, totalNoOfJobs, sizeof(myJobsTemp[0]), comparator);

        // When the input command = jobs
        if (totalArgsInEachCommand == 1) {
            for(int i = 0; i < totalNoOfJobs; i++){
                if (myJobsTemp[i].jobsStatus == -1)
                    continue;

                char procFile[1000];
                sprintf(procFile, "/proc/%d/stat", myJobsTemp[i].pid);
                char status;
                FILE  *procfd = fopen(procFile, "r");
                if (procfd == NULL)
                    continue;
                fscanf(procfd, "%*d %*s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d", &status);

                fclose(procfd);
                printf("[%d] ",myJobsTemp[i].jobsIndex + 1);
                if(status == 'T'){
                    printf("Stopped ");
                }
                else{
                    printf("Running ");
                }

                printf("%s [%d]\n", myJobsTemp[i].jobsNames, myJobsTemp[i].pid);
            }
        } else {
            // When the flags are also passed ie -r and -s

            // Running
            if (strcmp(listOfArgs[1], "-r") == 0) {

                for(int i = 0; i < totalNoOfJobs; i++){
                    if (myJobsTemp[i].jobsStatus == -1)
                        continue;

                    char procFile[1000];
                    sprintf(procFile, "/proc/%d/stat", myJobsTemp[i].pid);
                    char status;
                    FILE  *procfd = fopen(procFile, "r");
                    if (procfd == NULL)
                        continue;
                    fscanf(procfd, "%*d %*s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d", &status);

                    fclose(procfd);
                    if(status == 'S'){
                        printf("[%d] ", myJobsTemp[i].jobsIndex + 1);
                        printf("Running ");
                        printf("%s [%d]\n", myJobsTemp[i].jobsNames, myJobsTemp[i].pid);
                    }

                }
            }

            // Stopped
            else if (strcmp(listOfArgs[1], "-s") == 0) {

                for(int i = 0; i < totalNoOfJobs; i++){
                    if (myJobsTemp[i].jobsStatus == -1)
                        continue;

                    char procFile[1000];
                    sprintf(procFile, "/proc/%d/stat", myJobsTemp[i].pid);
                    char status;
                    FILE  *procfd = fopen(procFile, "r");
                    if (procfd == NULL)
                        continue;
                    fscanf(procfd, "%*d %*s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d", &status);

                    fclose(procfd);
                    if(status == 'T'){
			printf("Completed: ");
                        printf("[%d] ", myJobsTemp[i].jobsIndex + 1);
			printf("[%d]  %s & \n", myJobsTemp[i].pid, myJobsTemp[i].jobsNames);
                    }

                }
            }
            else {
                // Check error
                printf("Invalid arguments!\n");

            }
        }
    }
}

// When the child is called
int fgChildHandler(int totalArgsInEachCommand, char *repeatArgs[]) {
    // Make the child process as the leader of the new group of processes.
    // Setting the process ID of child = Process ID of group
    setpgid(0, 0);

    // Removing the last '&' as we need the name
    repeatArgs[totalArgsInEachCommand] = NULL;

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


void fgParentHandler(int pid, char *repeatArgs[], int totalArgsInEachCommand) {
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
        for (int i = 1; i < totalArgsInEachCommand - 1; i++) {
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
        for (int i = 1; i < totalArgsInEachCommand-1; i++) {
            strcat(myJobs[totalNoOfJobs].jobsNames, " ");
            strcat(myJobsTemp[totalNoOfJobs].jobsNames, " ");
            strcat(myJobs[totalNoOfJobs].jobsNames, listOfArgs[i]);
            strcat(myJobsTemp[totalNoOfJobs].jobsNames, listOfArgs[i]);
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

// This function calls the command which is not explicitly written by me
void foregroundProcess(long long int totalArgsInEachCommand, char *repeatArgs[]) {
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);


    // Forking child process
    int pid = fork();

    // Checking for error
    if (pid < 0) {
        printf("Unable to fork");
        return;
    }
    // Sending the child process to background
    else if (pid == 0) {
        fgChildHandler(totalArgsInEachCommand, repeatArgs);

    }

    // Means PID > 0, i.e. the parent's process
    else {

        fgParentHandler(pid, repeatArgs, totalArgsInEachCommand);

    }

}

void piping(char *command, char* listOfArgs[], long long totalArgsInEachCommand) {
	
	// printf("%s\n", command);
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
            		if(dup2(fd[1], STDOUT_FILENO) < 0) {
                		printf("error in dup2\n");
                	return;
            	}
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
            		if(dup2(fd[1], STDOUT_FILENO) < 0) {
                		printf("error in dup2\n");
                		return;
            		}
            	close(fd[1]);

        	}

        	// Fork the process
        	int returnedFork = fork();
        	int stat;
        	if(returnedFork < 0) {
            		printf("failed to fork\n");
        	}
        	else if(returnedFork == 0) {
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

void cd(int numArgs, char *commandArgument) {

    // Throw an error if no. of Arguments to the cd command > 2
    /*if (numArgs > 2) {
        printf("Error : too many arguments have been passed!");
        return;
    }*/

    // Means we have to move to the home directory as only cd is passed
    /*if (numArgs == 1) {

        // Get the current directory
        getCurrentDirectory();
        strcpy(pseudoHome, currentDir);
        if(chdir(pseudoHome) < 0){
            perror("cd ");
            return;
        }
        strcpy(lastCD, currentDir);
    }*/

    // Means we have 2 arguments to the command.
    //else {
        // Case - Move to the pseudoHome
        if(strcmp(commandArgument, "~") == 0) {
            if(chdir(pseudoHome) < 0) {
                perror("cd ");
                return;
            }
            strcpy(lastCD, currentDir);
            getCurrentDirectory();
        }

        // Case - When we have to store the last cd as well.
        /*else if (strcmp(commandArgument, "-") == 0) {
            printf("%s\n", lastCD);
            if(chdir(lastCD) < 0) {
                perror("cd ");
                return;
            }
            strcpy(lastCD, currentDir);
        }*/

        // Else move to the directory specified
        else {
            if (chdir(commandArgument) < 0) {
                perror("cd ");
                return;
            }
            strcpy(lastCD, currentDir);
            getCurrentDirectory();
        }

   // }
    return;
}


// For checking if a process ended or not.
void sigchildHandler() {
    pid_t pid;
    int status;

    pid = waitpid(-1, &status, WNOHANG);

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
                        printf("%s with pid %d exited normally with exit status %d\n",   myJobs[i].jobsNames, myJobs[i].pid, status);

                    break;
                }
            }
        }
    }
    return;
}

// This is the signal controller for CTRL+C and CTRL+Z
void signalControl(int signal)
{
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

// This is the function for running a background process, i.e a command ending with &
void backgroundProcess(long long totalArgsInEachCommand, char *listOfArgs[]) {
    int forkReturnedValue = fork();
    if(forkReturnedValue < 0) {
        fprintf(stderr, "Unable to fork\n");
        return;
    }

    if(forkReturnedValue == 0) {
        setpgrp();
        listOfArgs[totalArgsInEachCommand - 1] = NULL;
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        int execcvpReturnedValue = execl(listOfArgs[0], listOfArgs, NULL);
        if(execcvpReturnedValue < 0){
            fprintf(stderr, "Invalid command!\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    else {
        int len = strlen(listOfArgs[0]);
        for (int i = 1; i < totalArgsInEachCommand - 1; i++) {
            len += strlen(listOfArgs[i]);
        }
        myJobs[totalNoOfJobs].pid = forkReturnedValue;
        myJobsTemp[totalNoOfJobs].pid = forkReturnedValue;

        myJobs[totalNoOfJobs].jobsNames = malloc(len * sizeof(char));
        myJobsTemp[totalNoOfJobs].jobsNames = malloc(len * sizeof(char));

        if(listOfArgs[0] == NULL){
            fprintf(stderr, "Memory Error!\n");
        }

        strcpy(myJobs[totalNoOfJobs].jobsNames, listOfArgs[0]);
        strcpy(myJobsTemp[totalNoOfJobs].jobsNames, listOfArgs[0]);
        for (int i = 1; i < totalArgsInEachCommand-1; i++) {
            strcat(myJobs[totalNoOfJobs].jobsNames, " ");
            strcat(myJobsTemp[totalNoOfJobs].jobsNames, " ");
            strcat(myJobs[totalNoOfJobs].jobsNames, listOfArgs[i]);
            strcat(myJobsTemp[totalNoOfJobs].jobsNames, listOfArgs[i]);
        }

        myJobs[totalNoOfJobs].jobsStatus = 1;
        myJobsTemp[totalNoOfJobs].jobsStatus = 1;

        myJobs[totalNoOfJobs].jobsIndex = totalNoOfJobs;
        myJobsTemp[totalNoOfJobs].jobsIndex = totalNoOfJobs;

        totalNoOfJobs++;

        fprintf(stderr, "Background job started: %d %d  \n",totalNoOfJobs, myJobs[totalNoOfJobs-1].pid);
        return;
    }
}

int redirectionHandler(int totalArgsInEachCommand, char *listOfArgs[]) {
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

void commandHandler() {
    long long int noOfCommands = 0;
    tokenizer(listOfCommands, inputGiven, "\n", &noOfCommands);
    tokenizer(listOfCommands, inputGiven, "#", &noOfCommands);
    for (int i = 0; i < noOfCommands; i++) {
        char tempStr[10000];
        strcpy(tempStr, listOfCommands[i]);

        long long int totalArgsInEachCommand = 0;
	
        tokenizer(listOfArgs, listOfCommands[i], " \t", &totalArgsInEachCommand);

        // Check piping
        if(checkPiping(listOfArgs, totalArgsInEachCommand) == 1){ //tokenize pipe commands
			piping(tempStr, listOfArgs, totalArgsInEachCommand);
		}
        else {
            // Check redirection
            int returnedRedirectionValue = checkRedirection(totalArgsInEachCommand, listOfArgs);
            if (returnedRedirectionValue == 1) {
                // printf("Yes\n");
                redirectionHandler(totalArgsInEachCommand, listOfArgs);
            }

            // If the list is empty then simply return
            else {

                if (totalArgsInEachCommand == 0 || listOfArgs[0] == NULL)
                    return;

                // Checking the background process (if at last we have &)
                else if(strcmp(listOfArgs[totalArgsInEachCommand - 1],"&")==0){
                    backgroundProcess(totalArgsInEachCommand, listOfArgs);
                     return;
                }

                // Check for cd.
                else if(strcmp(listOfArgs[0], "cd") == 0) {
                    cd(totalArgsInEachCommand, listOfArgs[1]);
                }


                // Check for pwd
                else if(strcmp(listOfArgs[0], "pwd") == 0) {
                    if (totalArgsInEachCommand != 1) {
                        printf("Error : too many arguments have been passed!");
                        return;
                    }
		    char myPwd[SIZE];
		    if(getcwd(myPwd, SIZE) == NULL) {
			    perror("");
			    exit(0);
		    }
		    printf("%s\n", myPwd);
		    return;
		}

                // Check for echo
                else if(strcmp(listOfArgs[0], "echo") == 0) {
                        if (totalArgsInEachCommand > 1) {
				for(int i = 1; i < totalArgsInEachCommand; i++) {
					printf("%s ", listOfArgs[i]);
			       	}
		       	}
			printf("\n");
			return;
                }

		// Check for jobs
                else if(strcmp(listOfArgs[0], "jobs") == 0) {
                    jobs(totalArgsInEachCommand, listOfArgs);
                }

		// Check for bg
               // else if(strcmp(listOfArgs[0], "bg") == 0) {
                //    bg(totalArgsInEachCommand, listOfArgs);
               // }

                // Check for fg
                //else if(strcmp(listOfArgs[0], "fg") == 0) {
                 //   fg(totalArgsInEachCommand, listOfArgs);
                //}

                // Check for ls
                else if(strcmp(listOfArgs[0], "ls") == 0) {
			int pid = fork();
			if(pid == 0)
				execl("/bin/ls", "ls", NULL);
			else
				wait(NULL);
		}
   
                // Check for exit
                else if(strcmp(listOfArgs[0], "exit") == 0) {
                    exit(0);
                }

		else if (strcmp(listOfArgs[0], "quit") == 0){
			exit(0);
		}

		//export
		//else if (){
		
		//}

		//add # as comment
		//


                else {
                    foregroundProcess(totalArgsInEachCommand, listOfArgs);
                }
            }
        }
            // check if any child process terminated
             //signal(SIGCHLD, sigchildHandler);
        }


    return;
}

int main(){

	printf("Welcome to Quash.... \n \n");
	
	getPseudoHome();
	strcpy(lastCD, pseudoHome);
    	// make total no. of processes = 0
    	totalNoOfJobs = 0;
    	noOfForeProcesses = 0;
	while (1){
		
		// Check if any child process terminated
        	//signal(SIGCHLD, sigchildHandler);
        	// Signal Handlers for Ctrl+C and Ctrl+Z
        	signal(SIGINT, signalControl);
        	signal(SIGTSTP, signalControl);

		getCurrentDirectory();
		printPrompt();

		getQuashInput();
		commandHandler();
	
	}
	return(0);
}


