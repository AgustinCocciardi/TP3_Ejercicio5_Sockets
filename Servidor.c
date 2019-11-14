#include <Socket_Servidor.h>
#include <Socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#define MAX_CLIENTES 10		//DEFINO EL NUMERO MAXIMO DE CLIENTES QUE VOY A USAR

/* Prototipos de las funciones definidas en este fichero */
void nuevoCliente (int servidor, int *clientes, int *nClientes);
int dameMaximo (int *tabla, int n);
void compactaClaves (int *tabla, int *n);

void main (int argc, char* argv[])
{
	int socketServidor;				/* Descriptor del socket servidor */
	int socketCliente[MAX_CLIENTES];/* Descriptores de sockets con clientes */
	int numeroClientes = 0;			/* Número clientes conectados */
	fd_set descriptoresLectura;	/* Descriptores de interes para select() */
	
	t_socket estructura;	//CON ESTA ESTRUCTURA VOY A LEER Y ESCRIBIR

	int maximo;							/* Número de descriptor más grande */
	int i;								/* Para bubles */

	char delimitador[]=";\n";             //con este delimitador y la funcion strtok voy a separar los campos
    char palabra[200]; 						//voy a guardar una linea del archivo con esto

	char* item_id;					//guardo el id
    char* articulo;					//guardo el articulo
    char* producto;					//guardo el producto
    char* marca;					//guardo la marca

	char *ayuda="-Help"; //Uso esta cadena para ver si el usuario quiere ver la ayuda
    if (argc == 2 && strcmp(argv[1],ayuda) == 0) //Muestro la ayuda al usuario
    {
        puts("Este programa tiene la funcionalidad de actuar como un Servidor, esperando consultas que llegarán desde otras computadoras");
        puts("Lo que debe hacer es pasarle el nombre del archivo como parámetro y luego un puerto TCP que usted quiera abrir para las consultas");
        puts("Desde ahí, el programa leerá el archivo y esperará a consultas que le llegarán desde otras computadoras");
        puts("Forma de usar el programa: ./Servidor 'nombre_del_archivo' 'Puerto' ");
        puts("Ejemplo de ejecución: ./Servidor articulo.txt 8000");
        exit(3);
    }

    if (argc == 1)   //verifico que me haya pasado un archivo como parametro
    {
        printf("\nPor favor, pase el archivo de articulos y un puerto al cual conectarse");
        printf("\nEscriba './Servidor -Help' (sin las comillas) para recibir ayuda");
        printf("\n");
        exit(1);
    }

    if (argc >= 4)   //verifico que no me haya pasado mas parametros de los que pedi
    {
        printf("\nExceso de parámetros");
        printf("\nEscriba './Servidor -Help' (sin las comillas) para recibir ayuda");
        printf("\n");
        exit(1);
	}
	
	char* nombreArchivo= argv[1];		//me quedo con el nombre del archivo
	int puerto = atoi(argv[2]);			//me quedo con el puerto. ESTO TODAVIA NO HACE NADA PORQUE NO IMPLEMENTO TCP. SOLO LO GUARDO

	FILE *archivo;						//Abro el archivo en modo lectura
	archivo=fopen(nombreArchivo,"r");
	if(archivo == NULL)
	{
		puts("Error al leer el archivo. Es posible que no exista o no tenga permisos de lectura");
		exit(3);
	} 

	/* Se abre el socket servidor, avisando por pantalla y saliendo si hay 
	 * algún problema */
	socketServidor = Abre_Socket_Inet ("cpp_java");
	if (socketServidor == -1)
	{
		perror ("Error al abrir servidor");
		exit (-1);
	}

	/* Bucle infinito.
	 * Se atiende a si hay más clientes para conectar y a los mensajes enviados
	 * por los clientes ya conectados */
	while (1)
	{
		/* Cuando un cliente cierre la conexión, se pondrá un -1 en su descriptor
		 * de socket dentro del array socketCliente. La función compactaClaves()
		 * eliminará dichos -1 de la tabla, haciéndola más pequeña.
		 * 
		 * Se eliminan todos los clientes que hayan cerrado la conexión */
		compactaClaves (socketCliente, &numeroClientes);
		
		/* Se inicializa descriptoresLectura */
		FD_ZERO (&descriptoresLectura);

		/* Se añade para select() el socket servidor */
		FD_SET (socketServidor, &descriptoresLectura);

		/* Se añaden para select() los sockets con los clientes ya conectados */
		for (i=0; i<numeroClientes; i++)
			FD_SET (socketCliente[i], &descriptoresLectura);

		/* Se el valor del descriptor más grande. Si no hay ningún cliente,
		 * devolverá 0 */
		maximo = dameMaximo (socketCliente, numeroClientes);
		
		if (maximo < socketServidor)
			maximo = socketServidor;

		/* Espera indefinida hasta que alguno de los descriptores tenga algo
		 * que decir: un nuevo cliente o un cliente ya conectado que envía un
		 * mensaje */
		select (maximo + 1, &descriptoresLectura, NULL, NULL, NULL);

		/* Se comprueba si algún cliente ya conectado ha enviado algo */
		for (i=0; i<numeroClientes; i++)
		{
			if (FD_ISSET (socketCliente[i], &descriptoresLectura))
			{
				/* Se lee lo enviado por el cliente y se escribe en pantalla */
				if ((Lee_Socket (socketCliente[i], &estructura, sizeof(t_socket)) > 0)){
					printf ("\nEl cliente %d mando %s %s. Hora de atenderlo\n", i+1, estructura.campo, estructura.valor); //recibo una consulta
					while (feof(archivo) == 0)		//empiezo a recorrer el archivo
					{
						fgets(palabra,200,archivo);					//Leo una linea
						item_id = strtok(palabra,delimitador);		//separo el registro
						articulo = strtok(NULL,delimitador);
						producto = strtok(NULL,delimitador);
						marca = strtok(NULL,delimitador);
						if (strcmp(estructura.campo,"ID") == 0)			//Entro si comparo por ID. El procedimiento será el mismo para los demás. Me fijo si el campo del registro coincide con la consulta. Si coincide, lo agrego al vector
						{
							if(strcmp(estructura.valor,item_id) == 0){
								strcpy(estructura.item_id[estructura.tamanio],item_id);
								strcpy(estructura.articulo[estructura.tamanio],articulo);
								strcpy(estructura.producto[estructura.tamanio],producto);
								strcpy(estructura.marca[estructura.tamanio],marca);
								estructura.tamanio++;
							}
						}
						else
						{
							if (strcmp(estructura.campo,"PRODUCTO") == 0) //Entro si comparo por PRODUCTO
							{
								if(strcmp(estructura.valor,producto) == 0){
									strcpy(estructura.item_id[estructura.tamanio],item_id);
									strcpy(estructura.articulo[estructura.tamanio],articulo);
									strcpy(estructura.producto[estructura.tamanio],producto);
									strcpy(estructura.marca[estructura.tamanio],marca);
									estructura.tamanio++;
								}
							}
							else
							{
								if (strcmp(estructura.campo,"ARTICULO") == 0)	//Entro si comparo por ARTICULO
								{
									if(strcmp(estructura.valor,articulo) == 0){
										strcpy(estructura.item_id[estructura.tamanio],item_id);
										strcpy(estructura.articulo[estructura.tamanio],articulo);
										strcpy(estructura.producto[estructura.tamanio],producto);
										strcpy(estructura.marca[estructura.tamanio],marca);
										estructura.tamanio++;
									}
								}
								else											//Entro si comparo por MARCA
								{
									if(strcmp(estructura.valor,marca) == 0){
										strcpy(estructura.item_id[estructura.tamanio],item_id);
										strcpy(estructura.articulo[estructura.tamanio],articulo);
										strcpy(estructura.producto[estructura.tamanio],producto);
										strcpy(estructura.marca[estructura.tamanio],marca);
										estructura.tamanio++;
									}
								}
							}
						}
					}
					rewind(archivo);			//Rebobinar el archivo para leerlo en otra consulta
					printf("Atencion finalizada %d coincidencias encontradas\n", estructura.tamanio);	//Finalizo la atencion
					Escribe_Socket(socketCliente[i],&estructura,sizeof(t_socket));		//Escribo las consultas en el cliente que me las mandò
				}
				else
				{
					/* Se indica que el cliente ha cerrado la conexión y se
					 * marca con -1 el descriptor para que compactaClaves() lo
					 * elimine */
					printf ("Cliente %d ha cerrado la conexión\n", i+1);
					socketCliente[i] = -1;
				}
			}
		}

		/* Se comprueba si algún cliente nuevo desea conectarse y se le
		 * admite */
		if (FD_ISSET (socketServidor, &descriptoresLectura)){
			nuevoCliente (socketServidor, socketCliente, &numeroClientes);
		}
			
	}

	fclose(archivo);	//Cierro el archivo
}

