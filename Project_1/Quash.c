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
	char * token;
	const char s[2] = " ";

	//signals
	struct sigaction sa_init;
	memset(&sa_init, 0, sizeof(sa_init));
	sa_init.sa_handler = sig_init;
	sa_init.sa_flags = 0;
	sigaction(SIGINT, &sa_init, NULL);

	while(1) {
		printf("\n");
		fflush(stdout);
		fgets(cmd, sizeof(cmd), stdin);
		cmd[strcspn(cmd, "\n")] = 0;
		token = strtok(cmd, s);

			if (strcmp(token, "ls") == 0){
				token = strtok(NULL, s);
				//printf(" %s\n", token);
				pid = fork();
				if (pid == 0){
					printf("\n %s", token);
					if (strcmp(token, "-a") != 0)
						execl("/bin/ls", "ls", NULL);
					else
						execl("/bin/ls", "ls", "-a", NULL);
				} else{
					wait(NULL);
				}
			}
			/*else if ( strcmp(token, "ls -a") == 0){
                        	pid = fork();
                        	if (pid == 0){
                               		printf("\n");
                                	execl("/bin/ls", "ls", "-a", NULL);
                        	} else{
                                	wait(NULL);
                        	}
			}*/	
			else if( strcmp(token, "pwd") == 0){
				pid = fork();
				if(pid == 0){
					printf("\n");
					execl("/bin/pwd", "pwd", NULL);
				}
				else {
					wait(NULL);
				}
			}
              		else if( strcmp(token, "echo") == 0){
                        		pid = fork();
                        		if(pid == 0){
						token = strtok(NULL, s);
						execl("/bin/echo", "echo", strtok(strtok(NULL, '"'), '"'), NULL);
					}
                        		else {
                                		wait(NULL);
					}
			}
	}
	
	return 0;
}
