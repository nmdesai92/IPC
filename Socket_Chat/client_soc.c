/*Assignment 4: client for chat*/
/* This program implements a socket server for chat application.*/
/*server waits for a connection from client and
/*receives message from client after connected*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>    
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <errno.h>
 
/*signal handler for ^C*/
void my_handler_for_sigint(int signumber)
{
    char ans[2];
    if(signumber == SIGINT)
    {
        printf("received SIGINT\n");
        printf("Program received a CTRL-C\n");
        printf("Terminate Y/N:");
        scanf("%s", ans);
        if (strcmp(ans,"Y") == 0)
        {
            printf("Existing ....\n");
            exit(0);
        }
        else
        {
            printf("Continung ..\n");
        }
    }
}

int main(int argc , char *argv[])
{
    int socket_desc;
    struct sockaddr_in server;
    char *message;

    char newline = '\n';

     if(signal(SIGINT,my_handler_for_sigint) == SIG_ERR)
     {
              puts("\nsignal can't catched");
     }
    // printf("%d\n",check );
    /*Create socket*/
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
         
    server.sin_addr.s_addr = inet_addr(argv[1]);        /*IP address*/
    server.sin_family = AF_INET;                        /*connection over internet*/
    server.sin_port = htons( atoi(argv[2]));            /*Port number*/

   

    /*Connect to remote server*/
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed:");
        puts("connect error");
        return 1;
    }
     
    puts("Connected\n");
     
    /*Send some data*/

    while(1)
    { 

        
            message = malloc(sizeof(char) * 2000);
            scanf("%[^\n]%*c",message);
            if(send(socket_desc , message , strlen(message) , 0) < 0)
            {
                   puts("Send failed");
                    //return 1;
            }
            free(message); 
    }    

    return 0;
}