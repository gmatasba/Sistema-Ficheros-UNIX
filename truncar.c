/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: truncar.c
*/
#include "ficheros.h"
#include <string.h>


int main(int argc, char *argv[]){


	if (argc < 4){
		printf("Error, la sintaxis debe ser: ./truncar <nombre_dispositivo> <ninodo> <nbytes>\n" );
		exit(0);
	}

	int ejecucion = 0;
	ejecucion = bmount(argv[1]);
	if (ejecucion != -1){
		unsigned int nbytes = atoi(argv[3]);
		unsigned int ninodo = atoi(argv[2]);
		mi_truncar_f(ninodo,nbytes);
		bumount(); 
	}
}	