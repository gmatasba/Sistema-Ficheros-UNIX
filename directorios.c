/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: directorios.c
*/

#include "directorios.h"

int extraer_camino (const char *camino, char *inicial, char *final) {
	int i = 1, f = 0;
  //Si el camino no empieza por "/" retornamos error porque no es válido
	if (camino[0]!='/') return -1;

	while ((camino[i] != '/') && (camino[i] != '\0')){
		inicial[i-1] = camino[i];
		i++;
	}
	if (i == strlen(camino)) {
		final = "\0";
		return 0; // 0 = Fichero
	}
	while (i < strlen(camino)) {
		final[f] = camino[i];
		i++; f++;
	}
	return 1; //1 = Directorio
}


int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos) {
	//camino_parcial es “/”
	if(strcmp(camino_parcial,"/") == 0) {
		*p_inodo = 0; //la raiz siempre estara asociada al inodo 0
		*p_entrada = 0;
		return 0;
	}

	char inicial[longEntrada], final[strlen(camino_parcial)];
	memset(inicial, 0, longEntrada);
	memset(final, 0, strlen(camino_parcial));
	struct inodo in, aux;
	int camino;
	if((camino = extraer_camino(camino_parcial,inicial,final)) < 0) return -1; //ERROR_EXTRAER_CAMINO
	//Buscamos la entrada cuyo nombre se encuentra en inicial
	leer_inodo(*p_inodo_dir,&in);
	struct entrada entr;
	entr.nombre[0] = '\0';

	int numEntr = in.tamEnBytesLog/sizeof(struct entrada);
	int nentrada = 0; //Num. entrada inicial
	if(numEntr > 0) {
		if((in.permisos & 4) != 4) return -2; //ERROR_PERMISO_LECTURA
		if(mi_read_f(*p_inodo_dir,&entr,nentrada*sizeof(struct entrada),sizeof(struct entrada)) < 0) return -3; //ERROR_LECTURA
		while(nentrada < numEntr && strcmp(inicial,entr.nombre)!= 0) {
			nentrada++;
			if(mi_read_f(*p_inodo_dir,&entr,nentrada*sizeof(struct entrada),sizeof(struct entrada)) < 0) return -3; //ERROR_LECTURA
		}
	}
	if(nentrada == numEntr) { //La entrada no existe
		switch(reservar) {
		case 0: //modo consulta. Como no existe retornamos error
			return -4; //ERROR_NO_EXISTE_ENTRADA_CONSULTA
			break;
		case 1: //modo escritura. Creamos la entrada en el directorio referenciado por *p_inodo_dir
			strcpy(entr.nombre, inicial);
			int inR;
			leer_inodo(0,&aux);
			if(camino == 1) { //DIRECTORIO
				if(strcmp(final,"/") == 0 && in.tipo != 'f') {//Revisamos que sea final y no estemos creando dentro de un fichero
					if((inR = reservar_inodo('d', permisos)) < 0) return -5; //ERROR_RESERVAR_INODO
				}
				else return -6; //ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO
			} else { //FICHERO
				if((inR = reservar_inodo('f', permisos)) < 0) return -5; //ERROR_RESERVAR_INODO
			}
			entr.inodo = inR;
			if(mi_write_f(*p_inodo_dir,&entr,nentrada*sizeof(struct entrada), sizeof(struct entrada)) < 0) {
				//Si hay error, liberamos el inodo y devolvemos error
				if(liberar_inodo(entr.inodo) < 0) return -7; //ERROR_LIBERAR_INODO
				return -8; //ERROR_ESCRITURA
			}
		}
	}
	if(strcmp(final,"/") == 0 || camino == 0) { //Hemos llegado al final del camino o es un fichero
		if((nentrada < numEntr) && (reservar == 1)) return -9; //ERROR_ENTRADA_YA_EXISTENTE
		*p_inodo = entr.inodo;
		*p_entrada = nentrada;
	} else { //Si no es final extraemos de nuevo el camino
		*p_inodo_dir = entr.inodo;
		switch(buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos)) {
			case -1: return -1;
			case -2: return -2;
			case -3: return -3;
			case -4: return -4;
			case -5: return -5;
			case -6: return -6;
			case -7: return -7;
			case -8: return -8;
			case -9: return -9;
		}
	}
	return 0;
}


