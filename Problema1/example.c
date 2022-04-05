#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

char *msg[2]={"ping","pong"};


int main(int argc, char **argv){
    char buffer[20];
    int nread,aux,status;
    int fd1[2];
    int fd2[2];
    int i;
    // int status;
    pipe(fd1);
    pipe(fd2);
    if(fork()==0){
        /* Child proc. */
        close(fd1[0]);
        close(fd2[1]);
        for(i=0;i<10;i++){
            write(fd1[1],msg[0],strlen(msg[0])+1);
            read(fd2[0],buffer,strlen(msg[1])+1);
            printf("[child]: %s \n",buffer);
        }
    }
    else{
    /* Parent proc. */
        close(fd1[1]);
        close(fd2[0]);
        for(i=0;i<10;i++){
            // Ver a influencia do tamanho da mensagem
            read(fd1[0],buffer,strlen(msg[0])+1);
            printf("[parent]: %s\n",buffer);
            write(fd2[1],msg[1],strlen(msg[1])+1);
        }
        wait(&status);
    }
    return(0);
}