/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: mi_cat.c
*/

#include "directorios.h"

int main (int argc, char **argv) {
	if (argc != 3) {
		printf("Sintaxis: mi_cat <disco> <ruta_fichero>\n");
		return -1;
	}

	char *camino = argv[2];
    if(camino[strlen(camino)-1] == '/'){
        printf("ERROR: ( %s ) no es un fichero.\n",camino);
        return -1;
    }

	bmount(argv[1]);

 	int leidos = 0;
	unsigned char buf[BLOCKSIZE];
	memset(buf,0,BLOCKSIZE);
	int x = mi_read(camino,buf,0,BLOCKSIZE);
	unsigned int bucle;
	for(bucle = BLOCKSIZE; x > 0; bucle += BLOCKSIZE){ // Mientras no de error seguimos leyendo
		write(1,buf,x); //Mostrar los resultados por pantalla
		leidos += x;
		memset(buf,0,BLOCKSIZE);
		x = mi_read(camino,buf,bucle,BLOCKSIZE);
	}

	char string[128];
	sprintf(string, "\nBytes Leídos: %d\n", leidos); //Hay que mostrar el valor del num de bytes leidos y del tamanyo en bytes logico del inodo
	write(2, string, strlen(string)); //No sumara lo que ocupe esa informacion al fichero externo

	bumount();
}
