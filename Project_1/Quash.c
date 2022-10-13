#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

//Globals
char command [100];

void sig_init (int sig_num) {
	char prompt [30];
	printf("\nReally?? Want to Exit? [Y/N] \n");
	fflush(stdout);
	fgets(prompt, sizeof(prompt), stdin);
	if (prompt[0] == 'N' || prompt[0] == 'n'){
		printf("Good, continue .... \n");
		fflush(stdout);
	}
	else {
		printf("Exiting ... \n");
		fflush(stdout);
		exit(0);
	}
}


int main()
{
	printf("\n Welcome to Quash.... \n");
	fflush(stdout);
	int pid;
	char cmd[30];

	//signals
	struct sigaction sa_init;
	memset(&sa_init, 0, sizeof(sa_init));
	sa_init.sa_handler = sig_init;
	sa_init.sa_flags = 0;
	sigaction(SIGINT, &sa_init, NULL);

	while(1) {
		printf("\n");
		char cmd[30];
		fgets(cmd, sizeof(cmd), stdin);
		cmd[strcspn(cmd, "\n")] = 0; 
		
		if (  strcmp(cmd, "ls") ==  0){ 
			pid = fork();
			if (pid == 0){
				printf("\n");
				execl("/bin/ls", "ls", NULL);
			} else{
				wait(NULL);
			}
		}
		else if ( strcmp(cmd, "ls -a") == 0){
                        pid = fork();
                        if (pid == 0){
                                printf("\n");
                                execl("/bin/ls", "ls", "-a", NULL);
                        } else{
                                wait(NULL);
                        }
		}	
		else if( strcmp(cmd, "pwd") == 0){
			pid = fork();
			if(pid == 0){
				printf("\n");
				execl("/bin/pwd", "pwd", NULL);
			}
			else {
				wait(NULL);
			}
		}
              else if( strcmp(cmd, "echo") == 0){
                        pid = fork();
                        if(pid == 0){
                                execl("/bin/echo", "echo",  NULL);
                        }
                        else {
                                wait(NULL);
                        }
                }

	}
	return 0;
}
