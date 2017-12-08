//
//  VM.c
//  siscomp lab2
//
//  Created by Bernardo Ruga on 11/25/17.
//  Copyright © 2017 Bernardo Ruga. All rights reserved.
//

#include "VM.h"

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

int add_pos(int pos){
	int np = pos + 1;
	if (np < 100)
		return np;
	else{
		np = 0;
		return np;
	}
}

void trans(int program_pid, unsigned int page_index, unsigned int offset, char rw){
//    VPN is 8 bits,p offset is 24
    PageTable *pt, *pFault;
    int *Px, *vet;
    Idx *posicao;
    int segPT, segPx, segVet, segIdx, memID=0;
    int i;
    int frameNumber;
    unsigned int physicaladdr;
	int segPfault;
	if ((segPx = shmget(5555, 5*sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR)) == -1) {
		perror("shmget segPx trans");
		exit(1);
	}
    if ((segVet = shmget(6666, 100*sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmget vet gm");
        treat_ctrl_C(1);
    }
    if ((segIdx = shmget(7777, sizeof(Idx), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmget idx gm");
        treat_ctrl_C(1);
    } 

    Px = (int *) shmat(segPx, 0, 0);
    vet = (int *) shmat(segVet, 0, 0);
    posicao = (Idx *) shmat(segIdx, 0, 0);

    i = posicao->index_VM;
    posicao->index_VM = add_pos(posicao->index_VM);
    shmdt(posicao);
    if (program_pid == Px[0]){
        memID = 1111;
        vet[i] = 0;
    }
    else if(program_pid == Px[1]){
        memID = 2222;
        vet[i] = 1;
    }
    else if(program_pid == Px[2]){
        memID = 3333;
        vet[i] = 2;
    }
    else if(program_pid == Px[3]){
        memID = 4444;
        vet[i] = 3;
    }

    if ((segPT = shmget(memID, 256*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR)) == -1) {
		perror("shmget segPT trans");
		exit(1);
	}

    pt = (PageTable *) shmat(segPT, 0, 0);
    
//  Virtual -> Physical
	/*Traduzir page_index pra frame_number*/
	//memória contém uma instância de PageTable para guardar informações
	if ((segPfault = shmget(9999, 4*sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
		perror("shmget Pfault trans");
		exit(1);
	}
	pFault = (PageTable *) shmat(segPfault, 0, 0);
	
	frameNumber = pt[page_index].frameNum;

	if(frameNumber==-1||pt[page_index].vazio){
		/*PAGEFAULT*/		
		pFault[vet[i]].pid = getpid();
		pFault[vet[i]].page_index = page_index;
		pFault[vet[i]].frameNum = -1;
		pFault[vet[i]].rw = rw;
		pFault[vet[i]].vazio = 1;
	}
	shmdt(vet);
	/*Irá chamar o GM, independente se deu pageFault ou n
	o GM precisa atualizar o contador do frame e mudar o bit b_written se for "w"*/
	//printf("%d mandando SIGUSR1 para o GM\n", memID);

	

	kill(Px[4],SIGUSR1);
	raise(SIGSTOP);
	//agora já há um frameNum acossiado, se não havia antes
	frameNumber = pt[page_index].frameNum;
	physicaladdr = (frameNumber<<24) + offset;
	printf("%d, 0x%X, %c\n",getpid(),physicaladdr,rw);
    
    shmdt (pt);
    shmdt (Px);
    shmdt (pFault);
}