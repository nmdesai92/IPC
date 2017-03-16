/*Assignment 3: Que2:*/
/*This program prevents ovverwrite by other concurrent process */
/*in shared memory segment while reading in progress */ 
/*Input format: "./a.out arg1" arg1 is string to be written in shmem*/

#include <stdio.h>		/* standard I/O routines*/
#include <sys/types.h>	/*type definitions*/
#include <sys/ipc.h>	/*general sys V IPC structures*/
#include <sys/shm.h>	/* shared memory functions*/
#include <sys/sem.h>    /* semaphore functions*/
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include <error.h>
#include <errno.h>

#define SHM_SIZE 1024
#define MAX_RETRIES 10
union semun{
	int val;
	struct semid_ds *buf;
	ushort *array;
};



int initsem(key_t key, int nsems)  /* key from ftok() */
{
    int i;
    union semun arg;
    struct semid_ds buf;
    struct sembuf sb;
    int semid;

    semid = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0666);

    if (semid >= 0)			/* we got it first */ 
    { 
        arg.val = 1;
        sb.sem_op = 1;
        sb.sem_flg = 0;

        for(sb.sem_num = 0; sb.sem_num < nsems; sb.sem_num++) 
        { 
            /* set the iniial value of semaphores to '1'  */
            /* this sets the sem_otime field, as needed below. */
            if (semop(semid,&sb,1) == -1) 
            {
                int e = errno;
                semctl(semid, 0, IPC_RMID); /* clean up */
                errno = e;
                return -1; /* error, check errno */
            }
        }

    } 
    else if (errno == EEXIST) 			/* someone else got it first */
    { 
        int ready = 0;

        semid = semget(key, nsems, 0); /* get the id */
        if (semid < 0) return semid; /* error, check errno */

        /* wait for other process to initialize the semaphore: */
        arg.buf = &buf;
        for(i = 0; i < MAX_RETRIES && !ready; i++) 
        {
            semctl(semid, nsems-1, IPC_STAT, arg);
            if (arg.buf->sem_otime != 0) 
            {
                ready = 1;
            } 
            else 
            {
                sleep(1);
            }
        }
        if (!ready) {
            errno = ETIME;
            return -1;
        }
    } 
    else 
    {
        return semid; /* error, check errno */
    }

    return semid;
}

/*lock shared memory : make semaphore value zero*/
/*or wait till semaphore value is zero*/
void sem_lock(int sem_id)
{
	struct sembuf sem_opds;

	sem_opds.sem_num = 0;
	sem_opds.sem_op = -1;
	sem_opds.sem_flg = 0;
	if(semop(sem_id,&sem_opds,1) == -1){
		perror("semop:");
		exit(1);
	}
}

/*unlock shared memory: make semaphore value zero*/
void sem_unlock(int sem_id)
{
	struct sembuf sem_opds;

	sem_opds.sem_num = 0;
	sem_opds.sem_op = 1;
	sem_opds.sem_flg = 0;
	if(semop(sem_id,&sem_opds,1) == -1){
		perror("semop:");
		exit(1);
	}	
} 


void child_write(int sem_id,char *chalk,char *data)
{
	sem_lock(sem_id);
	strncpy(data,chalk,SHM_SIZE);
	sem_unlock(sem_id);
}

void parent_read(int sem_id,char *data)
{
	char *show_me;
	show_me = malloc(sizeof(char) *1024);
	sem_lock(sem_id);
	strncpy(show_me,data,SHM_SIZE);
	sem_unlock(sem_id);
	printf("data:%s\n",show_me );
	free(show_me);
}


int main(int argc,char **argv)
{
	int sem_id;
	int shm_id;
	int rc;
	char *data;
	struct shmid_ds shm_deall;
	key_t key;
	pid_t pid;

	key = ftok(".",'A');	/*generate key*/
	if(key == -1)
	{
		perror("ftok");
		exit(1);
	}
	/*get one semaphore */
	sem_id = initsem(key,1);
	if(sem_id == -1){
		perror("initsem:");
		exit(1);
	}

	/*create shared memory segment with rw permission*/
	shm_id = shmget(key,SHM_SIZE,0600 | IPC_CREAT | IPC_EXCL);
	if(shm_id == -1){
		perror("shmget");
		exit(1);
	}

	/*attach shared memory to our process's address space*/
	data = shmat(shm_id,NULL,0);
	if(data == (char *)(-1)){
		perror("shmat:");
		exit(1);
	}

	/*fork a child process that will make entry in shared memory segment*/
	pid = fork();
	switch(pid){
		case -1:
				perror("fork:");
				exit(1);
				break;
		case 0:
			child_write(sem_id,argv[1],data);	/*child process writes in shared memory*/
			exit(0);
			break;
		default:
			parent_read(sem_id,data);			/*parent process reads from shared memory*/
			break;
				
	}

	/* wait for child process terminarion*/
	{
		int child_status;

		wait(&child_status);
	}

	/*detach shared memory segment*/
	if(shmdt(data) == -1){
		perror("shmdt:");
	}

	/* de-allocate shared memory segment*/
	if(shmctl(shm_id,IPC_RMID,&shm_deall) == -1){
		perror("shmctl");
	}

	return 0;

}