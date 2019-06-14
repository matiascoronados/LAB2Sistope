#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <math.h>

#ifndef CLASES_H
#define CLASES_H

typedef struct Monitor{

  pthread_mutex_t mutex;
  pthread_mutex_t mutexLleno;
  pthread_mutex_t mutexVacio;
  pthread_mutex_t modificarA;
  pthread_mutex_t modificarB;
  pthread_cond_t bufferLleno;
  pthread_cond_t bufferVacio;
  int capacidadBuffer;
  int cantidadDeDatos;
  int buffer[20];

}monitor;

#endif
