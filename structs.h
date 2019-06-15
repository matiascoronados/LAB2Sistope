#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <math.h>

#ifndef CLASES_H
#define CLASES_H



typedef struct inputData
{
    float u;
    float v;
    float real;
    float imag;
    float ruido;
}datos;


//NUEVO
typedef struct outputData
{
    float acumMedia;
    float acumMediana;
    float acumRuido;
    float acumPoten;
    int cantidadVisibilidades;
    int numeroDisco;
}resultados;
//NUEVO

typedef struct Monitor{

  pthread_mutex_t mutex;
  pthread_cond_t bufferLleno;
  pthread_cond_t bufferVacio;

  //NUEVO
  int numeroDisco;
  //NUEVO

  int capacidadBuffer;
  int cantidadDeDatos;
  datos *buffer;

}monitor;


typedef struct inputParameters
{
    const char *archivoV;
    const char *archivoS;
    int ndiscos;
    int ancho;
    int bandera;
    int buffer;
}entrada;

#endif
