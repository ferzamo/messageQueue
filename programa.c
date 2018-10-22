#include <stdio.h>
#include <pthread.h>
#include "thpool.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#define MAXSIZE     128

FILE *archivo;

void separarMensaje (char mensaje[], char archivo[]);


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

void recibir(){

    int msqid;
    key_t key;
    struct msgbuf rcvbuffer;

    key = 1777;

    if ((msqid = msgget(key, 0666)) < 0)
      die("msgget");


    if (msgrcv(msqid, &rcvbuffer, MAXSIZE, 1, 0) < 0)
      die("msgrcv");

    printf("\nLeyendo mensaje\n");

    separarMensaje("ArchivoCSV.csv", rcvbuffer.mtext);

}

void separarMensaje (char archivo[], char mensaje[]) {

    int i = 0;
    int j = 0;
    char campos [5][sizeof(char)*strlen(mensaje)];

    char *token;

    token = strtok(mensaje, ":");
    while (token!=NULL) {
        sprintf(campos[i], "%s", token );
        token = strtok(NULL, ":");
        i++;
    }

    char *fichero = campos[0];
    char *nombreCampo1 = campos[1];
    int valorCampo1 = atoi(campos[2]);
    char *nombreCampo2 = campos[3];
    int valorCampo2  = atoi( campos[4]);

    printf("\nTrabajando hilo numero %u \n", (int)pthread_self());

    FILE *archivoLeer = fopen(archivo, "r");
    FILE *archivoEscribir;

    char *linea = malloc(sizeof(char)*12*10);
    char *lineaCSV = malloc(sizeof(char)*12*10);

    fgets(linea, 12*10, archivoLeer);
    strcpy(lineaCSV, linea);

    i=0;
    int numeroDeCampos = 5;

    char camposDelArchivo[5][sizeof(char)*strlen(linea)];

    i =0;

    token = strtok(linea,":");

    while(token!=NULL){

        sprintf(camposDelArchivo[i], "%s", token );
        token = strtok(NULL, ":");
        i++;

    }

    camposDelArchivo[i-1][strlen(camposDelArchivo[i-1])-1] = '\0';

    int numeroCampo1;
    int numeroCampo2;

    for(j=0;j<i;j++){
      if(strcmp(nombreCampo1,camposDelArchivo[j])==0){
        numeroCampo1=j;
      }
    }
    for(j=0;j<i;j++){
      if(strcmp(nombreCampo2,camposDelArchivo[j])==0){
        numeroCampo2=j;
      }
    }

    archivoEscribir = fopen(fichero, "a");
    fprintf(archivoEscribir, "%s", lineaCSV);
    fclose(archivoEscribir);

    while (!feof(archivoLeer)){


        char *lineaAUX = malloc(sizeof(char)*12*10);

         fgets(linea,100,archivoLeer);
         strcpy(lineaAUX,linea);
         i=0;

         token = strtok(linea,":");

         int contador=0;

         while(token!=NULL){

                if (i==numeroCampo1){
                        if(valorCampo1<atoi(token)){

                                contador++;
                        }

                }

                 if (i==numeroCampo2){

                        if(valorCampo2>atoi(token)){
                                contador++;

                        }
                }

                token = strtok(NULL, ":");
                i++;

        }

        if (contador==2){

                archivoEscribir = fopen(fichero, "a");
                fprintf(archivoEscribir, "%s", lineaAUX);
                fclose(archivoEscribir);

        }

    }

    fclose(archivoLeer);


     int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    struct msgbuf sbuf;
    size_t buflen;

    key = 1778;

    if ((msqid = msgget(key, msgflg )) < 0)
      die("msgget");


    sbuf.mtype = 1;

    strcpy(sbuf.mtext, fichero);

    buflen = strlen(sbuf.mtext) + 1 ;

    if (msgsnd(msqid, &sbuf, buflen, IPC_NOWAIT) < 0)
    {
        printf ("%d, %d, %s, %d\n", msqid, sbuf.mtype, sbuf.mtext, buflen);
        die("msgsnd");
    }




}


void crearCSV(){

   FILE * fp;

   if (NULL == (fp=fopen("ArchivoCSV.csv", "w"))) {
       perror("No puedo crear ArchivoCSV.csv");
       exit(-1);
   }
   /* Cabecera con nombres de campos */
   (void) fprintf(fp, "Codigo:Altura:Edad:X:Y\n");

   /* Lineas */
   int i;
   for (i=0; i<10000; i++) {
     (void) fprintf(fp,"%d:%d:%d:%d:%d\n", rand() % 1000, rand() % 1000,
                                           rand() % 1000, rand() % 1000,
                                           rand() % 1000);
   }
   fclose(fp);

}
int main(){



        crearCSV();

//-----------------------------------------------------------------------

         puts("\nCreando ThreadPool de 5 hilos\n");
        threadpool thpoolRecibir = thpool_init(5);

        puts("\nEsperando a 3 mensajes\n");

        int i;
        for (i=0; i<3; i++){
                thpool_add_work(thpoolRecibir, (void*)recibir, NULL);
        };

        thpool_wait(thpoolRecibir);

        puts("\nThreadPool destruido\n");
        thpool_destroy(thpoolRecibir);
//------------------------------------------------------------------------

        return 0;
}
