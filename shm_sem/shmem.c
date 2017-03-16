/*assignment 3: shared memory using fork()*/
/*This program forks child process to read data from shared memory*/
/*command line input : arg1 is string to be written in shmem*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024  /* make it a 1K shared memory segment */

int main(int argc, char *argv[])
{
    key_t key;
    int shmid;
    char *data,*outp;
    int mode;
    pid_t pid;

    if (argc > 2) {
        fprintf(stderr, "usage: shmdemo [data_to_write]\n");
        exit(1);
    }

    if ((key = ftok(".", 'R')) == -1) {	/*get the key*/
        perror("ftok");
        exit(1);
    }

    /* create the segment: */
    if ((shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

     /* attach to the segment to get a pointer to it: */
        data = shmat(shmid, (void *)0, 0);
        if (data == (char *)(-1)) {
            perror("shmat");
            exit(1);
        }
    pid = fork();	/*fork child process*/
    if(pid == -1)
    {
        perror("fork");
        exit(1);
    }

    else if(pid != 0)
    {          
        /*parent process:write into the segment, based on the command line input: */
        if (argc == 2) {
            printf("writing to segment: \"%s\"\n", argv[1]);
            strncpy(data, argv[1], SHM_SIZE);
        } else
            printf("segment contains: \"%s\"\n", data);
    }
    else
    {
    	/*child process:read from segment*/
        printf("data read:%s\n",data);  
    }

      /* detach from the segment: */
        if(shmdt(data) == -1) {
            perror("shmdt");
            exit(1);
        }


    return 0;
}
