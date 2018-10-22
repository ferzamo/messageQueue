#include <stdio.h>
#include <pthread.h>
#include "thpool.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXSIZE     128

void die(char *s)
{
  perror(s);
  exit(1);
}

typedef struct msgbuf
{
    long    mtype;
    char    mtext[MAXSIZE];
};


void recibirMensaje(){

    int msqid;
    key_t key;
    struct msgbuf rcvbuffer;

    key = 1778;

    if ((msqid = msgget(key, 0666)) < 0)
      die("msgget");

     //Receive an answer of message type 1.
    if (msgrcv(msqid, &rcvbuffer, MAXSIZE, 1, 0) < 0)
      die("msgrcv");

    printf("\nMensaje imprimido en el archivo: %s\n", rcvbuffer.mtext);







}


main()
{
    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    struct msgbuf sbuf;
    size_t buflen;

    key = 1777;

    if ((msqid = msgget(key, msgflg )) < 0)
      die("msgget");

    int i;
    for(i=0; i<3;i++){
    sbuf.mtype = 1;

    printf("Escribe un mensaje(Ej: salida.txt:Altura:20:X:100) : ");
    scanf("%[^\n]",sbuf.mtext);
    getchar();

    buflen = strlen(sbuf.mtext) + 1 ;

    if (msgsnd(msqid, &sbuf, buflen, IPC_NOWAIT) < 0)
    {
        printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buflen);
        die("msgsnd");
    }

    sleep(1);
    recibirMensaje();

   }

    exit(0);
}