/*
 * Crea un nuevo socket cliente.
 * Se le pasa el socket servidor y el array de clientes, con el número de
 * clientes ya conectados.
 */
void nuevoCliente (int servidor, int *clientes, int *nClientes)
{
	/* Acepta la conexión con el cliente, guardándola en el array */
	clientes[*nClientes] = Acepta_Conexion_Cliente (servidor);
	(*nClientes)++;
	t_socket estructura;
	/* Si se ha superado el maximo de clientes, se cierra la conexión,
	 * se deja todo como estaba y se vuelve. */
	if ((*nClientes) >= MAX_CLIENTES)
	{
		close (clientes[(*nClientes) -1]);
		(*nClientes)--;
		return;
	}
		
	/* Envía su número de cliente al cliente */
	Escribe_Socket (clientes[(*nClientes)-1], &estructura, sizeof(t_socket));

	/* Escribe en pantalla que ha aceptado al cliente y vuelve */
	printf ("Aceptado cliente %d\n", *nClientes);
	return;
}

/*
 * Función que devuelve el valor máximo en la tabla.
 * Supone que los valores válidos de la tabla son positivos y mayores que 0.
 * Devuelve 0 si n es 0 o la tabla es NULL */
int dameMaximo (int *tabla, int n)
{
	int i;
	int max;

	if ((tabla == NULL) || (n<1))
		return 0;
		
	max = tabla[0];
	for (i=0; i<n; i++)
		if (tabla[i] > max)
			max = tabla[i];

	return max;
}

/*
 * Busca en array todas las posiciones con -1 y las elimina, copiando encima
 * las posiciones siguientes.
 * Ejemplo, si la entrada es (3, -1, 2, -1, 4) con *n=5
 * a la salida tendremos (3, 2, 4) con *n=3
 */
void compactaClaves (int *tabla, int *n)
{
	int i,j;

	if ((tabla == NULL) || ((*n) == 0))
		return;

	j=0;
	for (i=0; i<(*n); i++)
	{
		if (tabla[i] != -1)
		{
			tabla[j] = tabla[i];
			j++;
		}
	}
	
	*n = j;
}