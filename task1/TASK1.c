#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#define SHARED_MEMORY_NAME "SHM333"
#define ChildId2 "SHM444" //the id number of child process 2

int main()
{
    int wpid;
    int status = 0;

    int SIZE_OF_MEMORY = sizeof(int) * 21;
    char str_memory_size[5];
    sprintf(str_memory_size, "%d", SIZE_OF_MEMORY);
    //create a shared memory to store the value of RandInt
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT, 0666);
    if (shm_fd == -1)
    {
        printf("failed to open shared memory\n");
        exit(1);
    }
    if (ftruncate(shm_fd, SIZE_OF_MEMORY) == -1)
        printf("shm: failed to set size of memory\n");
    int *rint = mmap(NULL, SIZE_OF_MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    rint[0] = rand() % 20 + 1;

    printf("The RandInt = %d, created by the parent process\n", rint[0]);

    if (munmap(rint, SIZE_OF_MEMORY) == -1)
        perror("Error un-mmapping the file");

    pid_t pid = fork();
    char strPid1[5];
    sprintf(strPid1, "%d", pid);
    if (pid > 0)
    {
        pid_t pid2 = fork();

        if (pid2 == 0)
            execl("ChildP2", "ChildP2", SHARED_MEMORY_NAME, str_memory_size, NULL);
        else if (pid2 > 0)
        {
            //put the id of child process 2 into a shared memory, so that child process 1 can access it
            int shm_pid2 = shm_open(ChildId2, O_RDWR | O_CREAT, 0666);
            if (shm_pid2 == -1)
            {
                printf("pid2:failed to open shared memory\n");
                exit(1);
            }
            if (ftruncate(shm_pid2, sizeof(int)) == -1)
                printf("shm: failed to set size of memory\n");
            int * pid_2 = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_pid2, 0);
            *pid_2 = pid2;
            if (munmap(pid_2, sizeof(int)) == -1)
                perror("Error un-mmapping the file");
        } else {
            printf("fork () error\n");
        }

        waitpid(pid2, &status, WUNTRACED);
    }
    else if (pid == 0)
    {
        sleep(rand()%5);
        execl("ChildP1", "ChildP1", SHARED_MEMORY_NAME, str_memory_size, ChildId2, NULL);
    }
    else
    {
        printf("fork () error\n");
    }

    waitpid(pid, &status, WUNTRACED);

    shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT, 0666);
    if (shm_fd == -1)
    {
        printf("failed to open shared memory\n");
        exit(1);
    }

    rint = mmap(NULL, SIZE_OF_MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    for (int i = 1; i <= 10; i++)
    {
        printf("In round %d, RandInt = %d, child process 1,\n", i, rint[2 * i - 1]);
        printf("In round %d, RandInt = %d, child process 2,\n", i, rint[2 * i]);
    }

    if (munmap(rint, SIZE_OF_MEMORY) == -1)
        perror("Error un-mmapping the file");
    shm_unlink(SHARED_MEMORY_NAME);
    shmctl(shm_fd, IPC_RMID, 0);
}
