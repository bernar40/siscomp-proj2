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
#define EVER ;;
#define DEBUG 1
#define IPCMNI 262144
#define DEFAULT_DELTA 30
void sleep1sec(int signal);
void sleep2sec(int signal);
void pageFault(int signal);
PageFrame **mem_fisica;
PageTable *PageTablep1;
PageTable *PageTablep2;
PageTable *PageTablep3;
PageTable *PageTablep4;
minHeap frameHeap;
int *Px;
int main(int argc, int *argv[]) {
    int P1, P2, P3, P4;
    unsigned int i, o;
    unsigned int addr;
    char rw;
    int *delta_miliseconds;
    P1 = fork();
    if(P1){
        P2 = fork();
        if(P2){
            P3 = fork();
            if (P3){
                P4 = fork();
                if(P4){
                    // ###########    Processo GM --- Pai     ###########
                    int segP1, segP2, segP3, segP4, segPx, segPfault;
					
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
                    if ((segPx = shmget(5555, 5*sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
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
                    Px[4] = getpid();


                    while(k<256){//inicializa tudo
						mem_fisica[k] = (PageFrame*)malloc(sizeof(PageFrame));
                        mem_fisica[k]->self_index = k;
                        mem_fisica[k]->vazio = 1;
						mem_fisica[k]->pid = -1;
						mem_fisica[k]->page_index = -1;
                        mem_fisica[k]->value = 0;
						mem_fisica[k]->b_written = 0;
						
                        PageTablep1[k].vazio = 1;
						PageTablep1[k].frameNum = -1;
						PageTablep1[k].rw = 0;
						PageTablep1[k].page_index = k;
						PageTablep1[k].pid = Px[0];
						
						
                        PageTablep2[k].vazio = 1;
						PageTablep2[k].frameNum = -1;
						PageTablep2[k].rw = 0;
						PageTablep2[k].page_index = k;
						PageTablep2[k].pid = Px[1];
						
                        PageTablep3[k].vazio = 1;
						PageTablep3[k].frameNum = -1;
						PageTablep3[k].rw = 0;
						PageTablep3[k].page_index = k;
						PageTablep3[k].pid = Px[2];
						
                        PageTablep4[k].vazio = 1;
						PageTablep4[k].frameNum = -1;
						PageTablep4[k].rw = 0;
						PageTablep4[k].page_index = k;
						PageTablep4[k].pid = Px[3];
						
                        k++;
                    }
					//Constroi Heap de minimos para os frames
					frameHeap = initMinHeap();
					buildMinHeap(frameHeap,mem_fisica,256);
					
                    signal( SIGUSR1, pageFault);
					//Lê, dos parâmetros dados, o delta que será utilizado para calcular a frequência de uso de um frame
					if(argc>1){
						#if DEBUG
							printf("\nargc>1,*delta_miliseconds = %d",*argv[1]);
						#endif
						*delta_miliseconds = *argv[1];
					}
					else{
						#if DEBUG
							printf("\nargc<=1,*delta_miliseconds = DEFAULT_DELTA");
						#endif
						*delta_miliseconds = DEFAULT_DELTA;
					}
                    for(EVER){
                        printf("GM executando...\n");
                        sleep(1);
						//#EDITING
						
						/*Dado um certo tempo passado (de acordo com o enunciado), subtrai 1 de cada frame->value*/
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
					signal(SIGUSR1, sleep1sec);					
					signal(SIGUSR2, sleep2sec);                   
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
				signal(SIGUSR1, sleep1sec);				
				signal(SIGUSR2, sleep2sec);                
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
			signal(SIGUSR1, sleep1sec);			
			signal(SIGUSR2, sleep2sec);            
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
		signal(SIGUSR1, sleep1sec);		
		signal(SIGUSR2, sleep2sec);        
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
void pageFault(int signal){	
	if(DEBUG)
		printf("\nSIGUSR1 received by %d: Entered pageFault",getpid());

	//Shmem para pegar as informações do processo que teve pagefault
	int segPfault;
	int requsitador_pid, perdedor_pid;
	PageFrame *min = deleteNode(&frameHeap);
	PageTable *pFault;
    PageTable *pt_requsitador,*pt_perdedor;
	int segPT;

	if ((segPx = shmget(9999, sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
		perror("shmget Pfault");
		exit(1);
	}
	pFault = (PageTable *) shmat(segPfault, 0, 0);
	if (pFault == (PageTable *)(-1)) {
		perror("shmat Pfault");
		exit(1);
	}

	requsitador_pid = pFault->pid;
	//carrega a pt do processo que enviou sigfault
	if (requsitador_pid == Px[0])
        memID = 1111;
    else if(requsitador_pid == Px[1])
        memID = 2222;
    else if(requsitador_pid == Px[2])
        memID = 3333;
    else if(requsitador_pid == Px[3])
        memID = 4444;
    segPT = shmget(memID, 64*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR);
    printf("SegPT: %d\n", segPT);
    pt_requsitador = (PageTable *) shmat(segPT, 0, 0);
	
	
	if((pFault->frameNum>-1)&&(pFault->vazio)){
		//CASO PAGEFAULT
		
		//kill(requsitador_pid,SIGSTOP);
		//Caso haja um frame vazio
		if(min->vazio){
			if(DEBUG)
				printf("\n\tmin is empty");
			kill(requsitador_pid,SIGUSR1);
		}
		else{
			if(DEBUG)
				printf("\n\tmin is not empty");
				
				//carrega a pt do processo que perderá a página
				perdedor_pid = min->pid;
				if (perdedor_pid == Px[0])
					memID = 1111;
				else if(perdedor_pid == Px[1])
					memID = 2222;
				else if(perdedor_pid == Px[2])
					memID = 3333;
				else if(perdedor_pid == Px[3])
					memID = 4444;
				segPT = shmget(memID, 64*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR);
				printf("SegPT: %d\n", segPT);
				pt_perdedor = (PageTable *) shmat(segPT, 0, 0);
				//apaga da pt, a ligação com o frame
				pt_perdedor[min->page_index].frameNum = -1;
				pt_perdedor[min->page_index].rw = 0;
				pt_perdedor[min->page_index].vazio = 1;
			
			//Caso a página eleita tenha sido modificada
			if(min->b_written){
				if(DEBUG)
					printf("\n\t\tmin is written");
				kill(requsitador_pid,SIGUSR2);
			}
			else{
				if(DEBUG)
					printf("\n\t\tmin is not written");
				kill(requsitador_pid,SIGUSR1);
			}
		}
		//Inserir no frame
		min->page_index = pFault->page_index;
		min->value = 1;
		min->vazio = 0;
		min->b_written = (pFault->rw == "W" || pFault->rw == "w")?1:0;
		min->pid = requsitador_pid;
		#if DEBUG
			printf("\n\t\t\tpFault:");
			printf("\n\t\t\t\tpage_index = %d",pFault->page_index);
			printf("\n\t\t\t\tvalue = %d",pFault->value);
			printf("\n\t\t\t\tvazio = %d",pFault->vazio);
			printf("\n\t\t\t\tb_written = %d",pFault->b_written);
			printf("\n\t\t\t\tpid = %d",pFault->pid);
			printf("\n\t\t\tFrame:");
			printf("\n\t\t\t\tpage_index = %d",min->page_index);
			printf("\n\t\t\t\tvalue = %d",min->value);
			printf("\n\t\t\t\tvazio = %d",min->vazio);
			printf("\n\t\t\t\tb_written = %d",min->b_written);
			printf("\n\t\t\t\tpid = %d",min->pid);
		#endif
		//Atualizar tabela do processo que ganhou
		pt_requsitador[min->page_index].frameNum = min->self_index;
		pt_requsitador[min->page_index].rw = (pFault->rw == "W" || pFault->rw == "w")?"w":"r";
		pt_requsitador[min->page_index].vazio = 0;
	}
	else{
		//CASO NÃO É PAGEFAULT
		
		//se é write
		if(pFault->rw=="W"||pFault->rw=="w")
			mem_fisica[pFault->frameNum]->b_written=1;
		//soma 1 a valor
		mem_fisica[pFault->frameNum]->value++;
		//Garante que o processo retorne a executar
		kill(requsitador_pid,SIGCONT);
		
	}
	
}
void sleep1sec(int signal){
	printf("\n\tProc %d dormirá 1 seg.",getpid());
	sleep(1);
}
void sleep2sec(int signal){
	printf("\n\tProc %d dormirá 2 seg.",getpid());
	sleep(2);
}