/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: bloques.c
*/

#include "bloques.h"

//Colocamos el semaforo como una variable global
static sem_t *mutex;
static unsigned int inside_sc = 0;

static int descriptor = 0;

 int bmount (const char *camino) {
  int errno = 0;
  if (descriptor > 0) {
    close(descriptor);
  }
  if ((descriptor=open(camino, O_RDWR|O_CREAT, 0666)) ==-1) {
    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
	}
  if (!mutex) {
  	mutex = initSem();
    if (mutex == SEM_FAILED) {
    	return -1;
    }
  }
  return descriptor;
}



int bumount(){
  int errno = 0;
  descriptor = close(descriptor);
  if(descriptor == -1){
    fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
    return -1;
   }
   deleteSem(); // borramos semaforo
   return 0;
}



int bwrite (unsigned int nbloque, const void *buf){
	lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
	int bytesEscritos = 0;
	bytesEscritos = write(descriptor,buf,BLOCKSIZE);
	if (bytesEscritos == -1) return -1;
	else return bytesEscritos;
}


int bread (unsigned int nbloque, void *buf){
	unsigned int n = BLOCKSIZE*nbloque;
	if (lseek(descriptor, n, SEEK_SET)== -1){
		printf("Error en lseek");
		return -1;
	}
	if (read(descriptor, buf, BLOCKSIZE) == -1) {
		printf("Error en read");
		return -1;
	}
	return 0;
}

//Funciones propias para semàforos
void mi_waitSem(){
	if (!inside_sc) {
		waitSem(mutex);
	}
	inside_sc++;
}

void mi_signalSem() {
	inside_sc--;
	if (!inside_sc) {
    signalSem(mutex);
	}
}
