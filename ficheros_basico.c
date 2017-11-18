/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: ficheros_basico.c
*/

#include "ficheros_basico.h"

// FASE 2

int tamMB (unsigned int nbloques){
	int r = 0;
	r = (nbloques/8)/BLOCKSIZE;
	if (((nbloques/8)%BLOCKSIZE) != 0){
		r = r+1;
	}
	return r;
}


int tamAI (unsigned int ninodos){
	int r = 0;
	r = ((ninodos * TAM_INODO) / BLOCKSIZE);
	if (((ninodos * TAM_INODO) % BLOCKSIZE) != 0) r = r+1;
	return r;

}


int initSB (unsigned int bloques, unsigned int inodos) {
	struct superbloque sb; //Creamos el superbloque
	sb.posPrimerBloqueMB = posSB+1;
	sb.posUltimoBloqueMB = sb.posPrimerBloqueMB+tamMB(bloques)-1;
	sb.posPrimerBloqueAI = sb.posUltimoBloqueMB+1;
	sb.posUltimoBloqueAI = sb.posPrimerBloqueAI+tamAI(inodos)-1;
	sb.posPrimerBloqueDatos = sb.posUltimoBloqueAI+1;
	sb.posUltimoBloqueDatos = bloques-1;
	sb.posInodoRaiz = 0;
	sb.posPrimerInodoLibre = 0;
	sb.cantBloquesLibres = bloques;
	sb.cantInodosLibres = inodos;
	sb.totBloques = bloques;
	sb.totInodos = inodos;

	if(bwrite(posSB,&sb) == -1) return -1; //Error en el bwrite
	return 0;
}


int initMB (unsigned int bloques) {
	struct superbloque sb;
  //Leemos el superbloque
	if(bread(posSB,&sb) == -1) return -1; //Error en el bread

	unsigned char buf[BLOCKSIZE];
	memset(buf,0,BLOCKSIZE); //Definimos el array de un bloque con todos los bits a cero

	unsigned int i;
	for (i = sb.posPrimerBloqueMB; i <= sb.posUltimoBloqueMB; i++){
		if(bwrite(i,buf) == -1) return -1; //Error en el bwrite
	}

	for(i = posSB; i <= sb.posUltimoBloqueAI; i++){
		if(escribir_bit(i,1) == -1) return -1; //Error ESCRIBIR_BIT
		sb.cantBloquesLibres--; //Restaremos los bloques (SB, MB, AI) de la cantidad de bloques libres
	}

	if(bwrite(posSB,&sb) == -1) return -1; //Error BWRITE

	return 0;
}


int initAI () {
	struct superbloque sb;
	if(bread(posSB,&sb) < 0) return -1; //Leemos el superbloque para localizar el mapa de bits Error BREAD

	struct inodo in[BLOCKSIZE/TAM_INODO];
	unsigned int x, i, j;
	x = 1;
	for(i = sb.posPrimerBloqueAI; i <= sb.posUltimoBloqueAI; i++) {
		for(j = 0; j < BLOCKSIZE/TAM_INODO; j++) {
			in[j].tipo = 'l';
			if(x < sb.totInodos) {
				in[j].punterosDirectos[0] = x;
				x++;
			} else {
				in[j].punterosDirectos[0] = UINT_MAX;
				j = BLOCKSIZE/TAM_INODO;
			}
		}
		if(bwrite(i,&in) < 0) return -2; //Escribimos el array de inodos Error BWRITE
	}
	return 0;
}



// FASE 3




int escribir_bit (unsigned int bloque, unsigned int bit) {
	struct superbloque sb;
	if(bread(posSB,&sb) < 0) return -1;

	int posbyte = bloque/8;
	int posbit = bloque%8;
	int MB = (posbyte/BLOCKSIZE) + sb.posPrimerBloqueMB; //Bloque a actualizar
	posbyte %= BLOCKSIZE;

	unsigned char buffer[BLOCKSIZE];
	if(bread(MB,buffer) < 0) return -1;

	unsigned char mascara = 128;
	mascara >>= posbit; //Desplazar a la derecha
	if(bit == 0) {
		buffer[posbyte]&=~mascara; //AND y NOT para bits
	} else {
		buffer[posbyte]|=mascara; //OR para bits
	}
	if(bwrite(MB,buffer) < 0) return -1;
	return 0;
}

