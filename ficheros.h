/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: ficheros.h
*/

#include "ficheros_basico.h"
#include <time.h>

struct STAT {
	unsigned char tipo; //Tipo (libre, directorio o fichero)
	unsigned char permisos; //Permisos (lectura y/o escritura y/o ejecucion)
	time_t atime; //Fecha y hora del ultimo acceso a los datos
	time_t mtime; //Fecha y hora de la ultima modificacion de los datos
	time_t ctime; //Fecha y hora de la ultima modificacion del inodo
	unsigned int nlinks; //Cantidad de enlaces de entradas en directorio
	unsigned int tamEnBytesLog; //Tamaño en bytes lógicos
	unsigned int numBloquesOcupados; //Cantidad de bloques ocupados en la zona de datosos

};

int mi_write_f (unsigned int inodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_read_f (unsigned int inodo, void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_chmod_f (unsigned int inodo, unsigned char permisos);
int mi_truncar_f (unsigned int inodo, unsigned int nbytes);
int mi_stat_f (unsigned int inodo, struct STAT *stat);
