#include "simulacion.h"

#define BUFFERSIZE 1024

int main(int argc, char **argv) {
	// Comprobamos los parametros
	if (argc != 3) {
		printf("ERROR: faltan argumentos\n");
		printf("SYNTAX:\n $./verificacion <disco> </directorio> \n");
		exit(0);
	}
	// Montamos el dispositivo virtual
	if (bmount(argv[1]) == -1) {
		printf("ERROR: Main: Fallo al montar el sistema de archivos\n");
		exit(0);
	}
	// Obtenemos la metainformacion del directorio de simulacion
	struct STAT stat;
 	if (mi_stat(argv[2], &stat) == -1) {
		printf("ERROR: main: Fallo obteniendo la metainformacion del directorio de simulacion\n");
		exit(0);
	}
	// Leemos las entradas del directorio de simulacion
	int numentradas = stat.tamEnBytesLog / sizeof(struct entrada);
	struct entrada entradas[numentradas];
	if (mi_read(argv[2], entradas, 0, sizeof(entradas)) == -1) {
		printf("ERROR: main: Error leyendo entradas del directorio de simulacion\n");
		exit(0);
	}
	// Inicializamos el camino del fichero de informe
	char camino_fichero_informe[128];
	sprintf(camino_fichero_informe, "%s%s", argv[2], "informe.txt");
	// Creamos el fichero de informe
	if (mi_creat(camino_fichero_informe, 7) == -1) {
		printf("ERROR: main: Error creando el fichero informe\n");
		exit(0);
	}
	// Verificamos las escrituras escribiendo en el fichero informe los resultados
	int i, nbytes_informe = 0;
	for (i = 0; i < numentradas; i++) {
		pid_t pid = atoi(strchr(entradas[i].nombre, '_') + 1);
		//printf("Verificando proceso con PID: %i\n", pid);
		int num_escrituras = 0;
		struct registro primera_escritura;
		struct registro ultima_escritura;
		struct registro menor_posicion;
		struct registro mayor_posicion;
		char camino_fichero_prueba[128];
		sprintf(camino_fichero_prueba, "%s%s/%s", argv[2], entradas[i].nombre, NOMBRE_FICHERO_PRUEBA);
		int offset = 0;
		struct registro registros[(BLOCKSIZE/sizeof(struct registro))*200];
		while (mi_read(camino_fichero_prueba, registros, offset, sizeof(registros)) > 0) {
			int j;
			for (j = 0; j < (BLOCKSIZE/sizeof(struct registro))*200; j++) {
				if (registros[j].pid == pid) {
					//printf("PID: %d, nEscritura: %d, posicion: %d\n", registros[j].pid, registros[j].nEscritura, registros[j].posicion);
					if (num_escrituras == 0) {
						primera_escritura = registros[j];
						ultima_escritura = registros[j];
						menor_posicion = registros[j];
						mayor_posicion = registros[j];
					} else {
						if(primera_escritura.nEscritura > registros[j].nEscritura){
							primera_escritura = registros[j];
						} else if(ultima_escritura.nEscritura < registros[j].nEscritura){
							ultima_escritura = registros[j];
						}
						if (registros[j].posicion < menor_posicion.posicion) {
							menor_posicion = registros[j];
						} else if (registros[j].posicion > mayor_posicion.posicion) {
							mayor_posicion = registros[j];
						}
					}
					num_escrituras++;
				}
			}
			memset(registros, 0, sizeof(registros));
			offset += sizeof(registros);
		}
		//Escribimos por consola para resultados.txt
		printf("PID: %i\nNumero de escrituras: %i\n", pid, num_escrituras);
		printf("Primera escritura: %i %i %s", primera_escritura.nEscritura, primera_escritura.posicion, asctime(localtime(&primera_escritura.fecha)));
		printf("Ultima escritura: %i %i %s", ultima_escritura.nEscritura, ultima_escritura.posicion, asctime(localtime(&ultima_escritura.fecha)));
		printf("Menor posicion: %i %i %s", menor_posicion.nEscritura, menor_posicion.posicion, asctime(localtime(&menor_posicion.fecha)));
		printf("Primera escritura: %i %i %s \n", mayor_posicion.nEscritura, mayor_posicion.posicion, asctime(localtime(&mayor_posicion.fecha)));

		// Escribimos la informacion del registros[j]_info en el fichero de informe
		char buffer[BUFFERSIZE];
		memset(buffer, 0, BUFFERSIZE);
		sprintf(buffer, "PID: %i\nNumero de escrituras: %i\n", pid, num_escrituras);

		sprintf(buffer + strlen(buffer), "%s %i %i %s",
		"Primera escritura: ",
		primera_escritura.nEscritura,
		primera_escritura.posicion,
		asctime(localtime(&primera_escritura.fecha)));

		sprintf(buffer + strlen(buffer), "%s %i %i %s",
		"Ultima escritura: ",
		ultima_escritura.nEscritura,
		ultima_escritura.posicion,
		asctime(localtime(&ultima_escritura.fecha)));

		sprintf(buffer + strlen(buffer), "%s %i %i %s",
		"Menor posicion: ",
		menor_posicion.nEscritura,
		menor_posicion.posicion,
		asctime(localtime(&menor_posicion.fecha)));

		sprintf(buffer + strlen(buffer), "%s %i %i %s %s",
		"Mayor posicion: ",
		mayor_posicion.nEscritura,
		mayor_posicion.posicion,
		asctime(localtime(&mayor_posicion.fecha)),
		"\n");

		if (mi_write(camino_fichero_informe, buffer, nbytes_informe, strlen(buffer)) == -1) {
			printf("ERROR: main: Error escribiendo en el informe\n");
		}
		nbytes_informe += strlen(buffer);
	}
	// Desmontamos el dispositivo
	if (bumount() == -1) {
		printf("ERROR: main: Error desmontando el dispositivo\n");
		exit(0);
	}
	printf("Verificacion finalizada\n");
	return 0;
}
