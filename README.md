# Operating-Systems

This is where I put all the materials and homeworks for EECS 678

**Lab 1:**

Name : Amin Mamandipoor
Student ID: 3073224

************************************

**Valgrind.c**

I have fixed the errors related to memory allocation by putting free() in some of the lines.

First error was uninitialized value, so I initialized it by zero. 

``
uninitialized_variable = 0;  
``

Then, the other ones were basically freeing up the allocated memory. 

a pointer that point to another pointer, so in order to free up the allocated memory, we should free up the pointer to pointer before free up the first pointer.

``
free (*definitely_lost);
free (definitely_lost);
``

Next one is really easy to free up: 

``
free (still_reachable);
``

Finally, the possibly_lost, as it has beed added by 4, so we have the wrong address. So, we need to decrease it by 4 and then free it up:

``
free(possibly_lost - 4);
``

******************************************

**bugs.c**

Growing the array by 2:

``
char* sentence[8];
``

As the echo amd echoohce functions are just printing an array and they stop whenever they encountered a NULL variable, so I added two NULL variables at the begining and at the end of the sentence array:

``
bug_info.sentence[0] = NULL;

...

bug_info.sentence[7] = NULL;
``

Also, there was an uninitialized variable:

``
bug_info.num_bugs_on_mars = 0;
``

Moreover, as we are giving a variable to the bug_info.sentence[3] twice, so we need to free up the first one before assigning a new thing to it :

``
free(bug_info.sentence[3]);
bug_info.sentence[3] = strdup("beautiful");

...

free(bug_info.sentence[0]);
free(bug_info.sentence[1]);
free(bug_info.sentence[2]);
free(bug_info.sentence[3]);
free(bug_info.sentence[4]);
free(bug_info.sentence[5]);
free(bug_info.sentence[6]);
free(bug_info.sentence[7]);	
...
``

So, we are done here!

















 
