 #include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

void *threadproc(void *arg)
{
	puts("to no threadproc");
    while(1)
    {
        sleep(2);
        printf("hello world\n");
    }
    return 0;
}
 int main(void) {


	pthread_t tid;
	pthread_create(&tid, NULL, &threadproc, NULL);
	while(1){
		printf("oi\n");
		sleep(4);
	}


  return 0;
}