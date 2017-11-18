/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: mi_chmod.c
*/

#include "directorios.h"

int main (int argc, char **argv) {
	if (argc != 4) {
		printf("Sintaxis: mi_chmod <disco> <permisos> <ruta>\n");
		return -1;
	}
	//Montamos el disco
	bmount(argv[1]);

	unsigned int permisos = atoi(argv[2]);
	if(permisos < 0 || permisos > 7) {
		puts("Los permisos introducidos no son válidos.\n");
		bumount();
		return -1;
	}

	if(mi_chmod (argv[3],permisos) == 0) printf("Permisos del fichero %s actualizados\n", argv[3]);
	else printf("Error: mi_chmod ha fallado\n");

	bumount();
	return 0;
}