int mi_creat (const char *camino, unsigned char permisos) {
	unsigned int pid = 0, pi = 0, pe = 0;
	mi_waitSem();
	int bentrada = buscar_entrada(camino, &pid, &pi, &pe, 1, permisos);
	if(bentrada < 0) {
		switch(bentrada){
			case -1: printf("\"%s\" El camino no es válido\n", camino); break;
			case -2: printf("No posees permisos de lectura\n"); break;
			case -3: printf("Error en la lectura de \"%s\"\n", camino); break;
			case -4: printf("No existe la entrada \"%s\"\n", camino); break;
			case -5: printf("Error al reservar un inodo\n"); break;
			case -6: printf("No existe directorio intermedio a \"%s\"\n", camino); break;
			case -7: printf("Error al liberar un inodo\n"); break;
			case -8: printf("Error en la escritura de \"%s\"\n", camino); break;
			case -9: printf("Ya existe \"%s\"\n", camino); break;
		}
		puts("Error en MI_CREAT");
		mi_signalSem();
		return -1;
	}
	mi_signalSem();
	return 0;
}


int mi_dir (const char *camino, char *buf) {
	unsigned int pid = 0, pi = 0, pe = 0;
	int bentrada = buscar_entrada(camino, &pid, &pi, &pe, 0, 0);
	if(bentrada < 0) {
		switch(bentrada){
		case -1: printf("\"%s\" El camino no es válido\n", camino); break;
		case -2: printf("No posees permisos de lectura\n"); break;
		case -3: printf("Error en la lectura de \"%s\"\n", camino); break;
		case -4: printf("No existe la entrada \"%s\"\n", camino); break;
		case -5: printf("Error al reservar un inodo\n"); break;
		case -6: printf("No existe directorio intermedio a \"%s\"\n", camino); break;
		case -7: printf("Error al liberar un inodo\n"); break;
		case -8: printf("Error en la escritura de \"%s\"\n", camino); break;
		case -9: printf("Ya existe \"%s\"\n", camino); break;
		}
		puts("Error en MI_DIR");
		return -1;
	}
	struct inodo in;
	leer_inodo(pi,&in);
	struct entrada entr;
	if(in.tipo != 'd' && in.permisos & 4) return -1;
	int numEntradas = in.tamEnBytesLog/sizeof(struct entrada);
	int nentrada = 0;
	//Vamos obteniendo informacion de todas las entradas
	while(nentrada < numEntradas){
			if(mi_read_f(pi,&entr,nentrada*sizeof(struct entrada),sizeof(struct entrada)) < 0) return -2; //Leemos una entrada Error LECTURA
			leer_inodo(entr.inodo,&in);

			if(in.tipo == 'd') strcat(buf,"d");
			else strcat(buf,"f");
			strcat(buf,"\t");
			//Para incorporar informacion acerca de los permisos:
			if(in.permisos & 4) strcat(buf,"r");
			else strcat(buf,"-");

			if(in.permisos & 2) strcat(buf,"w");
			else strcat(buf,"-");

			if(in.permisos & 1) strcat(buf,"x");
			else strcat(buf,"-");

			strcat(buf, "\t");
			//Para incorporar informacion acerca del tiempo:
			struct tm *tm;
			char tmp[100];
			tm = localtime(&in.mtime);
			sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d\t", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
			strcat(buf,tmp);

			char buffer[10];
			snprintf(buffer, 10, "%d", in.tamEnBytesLog);
			strcat(buf,buffer);

			strcat(buf, "\t");
			strcat(buf,entr.nombre);
			strcat(buf,"\n");
			nentrada++;
		}
	return numEntradas;
}


