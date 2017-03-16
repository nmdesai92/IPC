/*This program implements the 'ls -l | grep arg1' command*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char **argv)
{
    int pfds[2];

    pipe(pfds);         

    if (!fork()) {
        close(1);       /* close normal stdout */
        dup(pfds[1]);   /* make stdout same as pfds[1] */
        close(pfds[0]); /* we don't need this */
        execlp("sh","sh","-c","ls -l", NULL); /* ls utility*/
    } 
    else {
        close(0);       /* close normal stdin */
        dup(pfds[0]);   /* make stdin same as pfds[0]*/ 
        close(pfds[1]); /* we don't need this */
        execlp("grep", "grep",argv[1], NULL);  /* wl utility*/
    }

    return 0;
}  