unsigned char leer_bit (unsigned int bloque) {
	struct superbloque sb;
	if(bread(posSB,&sb) < 0) return -1;

	int posbyte = bloque/8;
	int posbit = bloque%8;
	int MB = (posbyte/BLOCKSIZE) + sb.posPrimerBloqueMB; //Bloque a actualizar
	posbyte %= BLOCKSIZE;

	unsigned char buffer[BLOCKSIZE];
	if(bread(MB,buffer) < 0) return -1;

	unsigned char mascara = 128;
	mascara >>= posbit; //Desplazar a la derecha
	mascara &= buffer[posbyte]; //Operador AND para bits
	mascara >>= (7-posbit); //Desplazar a la derecha
	return mascara; //0 o 1
}

int reservar_bloque () {
	struct superbloque sb;
	if(bread(posSB,&sb) < 0) return -1;

	if(sb.cantBloquesLibres <= 0) {
		puts("Error: No hay más bloques libres.");
		return -1;
	}

	unsigned char bufferAux[BLOCKSIZE];
	memset(bufferAux,255,BLOCKSIZE);
	unsigned char buffer[BLOCKSIZE];
	int posMB = sb.posPrimerBloqueMB;

	if(bread(posMB, buffer) == -1) return -1;

	while(memcmp(bufferAux,buffer,BLOCKSIZE) == 0){
		posMB++;
		if(bread(posMB, buffer) == -1) return -1;
	}

	unsigned char byte = 255;
	unsigned char mascara = 128;
	int posbyte = 0;
	int posbit = 0;
	int fin = 0;
	while(posbyte < BLOCKSIZE && !fin){
		byte = buffer[posbyte];
		if (byte < 255) { // Comprovamos que el byte no es todo 1
			while (byte & mascara) { // operador AND para bits
				byte <<= 1;	// Desplazar a la izquierda
				posbit++;
			}
            fin = 1;
		} else posbyte++;
	}
	int numbloque = ((posMB-sb.posPrimerBloqueMB)*BLOCKSIZE+posbyte)*8+posbit;
	if(escribir_bit(numbloque, 1) == -1) return -1;
	sb.cantBloquesLibres--;
	if (bwrite(posSB, &sb) == -1) return -1;

	return numbloque;
}


int liberar_bloque(unsigned int nbloque) {
    struct superbloque sb;
    /* Actualizamos la cantidad de bloques libres */
    if (bread(posSB, &sb) == -1) {
        printf("Error al leer bloque.\n");
        return -1;
    }
    /* Ponemos a 0 el bit del nbloque en el mapa de bits */
    if (escribir_bit(nbloque, 0) == -1) {
        printf("Error intentando escribir bit.\n");
        return -1;
    }
		/*Aumentamos en uno la cantidad de bloques libres */
    sb.cantBloquesLibres++;
    if (bwrite(posSB, &sb) ==-1 ) {
        printf("Error intentando escribir bloque.\n");
        return -1;
    }
    return nbloque;
}

int escribir_inodo(struct inodo inodo,unsigned int ninodo) {
  struct superbloque sb;
  unsigned int bloqueObjetivo;

  if (bread(posSB, &sb) == -1) {
    printf("Error al leer bloque.\n");
    return -1;
  }

  bloqueObjetivo = sb.posPrimerBloqueAI + ninodo / (BLOCKSIZE / TAM_INODO);

  struct inodo bufferAI[BLOCKSIZE / TAM_INODO];
  if (bread(bloqueObjetivo, &bufferAI) == -1) {
    printf("Error al leer buffer.\n");
    return -1;
  }

  bufferAI[ninodo % (BLOCKSIZE / TAM_INODO)] = inodo;

  if (bwrite(bloqueObjetivo, bufferAI) == -1) {
    printf("Error al escribir el inodo.\n");
    return -1;
  }
  return 0;
}

int leer_inodo (unsigned int ninodo, struct inodo *inodo) {
    unsigned int bloqueObjetivo;
    struct superbloque sb;
    struct inodo buffer [BLOCKSIZE / TAM_INODO];

    if (bread(posSB, &sb) == -1) {
        printf("Error al leer el superbloque.\n");
    }
    bloqueObjetivo = sb.posPrimerBloqueAI + (ninodo / (BLOCKSIZE / TAM_INODO));

    if (bread(bloqueObjetivo, buffer) == -1) {
        printf("Error al leer el inodo.\n");
    }

    *inodo = buffer[ninodo % (BLOCKSIZE / TAM_INODO)];
    return 1;
}

