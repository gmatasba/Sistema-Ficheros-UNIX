/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: mi_link.c
*/

#include "directorios.h"

int main (int argc, char **argv) {
	if (argc != 4) {
		printf("Sintaxis: mi_link <disco> <ruta_fichero_original> <ruta_enlace>\n");
		return -1;
	}
	//Montamos el disco
	bmount(argv[1]);

	if(mi_link (argv[2],argv[3]) == 0) printf("El enlace se ha creado correctamente\n");
	else printf("Error: mi_ln ha fallado\n");

	bumount();
	return 0;
}
