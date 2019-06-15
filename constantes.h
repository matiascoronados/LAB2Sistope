#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

#include <pthread.h>
#include <math.h>

#ifndef CONSTANTES_H
#define CONSTANTES_H

//CONSTANTES
char* parametros[6] = {"-i","-o","-n","-d","-s","-b"};


//NUEVO
resultados* resultadosExperimento;
//NUEVO


int padreleyendoA=1;


#define FLOATMAX 20
#define BUFFERLECTURA 100

#endif
