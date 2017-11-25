#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>

struct mem_fisica {
    int pid;
    unsigned int page_index;
}Page_Frame;

int main(int argc, const char * argv[]) {
    int P1, P2, P3, P4;
    unsigned int addr;
    char rw;
    P1 = fork();
    if(P1){
        P2 = fork();
        if(P2){
            P3 = fork();
            if (P3){
                P4 = fork();
                if(P4){
                    // ###########    Processo GM --- Pai     ###########

                }
                else{
                    // ###########    Processo P4 --- Filho 4     ###########
                    FILE *fp1 = fopen("compiler.log", "r");
                    while(fscanf(fp1, "%x %c", &addr, &rw)){
                    }
                }
            }
            else{
                // ###########    Processo P3 --- Filho 3     ###########
                FILE *fp1 = fopen("compressor.log", "r");
                while(fscanf(fp1, "%x %c", &addr, &rw)){
                }

            }
        }
        else{
            // ###########    Processo P2 --- Filho 2     ###########
            FILE *fp1 = fopen("matrix.log", "r");
            while(fscanf(fp1, "%x %c", &addr, &rw)){
            }
        }
    }
    else{
        // ###########    Processo P1 --- Filho 1     ###########
        FILE *fp1 = fopen("simulation.log", "r");
        while(fscanf(fp1, "%x %c", &addr, &rw)){
        }
    }
    return 0;
}
