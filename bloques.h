/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: bloques.h
*/

#define BLOCKSIZE 1024 // bytes

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>

#include "semaforo_mutex_posix.h"


int bmount(const char *camino);
int bumount();
int bwrite(unsigned int nbloque, const void *buf);
int bread(unsigned int nbloque,void *buf);

//Funciones propias para semàforos
void mi_waitSem();
void mi_signalSem();
