/*Assignment 2: Message Queue client*/
/*This client creates message queue and puts mathematical expression in it.*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct my_msgbuf
{
	long mtype;
	char mtext[200];
};

int main()
{	
	struct my_msgbuf buf;
	int msqid;
	key_t key;
	/*generate a unique key for msgget()*/
	if((key = ftok(".", 'B'))  == -1)
	{	
		perror("ftok");
		exit(1);
	}
	/* create a messege queue with rw-(0600) permissions*/
	/* Returns Queue id = msqid*/
	if((msqid = msgget(key,0644|IPC_CREAT)) == -1)
	{
		perror("msgget");
		exit(1);
	}

	printf("%d\n",msqid);					/*show msqid*/
	printf("Enter Lines of text, ^D to quit:\n");

	buf.mtype = 1;
	while(fgets(buf.mtext,sizeof(buf.mtext),stdin) != NULL)		/*get expression from user*/
	{	
		int len = strlen(buf.mtext);							/*must be witout spaces, e.g.55+89*/

		if(buf.mtext[len -1] == '\n')	buf.mtext[len -1] = '\0';

		if((msgsnd(msqid,&buf,len+1,0)) == -1)		/*put message into queue*/
		{
			perror("msgsnd");
		}
	}

	return 0;
}