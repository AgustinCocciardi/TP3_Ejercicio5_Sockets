#ifndef _SOCKET_H
#define _SOCKET_H

#define VALOR 100

//Defino mi estructura para guardar el campo, el valor, el numero de coincidencias y las coincidencias

typedef struct Socket
{   
    int tamanio;                        //Guardo la cantidad de coincidencias
    char campo[10];                     //Guardo el campo a buscar
    char valor[50];                     //Guardo el valor del campo
    char item_id[VALOR][10];            //Vector donde voy a guardar los ids
    char articulo[VALOR][60];           //Vector donde voy a guardar los articulos
    char producto[VALOR][60];           //Vector donde voy a guardar los productos
    char marca[VALOR][40];              //Vector donde voy a guardar las marcas
} t_socket;


//Funciones para Sockets// 
int Lee_Socket (int fd, t_socket *Datos, int Longitud);         //Con esto leo los datos que me llegan
int Escribe_Socket (int fd, t_socket *Datos, int Longitud);     //Con esto escribo los datos en un socket
/*
    int fd es el socket al que le voy a escribir
    t_socket *Datos son los datos que le escribo
    int longitud es el tamaño de lo que escribo. Siempre se pone sizeof(t_socket)
*/

#endif
