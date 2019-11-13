#include <Socket_Cliente.h>
#include <Socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include<pthread.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>

void main (int argc, char* argv[])
{
	int sock;		/* descriptor de conexión con el servidor */
	int error;		/* error de lectura por el socket */
	
	key_t Clave;                        //clave para recursos compartidos
	int Id_Semaforo;                    //Identificador de Semaforos
	struct sembuf Operacion;
	struct sembuf MutexDecrem;
	struct sembuf MutexIncrem;

	t_socket estructura;
	char *ayuda="-Help"; //Uso esta cadena para ver si el usuario quiere ver la ayuda
    if (argc == 2 && strcmp(argv[1],ayuda) == 0) //Muestro la ayuda al usuario
    {
        puts("Este es el proceso consumidor para consultar al archivo Servidor");
        puts("Usted no deberá pasarle ningún parámetro a este programa, solamente deberá llamarlo escribiendo: ./Cliente");
        puts("Posteriormente, usted podrá ingresar por teclado tantas consultas como desee hacerle al Servidor, usando el formato CAMPO=VALOR");
        puts("Ejemplos de ejecución:");
        puts("\tID=4444");
        puts("\tMARCA=GEORGALOS");
        puts("\tPRODUCTO=HELADO");
        puts("Cuando ya no desee hacer mas consultas, bastará con que escriba QUIT y su programa se cerrará");
        exit(3);
    }

    if (argc != 1)
    {
        printf("\nExceso de parámetros");
        printf("\nEscriba './Cliente -Help' (sin las comillas) para recibir ayuda");
        printf("\n");
        exit(1);
	}
	
	Clave = ftok("/bin/ls",VALOR);       //Pido una clave para recursos compartidos y verifico que haya podido recibirla
    if (Clave == (key_t) -1)
	{
		printf("No consigo clave para semáforos\n");
		exit(0);
    }

	Id_Semaforo = semget (Clave, 3, 0600 | IPC_CREAT);      //Pido ID para Semaforos y verifico que haya podido recibirla
	if (Id_Semaforo ==(key_t) -1)
	{
		printf("No puedo crear sem�foro\n");
		exit (0);
    }

	semctl(Id_Semaforo,1,SETVAL,0);


	Operacion.sem_num= 1;
	Operacion.sem_op= -1;
	Operacion.sem_flg= 0;

	//Con esto voy a incrementar y decrementar el Mutex
	MutexDecrem.sem_num= 2;
	MutexDecrem.sem_op= -1;
	MutexDecrem.sem_flg= 0;
	
	MutexIncrem.sem_num= 2;
	MutexIncrem.sem_op= 1;
	MutexIncrem.sem_flg= 0;

	/* Se abre una conexión con el servidor */
	sock = Abre_Conexion_Inet ("localhost", "cpp_java");

	char consulta[100];
    char *campo;
    char *valor;
    char delimitador[2]="=";
    puts("Ingrese su consulta");
    fflush(stdin);
    scanf("%s", consulta);
    while (strcmp(consulta,"QUIT") != 0)
    {
        campo=strtok(consulta,delimitador);
        valor=strtok(NULL,delimitador);
        for (int i = 0; i < strlen(valor); i++)
        {
            if (valor[i] == '.')
            {
                valor[i]= ' ';
            }
        }
		printf("Consulta: %s-%s\n", campo, valor);
		strcpy(estructura.campo,campo);
		strcpy(estructura.valor,valor);
		estructura.tamanio=0;
		semop(Id_Semaforo,&MutexDecrem,1);
		Escribe_Socket(sock,&estructura,sizeof(t_socket));
		semop(Id_Semaforo,&MutexIncrem,1);
		puts("Espero a que liberen mi semaforo");
		semop(Id_Semaforo,&Operacion,1);
		puts("Liberaron mi semaforo");
		semop(Id_Semaforo,&MutexDecrem,1);
		if ((Lee_Socket (sock, &estructura, sizeof(t_socket)) > 0)){
			if (estructura.tamanio > 0)
			{
				printf("Se encontraron %d coincidencias:\n", estructura.tamanio);
				for (int i = 0; i < estructura.tamanio; i++)
				{
					printf("Id: %s\tProducto: %s\tArticulo: %s\tMarca: %s\n", estructura.item_id[i], estructura.producto[i], estructura.articulo[i], estructura.marca[i]);
				}
			}
			else
			{
				puts("Su busqueda no produjo coincidencias\n");
			}
		}
		semop(Id_Semaforo,&MutexIncrem,1);
		puts("Ingrese su consulta");
    	fflush(stdin);
    	scanf("%s", consulta);
	}
}