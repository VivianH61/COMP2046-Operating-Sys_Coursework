#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>


int main(int argc, char *argv[])
{

    //the name of the shared memory is passed by execl from the parent process
    char SHARED_MEMORY_NAME[50];
    if (argv[1] != NULL)
        strcpy(SHARED_MEMORY_NAME, argv[1]);
    //the size of the shared memory is passed by execl from the parent process
    int SIZE_OF_MEMORY = (int)strtol(argv[2], (char **)NULL, 10);
    //receive the name of the shared address storing the pid of child process 2
    
    char ChildId2[10];
    
    if (argv[3] != NULL)
        strcpy(ChildId2, argv[3]);
    //obtain the id of child process2 from the shared address
    int shm_pid2 = shm_open(ChildId2, O_RDWR | O_CREAT, 0666);
    if (shm_pid2 == -1)
    {
        printf("failed to open shared memory\n");
        exit(1);
    }
    
    int *pid2_sh = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_pid2, 0);
    int pid2 = pid2_sh[0];
    if (munmap(pid2_sh, sizeof(pid2)) == -1)
        perror("Error un-mmapping the file");
    shm_unlink(ChildId2);
    shmctl(shm_pid2, IPC_RMID, 0);
 

    int round = 0;
    while (round < 10)
    {
        sleep(1);
        
        kill(pid2, SIGSTOP);

        int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT, 0666);

        int *rint = (int *)mmap(NULL, SIZE_OF_MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

        rint[2 * round + 1] = rint[2 * round] * 2;
        
        round++;
        

        if (munmap(rint, SIZE_OF_MEMORY) == -1)
            perror("Error un-mmapping the file");
        kill(pid2, SIGCONT);
    }
    kill(pid2, SIGCONT);

    exit(0);
}
