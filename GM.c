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
#define EVER ;;
#define DEBUG 0
#define IPCMNI 262144
#define DEFAULT_DELTA 30
void sleep1sec(int signal);
void sleep2sec(int signal);
void pageFault(int signal);
void treat_ctrl_C(int signal);
void *threadproc(void *arg);
PageFrame **mem_fisica;
// PageFrame *mem_fisica;
PageTable *PageTablep1;
PageTable *PageTablep2;
PageTable *PageTablep3;
PageTable *PageTablep4;
minHeap frameHeap;

clock_t start, end;
double cpu_time_used;

typedef struct {
    PageFrame **pf;
    minHeap mh;
    int m_seconds;
} args;

args *thread_arg;
int segP1, segP2, segP3, segP4, segPx, segPfault, segVet, segIdx, segnPF, segnSW;
int *Px, *vet;
Idx *posicao;
int *numPF, *numSwap;
int main(int argc, char *argv[]) {
    start = clock();
    int P1, P2, P3, P4;
    unsigned int i, o;
    unsigned int addr;
    char rw;
    int delta_miliseconds;
    P1 = fork();
    if(P1){
        P2 = fork();
        if(P2){
            P3 = fork();
            if (P3){
                P4 = fork();
                if(P4){
                    // ###########    Processo GM --- Pai     ###########
                    pthread_t tid;
                    int k = 0;
                    
                    if ((segP1 = shmget(1111, 256*sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmget P1 gm");
                        treat_ctrl_C(1);
                    }
                    if ((segP2 = shmget(2222, 256*sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmget P2 gm");
                        treat_ctrl_C(1);
                    }
                    if ((segP3 = shmget(3333, 256*sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmget P3 gm");
                        treat_ctrl_C(1);
                    }
                    if ((segP4 = shmget(4444, 256*sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmget P4 gm");
                        treat_ctrl_C(1);
                    }
                    if ((segPx = shmget(5555, 5*sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmget Px gm");
                        treat_ctrl_C(1);
                    }
                    if ((segVet = shmget(6666, 100*sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmget vet gm");
                        treat_ctrl_C(1);
                    }
                    if ((segIdx = shmget(7777, sizeof(Idx), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmget idx gm");
                        treat_ctrl_C(1);
                    }
                    if ((segnPF = shmget(10, sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmg idx gm");
                        treat_ctrl_C(1);
                    }
                    if ((segnSW = shmget(20, sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
                        perror("shmg idx gm");
                        treat_ctrl_C(1);
                    }                                
                    
                    PageTablep1 = (PageTable *) shmat(segP1, 0, 0);
                    PageTablep2 = (PageTable *) shmat(segP2, 0, 0);
                    PageTablep3 = (PageTable *) shmat(segP3, 0, 0);
                    PageTablep4 = (PageTable *) shmat(segP4, 0, 0);
                    Px = (int *) shmat(segPx, 0, 0);
                    vet = (int *) shmat(segVet, 0, 0);
                    posicao = (Idx *) shmat(segIdx, 0, 0);
                    numPF = (int *)shmat(segnPF, 0, 0);
                    numSwap = (int *)shmat (segnSW, 0, 0);
                    mem_fisica = (PageFrame **) malloc(256*sizeof(PageFrame*));
                    thread_arg = (args *)malloc(sizeof(args));
                   // mem_fisica = (PageFrame *) malloc(256*sizeof(PageFrame));
                    
                    Px[0] = P1;
                    Px[1] = P2;
                    Px[2] = P3;
                    Px[3] = P4;
                    Px[4] = getpid();
                    
                    posicao->index_VM = 0;
                    posicao->index_PF = 0;
                    shmdt(posicao);

                    *numPF = 0;
                    *numSwap = 0;
                    shmdt(numPF);
                    shmdt(numSwap);

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
                        PageTablep1[k].b_written = 0;
                        PageTablep1[k].page_index = k;
                        PageTablep1[k].pid = Px[0];
                        
                        PageTablep2[k].vazio = 1;
                        PageTablep2[k].frameNum = -1;
                        PageTablep2[k].b_written = 0;
                        PageTablep2[k].page_index = k;
                        PageTablep2[k].pid = Px[1];
                        
                        PageTablep3[k].vazio = 1;
                        PageTablep3[k].frameNum = -1;
                        PageTablep3[k].b_written = 0;
                        PageTablep3[k].page_index = k;
                        PageTablep3[k].pid = Px[2];
                        
                        PageTablep4[k].vazio = 1;
                        PageTablep4[k].frameNum = -1;
                        PageTablep4[k].b_written = 0;
                        PageTablep4[k].page_index = k;
                        PageTablep4[k].pid = Px[3];
                        
                        k++;
                    }
                    //Constroi Heap de minimos para os frames
                    frameHeap = initMinHeap();
                    buildMinHeap(&frameHeap,mem_fisica,256);
                    shmdt(vet);
                    shmdt(Px);
                    signal(SIGUSR1, pageFault);
                    signal(SIGINT, treat_ctrl_C);
                    //Lê, dos parâmetros dados, o delta que será utilizado para calcular a frequência de uso de um frame
                    if(argc>1){
#if DEBUG
                        //printf("argc>1, delta_miliseconds = %d\n",atoi(argv[1]));
#endif
                        delta_miliseconds = atoi(argv[1]);
                    }
                    else{
#if DEBUG
                        //printf("argc<=1, delta_miliseconds = DEFAULT_DELTA\n");
#endif
                        delta_miliseconds = DEFAULT_DELTA;
                    }
                    thread_arg->pf = mem_fisica;
                    thread_arg->mh = frameHeap;
                    thread_arg->m_seconds = delta_miliseconds;
                    pthread_create(&tid, NULL, &threadproc, (void *)thread_arg);
                    for(EVER){
                        //printf("GM executando...\n");
                        //sleep(1);
                        
                        /*Dado um certo tempo passado (de acordo com o enunciado), subtrai 1 de cada frame->value*/
                    }
                        
                }
                else{
                    // ###########    Processo P4 --- Filho 4     ###########
                    signal(SIGUSR1, sleep1sec);
                    signal(SIGUSR2, sleep2sec);
                    sleep(1);
                    FILE *fp4 = fopen("compilador.log", "r");
                    int pidP4 = getpid();
                    // printf("PID P4 = %d\n", pidP4);
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
                sleep(1);
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
            sleep(1);
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
        sleep(1);
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

void pageFault(int signal){    
    //Shmem para pegar as informações do processo que teve pagefault
    int segPfault;
    int memID;
    int *Py, *vet;
    Idx *posicao;
    int i;
    int requsitador_pid, perdedor_pid;
    PageFrame *min = deleteNode(&frameHeap);
    PageTable *pFault;
    PageTable *pt_requsitador,*pt_perdedor;
    int segPy, segPT, segPTp;
    
    if ((segPy = shmget(5555, 5*sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmget segPy pf");
        treat_ctrl_C(1);
    }
    if ((segPfault = shmget(9999, 4*sizeof(PageTable), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmget segPfault pf");
        treat_ctrl_C(1);
    }
    if ((segVet = shmget(6666, 100*sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmget vet gm");
        treat_ctrl_C(1);
    }
    if ((segIdx = shmget(7777, sizeof(Idx), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmget idx gm");
        treat_ctrl_C(1);
    }

    Py = (int *) shmat(segPy, 0, 0);
    pFault = (PageTable *) shmat(segPfault, 0, 0);
    vet = (int *) shmat(segVet, 0, 0);
    posicao = (Idx *) shmat(segIdx, 0, 0);

    i = posicao->index_PF;
    posicao->index_PF = add_pos(posicao->index_PF);

    requsitador_pid = pFault[vet[i]].pid;
    //carrega a pt do processo que enviou sigfault
    if (requsitador_pid == Py[0])
        memID = 1111;
    else if(requsitador_pid == Py[1])
        memID = 2222;
    else if(requsitador_pid == Py[2])
        memID = 3333;
    else if(requsitador_pid == Py[3])
        memID = 4444;
    #if DEBUG
         printf("SIGUSR1 received by %d: Entered pageFault\n",memID);
    #endif
    segPT = shmget(memID, 256*sizeof(PageTable), IPC_CREAT | S_IRUSR | S_IWUSR);
    pt_requsitador = (PageTable *) shmat(segPT, 0, 0);

    if((pFault[vet[i]].frameNum == -1)&&(pFault[vet[i]].vazio)){
        //CASO PAGEFAULT
        //printf("Entrando no caso pagefault\n");
        
        //Caso haja um frame vazio
        if(min->vazio){
            #if DEBUG
        #endif
            kill(requsitador_pid,SIGCONT);
            kill(requsitador_pid,SIGUSR1);
        }
        else{
            #if DEBUG
                printf("\tmin is not empty\n");
            #endif
            
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
            pt_perdedor = (PageTable *) shmat(segPTp, 0, 0);
            //apaga da pt, a ligação com o frame
            pt_perdedor[min->page_index].frameNum = -1;
            pt_perdedor[min->page_index].b_written = 0;
            pt_perdedor[min->page_index].vazio = 1;
            
            //Caso a página eleita tenha sido modificada
            if(min->b_written){
                #if DEBUG
                    printf("\t\tmin is written\n");
                #endif
                kill(requsitador_pid,SIGCONT);
                kill(requsitador_pid,SIGUSR2);
            }
            else{
                #if DEBUG
                    printf("\t\tmin is not written\n");
                #endif
                kill(requsitador_pid,SIGCONT);
                kill(requsitador_pid,SIGUSR1);
            }
        }
        //Inserir no frame
        min->page_index = pFault[vet[i]].page_index;
        min->value = 1;
        min->vazio = 0;
        min->b_written = (pFault[vet[i]].rw == 'W' || pFault[vet[i]].rw == 'w')?1:0;
        min->pid = requsitador_pid;

        //Atualizar tabela do processo que ganhou
        pt_requsitador[min->page_index].frameNum = min->self_index;
        pt_requsitador[min->page_index].rw = (pFault[vet[i]].rw == 'W' || pFault[vet[i]].rw == 'w')?'w':'r';
        pt_requsitador[min->page_index].vazio = 0;

#if DEBUG
        printf("\t\t\tpFault:\n");
        printf("\t\t\t\tpage_index = %d\n",pFault[vet[i]].page_index);
        printf("\t\t\t\tFrameNum = %d\n",pFault[vet[i]].frameNum);
        printf("\t\t\t\tvazio = %d\n",pFault[vet[i]].vazio);
        printf("\t\t\t\tb_written = %d\n",pFault[vet[i]].b_written);
        printf("\t\t\t\tpid = %d\n",pFault[vet[i]].pid);
        printf("\t\t\tFrame:\n");
        printf("\t\t\t\tpage_index = %d\n",min->page_index);
        printf("\t\t\t\tvalue = %d\n",min->value);
        printf("\t\t\t\tvazio = %d\n",min->vazio);
        printf("\t\t\t\tb_written = %d\n",min->b_written);
        printf("\t\t\t\tpid = %d\n",min->pid);
#endif
        
    insertNode(&frameHeap,min);
    heapify(&frameHeap,256);
    }
    else{
        //CASO NÃO É PAGEFAULT
        //printf("Caso nao eh pageFault\n");
        
        //se é write
        if(pFault[vet[i]].rw=='W'||pFault[vet[i]].rw=='w')
            mem_fisica[pFault[vet[i]].frameNum]->b_written=1;
        //soma 1 a valor
        mem_fisica[pFault[vet[i]].frameNum]->value++;
        insertNode(&frameHeap,min);
        heapify(&frameHeap,256);
        //Garante que o processo retorne a executar
        kill(requsitador_pid,SIGCONT);
        
    }
    shmdt (pFault);
    shmdt (Py);
    shmdt (pt_requsitador);
    shmdt (pt_perdedor);
}
void sleep1sec(int signal){
    //printf("\tProc %d dormirá 1 seg.\n",getpid());
    int segnPF;
    int *numPF;
    if ((segnPF = shmget(10, sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmg idx gm");
        treat_ctrl_C(1);
    }
    numPF = (int *)shmat(segnPF, 0, 0);
    *numPF= *numPF + 1;
    sleep(1);
}
void sleep2sec(int signal){
    //printf("\tProc %d dormirá 2 seg.\n",getpid
    int segnSW;
    int *numSwap;
    if ((segnSW = shmget(20, sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmg idx gm");
        treat_ctrl_C(1);
    } 
    numSwap = (int *)shmat (segnSW, 0, 0);
    *numSwap = *numSwap + 1;
    sleep(2);
}
void *threadproc(void *arg){
    
    int i;
    struct timespec req, rem;
    args *actual_args = arg;
    /*
        actual_args->pf = mem_fisica (tratar do mesmo modo)
        actual_args->mh = frameHeap (tratar do mesmo modo)
    */
    while(1){
        req.tv_sec = 0;
        req.tv_nsec = actual_args->m_seconds*1000000;
        while(nanosleep(&req,&rem)<0){
            req = rem;
        }

        for(i=0;i<256;i++){   
            if(actual_args->pf[i]->value>0)
                actual_args->pf[i]->value--;
        }
        heapify(&(actual_args->mh),256);
    }
 
    return 0;
}
void treat_ctrl_C(int signal){
    
    shmdt (PageTablep1);
    shmdt (PageTablep2);
    shmdt (PageTablep3);
    shmdt (PageTablep4);
    shmdt (Px);
    shmctl (segP1, IPC_RMID, 0);
    shmctl (segP2, IPC_RMID, 0);
    shmctl (segP3, IPC_RMID, 0);
    shmctl (segP4, IPC_RMID, 0);
    shmctl (segPx, IPC_RMID, 0);

    if ((segnPF = shmget(10, sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmg idx gm");
        treat_ctrl_C(1);
    }
    if ((segnSW = shmget(20, sizeof(int), IPC_CREAT | S_IWUSR | S_IRUSR)) == -1) {
        perror("shmg idx gm");
        treat_ctrl_C(1);
    } 
    numPF = (int *)shmat(segnPF, 0, 0);
    numSwap = (int *)shmat (segnSW, 0, 0);

    end = clock();
    cpu_time_used = ((double) (end - start))/ CLOCKS_PER_SEC;
    printf("##############################\n");
    printf("Tempo de execucao do programa -> %lf\n", cpu_time_used);
    printf("Numero de PageFaults -> %d\n", *numPF);
    printf("Numero de Swaps -> %d\n", *numSwap);
    printf("##############################\n");

    shmdt(numPF);
    shmdt(numSwap);

    shmctl(segIdx, IPC_RMID, 0);
    shmctl(segVet, IPC_RMID, 0);
    shmctl(segnPF, IPC_RMID, 0);
    shmctl(segnSW, IPC_RMID, 0);

    exit(0);
}