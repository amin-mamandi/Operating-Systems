#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS  3

typedef struct thread_args {
  int tid;
  int inc;
  int loop;
} thread_args ;

int count = 0;

/*
 * This routine will be executed by each thread we choose to create.
 * The routine a new thread will execute is given as an arguent to the
 * pthread_create() call.
 */
void *inc_count(void *arg)
{
  int i,loc;
  thread_args *my_args = (thread_args*) arg;

  loc = 0;
  for (i = 0; i < my_args->loop; i++) {
    /*
     * How many machine instructions are required to increment count
     * and loc. Where are these variables stored? What implications
     * does their repsective locations have for critical section
     * existence and the need for Critical section protection?
     * Consider using atomic operation(s) here.
     */
    count = count + my_args->inc;
    loc = loc + my_args->inc;
  }
  printf("Thread: %d finished. Counted: %d\n", my_args->tid, loc);
  free(my_args);
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int i, loop, inc;
  struct thread_args *targs;
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;

  if (argc != 3) {
    printf("Usage: ./ptcount_atomic LOOP_BOUND INCREMENT\n");
    exit(0);
  }

  /*
   * First argument is how many times to loop. The second is how much
   * to increment each time.
   */
  loop = atoi(argv[1]);
  inc = atoi(argv[2]);

  /* For portability, explicitly create threads in a joinable state */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  /* Create each thread using pthread_create.  The start routine for
   * each thread should be inc_count. The attribute object should be
   * attr. You should pass as the thread's sole argument the populated
   * targs struct. Note we create a different copy of it for each
   * thread.
   */
  for (i = 0; i < NUM_THREADS; i++) {
    targs = malloc(sizeof(thread_args));
    targs->tid = i;
    targs->loop = loop;
    targs->inc = inc;
    /* Make call to pthread_create here */
  }

  /* Wait for all threads to complete using pthread_join.  The threads
   * do not return anything on exit, so the second argument is NULL
   */
  for (i = 0; i < NUM_THREADS; i++) {
    /* Make call to pthread_join here */
  }

  printf ("Main(): Waited on %d threads. Final value of count = %d. Done.\n",
          NUM_THREADS, count);

  /* Clean up and exit */
  pthread_attr_destroy(&attr);
  pthread_exit (NULL);
}

