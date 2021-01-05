#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>




int main(int argc, char *argv[]) {
    kill(getpid(), SIGSTOP);
    //the name of the shared memory is passed by execl from the parent process
    char SHARED_MEMORY_NAME[50];
    if (argv[1] != NULL)
        strcpy(SHARED_MEMORY_NAME, argv[1]);
    //the size of the shared memory is passed by execl from the parent process
    int SIZE_OF_MEMORY = (int) strtol(argv[2], (char **)NULL, 10);
    
  
    int *rint;
    int round = 0;
    
    //suspend until child process 1 send signal to this process
    
    
    while (round < 10) {
        int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT, 0666);
        
        
        rint = (int *) mmap(NULL, SIZE_OF_MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        rint[2 * round + 2] = rint[2 * round + 1] - 10;
        
        round ++;
        
        if (munmap(rint, SIZE_OF_MEMORY) == -1)
            perror("Error un-mmapping the file");
        kill(getpid(), SIGSTOP);
    }
    

    exit(0); 
}
