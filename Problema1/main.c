#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
    //Command
    find . -type f -ls | cut -c 2- | sort -n -k 7 >file.txt ; less <file.txt
    find - processo 1
    cut - processo 2
    sort - processo 3
    less - processo 4
*/



int processo1(void){
    execl("/bin/find","find",".","-type","f","-ls",NULL);
    return 0;
}

int processo2(void){
    // execl("/bin/find","find",".","-type","f","-ls",NULL);
    return 0;
}

int processo3(void){
    // execl("/bin/find","find",".","-type","f","-ls",NULL);
    return 0;
}

int processo4(void){
    // execl("/bin/find","find",".","-type","f","-ls",NULL);
    return 0;
}

int main(int arc, char **argv){
    printf("Problema 1\n");

    printf("Processo Pai realizando um fork|PID: %i\n",getpid());
    int childStatus; int myPID = fork();
    if(myPID == -1){
        perror("Failed to fork\n");
        return 1;
    }
    if(myPID == 0){ // Selecionando processo filho
        printf("Processo filho|PID: %i\n",getpid());
        processo1();
    }
    else{ // Processo pai
        printf("Processo pai|PID: %i\n",getpid());
        printf("Esperando o processo filho\n");
        wait(&childStatus);
    }
    return 0;
}
