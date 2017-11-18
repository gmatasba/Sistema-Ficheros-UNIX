
/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: escribir.c
*/


#include "ficheros.h"


int main (int argc, char **argv) {

	if (argc != 4) {
		printf("Sintaxis: escribir <nombre_dispositivo> <$(cat fichero)> <diferentes_inodos>\n");
		return -1;
	}

	bmount(argv[1]);
	int size = strlen(argv[2]);
	char buffer[size];
	strcpy(buffer,argv[2]);
	struct STAT stat;
	int diferentes_inodos = atoi(argv[3]);
	int ninodo = reservar_inodo ('f', 6);
	int offset[5] = {0,5120,256000,30720000,71680000};

	for (int i = 0; i < 5; i++) {
		if (diferentes_inodos == 1 && i != 0) ninodo = reservar_inodo ('f', 6);


		if(mi_write_f(ninodo, &buffer, offset[i], size) == -1) {
			printf("Error escribiendo en el disco.\n");
		}
		mi_stat_f(ninodo, &stat);
		printf("Inodo reservado: %d\n", ninodo);
		printf("Offset: %d\n", offset[i]);
		printf("Bytes escritos: %d\n", size);
		printf("stat.tamEnBytesLog: %d\n", stat.tamEnBytesLog);
		printf("stat.numBloquesOcupados: %d\n", stat.numBloquesOcupados);
		printf("\n");
	}
	bumount();
}
