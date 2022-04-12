#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>

int AgentPID;

void catch_sigint(int val){
    kill(AgentPID,SIGINT);
    exit(0);
}


int main(int arc, char **argv){

    // Escrever PID no file "Agent.pid"
    int myPid = getpid();
    printf("Initializing Controller|PID: %i\n",myPid);
    int std_in = dup(0);
    close(0);
    int fd = open("Agent.pid",O_RDONLY,0666);
    scanf("%i",&AgentPID);
    printf("Searching for PID: %i\n",AgentPID);
    close(fd);

    dup(std_in);

    if(kill(AgentPID,0) == 0) printf("Agent found.\n");
    else{
        printf("Error: no agent found.\n");
        return 0;
    }


    signal(SIGINT, catch_sigint);

    char c[2];

    while(1){
        printf("\nChoose a signal to send [1: HUP; 15: TERM]: \n");
        
        memset(c,' ',5);
        scanf("%s",c);
        
        if(c[0]=='1'){
            if(c[1]=='5'){
                printf("\nSIGTERM\n");
                kill(AgentPID,SIGTERM);
                exit(0);
            }
            else{
                printf("\nSIGUP\n");
                kill(AgentPID,SIGHUP);
            }
        }
    }

    
    return 0;
}
