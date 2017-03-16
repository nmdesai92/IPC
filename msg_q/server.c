/*Assignment 2: Message Queue server*/
/*This server takes message from msgqs created by muliple clients*/ 

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

struct my_msgbuf
{

	long mtype;
	char mtext[200];
};

FILE *fp;

/*signal handler for ^C*/
void my_handler_for_sigint(int signumber)
{
	char ans[2];
	if(signumber == SIGINT)
	{
		printf("received SIGINT");
		printf("Program received a CTRL-C\n");
		printf("Terminate Y/N:");
		scanf("%s", ans);
    	if (strcmp(ans,"Y") == 0)
    	{
       		printf("Existing ....\n");
       		fclose(fp);					/*save file while exiting...*/
       		exit(0);
    	}
    	else
    	{
       		printf("Continung ..\n");
    	}
  	}
}

/*calculate inpute formula*/
void calculate(char *exp,int msqid)
{
	struct msqid_ds msqid_dsd,*qbuf;			/*structure to get message queue stats*/
	int client_id;
	char num1[8],num2[8],ch;
	int i = 0,j = 0;
	while((exp[i] & 0x30) == 0x30)	num1[j++] = exp[i++];	/*extact first number from string*/
	ch = exp[i++];											/*operand*/
	j = 0;
	while((exp[i] & 0x30) == 0x30)	num2[j++] = exp[i++];	/*extact second number from string*/

	qbuf = &msqid_dsd;
	if(msgctl(msqid,IPC_STAT,qbuf) == -1)	/*get stats*/
	{		
		printf("%d\n",errno );
		perror("control error");
		exit(1);
	}
	
	client_id = (int)qbuf->msg_lspid;			/*set Last send message pid as client_id*/
	fprintf(fp,"Client ID %d : %d %c %d = ",client_id,atoi(num1),ch,atoi(num2) );	/*mention client-id in file*/
	switch(ch)
	{
		case '+':
				fprintf(fp,"%d",atoi(num1) + atoi(num2));
				break;
		case '-':
				fprintf(fp,"%d",atoi(num1) - atoi(num2));
				break;
		case '*':
				fprintf(fp,"%d",atoi(num1) * atoi(num2));
				break;
		case '/':
				if(num2)	fprintf(fp,"%d",atoi(num1) / atoi(num2));
				else		fprintf(fp,"Infinite");
				break;
	}
	fprintf(fp,"\n");
}

int main()
{	
	struct my_msgbuf buf;

	int msqid;
	char filename[30];
	key_t key;

	if((key = ftok(".", 'B'))  == -1)	/*generate a unique key for msgget()*/
	{
		perror("ftok");
		exit(1);
	}

	/* messege queue with rw-(0600) permissions*/
	/* Returns Queue id = msqid*/
	if((msqid = msgget(key,0600)) == -1)
	{
		perror("msgget");
		exit(1);
	}

	printf("%d\n",msqid);
	
	sprintf(filename,"%d\n",msqid );
	strcat(filename,".txt");				/*setfilename as msqid*/
	printf("Ready to receive messege:\n");	
	if((fp = fopen(filename,"w")) == NULL)	/*open file to write*/
	{
		printf("could not open file\n");
		return 1;
	}

	for(;;)		/*forever*/
	{
		if(signal(SIGINT,my_handler_for_sigint) == SIG_ERR)		/*press ^C to come out of loop*/
			printf("\ncan't catch SIGINT\n");

		if(msgrcv(msqid,&buf,sizeof(buf.mtext),0,0) == -1)	/* receive message*/
		{		
			perror("msgrcv");
			exit(1);
		}
		printf("expression: \"%s\"\n",buf.mtext);		/*display state*/
		calculate(buf.mtext,msqid);
	}

	return 0;
}