#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>

char buffer[50];

void catch_sigup(int val){
    // É criado um novo file descriptor em cada chamada para garantir que será
    // lida a informação mais recente de "text.in"
    printf("\n\nReading text.in:\n");
    close(0);
    int fd = open("text.in",O_RDONLY,0666);
    int ret = 0;
    char c;
    while(read(fd,&c,1*sizeof(char)) != 0){
        printf("%s",&c);
    }
    printf("\n");
    close(fd);
}

void catch_sigterm(int val){
    printf("Process terminating...\n");
    exit(0);
}

int main(int arc, char **argv){

    // Escrever PID no file "Agent.pid"
    int myPid = getpid();
    printf("Initializing Agent|PID: %i\n",myPid);
    int std_out = dup(1);
    close(1);
    int fd1 = open("Agent.pid",O_CREAT | O_WRONLY,0666);
    printf("%i\n",myPid);
    close(fd1);
    dup(std_out);

    // Setup dos sinais
    signal(SIGTERM, catch_sigterm);
    signal(SIGHUP, catch_sigup);

    // Ler instruções do file "text.in"
    kill(getpid(),SIGHUP);

    while(1){
        pause();
    }
    
    return 0;
}






