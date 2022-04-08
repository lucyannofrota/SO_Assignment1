#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void catch_int(int sig_num){
/* re-set the signal handler again to catch_int, for next time */
    // signal(SIGINT, catch_int); /* May be unnecessary ... */
    /* and print the message */
    printf("Pf nao vale a pena fazersdasd Ctrl-C...");
    fflush(stdout);
}


int main(){
    /* and somewhere later in the code.... */
    /* set the INT (Ctrl-C) signal handler to 'catch_int' */
    signal(SIGINT, catch_int);
    /* now, lets get into an infinite loop of doing nothing. */
    for ( ; ; )
    pause();
    return 0;
}