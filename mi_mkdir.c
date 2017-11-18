/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: mi_mkdir.c
*/

#include "directorios.h"

int main (int argc, char **argv) {
	if (argc != 4) {
		printf("Sintaxis: mi_mkdir <disco> <permisos> <ruta>\n");
		return -1;
	}

	bmount(argv[1]);

	unsigned int permisos = atoi(argv[2]);
	if(permisos < 0 || permisos > 7) {
		puts("Los permisos no son válidos\n");
		bumount();
		return -1;
	}
	int charF = strlen(argv[3])-1;

	if(argv[3][charF] == '/') { // DIRECTORIO
		if(mi_creat(argv[3], permisos) == 0){
			printf("El directorio %s se ha creado correctamente\n", argv[3]);
		} else printf("Error: mi_mkdir ha fallado\n");
	} else { // FICHERO
		if(mi_creat(argv[3], permisos) == 0){
			printf("El archivo %s se ha creado correctamente\n", argv[3]);
		} else printf("Error: mi_mkdir ha fallado\n");
	}
	bumount();
	return 0;
}
