#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

#include <pthread.h>
#include <math.h>

#ifndef FUNCIONES_H
#define FUNCIONES_H

datos *crearDato();
void asignarResultados(float acumMedia, float acumMediana, float acumRuido, float acumPoten, int cantidadVisibilidades,int numeroDisco);
entrada* crearEntrada();
void inicializarMonitor(monitor *mon, entrada *entradas, int numeroDisco);
entrada* analizarEntradas(int argc,char const *argv[]);
float mediaReal(datos* datos, float acumulador);
float medianaImaginaria(datos* datos, float acumulador);
float ruidoTotal(datos* datos, float acumulador);
float potencia(datos* datos, float acumulador);
datos* crearTabla();
void *funcion (void* entrada);
float distanciaVisibilidad(datos *datos);
int calcularDisco(entrada *entradas, datos *dato);
void procesarDatos(monitor **monitores, entrada *entrada);

#endif
