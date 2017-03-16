/*Assignment 4 server for chat*/
/* This program implements a socket server for chat application.*/
/*server waits for a connection from client and
/*receives message from client after connected*/
#include    <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>    //strlen
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <sys/wait.h>
#include <errno.h>
#include <netdb.h>


#define PORT "8888"
/*kills child process*/
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

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

int main()
{
    int socket_desc , new_socket , c;
    int i = 0,bytes;
    struct addrinfo hints, *server, *p;
    struct sockaddr_in client;
    char *message,reply[2000];
    struct sigaction sa;
    int rv;
    char cr = '\n';
    int flag = 0;
    message = malloc(sizeof (char) * 40);

   
    if(signal(SIGINT,my_handler_for_sigint) == SIG_ERR)
    {
                    puts("\nsignal can't catched");
    }

    /*reap all dead processes*/   
    sa.sa_handler = sigchld_handler; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &server)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    p = server;

    /*opens socket*/
    socket_desc = socket(p->ai_family , p->ai_socktype , p->ai_protocol);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
       
    /*Bind to the socket*/
    if( bind(socket_desc, p->ai_addr,p->ai_addrlen) < 0)
    {
        close(socket_desc);
        perror("bind failed:");
        puts("bind failed");
        return 1;
    }
    puts("bind done");
     
    freeaddrinfo(server);
    /*Listen*/
    listen(socket_desc , 10);
     
    /*Accept and incoming connection*/
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    while(1)
    {
        flag = 0;
        new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c) ;
        if(new_socket == -1){
            perror("accept");
            continue;
        }

        puts("Connection accepted");
        if(!fork())
        {
            close(socket_desc);

            while(!flag)
            {
            	i = 0;
                if((bytes = recv(new_socket ,reply,2000,0)) < 0)
                {
                    puts("receive fail");
                    
                }
                if(bytes == 0)      flag = 1;
                else
                {
                    while(i < bytes)	printf("%c",reply[i++]);
                    i++;
                    printf("\n");
                }
            }
            
                close(new_socket);
                exit(0);
            
        }
        close(new_socket);
    }
        
    return 0;
}