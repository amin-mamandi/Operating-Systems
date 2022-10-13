#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Globals
char command [100];

void signal_handler () {


}



int main()
{


       	/* ensure that the program keeps running to receive the signals */
       	for ( ;; )
	       	pause();
	return 0;

}
