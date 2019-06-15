#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

#include <pthread.h>
#include <math.h>

#ifndef CONSTANTES_H
#define CONSTANTES_H

#define FLOATMAX 20
#define BUFFERLECTURA 100

//Usada para verificar los parametros de entrada al main.
char* parametros[6] = {"-i","-o","-n","-d","-s","-b"};
//Utilizada como variable global entre las hebras para almacenar informacion.
resultados* resultadosExperimento;
//Entero que norma el comportamiento de lectura del padre sobre los hijos.
int padreleyendoA=1;

#endif
