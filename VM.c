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
//    VPN is 8 bits, offset is 24
    PageTable *pt;
    int *Px;
    int segPT, segPx, memID;
    int i=0;
    int frameNumber;
    unsigned int physicaladdr;
    
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
    
//    printf("Program %d is executing trans\n", memID);
    segPT = shmget(memID, 64*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR);
    printf("SegPT: %d\n", segPT);

    pt = (PageTable *) shmat(segPT, 0, 0);
    
//  Virtual -> Physical
	//frameNumber = page_index;
	/*Traduzir page_index pra frame_number*/
	frameNumber = pt[frame_number];
	physicaladdr = frameNumber<<24 + offset;
    
    
    
    /*Se der pageFault, essa é a memória compartilhada a ser criada
	int segPfault;
    if ((segPx = shmget(9999, sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
		perror("shmget Pfault");
		exit(1);
	}
	PageTablepfault = (PageTable *) shmat(segPfault, 0, 0);
	if (PageTablepfault == (PageTable *)(-1)) {
		perror("shmat Pfault");
		exit(1);
	}
    
    
    
    */
    
    
    
//  Add to page table
/*
    while(i<64){
        if(pt[i].vazio == 0){
            pt[i].page_index = page_index;
            pt[i].frameNum = frameNumber;
            pt[i].rw = rw;
            pt[i].vazio = 1;
            printf("PID: %d -- i: %d -- FN: %d -- bit: %c -- vazio: %d\n", program_pid, pt[i].page_index, pt[i].frameNum, pt[i].rw, pt[i].vazio = 1);
            break;
        }
        i++;
    }*/
    
    shmdt (pt);
    shmctl (segPT, IPC_RMID, 0);
}
