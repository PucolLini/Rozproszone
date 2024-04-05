#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(){
    int input = -1;
    printf("Podaj liczbe:");
    scanf("%d", &input);

    char* commands[]={
	    "wget https://wordpress.org/latest.zip",
        "ping -c 20 wikipedia.com",
        "sleep 40",
        "find / -type f -size -100M > wynik.txt",
        "sleep 10",
    };
    int commandsNumber = sizeof(commands) / sizeof(commands[0]);
    int pids[input];

    for(int i=0; i<input; i++){
        int pid = fork(); //stworzenie nowego procesu
        if(pid == -1){
            perror("Error");
        }
        else if(pid==0){
            execlp("sh", "sh", "-c", commands[i%commandsNumber], NULL);
        }
        else{
            pids[i]=pid;
        }
    }
    for(int i=0; i<commandsNumber; i++){
        int status;
        //wait(&status);
        waitpid(pids[i], &status, 0);
        printf("Zamknięto proces %d o pid: %d\n",i, pids[i]);
    }
    printf("Wszystkie procesy potomne zakonczyly sie.\n");
    return 0;
}