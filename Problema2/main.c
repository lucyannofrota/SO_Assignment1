#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <errno.h> // [MUD]


// Seguinte Codigo é baseado no codigo do problema 1. As mudanças realizadas estão sinalizadas com o comentario [MUD]


#define MY_PIPE_NAME "PFIFO" // [MUD]

#define MY_SOCKET_NAME "SKT"

int processo1(int *childStatus){

    // Fazendo Fork
    int myPID = fork();
    if(myPID == -1) perror("Failed to fork\n");
    if(myPID == 0){ // Selecionando processo filho
        // Pretende-se redirecionar o output para o socket criado
        printf("Processo 1 |PID: %i\n",getpid());

        // Verificando se o socket ja existe
        struct stat stat_sock;
        if(stat(MY_SOCKET_NAME, &stat_sock) == 0){
            printf("O socket ja existe!\n");
            if(unlink(MY_SOCKET_NAME) == -1) perror("Unlink Error\n");
            else printf("Socket deletado!\n");
        }

        // Criando Socket
        int my_socket = socket(AF_UNIX, SOCK_STREAM, AF_LOCAL);
        if (my_socket == -1) perror("Socket Error");
        else printf("Criando novo Socket!\n");
        struct sockaddr_un socket_address;

        socket_address.sun_family = AF_LOCAL;
        strncpy (socket_address.sun_path, MY_SOCKET_NAME, sizeof(socket_address.sun_path) - 1);
        if(bind(my_socket, &socket_address, sizeof(socket_address)) == -1) perror("Bind Error");

        if (listen (my_socket, 1) == -1)
            perror ("Listen Error");

        int connection = accept(my_socket, NULL, NULL);
        if (connection < 0) perror("Accept() failed");
        else{
            printf("Server| Incoming Connection\n");

            close(1); // Fechar porta de escrita
            dup2(connection,1); // Redirecionar porta de escrita para socket
            execl("/bin/find","find",".","-type","f","-ls",NULL);
        }

        // Fechar Conexões abertas 
        close(my_socket);
        close(connection);
        unlink(MY_SOCKET_NAME);
        exit(1);
    }
    else{ // Processo pai
        return myPID;
    }
}

int processo2(int *childStatus){ // [MUD]
    // Fazendo Fork
    int myPID = fork();
    if(myPID == -1) perror("Failed to fork\n");
    if(myPID == 0){ // Selecionando processo filho
        // Pretende-se redirecionar o input para o socket do servidor e redirecionar o output para o pipe2

        printf("Processo 2 |PID: %i\n",getpid());

        // Inicializando Socket
        int my_socket = socket(AF_UNIX, SOCK_STREAM, AF_LOCAL);
        if (my_socket == -1) perror("Socket Error");
        struct sockaddr_un socket_address;

        socket_address.sun_family = AF_LOCAL;
        strncpy (socket_address.sun_path, MY_SOCKET_NAME, sizeof(socket_address.sun_path) - 1);


        close(0); // Fechar porta de leitura
        if(connect(my_socket, &socket_address, sizeof(socket_address)) == -1) perror("Connect Error");
        sleep(1); //Sleep para garantir que o processo 1 enviará as informações antes que o processo 2 tente fazer a leitura
        printf("Client| Connecting\n");
        dup2(my_socket,0);

        close(1); // Fechar porta de Escrita // [MUD]
        dup2(open(MY_PIPE_NAME,O_WRONLY),1); // Redirecionar porta de escrita // [MUD]
        execl("/bin/cut","cut","-c","2-",NULL);

        exit(1);
    }
    else{ // Processo pai
        return myPID; 
    }
}

int processo3(int *childStatus){ // [MUD]
    // Fazendo Fork
    int myPID = fork();
    if(myPID == -1) perror("Failed to fork\n");
    if(myPID == 0){ // Selecionando processo filho
        printf("Processo 3 |PID: %i\n",getpid());
        // Pretende-se redirecionar o input para o pipe2 e redirecionar o output para o file: "file.txt"
        close(0); // Fechar porta de Lietura // [MUD]
        dup2(open(MY_PIPE_NAME,O_RDONLY),0); // Redirecionar porta de leitura // [MUD]

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
    if(myPID == -1) perror("Failed to fork\n");
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

int main(int arc, char **argv){
    printf("Problema 2\n");

    printf("Processo Pai inicializando processos |PID: %i\n",getpid());


    // Criando Named Pipe
    unlink(MY_PIPE_NAME); // [MUD]
    if(mkfifo(MY_PIPE_NAME,0666) == -1){ // [MUD]
        perror("Named Pipe Creation"); // [MUD]
    } 


    int childStatus1; processo1(&childStatus1); // Chamando Processo 1
    int childStatus2; processo2(&childStatus2); // Chamando Processo 2 // [MUD]
    int childStatus3; processo3(&childStatus3); // Chamando Processo 3 // [MUD]

    // Esperando que os 3 processos terminem
    wait(&childStatus1);
    sleep(1); // Sleep necessario para que o servidor possa inicializar o socket
    wait(&childStatus2);
    wait(&childStatus3);

    // Executando o ultimo processo
    int childStatus4; processo4(&childStatus4); // Chamando Processo 4
    wait(&childStatus4);
    printf("Done!\n");

    return 0;
}
