/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: leer.c
*/

#include "ficheros.h"

#define TAM 1024
int main(int argc, char **argv) {
	int OFFSET = 0;
	unsigned char buffAux[TAM];
	unsigned char buffer[TAM];
	char string[128];
	struct STAT stat;
	int acumulador = 0;
	int leidos;
	memset (buffAux, 0, TAM);
	memset (buffer, 0, TAM);

	if(argc != 3) {
		fprintf(stderr, "Sintaxis: leer <nombre_dispositivo> <nºinodo>\n");
		return -1;
	}
	if(bmount(argv[1]) == -1) return -1;
	while((leidos = mi_read_f(atoi(argv[2]), buffAux, OFFSET, TAM))  > 0) {
		write(1, buffAux, TAM);
		memset (buffAux, 0, TAM);
		memset (buffer, 0, TAM);
		acumulador = leidos + acumulador;
		OFFSET = OFFSET + TAM;
	}
	mi_stat_f(atoi(argv[2]), &stat);

	sprintf(string, "\ntotal_leídos %d\n", acumulador);
	write(2, string, strlen(string));

	fprintf(stderr, "tamEnBytesLog %u\n", stat.tamEnBytesLog);
	bumount();
}
