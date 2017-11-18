/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: ficheros.c
*/

#include "ficheros.h"

int mi_write_f (unsigned int inodo, const void *buf_original, unsigned int offset, unsigned int nbytes) {
	struct inodo mi_inodo;
  leer_inodo(inodo,&mi_inodo);
  //Comprobamos si el inodo tiene permisos de escritura y, en caso de que no, devolvemos -1.
	if((mi_inodo.permisos & 2) != 2) return -1;

	int bloqueI = offset/BLOCKSIZE;
  int bloqueF = (offset+nbytes-1)/BLOCKSIZE;
  int bfisico, bytes = 0;
	unsigned char bufBloque[BLOCKSIZE];
	int desp1 = offset%BLOCKSIZE; //Calculamos los desplazamientos
	int desp2 = (offset+nbytes-1)%BLOCKSIZE;

	if(bloqueI == bloqueF) {
		mi_waitSem();
		bfisico= traducir_bloque_inodo(inodo,bloqueI,1);
		mi_signalSem();
		if(bread(bfisico, bufBloque) == -1) return -1;
		memcpy (bufBloque+desp1, buf_original, desp2-desp1+1);
		if(bwrite(bfisico, bufBloque) == -1) return -1;
		bytes = nbytes;
	} else {
	    //Primer bloque
			mi_waitSem();
	    bfisico = traducir_bloque_inodo(inodo,bloqueI,1);
			mi_signalSem();
	    if(bread(bfisico, bufBloque) == -1) return -1;
	    memcpy (bufBloque+desp1, buf_original, BLOCKSIZE-desp1);
	    if(bwrite(bfisico, bufBloque) == -1) return -1;
	    bytes = BLOCKSIZE-desp1;

	    //Bloque intermedio
	    int bucle;
	    for(bucle = bloqueI+1; bucle < bloqueF; bucle++) {
				mi_waitSem();
		    bfisico= traducir_bloque_inodo(inodo,bucle,1);
				mi_signalSem();
				bytes += bwrite(bfisico, buf_original+(BLOCKSIZE-desp1)+(bucle-bloqueI-1)*BLOCKSIZE);
	    }

	    //Último bloque
			mi_waitSem();
	    bfisico= traducir_bloque_inodo(inodo,bloqueF,1);
			mi_signalSem();
	    if(bread(bfisico, bufBloque) == -1) return -1;
	    memcpy(bufBloque,buf_original+(nbytes-desp2-1),desp2+1);
	    if(bwrite(bfisico, bufBloque) == -1) return -1;
	    bytes += desp2+1;
	}
	mi_waitSem();
	leer_inodo(inodo,&mi_inodo);
	if(mi_inodo.tamEnBytesLog<offset+bytes) {
		mi_inodo.tamEnBytesLog = offset+bytes;
		mi_inodo.ctime = time(NULL);
	}
	mi_inodo.mtime = time(NULL);
	//Escribimos el inodo
	if(escribir_inodo(mi_inodo, inodo) == -1) {
		mi_signalSem();
		return -1;
	}
	mi_signalSem();
	return bytes;
}


