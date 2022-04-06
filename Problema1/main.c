#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

/*
    //Command
    find . -type f -ls | cut -c 2- | sort -n -k 7 >file.txt ; less <file.txt
    find - processo 1
    cut - processo 2
    sort - processo 3
    less - processo 4
*/


// O codigo foi separado em processos para simpleificar o entendimento.
// Cada processo se encarrega de fazer um fork para poder utilizar o execl() e retorna PID do processo filho

int processo1(int *childStatus,int *fd){

    // Fazendo Fork
    int myPID = fork();
    if(myPID == -1){
        perror("Failed to fork\n");
        return 1;
    }
    if(myPID == 0){ // Selecionando processo filho
        // Pretende-se redirecionar o output para o pipe1

        printf("Processo 1 |PID: %i\n",getpid());
        close(fd[0]); // Fechar porta de leitura
        dup2(fd[1],1); // Redirecionar porta de escrita
        close(fd[1]); // Fechar porta de escrita
        execl("/bin/find","find",".","-type","f","-ls",NULL);
        exit(1);
    }
    else{ // Processo pai
        return myPID;
    }
}

int processo2(int *childStatus, int *fd1,int *fd2){
    // Fazendo Fork
    int myPID = fork();
    if(myPID == -1){
        perror("Failed to fork\n");
        return 1;
    }
    if(myPID == 0){ // Selecionando processo filho
        // Pretende-se redirecionar o input para o pipe1 e redirecionar o output para o pipe2

        printf("Processo 2 |PID: %i\n",getpid());
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
    else{ // Processo pai
        return myPID; 
    }
}

int processo3(int *childStatus, int *fd){
    // Fazendo Fork
    int myPID = fork();
    if(myPID == -1){
        perror("Failed to fork\n");
        return 1;
    }
    if(myPID == 0){ // Selecionando processo filho
        printf("Processo 3 |PID: %i\n",getpid());
        // Pretende-se redirecionar o input para o pipe2 e redirecionar o output para o file: "file.txt"
        close(fd[1]); // Fechar porta de escrita
        dup2(fd[0],0); // Redirecionar porta de leitura
        close(fd[0]); // Fechar porta de leitura

        close(1); // Fechar porta de escrita
        int fdf = open("file.txt",O_WRONLY|O_CREAT,0666); // Abrir/criar o ficheiro em mode de escrita
        execl("/bin/sort","sort","-n","-k","7",NULL);
        exit(1);
    }
    else{ // Processo pai
        return myPID; 
    }
}

int processo4(int *childStatus){
    // Fazendo Fork
    int myPID = fork();
    if(myPID == -1){
        perror("Failed to fork\n");
        return 1;
    }
    if(myPID == 0){ // Selecionando processo filho
        printf("Processo 4 |PID: %i\n",getpid());
        // Pretende-se redirecionar o input para o file: "file.txt" 
        close(0); // Fechar porta de leitura
        int fdf = open("file.txt",O_RDONLY,0666); // Abrir/criar o ficheiro em mode de escrita
        dup2(fdf,0); // Redirecionar porta de leitura
        execl("/bin/less","less",NULL);
        exit(1);
    }
    else{ // Processo pai
        return myPID; 
    }
}

#define MY_SOCKET_NAME "SKT"

int main(int arc, char **argv){
    printf("Problema 1\n");

    printf("Processo Pai inicializando processos |PID: %i\n",getpid());

    // Verificando se o socket ja existe
    struct stat stat_sock;
    if(stat(MY_SOCKET_NAME, &stat_sock) == 0){
        printf("O socket ja existe!\n");
        if(unlink(MY_SOCKET_NAME) == -1){
            perror("unlink\n");
        }
        else printf("Socket deletado!\n");
    }

    // Criando Socket
    int my_socket = socket(AF_UNIX, SOCK_STREAM, AF_LOCAL);
    if (my_socket == -1){
        perror("Socket Error");
    }
    else{
        printf("Criando novo Socket!\n");
        struct sockaddr_un socket_address;

        socket_address.sun_family = AF_LOCAL;
        // *socket_address.sun_path = MY_SOCKET_NAME;
        // memset (&socket_address, AF_UNIX, sizeof (struct sockaddr_un));
        strncpy (socket_address.sun_path, MY_SOCKET_NAME, sizeof(socket_address.sun_path) - 1);
        printf("Socket:\nFamily: %i|Name: %s\n",socket_address.sun_family,socket_address.sun_path);
        if(bind(my_socket, &socket_address, sizeof(socket_address)) == -1){
            perror("Bind Error");
        }
    }

    if (listen (my_socket, 1) == -1)
        error ("listen");

    // Criando Pipes
    int fd_1[2]; pipe(fd_1);
    int fd_2[2]; pipe(fd_2);

    int childStatus1; processo1(&childStatus1,fd_1); // Chamando Processo 1
    int childStatus2; processo2(&childStatus2,fd_1,fd_2); // Chamando Processo 2
    close(fd_1[0]); close(fd_1[1]); // Fechando o pipe que existe entre o processo 1 e processo 2
    int childStatus3; processo3(&childStatus3,fd_2); // Chamando Processo 3
    close(fd_2[0]); close(fd_2[1]); // Fechando o pipe que existe entre o processo 2 e processo 3

    // Esperando que os 3 processos terminem
    wait(&childStatus1);
    wait(&childStatus2);
    wait(&childStatus3);

    // Executando o ultimo processo
    // int childStatus4; processo4(&childStatus4); // Chamando Processo 4
    // wait(&childStatus4);
    printf("Done!\n");

    return 0;
}
