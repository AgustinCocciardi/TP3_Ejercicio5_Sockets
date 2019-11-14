#include <Socket_Cliente.h>
#include <Socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

void main (int argc, char* argv[])
{
	int sock;		/* descriptor de conexión con el servidor */
	int error;		/* error de lectura por el socket */

    t_socket estructura; //CON ESTA ESTRUCTURA VOY A LEER Y ESCRIBIR
    
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

	/* Se abre una conexión con el servidor */
	sock = Abre_Conexion_Inet ("localhost", "cpp_java"); //En el futuro va a haber que reemplazar estos datos.

	char consulta[100]; //Este será el string donde el usuario ingresará la consulta
    char *campo;        //aca guardo el campo
    char *valor;        //aca guardo el valor del campo
    char delimitador[2]="=";    //con este delimitador voy a separar la consulta
    /*Ingreso la consulta*/
    puts("Ingrese su consulta");
    fflush(stdin);  //recuerden que hay que limpiar el buffer de entrada cuando se lee una cadena 
    scanf("%s", consulta);
    while (strcmp(consulta,"QUIT") != 0)    //si el usuario ingreso QUIT, debo salir
    {
        campo=strtok(consulta,delimitador);             //obtengo el campo
        valor=strtok(NULL,delimitador);                 //obtengo el valor
        for (int i = 0; i < strlen(valor); i++)         //Esto es para buscar nombres con espacio. Ej: si quiero consultar por la marca "EL COLOSO", en la consulta debo escribir "EL.COLOSO" luego se separa
        {
            if (valor[i] == '.')
            {
                valor[i]= ' ';
            }
        }
		printf("Consulta Ingresada: %s-%s\n", campo, valor);    //Consulta que se ingresa
		strcpy(estructura.campo,campo);                         //escribo en el buffer del socket
		strcpy(estructura.valor,valor);                         //escribo en el buffer del socket
		estructura.tamanio=0;                                   //seteo el tamaño en 0
		
		Escribe_Socket(sock,&estructura,sizeof(t_socket));      //escribo en el servidor la consulta

		if ((Lee_Socket (sock, &estructura, sizeof(t_socket)) > 0)){ //este if es para ver si pude recibir lo que me envio el servidor
			if (estructura.tamanio > 0) //Si el tamaño es mayor a 0, quiere decir que tengo una coincidencia
			{
				printf("Se encontraron %d coincidencias:\n", estructura.tamanio);   //informo las coincidencias
				for (int i = 0; i < estructura.tamanio; i++)
				{   /* Muestro los datos */
					printf("Id: %s\tProducto: %s\tArticulo: %s\tMarca: %s\n", estructura.item_id[i], estructura.producto[i], estructura.articulo[i], estructura.marca[i]);
				}
			}
			else
			{
				puts("Su busqueda no produjo coincidencias\n"); //informo si la busqueda no obtuvo resultados
			}
		}
		puts("Ingrese su consulta");    //pido una consulta nueva
    	fflush(stdin);
    	scanf("%s", consulta);
	}
}