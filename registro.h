/*
	Autores: Gabriel Matas, Pere Joan Palou, Antoni Rotger
	 Nombre: registro.h

  Declaramos una estructura de datos registro en una cabecera aparte para que tanto
  simulacion.c como verificacion.c puedan usarlo
*/

struct registro {
	unsigned int time; //Entero (time_t) con la fecha y hora en formato epoch
	int pid; //Entero con el PID del proceso que lo creo
	int nEscritura; //Entero con el numero de escritura (de 1 a 50)
	int posicion; //Entero con el número del registro (posición) dentro del fichero
};
