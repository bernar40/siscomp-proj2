#include <stdio.h>

typedef struct {
    int pid;
    unsigned int page_index;
	unsigned int value;
    int vazio;
	unsigned int b_written;
}PageFrame;

typedef struct {
    unsigned int page_index;
    int frameNum;
    char rw;
    int vazio;
}PageTable;

void trans(int program_pid, unsigned int page_index, unsigned int offset, char rw);


