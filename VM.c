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

void trans(int program_pid, unsigned int page_index, unsigned int offset, char rw){
//    VPN is 8 bits,p offset is 24
    PageTable *pt, *PageTablepfault;
    int *Px;
    int segPT, segPx, memID;
    int i=0;
    int frameNumber;
    unsigned int physicaladdr;
	int segPfault;
    
    segPx = shmget(5555, 4*sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    Px = (int *) shmat(segPx, 0, 0);
    
    if (program_pid == Px[0])
        memID = 1111;
    else if(program_pid == Px[1])
        memID = 2222;
    else if(program_pid == Px[2])
        memID = 3333;
    else if(program_pid == Px[3])
        memID = 4444;
    
    segPT = shmget(memID, 256*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR);
    printf("Programa no trans: %d\n", memID);

    pt = (PageTable *) shmat(segPT, 0, 0);
    
//  Virtual -> Physical
	/*Traduzir page_index pra frame_number*/
	//memória contém uma instância de PageTable para guardar informações
	if ((segPfault = shmget(9999, sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
		perror("shmget Pfault");
		exit(1);
	}
	PageTablepfault = (PageTable *) shmat(segPfault, 0, 0);
	if (PageTablepfault == (PageTable *)(-1)) {
		perror("shmat Pfault");
		exit(1);
	}
	if((frameNumber = pt[page_index].frameNum)==-1||pt[page_index].vazio){
		/*PAGEFAULT*/		
		PageTablepfault->pid = getpid();
		PageTablepfault->page_index = page_index;
		PageTablepfault->frameNum = -1;
		PageTablepfault->rw = rw;
		PageTablepfault->vazio = 1;
	}
	/*Irá chamar o GM, independente se deu pageFault ou n
	o GM precisa atualizar o contador do frame e mudar o bit b_written se for "w"*/
	printf("%d mandando SIGUSR1 para o GM\n", memID);
	kill(Px[4],SIGUSR1);
	raise(SIGSTOP);
	//agora já há um frameNum acossiado, se não havia antes
	frameNumber = pt[page_index].frameNum;
	physicaladdr = (frameNumber<<24) + offset;
	printf("%d, 0x%X, %c\n",getpid(),physicaladdr,rw);
    
    shmdt (pt);
    shmctl (segPT, IPC_RMID, 0);
    shmdt (Px);
    shmctl (segPx, IPC_RMID, 0);
    shmdt (PageTablepfault);
    shmctl (segPfault, IPC_RMID, 0);
}
