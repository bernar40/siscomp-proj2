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
#define IPCMNI 262144
void sleep1sec(int signal);
void sleep2sec(int signal);
void pageFault(int signal);
PageFrame **mem_fisica;
PageTable *PageTablep1;
PageTable *PageTablep2;
PageTable *PageTablep3;
PageTable *PageTablep4;
minHeap frameHeap;
int main(void) {
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
                    int segP1, segP2, segP3, segP4, segPx;
                    int *Px;
                    int k = 0;

                    if ((segP1 = shmget(1111, 64*sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmget P1");
                        exit(1);
                    }
                    if ((segP2 = shmget(2222, 64*sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmget P1");
                        exit(1);
                    }
                    if ((segP3 = shmget(3333, 64*sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmget P1");
                        exit(1);
                    }
                    if ((segP4 = shmget(4444, 64*sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmget P1");
                        exit(1);
                    }
                    if ((segPx = shmget(5555, 4*sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmget Px");
                        exit(1);
                    }

                    PageTablep1 = (PageTable *) shmat(segP1, 0, 0);
                    PageTablep2 = (PageTable *) shmat(segP2, 0, 0);
                    PageTablep3 = (PageTable *) shmat(segP3, 0, 0);
                    PageTablep4 = (PageTable *) shmat(segP4, 0, 0);
                    Px = (int *) shmat(segPx, 0, 0);
                    mem_fisica = (PageFrame **) malloc(256*sizeof(PageFrame*));
                    
                    if (PageTablep1 == (PageTable *)(-1)) {
                        perror("shmat P1");
                        exit(1);
                    }
                    if (PageTablep2 == (PageTable *)(-1)) {
                        perror("shmat P2");
                        exit(1);
                    }
                    if (PageTablep3 == (PageTable *)(-1)) {
                        perror("shmat P3");
                        exit(1);
                    }
                    if (PageTablep4 == (PageTable *)(-1)) {
                        perror("shmat P4");
                        exit(1);
                    }
                    
                    Px[0] = P1;
                    Px[1] = P2;
                    Px[2] = P3;
                    Px[3] = P4;


                    while(k<256){
						mem_fisica[k] = (PageFrame*)malloc(sizeof(PageFrame));
                        mem_fisica[k]->vazio = 0;
                        mem_fisica[k]->value = 0;
                        PageTablep1[k].vazio = 0;
                        PageTablep2[k].vazio = 0;
                        PageTablep3[k].vazio = 0;
                        PageTablep4[k].vazio = 0;
                        k++;
                    }
					//Constroi Heap de minimos para os frames
					frameHeap = initMinHeap();
					buildMinHeap(frameHeap,mem_fisica,256);
					
                    signal( SIGUSR1, pageFault);
					
                    for(EVER){
                        printf("GM executando...\n");
                        sleep(1);
//                        checar se alguma coisa foi posta em alguma das tabelas e por no frame?
                    }
                    
//                    shmdt (PageTablep1);
//                    shmdt (PageTablep2);
//                    shmdt (PageTablep3);
//                    shmdt (PageTablep4);
//                    shmctl (segP1, IPC_RMID, 0);
//                    shmctl (segP2, IPC_RMID, 0);
//                    shmctl (segP3, IPC_RMID, 0);
//                    shmctl (segP4, IPC_RMID, 0);

                    
                }
                else{
                    // ###########    Processo P4 --- Filho 4     ###########
					//signal(SIGUSR1, sleep1sec);					
					//signal(SIGUSR2, sleep2sec);                   
					FILE *fp4 = fopen("compilador.log", "r");
                    int pidP4 = getpid();
                    while(fscanf(fp4, "%x %c", &addr, &rw)){
                        i = addr >> 24;
                        o = addr & 0x00FFFFFF;
                        trans(pidP4, i, o, rw);
                    }
                    printf("compilador.log Done\n");

                }
            }
            else{
                // ###########    Processo P3 --- Filho 3     ###########
				//signal(SIGUSR1, sleep1sec);				
				//signal(SIGUSR2, sleep2sec);                
				FILE *fp3 = fopen("compressor.log", "r");
                int pidP3 = getpid();
                while(fscanf(fp3, "%x %c", &addr, &rw)){
                    i = addr >> 24;
                    o = addr & 0x00FFFFFF;
                    trans(pidP3, i, o, rw);
                }
                printf("compressor.log Done\n");

            }
        }
        else{
            // ###########    Processo P2 --- Filho 2     ###########
			//signal(SIGUSR1, sleep1sec);			
			//signal(SIGUSR2, sleep2sec);            
			FILE *fp2 = fopen("matriz.log", "r");
            int pidP2 = getpid();
            while(fscanf(fp2, "%x %c", &addr, &rw)){
                i = addr >> 24;
                o = addr & 0x00FFFFFF;
                trans(pidP2, i, o, rw);
            }
            printf("matriz.log Done\n");

        }
    }
    else{
        // ###########    Processo P1 --- Filho 1     ###########
		//signal(SIGUSR1, sleep1sec);		
		//signal(SIGUSR2, sleep2sec);        
		FILE *fp1 = fopen("simulador.log", "r");
        int pidP1 = getpid();
        while(fscanf(fp1, "%x %c", &addr, &rw)){
            i = addr >> 24;
            o = addr & 0x00FFFFFF;
            trans(pidP1, i, o, rw);
        }
        printf("simluador.log Done\n");

    }
    return 0;
}

void sleep1sec(int signal){
	sleep(1);
}
void sleep2sec(int signal){
	sleep(2);
}
void pageFault(int signal){//#EDITING
	PageFrame *min = deleteNode(&frameHeap);
	int sleep_time;	//Quanto o processo que deu page fault deverá dormir
	//kill(,SIGSTOP) <--que pid deu pageFault?
	//Caso a página eleita tenha sido modificada
	if (min->vazio){
		sleep_time = 1;
		//Inserir no frame
		min->vazio = 0;
	}
	else{
		if(min->b_written){
			sleep_time = 2;
		}
		else{
			sleep_time = 1;
		}
		//Trocar o conteúdo do frame
		min->value = 0;
		min->vazio = 0;
		//Atualizar tabela do processo que perdeu
	}
	//Atualizar tabela do processo que ganhou


}
