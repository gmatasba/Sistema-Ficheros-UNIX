/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: mi_rm.c
*/

#include "directorios.h"

int main (int argc, char **argv) {
	if (argc != 3) {
		printf("Sintaxis: mi_rm <disco> <ruta>\n");
		return -1;
	}
	//Montamos el disco
	bmount(argv[1]);

	struct STAT stat;
	if(mi_stat(argv[2],&stat) < 0){
		bumount();
		return 0;
	}

	if(stat.tipo == 'd' && stat.tamEnBytesLog != 0){
		printf("El directorio %s no está vacio, no puede ser borrado\n", argv[2]);
		bumount();
		return 0;
	}
	if(mi_unlink(argv[2]) == 0){
		printf("El borrado se ha realizado correctamente\n");
	} else puts ("Error: mi_rm ha fallado\n");

	bumount();
	return 0;
}
