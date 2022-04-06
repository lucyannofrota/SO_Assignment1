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


// O codigo foi separado em processos para simpleificar o entendimento.
// Cada processo se encarrega de fazer a sua funcao e de acionar outro processo se necessario

int processo1(int *fd){
    // Pretende-se redirecionar o output para o pipe1
    close(fd[0]); // Fechar porta de leitura
    dup2(fd[1],1); // Redirecionar porta de escrita
    close(fd[1]); // Fechar porta de escrita
    execl("/bin/find","find",".","-type","f","-ls",NULL);
    exit(1);
}

int processo2(int *fd1,int *fd2){
    // Pretende-se redirecionar o input para o pipe1 e redirecionar o output para o pipe2

    // fd1
    close(fd1[1]); // Fechar porta de escrita
    dup2(fd1[0],0); // Redirecionar porta de leitura
    close(fd1[0]); // Fechar porta de leitura

    // fd2
    close(fd2[0]); // Fechar porta de leitura
    dup2(fd2[1],1); // Redirecionar porta de escrita
    close(fd2[1]); // Fechar porta de escrita
    execl("/bin/cut","cut","-c","2-",NULL);
    exit(1);
}

int processo3(int *fd){
    // Pretende-se redirecionar o input para o pipe2 e redirecionar o output para o file: "file.txt"
    close(fd[1]); // Fechar porta de escrita
    dup2(fd[0],0); // Redirecionar porta de leitura
    close(fd[0]); // Fechar porta de leitura

    close(1); // Fechar porta de escrita
    int fdf = open("file.txt",O_WRONLY|O_CREAT,0666); // Abrir/criar o ficheiro em mode de escrita
    execl("/bin/sort","sort","-n","-k","7",NULL);
    exit(1);
}

int processo4(void){
    // Pretende-se redirecionar o input para o file: "file.txt" 
    close(0); // Fechar porta de leitura
    int fdf = open("file.txt",O_RDONLY,0666); // Abrir/criar o ficheiro em mode de escrita
    dup2(fdf,0); // Redirecionar porta de leitura
    execl("/bin/less","less","",NULL);
    exit(1);
}

int main(int arc, char **argv){
    printf("Problema 1\n");

    printf("Processo Pai realizando um fork|PID: %i\n",getpid());

    // Criando Pipes
    int fd_1[2]; pipe(fd_1);
    int fd_2[2]; pipe(fd_2);

    // Fazendo Fork
    int childStatus1; int myPID1 = fork();
    if(myPID1 == -1){
        perror("Failed to fork\n");
        return 1;
    }
    if(myPID1 == 0){ // Selecionando processo filho
        printf("Processo 1 |PID: %i\n",getpid());
        close(fd_2[0]);
        close(fd_2[1]);
        processo1(fd_1);
    }
    else{ // Processo pai
        printf("Processo Pai realizando um fork|PID: %i\n",getpid());
        int childStatus2; int myPID2 = fork();
        if(myPID2 == -1){
            perror("Failed to fork\n");
            return 1;
        }
        if(myPID2 == 0){ // Selecionando processo filho
            printf("Processo 2 |PID: %i\n",getpid());
            processo2(fd_1,fd_2);
        }
        else{ // Processo pai
            close(fd_1[0]);
            close(fd_1[1]);
            printf("Processo Pai realizando um fork|PID: %i\n",getpid());
            int childStatus3; int myPID3 = fork();
            if(myPID3 == -1){
                perror("Failed to fork\n");
                return 1;
            }
            if(myPID3 == 0){ // Selecionando processo filho
                printf("Processo 3 |PID: %i\n",getpid());
                processo3(fd_2);
            }
            else{ // Processo pai
                close(fd_2[0]);
                close(fd_2[1]);
                // printf("Processo Pai realizando um fork|PID: %i\n",getpid());
                printf("Esperando os processos filho\n");
                wait(&childStatus1);
                wait(&childStatus2);
                wait(&childStatus3);
                printf("Processo 4 |PID: %i\n",getpid());
                processo4();
            }
        }
    }
    return 0;
}
