#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
// #include <semaphore.h>

#define MAXBUF 5

// sem_t sem;

void initialize_table(int *table){
    int i;
    for(i = 0; i < MAXBUF; i++){
        table[i] = i+1;
    }
}

void save_table(const int *table){
    int fd = open("filetable.bin",O_CREAT|O_WRONLY|O_TRUNC,0666);
    write(fd,table,MAXBUF*sizeof(int));
    close(fd);
}

void load_table(int *table){
    int fd = open("filetable.bin",O_RDONLY,0666);
    read(fd,table,MAXBUF*sizeof(int));
    close(fd);
}

void print_table(const int *table){
    int i;
    for(i = 0; i < MAXBUF; i++){
        printf("%i\n",table[i]);
    }
}


int processo_filho(int *childStatus,int pai_PID){
    // Fazendo Fork
    int myPID = fork();
    if(myPID == -1) perror("Failed to fork\n");
    if(myPID == 0){ // Selecionando processo filho
        int filho_table[MAXBUF];
        load_table(filho_table);
        
        print_table(filho_table);

        printf("Filho: %i\n",getpid());

        sleep(5);
        pause();

        // sem_wait(sem);

        // while(kill(pai_PID,0) == 0){
        //     sleep(1);
        // }
        // if(kill(pai_PID,0) == 0) printf("Agent found.\n");
        // else{
        //     printf("Error: no agent found.\n");
        
        //     return 0;
        // }
        printf("The child process is terminating.\n");
        // sem_destroy(sem);
    }
    else{ // Processo pai
        return myPID; 
    }
}

int main(int arc, char **argv){

    // Inicializando a tabela
    int table[MAXBUF];
    initialize_table(table);

    // Salvar a tabela no file "filetable.bin"
    save_table(table);

    // Criando Semaforo
    // sem_init(&sem, 1, 0);

    // Criando Processo Filho
    int pai_PID = getpid();
    int status_filho; int filho_PID = processo_filho(&status_filho,pai_PID);
    // printf("asdsa\n");
    kill(filho_PID,0);
    printf("The parent process is terminating.\n");
    // sem_post(&sem);
    return 0;
}






