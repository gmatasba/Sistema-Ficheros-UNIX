CC=gcc
CFLAGS=-c -g -Wall -std=gnu99 #antes c99
LDFLAGS=-pthread

SOURCES=bloques.c ficheros_basico.c ficheros.c directorios.c mi_mkfs.c leer_sf.c escribir.c leer.c truncar.c permitir.c mi_mkdir.c mi_chmod.c mi_ls.c mi_link.c mi_escribir.c mi_cat.c mi_stat.c mi_touch.c mi_rm.c semaforo_mutex_posix.c
LIBRARIES=bloques.o ficheros_basico.o ficheros.o directorios.o semaforo_mutex_posix.o
INCLUDES=bloques.h ficheros_basico.h ficheros.h directorios.h semaforo_mutex_posix.h
PROGRAMS=mi_mkfs leer_sf escribir leer truncar permitir mi_mkdir mi_chmod mi_ls mi_link mi_escribir mi_cat mi_stat mi_touch mi_rm
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
    $(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES)
    $(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
    rm -rf *.o *~ $(PROGRAMS)
