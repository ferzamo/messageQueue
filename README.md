# messageQueue
Se compila con “gcc programa.c thpool.c -D THPOOL_DEBUG -pthread -o programa”.
Iniciar primero send.c y luego programa.c .Para enviar los mensajes a la cola utilizamos
“send”, en este caso pide 3 mensajes. El programa los saca de la cola, los filtra y manda otro
mensaje a otra cola de mensajes, el cual lo leera send.c.
