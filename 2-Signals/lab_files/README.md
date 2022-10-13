## Lab 2 - Report 


The most important thing about the signals is how to register them. 

We should specify the handler function for the specific signal, and also what other signals might be blocked or not blocked while we are handling a specific signal. 

So, for setting up the mask_set, we could either use sigfillset which blocks all other signals, or we could add the signals manually by sigaddset. 

The other thing that has been discussed was memset which only clear up the memory from the garbage data that might been there before. 

Furthermore, using pause() inside a loop will help us to run the program until we get a sigstp signall in order to exit from the program. 

I have also included a couple of lines of comments for the sigaction struct. It basically help us with the arguments it takes. 

