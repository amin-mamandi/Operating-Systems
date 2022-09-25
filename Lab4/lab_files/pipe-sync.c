/*
  pipe-sync.c: Use Pipe for Process Synchronization
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// Question: Update this program to synchronize execution of the parent and
// child processes using pipes, so that we are guaranteed the following
// sequence of print messages:
// Child line 1
// Parent line 1
// Child line 2
// Parent line 2


int main()
{
  char *s, buf[1024];
  int ret, stat;
  s  = "Use Pipe for Process Synchronization\n";

  /* create pipe */
  int fd[2];
  pipe(fd);

  ret = fork();
  if (ret == 0) {
	  
    close(fd[0]);
    write(fd[1], "Child line 1\n", 15);
    close(fd[1]);
    
    sleep(1);
    /* child process. */
       
    printf("Child line 2\n");
    
    return stat;

  } else {

    /* parent process */

    close(fd[1]);
    char s [30];
    read(fd[0], s, sizeof(s));
    printf("%s", s);
    printf("Parent line 1\n");
    
    wait(&stat);

    printf("Parent line 2\n");

    //wait(&stat);
  }
}
