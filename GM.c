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
#include "VM.h"
#define EVER ; ;

int main(int argc, const char * argv[]) {
    int P1, P2, P3, P4;
    unsigned int i, o;
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
                    int segmemFis, segP1, segP2, segP3, segP4;
                    int k = 0;
                    
                    segP1 = shmget(P1, 256*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR);
                    segP2 = shmget(P2, 256*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR);
                    segP3 = shmget(P3, 256*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR);
                    segP4 = shmget(P4, 256*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR);
                    segmemFis = shmget(5555, 64*sizeof(PageFrame), IPC_CREAT | S_IRUSR | S_IWUSR);
                    
                    PageTable *PageTablep1;
                    PageTablep1 = (PageTable *) shmat(segP1, 0, 0);
                    PageTable *PageTablep2;
                    PageTablep2 = (PageTable *) shmat(segP2, 0, 0);
                    PageTable *PageTablep3;
                    PageTablep3 = (PageTable *) shmat(segP3, 0, 0);
                    PageTable *PageTablep4;
                    PageTablep4 = (PageTable *) shmat(segP4, 0, 0);
                    
                    PageFrame *mem_fisica;
                    mem_fisica = (PageFrame *) shmat(segmemFis, 0, 0);

                    while(k<256){
                        mem_fisica[k].vazio=0;
                        k++;
                    }
                    for(EVER){
//                        checar se alguma coisa foi posta em alguma das tabelas e por no frame?
                    }
                    
                }
                else{
                    // ###########    Processo P4 --- Filho 4     ###########
                    FILE *fp4 = fopen("compilador.log", "r");
                    int pidP4 = getpid();
                    while(fscanf(fp4, "%x %c", &addr, &rw)){
                        i = addr >> 24;
                        o = addr & 0x00FFFFFF;
                        trans(pidP4, i, o, rw);
                    }
                }
            }
            else{
                // ###########    Processo P3 --- Filho 3     ###########
                FILE *fp3 = fopen("compressor.txt", "r");
                int pidP3 = getpid();
                while(fscanf(fp3, "%x %c", &addr, &rw)){
                    i = addr >> 24;
                    o = addr & 0x00FFFFFF;
                    trans(pidP3, i, o, rw);
                }

            }
        }
        else{
            // ###########    Processo P2 --- Filho 2     ###########
            FILE *fp2 = fopen("matriz.txt", "r");
            int pidP2 = getpid();
            while(fscanf(fp2, "%x %c", &addr, &rw)){
                i = addr >> 24;
                o = addr & 0x00FFFFFF;
                trans(pidP2, i, o, rw);
            }
        }
    }
    else{
        // ###########    Processo P1 --- Filho 1     ###########
        FILE *fp1 = fopen("simulador.txt", "r");
        int pidP1 = getpid();
        while(fscanf(fp1, "%x %c", &addr, &rw)){
            i = addr >> 24;
            o = addr & 0x00FFFFFF;
            trans(pidP1, i, o, rw);
        }
    }
    return 0;
}
