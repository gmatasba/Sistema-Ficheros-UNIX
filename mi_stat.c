/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: mi_stat.c
*/

#include "directorios.h"

int main (int argc, char **argv) {
	if (argc != 3) {
		printf("Sintaxis: mi_stat <disco> <ruta>\n");
		return -1;
	}
	//Montamos el disco
	bmount(argv[1]);

	struct STAT stat;
	struct tm *tm;

	if(mi_stat (argv[2],&stat) == 0) {
		printf("------- Informacion %s -------\n", argv[2]);
		printf("tipo: %c. \n", stat.tipo);
		printf("permisos: %d. \n", stat.permisos);
		//Fechas (atime,mtime,ctime)
		tm = localtime(&stat.atime); //Ultimo acceso a datos
		printf("atime: %d-%02d-%02d %02d:%02d:%02d\t \n", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

		tm = localtime(&stat.mtime); //Ultimo modificacion de los datos
		printf("mtime: %d-%02d-%02d %02d:%02d:%02d\t \n", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

		tm = localtime(&stat.ctime); //Ultimo modificacion del inodo
		printf("ctime: %d-%02d-%02d %02d:%02d:%02d\t \n", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
		printf("nlinks: %d. \n", stat.nlinks);
		printf("tamEnBytesLog: %d. \n", stat.tamEnBytesLog);
		printf("numBloquesOcupados: %d. \n", stat.numBloquesOcupados);
	} else printf("Error: mi_stat ha fallado\n");

	bumount();
	return 0;
}
