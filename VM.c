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
    int segPT;
    PageTable *pt;
    
    segPT = shmget(program_pid, 256*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR);
    pt = (PageTable *) shmat(segPT, 0, 0);
    
//    (physical page number)*512 + offset

    
    
    
    
    
}