int mi_read_f (unsigned int inodo, void *buf_original, unsigned int offset, unsigned int nbytes) {
	struct inodo mi_inodo;
	leer_inodo(inodo,&mi_inodo);

	//Comprobamos los permisos de lectura y, si el inodo no los tiene, devolvemos -1 (error).
	if((mi_inodo.permisos & 4) != 4) return -1;


	// El tamaño máximo de lectura es el tamaño en bytes lógicos del inodo
	// Lectura desde el offset hasta el final de fichero
	if (offset + nbytes >= mi_inodo.tamEnBytesLog) nbytes = mi_inodo.tamEnBytesLog - offset;
	//Si no se ha podido leer
	if (offset > mi_inodo.tamEnBytesLog || mi_inodo.tamEnBytesLog == 0){
		return 0;
	}
	if(nbytes == 0) return 0; //No leemos

	unsigned char bufBloque[BLOCKSIZE];
	memset(bufBloque,0,BLOCKSIZE);

	int bloqueI = offset/BLOCKSIZE;
	int bloqueF = (offset+nbytes-1)/BLOCKSIZE;
	int bfisico, bytes = 0;


	int	desp1 = offset%BLOCKSIZE; //Se calculan los desplazamientos en el bloque
	int desp2 = (offset+nbytes-1) % BLOCKSIZE;

	if(bloqueI == bloqueF) { //Si el bloque inicial y el final son iguales


		if((bfisico = traducir_bloque_inodo(inodo,bloqueI,0)) != -1)  {
			bread(bfisico, bufBloque);
			memcpy (buf_original, bufBloque+desp1, desp2-desp1+1);
		}
		bytes += desp2-desp1+1;
	} else {
		//3 fases
		//Primer bloque

		if((bfisico = traducir_bloque_inodo(inodo,bloqueI,0)) != -1) {
			bread(bfisico, bufBloque); //Si no hay error
			memcpy (buf_original, bufBloque+desp1, BLOCKSIZE-desp1);
		}
		bytes += BLOCKSIZE-desp1;

		//Intermedios
		int bucle;
		for(bucle = bloqueI+1; bucle < bloqueF; bucle++) {
			if((bfisico = traducir_bloque_inodo(inodo,bucle,0)) != -1 ){
				bread(bfisico, bufBloque);
				memcpy(buf_original+(BLOCKSIZE-desp1)+(bucle-bloqueI-1)*BLOCKSIZE, bufBloque, BLOCKSIZE);
			}
			bytes += BLOCKSIZE;
		}

		//Ultimo bloque
		if((bfisico = traducir_bloque_inodo(inodo,bloqueF,0)) != -1) {
			bread(bfisico, bufBloque);
			memcpy (buf_original+(nbytes-desp2-1), bufBloque, desp2+1);
		}
		bytes += desp2+1;
	}
	mi_waitSem();
	leer_inodo(inodo,&mi_inodo); //Leemos el inodo
	mi_inodo.atime = time(NULL);
	if(escribir_inodo(mi_inodo, inodo) == -1) {
		mi_signalSem();
		return -1; //Escribimos el inodo modificado en el superbloque
	}
	mi_signalSem();
	return bytes;
}


int mi_chmod_f (unsigned int inodo, unsigned char permisos) {
	struct inodo mi_inodo;
	leer_inodo(inodo,&mi_inodo);
	//Comprobamos que los permisos entran en el rango de posibles permisos.
	if(permisos < 0 || permisos > 7) return -1;
		mi_inodo.permisos = permisos;
		mi_inodo.ctime = time(NULL);
		//Escribimos el inodo con los permisos modificados.
	mi_waitSem();
	if(escribir_inodo(mi_inodo, inodo) == -1) {
		mi_signalSem();
		return -1;
	}
	mi_signalSem();
	return 0;
}


int mi_truncar_f (unsigned int inodo, unsigned int nbytes) {
	struct inodo mi_inodo;
	leer_inodo(inodo,&mi_inodo);
	int bytesl;
  //Comprobamos si el inodo tiene permisos de escritura.
	if((mi_inodo.permisos & 2) != 2){
		printf("El inodo %d no tiene permisos de escritura. \n",inodo );
		return -1;}
	//Comprobamos los de lectura
	if((mi_inodo.permisos & 4) != 4){
		printf("El inodo %d no tiene permisos de lectura. \n",inodo );
		return -1;}

	if(nbytes == 0) bytesl = 0;
	else if(nbytes%BLOCKSIZE == 0) bytesl = nbytes/BLOCKSIZE;
	else bytesl = nbytes/BLOCKSIZE+1;
	int liberados;
	if((liberados = liberar_bloques_inodo(inodo,bytesl)) == -1) return -1;
	leer_inodo(inodo,&mi_inodo);
  //Modificamos el inodo.
	mi_inodo.mtime = time(NULL);
	mi_inodo.ctime = time(NULL);
	mi_inodo.tamEnBytesLog = nbytes;
	mi_inodo.numBloquesOcupados = mi_inodo.numBloquesOcupados - liberados;
	//Escribimos el inodo modificado.
	if(escribir_inodo(mi_inodo, inodo) == -1) {
		return -1;
	}
	return 0;
}


int mi_stat_f (unsigned int inodo, struct STAT *stat) {
	struct inodo mi_inodo;
	leer_inodo(inodo,&mi_inodo);

	//Cogemos la informacion del inodo.
	stat->tipo = mi_inodo.tipo;
	stat->permisos = mi_inodo.permisos;
	stat->atime = mi_inodo.atime;
	stat->mtime = mi_inodo.mtime;
	stat->ctime = mi_inodo.ctime;
	stat->nlinks = mi_inodo.nlinks;
	stat->tamEnBytesLog = mi_inodo.tamEnBytesLog;
	stat->numBloquesOcupados = mi_inodo.numBloquesOcupados;

	return 0;
}
