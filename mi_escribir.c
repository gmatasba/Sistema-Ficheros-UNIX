/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: mi_write.c
*/

#include "directorios.h"

int main (int argc, char **argv) {
	if (argc != 5) {
		printf("Sintaxis: mi_escribir <disco> <ruta_fichero> <$texto> <offset>\n");
		return -1;
	}

	char *camino = argv[2];
    if(camino[strlen(camino)-1] == '/'){
        printf("ERROR: ( %s ) no es un fichero.\n",camino);
        return -1;
    }

	bmount(argv[1]);
	int offset = atoi(argv[4]);

	int size = strlen(argv[3]);
  char buffer[size];
  strcpy (buffer, argv[3]);
  int bytes_escritos = mi_write(camino, &buffer, offset, size);
	if(bytes_escritos == -1) {
		printf("Error escribiendo.\n");
	}
	printf("Bytes escritos: %d\n",bytes_escritos);
	bumount();
}
