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

