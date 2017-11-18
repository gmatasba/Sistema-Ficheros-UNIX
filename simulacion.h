#include "directorios.h"

#define NOMBRE_FICHERO_PRUEBA "prueba.dat"
#define t_escritura   50000
#define t_proceso			200000
#define NUM_PROCESOS 100
#define NUM_REGISTROS 50
#define POS_MAX 500000

//Estructura para guardar en prueba.dat
struct registro {
	time_t fecha;
	pid_t pid;
	unsigned int nEscritura;
	unsigned int posicion;
};

void reaper(); //Enterrador
