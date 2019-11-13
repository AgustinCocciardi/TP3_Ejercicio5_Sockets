#ifndef _SOCKET_H
#define _SOCKET_H

#define VALOR 100

typedef struct Socket
{
    int tamanio;
    char campo[10];
    char valor[50];
    char item_id[VALOR][10];
    char articulo[VALOR][60];
    char producto[VALOR][60];
    char marca[VALOR][40];
} t_socket;


int Lee_Socket (int fd, t_socket *Datos, int Longitud);
int Escribe_Socket (int fd, t_socket *Datos, int Longitud);


#endif
