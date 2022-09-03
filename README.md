# Operating-Systems

**Lab 1**

Name : Amin Mamandipoor

Student ID: 3073224

************************************

**Valgrind.c**

I have fixed the errors related to memory allocation by putting a couple of  <em> free() </em> before <em> malloc() </em>

First error was uninitialized value, so I initialized it by zero. 

```
uninitialized_variable = 0;  
```

Then, the other ones were basically freeing up the allocated memory. 

1. A pointer that point to another pointer, so in order to free up the allocated memory, we should free up the pointer to pointer before free up the first pointer.

```
free (*definitely_lost);
free (definitely_lost);
```

2. Next one is really easy to free up: 

```
free (still_reachable);
```

3. Finally, the <em>possibly_lost</em> has been added by 4, so we have the wrong address(increamented). We need to decrease it by 4 and then free it up:

```
free(possibly_lost - 4);
```

******************************************

**bugs.c**

1. Since we need to add a <em>NULL</em> to the end of the sentence (because we want to detect the end of thesentence), we should  growing the array by 1:

```
char* sentence[7];
```

2. Also, there was an uninitialized variable:

```
bug_info.num_bugs_on_mars = 0;
```

3. Fixing the problem with the <em>echooche</em> function. Since we want to print the value stored in the address ,we need to add <em>*</em> to the iter. So, it prints out the value of the iter instead of its address. 
```
for (iter = strs ; *iter != NULL; iter++){
    printf("%s ", *iter);
  }
```

3. Basically before iterating over the strs, we are storing the stored value in the address before strs (complex sentence). So we know when to stop printing it out reverse. 

```
for (; *iter != *stop_beginning ; iter--)
    printf("%s ", *iter);
```

4. Finally, we are freeing up the allocated memories.

```
free(bug_info.sentence[0]);
free(bug_info.sentence[1]);
free(bug_info.sentence[3]);
free(bug_info.sentence[4]);
free(bug_info.sentence[5]);
free(bug_info.sentence[6]);
```


















 
