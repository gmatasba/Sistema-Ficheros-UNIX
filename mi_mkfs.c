/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: mi_mkfs.c
*/

#include "ficheros_basico.h"
#include <string.h>


int main(int argc, char *argv[]){
	if (argc < 3){
		printf("Error, la sintaxis debe ser: ./mi_mkfs fichero nbloques\n" );
		exit(0);
	}
	else{
		int ejecucion = 0;
		ejecucion = bmount(argv[1]);
		if (ejecucion != -1){
			int nbloques = atoi(argv[2]);
			int ninodos = nbloques / 4;
			unsigned char buf[BLOCKSIZE];
			memset(buf,0,BLOCKSIZE);
			initSB(nbloques,ninodos);
			initMB(nbloques);
			initAI(ninodos);

			for (int i = tamSB +tamMB(nbloques)+tamAI(ninodos); i < nbloques; i++){
				bwrite(i,buf);
			}
		}
		reservar_inodo ('d', 7);
		bumount();
	}
	return 0;
}
