#include "simulacion.h"
#include <sys/wait.h>

static unsigned int acabados = 0;

void reaper() {
	while (wait3(NULL, WNOHANG, NULL) > 0) {
		acabados++;
	}
}

int main(int argc, char **argv) {
	// Comprobamos los parametros
	if (argc != 2) {
		printf("ERROR: faltan argumentos\n");
		printf("SYNTAX:\n $./simulacion <disco> \n");
		exit(0);
	}
	// Montamos el dispositivo virtual
	if (bmount(argv[1]) == -1) {
		printf("ERROR: main: Error montando el dispositivo\n");
		exit(0);
	}
	// Creamos el directorio de simulacion
	char camino[128] = "/simul_";
	time_t time_now;
	time(&time_now);
	struct tm *tm = localtime(&time_now);
	sprintf(camino + strlen(camino), "%d%02d%02d%02d%02d%02d/", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	if (mi_creat(camino, 7) == -1) {
		printf("ERROR: Error creando el directorio de simulacion %s\n", camino);
		exit(0);
	}
	// Creamos los procesos de escritura
	signal(SIGCHLD, reaper);
	pid_t pid;
	int nproceso;
	for (nproceso = 0; nproceso < NUM_PROCESOS; nproceso++) {
		struct registro registro;
		if((pid = fork()) == -1){
			exit(0);
		}else if (pid == 0) {
			bmount(argv[1]);
			// Directorio del proceso
			char nombredir[60];
			sprintf(nombredir, "%sproceso_%d/", camino, getpid());
			if (mi_creat(nombredir, '7') < 0) {
				printf("ERROR: main: Error de simulación creando el fichero del proceso");
				bumount();
				exit(0);
			}

			// Fichero prueba.dat del proceso actual
			char nombrefich[100];
			sprintf(nombrefich, "%sprueba.dat", nombredir);
			if (mi_creat(nombrefich, '7') < 0) {
				printf("Error de simulación creando el fichero prueba.dat del proceso\n");
				bumount();
				exit(0);
			}
			printf("--> %s\n", nombrefich);
			srand(time(NULL) + getpid());

			int j=0;
			// Hacemos NUM_ESC escrituras para el proceso actual
			for (j = 0; j < NUM_REGISTROS; j++) {
				registro.fecha = time(NULL);
				registro.pid = getpid();
				registro.nEscritura = j+1;
				registro.posicion = rand()%POS_MAX;
				mi_write(nombrefich, &registro, registro.posicion * sizeof(struct registro), sizeof(struct registro));
				//printf("PID: %d, nEscritura: %d, posicion: %d\n", registro.pid, registro.nEscritura, registro.posicion);
				usleep(t_escritura);
			}
			bumount();
			exit(0);
		}
		usleep(t_proceso);
	}
	// Esperamos a que acaben los procesos hijo
	while (acabados < NUM_PROCESOS) pause();
	// Desmontamos el dispositivo
	if (bumount() == -1) {
		printf("ERROR: main: Error desmontando el dispositivo\n");
		exit(0);
	}
	printf("Simulacion finalizada\n");
	return 0;
}