int reservar_inodo (unsigned char tipo, unsigned char permisos) {
	struct superbloque sb;
	if(bread(posSB,&sb) == -1) {
		puts("Error a la hora de leer el superbloque");
		return -1;
	}
	if(sb.cantInodosLibres == 0) {
		puts("No hay inodos libres");
		return -1;
	}

	int posInodo = sb.posPrimerInodoLibre;

	struct inodo inodoAux;
	leer_inodo(posInodo, &inodoAux);

	//Modificamos el super bloque
	sb.posPrimerInodoLibre = inodoAux.punterosDirectos[0];
	sb.cantInodosLibres--; //Al reservar un inodo, restamos uno a la cantidad de inodos libres

	//printf("reservar_inodo ----> Estamos tocando el inodo: %i\n",posInodo);
	//Inicializamos el inodo reservado
	inodoAux.tipo = tipo;
	inodoAux.permisos = permisos;
	inodoAux.atime = time(NULL);
	inodoAux.mtime = time(NULL);
	inodoAux.ctime = time(NULL);
	inodoAux.nlinks = 1;
	inodoAux.tamEnBytesLog = 0;
	inodoAux.numBloquesOcupados = 0;

	//Inicializamos los punteros de dicho inodo

	for(int i = 0; i < 12; i++) inodoAux.punterosDirectos[i] = 0;
	for(int i = 0; i < 3; i++) inodoAux.punterosIndirectos[i] = 0;
	if(escribir_inodo(inodoAux,posInodo) == -1) {
		puts("Error a la hora de escribir el inodo");
		return -1;
	}
	if(bwrite(posSB,&sb) == -1) {
		puts("Error a la hora de escribir en el superbloque");
		return -1;
	}


	return posInodo; //Devolvemos la posición del inodo reservado.
}

int obtener_nrangoBL(struct inodo inodo, unsigned int nblogico,  int *ptr){

  if (nblogico<DIRECTOS){
    *ptr=inodo.punterosDirectos[nblogico];
    return 0;
  }
  else if(nblogico<INDIRECTOS0){
    *ptr=inodo.punterosIndirectos[0];
    return 1;
  }
  else if(nblogico<INDIRECTOS1){
    *ptr=inodo.punterosIndirectos[1];
    return 2;
  }
  else if(nblogico<INDIRECTOS2){
    *ptr=inodo.punterosIndirectos[2];
    return 3;
  }
  else{
    *ptr=0;
    puts("Bloque lógico fuera de rango");
    return -1;
  }
  return 0;
}




int obtener_indice(unsigned int nblogico, unsigned int nivel_punteros){

  if (nblogico<DIRECTOS){
    return nblogico;
  }
  else if (nblogico<INDIRECTOS0){
    return nblogico-DIRECTOS;
  }
  else if (nblogico<INDIRECTOS1) {
    if (nivel_punteros==2) {
      return (nblogico-INDIRECTOS0)/ NPUNTEROS;
    }
    else if (nivel_punteros==1) {
      return (nblogico-INDIRECTOS0)% NPUNTEROS;
    }
  }
  else if (nblogico<INDIRECTOS2) {
    if (nivel_punteros==3) {
      return (nblogico-INDIRECTOS1)/(NPUNTEROS*NPUNTEROS);
    }
    else if (nivel_punteros==2){
      return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))/NPUNTEROS;
    }
    else if (nivel_punteros==1){
      return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))%NPUNTEROS;
    }
  }
  return 0;
}



int traducir_bloque_inodo(unsigned int ninodo,unsigned int nblogico,char reservar){
  struct inodo inodo;
  int ptr, ptr_ant, salvar_inodo, nRangoBL, nivel_punteros, buffer[NPUNTEROS], indice;
  if(leer_inodo (ninodo, &inodo)==-1){
    return -1;
  }
  ptr = 0, ptr_ant = 0, salvar_inodo = 0;
  if((nRangoBL = obtener_nrangoBL(inodo, nblogico, &ptr))==-1){
    return -1;
  } //0:D, 1:I0, 2:I1, 3:I2
  nivel_punteros = nRangoBL;
  while (nivel_punteros>0){//iterar para cada nivel de indirectos
    if (ptr==0){ //no cuelgan bloques de punteros
      if (reservar==0){
        return -1; //error lectura bloque inexistente
      }else{//construir bloques intermedios y enlaces desde inodo hasta datos
        salvar_inodo = 1;
        if((ptr = reservar_bloque())==-1){
          return -1;
        }  //de punteros
        inodo.numBloquesOcupados++;
        inodo.ctime = time(NULL);
        if (nivel_punteros == nRangoBL){ //el bloque colgará directamente del inodo
          inodo.punterosIndirectos[nRangoBL-1] = ptr;
        }else{//el bloque cuelga de otro bloque de punteros
          buffer[indice] = ptr;
          if(bwrite(ptr_ant, buffer)==-1){
            return -1;
          }
        }
      }
    }
    if(bread(ptr, buffer)==-1){
      return -1;
    }
    ptr_ant = ptr;
    if((indice = obtener_indice(nblogico, nivel_punteros))==-1){
      return -1;
    }

    ptr = buffer[indice];
    nivel_punteros-- ;
  }//al salir de este i ya estamos al nivel de datos
  if (ptr==0){ //no existe bloque de datos
    if (reservar==0){
      return -1;//error lectura ∄ bloque
    }else{
      salvar_inodo = 1;
      if((ptr = reservar_bloque())==-1){
        return -1;
      }  //de datos
      inodo.numBloquesOcupados++;
      inodo.ctime = time(NULL);
      if (nRangoBL==0){
        inodo.punterosDirectos[nblogico] = ptr;
      }else{
        buffer[indice] = ptr;
        if(bwrite(ptr_ant, buffer)==-1){
          return -1;
        }
      }
    }
  }
  if (salvar_inodo==1){
    if(escribir_inodo(inodo,ninodo)==-1){
			return -1;
    }  //sólo si lo hemos actualizado
  }
  return ptr; //nbfisico
}

