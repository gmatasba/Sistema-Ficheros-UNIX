/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: mi_ls.c
*/

#include "directorios.h"

int main (int argc, char **argv) {
	if (argc != 3) {
		printf("Sintaxis: mi_ls <disco> <ruta_directorio>\n");
		return -1;
	}
	//Montamos el disco
	if(bmount(argv[1]) < 0){
		printf("Error en la apertura del fichero: %s\n", argv[1]);
		return 0;
	}

	char buffer[64*200];
	memset(buffer,0,BLOCKSIZE);
	

	if(mi_dir (argv[2],(char *)buffer) >= 0) {
		if(buffer[0]!=0){
		printf("Directorio %s:\n", argv[2]);
		printf("%c[%d;%dmTipo\tPerm.\tmTime\t\t\tTamaño\tNombre%c[%dm\n",27,0,32,27,0);
		puts("-------------------------------------------------------------------");
		printf("%s\n", buffer);}
	} else puts("Error: mi_ls ha fallado\n");

	bumount();
	return 0;
}