int mi_link (const char *camino1, const char *camino2) {
	unsigned int pid = 0, pi = 0, pe = 0;
	mi_waitSem();
	int bentrada_origen = buscar_entrada(camino1, &pid, &pi, &pe, 0, 0);
	if(bentrada_origen < 0) {
		switch(bentrada_origen){
			case -1: printf("\"%s\" El camino no es válido\n", camino1); break;
			case -2: printf("No posees permisos de lectura\n"); break;
			case -3: printf("Error en la lectura de \"%s\"\n", camino1); break;
			case -4: printf("No existe la entrada \"%s\"\n", camino1); break;
			case -5: printf("Error al reservar un inodo\n"); break;
			case -6: printf("No existe directorio intermedio a \"%s\"\n", camino1); break;
			case -7: printf("Error al liberar un inodo\n"); break;
			case -8: printf("Error en la escritura de \"%s\"\n", camino1); break;
			case -9: printf("Ya existe \"%s\"\n", camino1); break;
		}
		mi_signalSem();
		return -1;
	}
	mi_signalSem();
	int inodo = pi;
	struct inodo in;
	leer_inodo(inodo,&in);
	if(in.tipo != 'f') return -2; //Error porque camino1 ha de referirse a un fichero

	pid = 0, pi = 0, pe = 0;
	mi_waitSem();
	int bentrada_destino = buscar_entrada(camino2, &pid, &pi, &pe, 1, 6);
	if(bentrada_destino < 0) {
		switch(bentrada_destino){
			case -1: printf("\"%s\" El camino no es válido\n", camino2); break;
			case -2: printf("No posees permisos de lectura\n"); break;
			case -3: printf("Error en la lectura de \"%s\"\n", camino2); break;
			case -4: printf("No existe la entrada \"%s\"\n", camino2); break;
			case -5: printf("Error al reservar un inodo\n"); break;
			case -6: printf("No existe directorio intermedio a \"%s\"\n", camino2); break;
			case -7: printf("Error al liberar un inodo\n"); break;
			case -8: printf("Error en la escritura de \"%s\"\n", camino2); break;
			case -9: printf("Ya existe \"%s\"\n", camino2); break;
		}
		mi_signalSem();
		return -1;
	}
	mi_signalSem();
	struct entrada entr;
	if(mi_read_f(pid,&entr,pe*sizeof(struct entrada), sizeof(struct entrada)) < 0) return -3; //Error LECTURA
	mi_waitSem();
	liberar_inodo(pi); //Liberamos el inodo que se ha asociado a la entrada creada
	mi_signalSem();
	entr.inodo = inodo;
	if(mi_write_f(pid,&entr,pe*sizeof(struct entrada), sizeof(struct entrada)) < 0) return -4; //Error ESCRITURA

	mi_waitSem();
	in.nlinks++; //Actualizamos nlinks (tiene uno nuevo)
	in.ctime = time(NULL); //Actualizamos ctime
	escribir_inodo(in, inodo);
	mi_signalSem();
	return 0;
}


int mi_unlink (const char *camino) {
	unsigned int pid = 0, pi = 0, pe = 0;
	mi_waitSem();
	int bentrada = buscar_entrada(camino, &pid, &pi, &pe, 0, 0);
	if(bentrada < 0) {
		switch(bentrada){
			case -1: printf("\"%s\" El camino no es válido\n", camino); break;
			case -2: printf("No posees permisos de lectura\n"); break;
			case -3: printf("Error en la lectura de \"%s\"\n", camino); break;
			case -4: printf("No existe la entrada \"%s\"\n", camino); break;
			case -5: printf("Error al reservar un inodo\n"); break;
			case -6: printf("No existe directorio intermedio a \"%s\"\n", camino); break;
			case -7: printf("Error al liberar un inodo\n"); break;
			case -8: printf("Error en la escritura de \"%s\"\n", camino); break;
			case -9: printf("Ya existe \"%s\"\n", camino); break;
		}
		mi_signalSem();
		return -1;
	} else {
		mi_signalSem();
		struct inodo in;
		leer_inodo(pid,&in);
		int nentradas = in.tamEnBytesLog/sizeof(struct entrada);

		//Si no es la ultima entrada
		if(nentradas-1 != pe) {
			struct entrada entr;
			if(mi_read_f(pid, &entr, (nentradas-1) * sizeof(struct entrada), sizeof(struct entrada)) < 0) return -2; //Error LECTURA
			mi_write_f(pid,&entr, pe*sizeof(struct entrada), sizeof(struct entrada)); //Error ESCRITURA
		}
		mi_waitSem();
		if(mi_truncar_f(pid,(nentradas-1)*sizeof(struct entrada)) < 0) return -4; //Error TRUNCAR
		mi_signalSem();

		leer_inodo(pi,&in);

		//Si no tiene mas links, borramos
		if(in.nlinks == 1) {
			mi_waitSem();
			if(liberar_inodo(pi) < 0) return -6; //Error LIBERAR_INODO
			mi_signalSem();
		} else { //Si no, actualizamos
			in.ctime = time(NULL);
			in.nlinks--; //Actualizamos nlinks (tiene uno menos)
			mi_waitSem();
			if(escribir_inodo(in, pi) < 0)return -5; //Error ESCRIBIR_INODO
			mi_signalSem();
		}
	}
	return 0;
}


