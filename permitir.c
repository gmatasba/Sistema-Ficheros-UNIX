/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: permitir.c
*/

#include "ficheros.h"
#include <string.h>


int main(int argc, char *argv[]){


	if (argc < 4){
		printf("Error, la sintaxis debe ser: ./permitir <nombre_dispositivo> <ninodo> <permisos>\n" );
		exit(0);
	}

	int ejecucion = 0;
	ejecucion = bmount(argv[1]);
	if (ejecucion != -1){
		unsigned int permisos = atoi(argv[3]);
		unsigned int ninodo = atoi(argv[2]);
		mi_chmod_f(ninodo,permisos);
		bumount(); 
	}
}	