int liberar_bloques_inodo(unsigned int ninodo, unsigned int nblogico){
	struct inodo inodo;
  int nRangoBL, nivel_punteros, indice, ptr, nblog, ultimoBL;
  unsigned char bufferAux[BLOCKSIZE];
  int bloques_punteros[3][NPUNTEROS];
  int ptr_nivel [3];   //punteros auxiliares a bloques
  int indices[3];    //indices auxiliares
  int liberados;

  memset(bufferAux, 0, BLOCKSIZE);
  liberados=0;
  leer_inodo(ninodo, &inodo);
	if (inodo.tamEnBytesLog==0) return 0;
  if((inodo.tamEnBytesLog%BLOCKSIZE)==0){
  	ultimoBL=inodo.tamEnBytesLog/BLOCKSIZE - 1;
	}
  else{
		ultimoBL=inodo.tamEnBytesLog/BLOCKSIZE;
	}
  ptr=0;
  for (nblog=nblogico; nblog<=ultimoBL; nblog++) {
    nRangoBL=obtener_nrangoBL(inodo,nblog,&ptr);
		if (nRangoBL < 0) return -1;
    nivel_punteros=nRangoBL;
  	while (ptr>0 && nivel_punteros>0){
      bread(ptr,bloques_punteros[nivel_punteros-1]);
      indice=obtener_indice(nblog,nivel_punteros);
      ptr_nivel[nivel_punteros-1]=ptr;
      indices[nivel_punteros-1]=indice;
      ptr=bloques_punteros[nivel_punteros-1][indice];
      nivel_punteros--;
    }
    if(ptr>0){ //existe bloque de datos
      liberar_bloque(ptr);
      liberados++;
      if(nRangoBL==0) // puntero Directo
        inodo.punterosDirectos[nblog]=0;
      else{
        while(nivel_punteros < nRangoBL){
          indice=indices[nivel_punteros];
          bloques_punteros[nivel_punteros][indice]=0;
          ptr=ptr_nivel[nivel_punteros];
        	if(memcmp(bloques_punteros[nivel_punteros],bufferAux,BLOCKSIZE)==0){
            //No cuelgan bloques ocupados, hay que liberar el bloque de punteros
            liberar_bloque(ptr);
            liberados++;
            nivel_punteros++;
            if(nivel_punteros==nRangoBL) {
							inodo.punterosIndirectos[nRangoBL-1]=0;
						}
					}
          else{  //escribimos en el dispositivo el bloque de punteros modificado
						bwrite(ptr, bloques_punteros[nivel_punteros]);
            nivel_punteros=nRangoBL;// para salir del i
          }
      	}
    	}
  	}
	}
  return liberados;
}



int liberar_inodo(unsigned int ninodo){
	struct inodo inodo;
	struct superbloque SB;
	int liberados;

	if((liberados = liberar_bloques_inodo(ninodo,0)) == -1) return -1;
	leer_inodo(ninodo,&inodo);
  inodo.tipo = 'l';
	inodo.numBloquesOcupados = inodo.numBloquesOcupados - liberados;
  bread(posSB,&SB);
  inodo.punterosDirectos[0]=SB.posPrimerInodoLibre;
  SB.posPrimerInodoLibre = ninodo;
  SB.cantInodosLibres++;
  if(escribir_inodo(inodo,ninodo)==-1) return -1;
  if(bwrite(posSB,&SB)==-1) return -1;
  return ninodo;
}
