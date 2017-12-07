#include <stdio.h>
#include <time.h>
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
#include <pthread.h>
#include "VM.h"
#include "minHeap.h"

void pageFaultP1(int signal){
    // if(DEBUG)
    //     printf("SIGUSR1 received by %d: Entered pageFault\n",getpid());
    
    //Shmem para pegar as informações do processo que teve pagefault
    int segPfault;
    int memID;
    int *Py;
    int requsitador_pid, perdedor_pid;
    PageFrame *min = deleteNode(&frameHeap);
    PageTable *pFault;
    PageTable *pt_requsitador,*pt_perdedor;
    int segPy, segPT, segPTp;
    
    if ((segPy = shmget(5555, 5*sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmget segPy pf");
        treat_ctrl_C(1);
    }
    if ((segPfault = shmget(9999, sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmget segPfault pf");
        treat_ctrl_C(1);
    }

    Py = (int *) shmat(segPy, 0, 0);
    pFault = (PageTable *) shmat(segPfault, 0, 0);
    
    requsitador_pid = pFault->pid;
    //carrega a pt do processo que enviou sigfault
    if (requsitador_pid == Py[0])
        memID = 1111;
    else if(requsitador_pid == Py[1])
        memID = 2222;
    else if(requsitador_pid == Py[2])
        memID = 3333;
    else if(requsitador_pid == Py[3])
        memID = 4444;
    if(DEBUG)
        printf("SIGUSR1 received by %d: Entered pageFault\n",memID);
    segPT = shmget(memID, 256*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR);
    printf("Programa requisatando pFault: %d\n", memID);
    pt_requsitador = (PageTable *) shmat(segPT, 0, 0);


    printf("pFault->frameNum = %d\n", pFault->frameNum);
    printf("pFault->vazio = %d\n", pFault->vazio);

    if((pFault->frameNum == -1)&&(pFault->vazio)){
        //CASO PAGEFAULT
        printf("Entrando no caso pagefault\n");
        
        //Caso haja um frame vazio
        if(min->vazio){
            if(DEBUG)
                printf("\tmin is empty\n");
            kill(requsitador_pid,SIGCONT);
            kill(requsitador_pid,SIGUSR1);
        }
        else{
            if(DEBUG)
                printf("\tmin is not empty\n");
            
            //carrega a pt do processo que perderá a página
            perdedor_pid = min->pid;
            if (perdedor_pid == Py[0])
                memID = 1111;
            else if(perdedor_pid == Py[1])
                memID = 2222;
            else if(perdedor_pid == Py[2])
                memID = 3333;
            else if(perdedor_pid == Py[3])
                memID = 4444;  
			if ((segPTp = shmget(memID, 256*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR)) == -1) {
				perror("shmget segPTp pf");
				treat_ctrl_C(1);
    		}
            printf("Programa que perdera Frame: %d\n", memID);
            pt_perdedor = (PageTable *) shmat(segPTp, 0, 0);
            //apaga da pt, a ligação com o frame
            pt_perdedor[min->page_index].frameNum = -1;
            pt_perdedor[min->page_index].b_written = 0;
            pt_perdedor[min->page_index].vazio = 1;
            
            //Caso a página eleita tenha sido modificada
            if(min->b_written){
                if(DEBUG)
                    printf("\t\tmin is written\n");
                kill(requsitador_pid,SIGCONT);
                kill(requsitador_pid,SIGUSR2);
            }
            else{
                if(DEBUG)
                    printf("\t\tmin is not written\n");
                kill(requsitador_pid,SIGCONT);
                kill(requsitador_pid,SIGUSR1);
            }
        }
        //Inserir no frame
        min->page_index = pFault->page_index;
        min->value = 1;
        min->vazio = 0;
        min->b_written = (pFault->rw == 'W' || pFault->rw == 'w')?1:0;
        min->pid = requsitador_pid;

        //Atualizar tabela do processo que ganhou
        pt_requsitador[min->page_index].frameNum = min->self_index;
        pt_requsitador[min->page_index].rw = (pFault->rw == 'W' || pFault->rw == 'w')?'w':'r';
        pt_requsitador[min->page_index].vazio = 0;

#if DEBUG
        printf("\t\t\tpFault:\n");
        printf("\t\t\t\tpage_index = %d\n",pFault->page_index);
        printf("\t\t\t\tFrameNum = %d\n",pFault->frameNum);
        printf("\t\t\t\tvazio = %d\n",pFault->vazio);
        printf("\t\t\t\tb_written = %d\n",pFault->b_written);
        printf("\t\t\t\tpid = %d\n",pFault->pid);
        printf("\t\t\tFrame:\n");
        printf("\t\t\t\tpage_index = %d\n",min->page_index);
        printf("\t\t\t\tvalue = %d\n",min->value);
        printf("\t\t\t\tvazio = %d\n",min->vazio);
        printf("\t\t\t\tb_written = %d\n",min->b_written);
        printf("\t\t\t\tpid = %d\n",min->pid);
#endif
        
	insertNode(&frameHeap,min);
	heapify(&frameHeap,256);
        //printf("pt_requsitador[%d].frameNum = %d\n",  min->page_index, min->self_index);

    }
    else{
        //CASO NÃO É PAGEFAULT
        printf("Caso nao eh pageFault\n");
        
        //se é write
        printf("pFault->rw = %c\n", pFault->rw);
        printf("pFault->frameNum = %d\n", pFault->frameNum);
        if(pFault->rw=='W'||pFault->rw=='w')
            mem_fisica[pFault->frameNum]->b_written=1;
        //soma 1 a valor
        mem_fisica[pFault->frameNum]->value++;
	insertNode(&frameHeap,min);
        heapify(&frameHeap,256);
		//Garante que o processo retorne a executar
        kill(requsitador_pid,SIGCONT);
        
    }
    shmdt (pFault);
    //shmctl (segPfault, IPC_RMID, 0);
    shmdt (Py);
    //shmctl (segPy, IPC_RMID, 0);
    shmdt (pt_requsitador);
    shmdt (pt_perdedor);
    //shmctl (segPTp, IPC_RMID, 0); 
    //shmctl (segPT, IPC_RMID, 0);    
}

void pageFaultP2(int signal){
    // if(DEBUG)
    //     printf("SIGUSR1 received by %d: Entered pageFault\n",getpid());
    
    //Shmem para pegar as informações do processo que teve pagefault
    int segPfault;
    int memID;
    int *Py;
    int requsitador_pid, perdedor_pid;
    PageFrame *min = deleteNode(&frameHeap);
    PageTable *pFault;
    PageTable *pt_requsitador,*pt_perdedor;
    int segPy, segPT, segPTp;
    
    if ((segPy = shmget(5555, 5*sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmget segPy pf");
        treat_ctrl_C(1);
    }
    if ((segPfault = shmget(9999, sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmget segPfault pf");
        treat_ctrl_C(1);
    }

    Py = (int *) shmat(segPy, 0, 0);
    pFault = (PageTable *) shmat(segPfault, 0, 0);
    
    requsitador_pid = pFault->pid;
    //carrega a pt do processo que enviou sigfault
    if (requsitador_pid == Py[0])
        memID = 1111;
    else if(requsitador_pid == Py[1])
        memID = 2222;
    else if(requsitador_pid == Py[2])
        memID = 3333;
    else if(requsitador_pid == Py[3])
        memID = 4444;
    if(DEBUG)
        printf("SIGUSR1 received by %d: Entered pageFault\n",memID);
    segPT = shmget(memID, 256*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR);
    printf("Programa requisatando pFault: %d\n", memID);
    pt_requsitador = (PageTable *) shmat(segPT, 0, 0);


    printf("pFault->frameNum = %d\n", pFault->frameNum);
    printf("pFault->vazio = %d\n", pFault->vazio);

    if((pFault->frameNum == -1)&&(pFault->vazio)){
        //CASO PAGEFAULT
        printf("Entrando no caso pagefault\n");
        
        //Caso haja um frame vazio
        if(min->vazio){
            if(DEBUG)
                printf("\tmin is empty\n");
            kill(requsitador_pid,SIGCONT);
            kill(requsitador_pid,SIGUSR1);
        }
        else{
            if(DEBUG)
                printf("\tmin is not empty\n");
            
            //carrega a pt do processo que perderá a página
            perdedor_pid = min->pid;
            if (perdedor_pid == Py[0])
                memID = 1111;
            else if(perdedor_pid == Py[1])
                memID = 2222;
            else if(perdedor_pid == Py[2])
                memID = 3333;
            else if(perdedor_pid == Py[3])
                memID = 4444;  
			if ((segPTp = shmget(memID, 256*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR)) == -1) {
				perror("shmget segPTp pf");
				treat_ctrl_C(1);
    		}
            printf("Programa que perdera Frame: %d\n", memID);
            pt_perdedor = (PageTable *) shmat(segPTp, 0, 0);
            //apaga da pt, a ligação com o frame
            pt_perdedor[min->page_index].frameNum = -1;
            pt_perdedor[min->page_index].b_written = 0;
            pt_perdedor[min->page_index].vazio = 1;
            
            //Caso a página eleita tenha sido modificada
            if(min->b_written){
                if(DEBUG)
                    printf("\t\tmin is written\n");
                kill(requsitador_pid,SIGCONT);
                kill(requsitador_pid,SIGUSR2);
            }
            else{
                if(DEBUG)
                    printf("\t\tmin is not written\n");
                kill(requsitador_pid,SIGCONT);
                kill(requsitador_pid,SIGUSR1);
            }
        }
        //Inserir no frame
        min->page_index = pFault->page_index;
        min->value = 1;
        min->vazio = 0;
        min->b_written = (pFault->rw == 'W' || pFault->rw == 'w')?1:0;
        min->pid = requsitador_pid;

        //Atualizar tabela do processo que ganhou
        pt_requsitador[min->page_index].frameNum = min->self_index;
        pt_requsitador[min->page_index].rw = (pFault->rw == 'W' || pFault->rw == 'w')?'w':'r';
        pt_requsitador[min->page_index].vazio = 0;

#if DEBUG
        printf("\t\t\tpFault:\n");
        printf("\t\t\t\tpage_index = %d\n",pFault->page_index);
        printf("\t\t\t\tFrameNum = %d\n",pFault->frameNum);
        printf("\t\t\t\tvazio = %d\n",pFault->vazio);
        printf("\t\t\t\tb_written = %d\n",pFault->b_written);
        printf("\t\t\t\tpid = %d\n",pFault->pid);
        printf("\t\t\tFrame:\n");
        printf("\t\t\t\tpage_index = %d\n",min->page_index);
        printf("\t\t\t\tvalue = %d\n",min->value);
        printf("\t\t\t\tvazio = %d\n",min->vazio);
        printf("\t\t\t\tb_written = %d\n",min->b_written);
        printf("\t\t\t\tpid = %d\n",min->pid);
#endif
        
	insertNode(&frameHeap,min);
	heapify(&frameHeap,256);
        //printf("pt_requsitador[%d].frameNum = %d\n",  min->page_index, min->self_index);

    }
    else{
        //CASO NÃO É PAGEFAULT
        printf("Caso nao eh pageFault\n");
        
        //se é write
        printf("pFault->rw = %c\n", pFault->rw);
        printf("pFault->frameNum = %d\n", pFault->frameNum);
        if(pFault->rw=='W'||pFault->rw=='w')
            mem_fisica[pFault->frameNum]->b_written=1;
        //soma 1 a valor
        mem_fisica[pFault->frameNum]->value++;
	insertNode(&frameHeap,min);
        heapify(&frameHeap,256);
		//Garante que o processo retorne a executar
        kill(requsitador_pid,SIGCONT);
        
    }
    shmdt (pFault);
    //shmctl (segPfault, IPC_RMID, 0);
    shmdt (Py);
    //shmctl (segPy, IPC_RMID, 0);
    shmdt (pt_requsitador);
    shmdt (pt_perdedor);
    //shmctl (segPTp, IPC_RMID, 0); 
    //shmctl (segPT, IPC_RMID, 0);    
}