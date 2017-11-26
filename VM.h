#include <stdio.h>

typedef struct page_frame {
    int pid;
    unsigned int page_index;
    int vazio;
}PageFrame;

typedef struct page_table {
    unsigned int page_index;
    int mem_index;
    char rw;
}PageTable;

void trans(int program_pid, unsigned int page_index, unsigned int offset, char rw);


