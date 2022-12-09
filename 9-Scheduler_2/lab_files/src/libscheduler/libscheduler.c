/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

priqueue_t queue;

int preemptive;
int numCores;
int (*comp) (const void *, const void *);
float waitingTime, turnaroundTime, responseTime;
int numJobs;
int currentTime;

/**
  Stores information making up a job to be scheduled including any statistics.
  You may need to define some global variables or a struct to store your job queue elements. 
*/
typedef struct _job_t
{
	int number;
  	int arrival_time;
  	int start_time;
  	int running_time;
  	int remaining_time;
  	int priority;
} job_t;

job_t** coreInUse;

int fcfs(const void *a, const void *b)
{
  job_t* joba = (job_t*)a;
  job_t* jobb = (job_t*)b;
  if(joba->number == jobb->number)
    return 0;
  return joba->arrival_time - jobb->arrival_time;
}

int sjf(const void *a, const void *b)
{
  job_t* joba = (job_t*)a;
  job_t* jobb = (job_t*)b;
  if(joba->number == jobb->number)
    return 0;
  int diff = joba->remaining_time - jobb->remaining_time;
  if(diff == 0)
    return joba->arrival_time - jobb->arrival_time;
  else
    return diff;
}

int pri(const void *a, const void *b)
{
  job_t* joba = (job_t*)a;
  job_t* jobb = (job_t*)b;
  if(joba->number == jobb->number)
    return 0;
  int diff = joba->priority - jobb->priority;
  if(diff == 0)
    return joba->arrival_time - jobb->arrival_time;
  else
    return diff;
}

int rr(const void *a, const void *b)
{
  job_t* joba = (job_t*)a;
  job_t* jobb = (job_t*)b;
  if(joba->number == jobb->number)
    return 0;
  return -1;
}


/**
  Initalizes the scheduler.
 
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{

	waitingTime = 0.0;
  	turnaroundTime = 0.0;
  	responseTime = 0.0;
 	numJobs = 0;
  	currentTime = 0;

  	numCores = cores;

  	coreInUse = malloc(sizeof(job_t) * cores);

  	int i = 0;
  	while(i < cores)
  	{
    		coreInUse[i] = 0;
    		i++;
  	}

  	switch(scheme)
  	{
    	case FCFS: comp = fcfs; preemptive = 0; break;
    	case SJF:  comp = sjf;  preemptive = 0; break;
    	case PSJF: comp = sjf;  preemptive = 1; break;
    	case PRI:  comp = pri;  preemptive = 0; break;
    	case PPRI: comp = pri;  preemptive = 1; break;
    	case RR:   comp = rr;   preemptive = 0; break;
  	}

  	priqueue_init(&queue,comp);
}


/**
  Called when a new job arrives.
 
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumption:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. 
 
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{

  deincrement_Remaining_Times(time);
  job_t* job = malloc(sizeof(job_t));
  job->number = job_number;
  job->arrival_time = time;
  job->start_time = -1;
  job->running_time = running_time;
  job->remaining_time = running_time;
  job->priority = priority;

  int core = are_Any_Cores_Idle();
  if(core != -1)
  {
    job->start_time = time;
    // printf("CHANGING START TIME ROFLCOPTER: job: %d arrival: %d start: %d\n",
    //         job->number, job->arrival_time, job->start_time);
    coreInUse[core] = job;
    return core;
  }

  if(preemptive)
  {
    core = get_Least_Preferential_Job(job);
    if(core > -1)
    {
      job_t* temp = coreInUse[core];
      if(time == temp->start_time)
      {
        temp->start_time = -1;
        // printf("CHANGING START TIME ROFLCOPTER: job: %d arrival: %d start: %d\n",
        //         temp->number, temp->arrival_time, temp->start_time);
      }
      job->start_time = time;
      // printf("CHANGING START TIME ROFLCOPTER: job: %d arrival: %d start: %d\n",
      //         job->number, job->arrival_time, job->start_time);
      coreInUse[core] = job;
      priqueue_offer(&queue,temp);
      return core;
    }
  }

  priqueue_offer(&queue,(void*)job);

  return -1;
}


/**
  Called when a job has completed execution.
 
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
 
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
	deincrement_Remaining_Times(time);

  job_t* finJob = coreInUse[core_id];
  numJobs++;
  waitingTime += (time - finJob->arrival_time - finJob->running_time);
  turnaroundTime += (time - finJob->arrival_time);
  responseTime+=(finJob->start_time - finJob->arrival_time);
  // printf("---Added %d to response time.\n",finJob->start_time - finJob->arrival_time);


  free(coreInUse[core_id]);
  coreInUse[core_id] = 0;

  if(priqueue_size(&queue) > 0)
  {
    job_t* job = (job_t*)priqueue_poll(&queue);
    if(job->start_time == -1)
    {
      job->start_time = time;
      // printf("CHANGING START TIME ROFLCOPTER: job: %d arrival: %d start: %d\n",
      //         job->number, job->arrival_time, job->start_time);
    }
    coreInUse[core_id] = job;
    return job->number;
  }

	return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
 
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator. 
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	deincrement_Remaining_Times(time);

  job_t* job = coreInUse[core_id];

  if(priqueue_size(&queue) > 0)
  {
    priqueue_offer(&queue,job);
    job = priqueue_poll(&queue);
    if(job->start_time == -1)
      job->start_time = time;
    coreInUse[core_id] = job;
  }
  return job->number;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	if(numJobs > 0)
  {
    return waitingTime/numJobs;
  }
	return 0.0;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	if(numJobs > 0)
  {
    return turnaroundTime/numJobs;
  }
	return 0.0;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	if(!preemptive && comp != rr)
    return waitingTime/numJobs;
  else
    return responseTime/numJobs;
}


/**
  Free any memory associated with your scheduler.
 
  Assumption:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
	priqueue_destroy(&queue);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)  
  
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{

	int x = 0;
  int size = priqueue_size(&queue);
  if(size == 0)
  {
    printf("Queue is empty");
    return;
  }
  while(x < size)
  {
    job_t* job = (job_t*)priqueue_at(&queue, x);
    printf("Index: %d Job Number:%d Arrival Time: %d Remaining Time: %d Priority: %d\n",
           x, job->number, job->arrival_time, job->remaining_time, job->priority);
    x++;
  }
}

int are_Any_Cores_Idle()
{
  int i = 0;
  while(i < numCores)
  {
    if(coreInUse[i] == 0)
      return i;
    i++;
  }
  return -1;
}

void deincrement_Remaining_Times(int time)
{
  int timeDifference = (time - currentTime);

  int i = 0;
  while(i < numCores)
  {
    if(coreInUse[i] != 0)
      coreInUse[i]->remaining_time -= timeDifference;
    i++;
  }
  currentTime = time;
}

int get_Least_Preferential_Job(void* job)
{
  job_t* currentJob = (job_t*)job;
  int core = -1;

  int i = 0;
  while(i < numCores)
  {
    if(comp(currentJob,coreInUse[i]) < 0)
    {
      core = i;
      currentJob = coreInUse[i];
    }
    i++;
  }
  return core;
}