int mi_chmod (const char *camino, unsigned char permisos) {
	unsigned int pid = 0, pi = 0, pe = 0;
	int bentrada = buscar_entrada(camino, &pid, &pi, &pe, 0, 0);

	if(bentrada < 0) {
		switch(bentrada){
			case -1: printf("\"%s\" El camino no es válido\n", camino); break;
			case -2: printf("No posees permisos de lectura\n"); break;
			case -3: printf("Error en la lectura de \"%s\"\n", camino); break;
			case -4: printf("No existe la entrada \"%s\"\n", camino); break;
			case -5: printf("Error al reservar un inodo\n"); break;
			case -6: printf("No existe directorio intermedio a \"%s\"\n", camino); break;
			case -7: printf("Error al liberar un inodo\n"); break;
			case -8: printf("Error en la escritura de \"%s\"\n", camino); break;
			case -9: printf("Ya existe \"%s\"\n", camino); break;
		}
		return -1;
	} else {
		if(mi_chmod_f(pi,permisos) < 0) return -2; //Error CHMOD
	}
	return 0;
}


int mi_stat (const char *camino, struct STAT *p_stat) {
	unsigned int pid = 0, pi = 0, pe = 0;
	int bentrada = buscar_entrada(camino, &pid, &pi, &pe, 0, 0);

	if(bentrada < 0) {
		switch(bentrada){
			case -1: printf("\"%s\" El camino no es válido\n", camino); break;
			case -2: printf("No posees permisos de lectura\n"); break;
			case -3: printf("Error en la lectura de \"%s\"\n", camino); break;
			case -4: printf("No existe la entrada \"%s\"\n", camino); break;
			case -5: printf("Error al reservar un inodo\n"); break;
			case -6: printf("No existe directorio intermedio a \"%s\"\n", camino); break;
			case -7: printf("Error al liberar un inodo\n"); break;
			case -8: printf("Error en la escritura de \"%s\"\n", camino); break;
			case -9: printf("Ya existe \"%s\"\n", camino); break;
		}
		return -1;
	} else {
		if(mi_stat_f(pi,p_stat) < 0) return -2; //Error STAT
	}
	return 0;
}


int mi_read (const char *camino, void *buf, unsigned int offset, unsigned int nbytes) {
	unsigned int pid = 0, pi = 0, pe = 0, bytesLeidos;
	int bentrada = buscar_entrada(camino, &pid, &pi, &pe, 0, 0);
	if(bentrada < 0) {
		switch(bentrada){
			case -1: printf("\"%s\" El camino no es válido\n", camino); break;
			case -2: printf("No posees permisos de lectura\n"); break;
			case -3: printf("Error en la lectura de \"%s\"\n", camino); break;
			case -4: printf("No existe la entrada \"%s\"\n", camino); break;
			case -5: printf("Error al reservar un inodo\n"); break;
			case -6: printf("No existe directorio intermedio a \"%s\"\n", camino); break;
			case -7: printf("Error al liberar un inodo\n"); break;
			case -8: printf("Error en la escritura de \"%s\"\n", camino); break;
			case -9: printf("Ya existe \"%s\"\n", camino); break;
		}
		puts("Error en MI_READ");
		return -1;
	} else {
		if((bytesLeidos = mi_read_f(pi,buf,offset,nbytes)) < 0) return -2; //Error READ
	}
	return bytesLeidos;
}


int mi_write (const char *camino, const void *buf, unsigned int offset, unsigned int nbytes) {
	unsigned int pid = 0, pi = 0, pe = 0, bytesEscritos;
	int bentrada = buscar_entrada(camino, &pid, &pi, &pe, 0, 6);
	if(bentrada < 0) {
		switch(bentrada){
			case -1: printf("\"%s\" El camino no es válido\n", camino); break;
			case -2: printf("No posees permisos de lectura\n"); break;
			case -3: printf("Error en la lectura de \"%s\"\n", camino); break;
			case -4: printf("No existe la entrada \"%s\"\n", camino); break;
			case -5: printf("Error al reservar un inodo\n"); break;
			case -6: printf("No existe directorio intermedio a \"%s\"\n", camino); break;
			case -7: printf("Error al liberar un inodo\n"); break;
			case -8: printf("Error en la escritura de \"%s\"\n", camino); break;
			case -9: printf("Ya existe \"%s\"\n", camino); break;
		}
		puts("Error en MI_WRITE");
		return -1;
	}
	struct inodo in;
	leer_inodo(pi,&in);
	if(in.tipo != 'f'){
		puts("Se está intentando trabajar con un directorio.\n"); //Error LECTURA_DIRECTORIO
		return -3;
	}

	if((bytesEscritos = mi_write_f(pi,buf,offset,nbytes)) < 0) return -2; //Error WRITE
	return bytesEscritos;
}
