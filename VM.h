#include <stdio.h>

typedef struct {
    int pid;
    int page_index;
	int self_index;
	unsigned int value;
	unsigned int vazio;
	unsigned int b_written;
}PageFrame;

typedef struct {
	//informações imutáveis após a criação
    int pid;
    unsigned int page_index;
	
	//informações mutáveis após a criação
    int frameNum;
    char rw;
    int vazio;
}PageTable;

void trans(int program_pid, unsigned int page_index, unsigned int offset, char rw);


