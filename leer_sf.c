/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: leer_sf.c
*/

#include "ficheros_basico.h"


int main(int argc, char *argv[]){
	if (argc < 2){
		printf("Error, la sintaxis debe ser: ./leer_sf fichero\n" );
		exit(0);
	}
	else{
		struct superbloque sb;
		bmount(argv[1]);
		bread(posSB,&sb);
		printf("DATOS DEL SUPERBLOQUE: \n");
		printf("posPrimerBloqueMB = %d\n",sb.posPrimerBloqueMB);
		printf("posUltimoBloqueMB = %d\n",sb.posUltimoBloqueMB);
		printf("posPrimerBloqueAI = %d\n",sb.posPrimerBloqueAI);
		printf("posUltimoBloqueAI = %d\n",sb.posUltimoBloqueAI);
		printf("posPrimerBloqueDatos = %d\n",sb.posPrimerBloqueDatos);
		printf("posUltimoBloqueDatos = %d\n",sb.posUltimoBloqueDatos);
		printf("posInodoRaiz = %d\n",sb.posInodoRaiz);
		printf("posPrimerInodoLibre = %d\n",sb.posPrimerInodoLibre);
		printf("cantBloquesLibres = %d\n",sb.cantBloquesLibres);
		printf("cantInodosLibres = %d\n",sb.cantInodosLibres);
		printf("totBloques = %d\n",sb.totBloques);
		printf("totInodos = %d\n",sb.totInodos);
		printf("Sizeof struct Superbloque is: %lu\n", sizeof(struct superbloque));
		printf("Sizeof struct inodo is: %lu\n", sizeof(struct inodo));

		printf("\n");
		printf("METAINFORMACIÓN DE LOS INODO OCUPADOS\n");

		struct tm *ts;
		char atime[80];
		char mtime[80];
		char ctime[80];
		unsigned int x = 0;
		struct inodo in;

		for(int i = 0; i < sb.totInodos && x < (sb.totInodos - sb.cantInodosLibres); i++){
		leer_inodo(i,&in);
			if(in.tipo != 'l'){
				x++;
				printf ("\n");
				printf("Numero inodo: %d\n",i);
				printf("Tipo de inodo: %c\n",in.tipo);
				printf("Permisos: %d\n",in.permisos);
				ts = localtime(&in.atime);
				strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
				ts = localtime(&in.mtime);
				strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
				ts = localtime(&in.ctime);
				strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
				printf("ATIME: %s\nCTIME: %s\nMTIME: %s\n",atime,ctime,mtime);
				printf("nlinks: %d\n",in.nlinks);
				printf("tamEnBytesLog: %d\n",in.tamEnBytesLog);
				printf("numBloquesOcupados: %d\n",in.numBloquesOcupados);
			}
		}

    //puts("Recorrido lista de Inodos Libres");
    //struct inodo in[BLOCKSIZE/TAM_INODO];
    //for(int i = sb.posPrimerBloqueAI; i <= sb.posUltimoBloqueAI; i++) {
      //bread(i,in);
  		//for(int j = 0; j < BLOCKSIZE/TAM_INODO; j++) {
        //printf("%i ",in[j].punterosDirectos[0]);
      //}
    //}
		//printf("INODO CON BLOQUES LOGICOS 8, 204, 30.004, 400.004 y 16.843.019\n");
   		//int ninodo=reservar_inodo('f', 6);
   		//printf ("ninodo: %d\n", ninodo);
   		//traducir_bloque_inodo(ninodo, 8, 1);
   		//traducir_bloque_inodo(ninodo, 204, 1);
   		//traducir_bloque_inodo(ninodo, 30004, 1);
   		//traducir_bloque_inodo(ninodo, 400004, 1);
   		//traducir_bloque_inodo(ninodo, 16843019, 1); //bloque lógico máximo del SF

	}
	bumount();
	return 0;
}
