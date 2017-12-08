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
#include "fila.h"

 int main(void) {
 	int pid;


	if(pid = fork()){
		int segFila;
		int status;
	 	segFila = shmget(6666, sizeof(Fila), IPC_CREAT | S_IWUSR | S_IRUSR);

		Fila *q;

		q = (Fila *) shmat(segFila, 0, 0);

		q = fila_cria();

		int x = 3;
		int y = 4;

		fila_insere(q, x);
		fila_insere(q, y);
		waitpid(pid, NULL, 0);
	}
	else{
		//filho
		sleep(2);
		int segFila;
	 	segFila = shmget(6666, sizeof(Fila), IPC_CREAT | S_IWUSR | S_IRUSR);

		Fila *q;

		q = (Fila *) shmat(segFila, 0, 0);
		puts("oi");
		printf("%d\n", fila_retira(q));
	}

  return 0;
}