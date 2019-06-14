#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

#include <pthread.h>
#include <math.h>

//OJO: Separar funciones de main despues.
//OJO: Debe reconocer errores de entrada.

//CONSTANTES
//--------------------------------------------------------------------------------
char* parametros[5] = {"-i","-o","-n","-d","-b"};
//--------------------------------------------------------------------------------

//ESTRUCTURAS
typedef struct inputParameters
{
    const char *archivoV;
    const char *archivoS;
    int ndiscos;
    int ancho;
    int bandera;
}entrada;

//--------------------------------------------------------------------------------

//FUNCIONES
entrada* crearEntrada()
{
    entrada *p_entrada = malloc(sizeof(entrada));
    p_entrada->archivoV = "null";
    p_entrada->archivoS = "null";
    p_entrada->ancho = -1;
    p_entrada->ndiscos = -1;
    p_entrada->bandera = 0;
    return p_entrada;
}


entrada* analizarEntradas(int argc,char const *argv[])
{
    entrada *p_entrada = crearEntrada();
    if(argc < 9){
        printf("Faltan parametros, verifique los datos de entrada\n");
        exit(0);
    }
    else if(argc > 10){
        printf("Demasiados parametros, verifique los datos de entrada\n");
        exit(0);
    }
    else{
        for(int i = 1; i < argc ; i++){
            for(int j = 0; j < 5;j++){
                char* tipoEntrada = parametros[j];
                if(strncmp(tipoEntrada,argv[i],2) == 0){
                    if(j == 0){p_entrada->archivoV = argv[i+1];}
                    else if(j == 1){p_entrada->archivoS = argv[i+1];}
                    else if(j == 2){p_entrada->ndiscos = atoi(argv[i+1]);}
                    else if(j == 3){p_entrada->ancho = atoi(argv[i+1]);}
                    else {p_entrada->bandera = 1;}
                }
            }
        }
        if(p_entrada->ancho == -1 ||p_entrada->ndiscos == -1 || strncmp(p_entrada->archivoV,"null",3) == 0 || strncmp(p_entrada->archivoS,"null",3) == 0){
            printf("Faltan parametros, verifique los datos de entrada\n");
            exit(0);
        }
        else {return p_entrada;}
    }
}

void escribirDatos(monitor *m){

  int i, dato, pos = 0;
  pthread_mutex_lock(&m -> mutex);
  for(i = 0; i < 200; i++){

    dato = i;
    //printf("hebra1 cantidad de datos: %d\n", m -> cantidadDeDatos);
    if(m -> cantidadDeDatos == m -> capacidadBuffer) {

      pthread_mutex_lock(&m -> mutexVacio);
      pthread_mutex_unlock(&m -> mutex);
      pthread_cond_wait(&m -> bufferVacio, &m -> mutexVacio);
      pthread_mutex_unlock(&m -> mutexVacio);
    }
    pthread_mutex_lock(&m -> modificarB);
    m -> buffer[pos] = i;
    //printf("hebra1 escribe dato: %d\n", i);
    pos = (pos + 1);
    m -> cantidadDeDatos = m -> cantidadDeDatos + 1;
    if (m -> cantidadDeDatos < 19) {
      printf("%d ", dato);
    }
    else{
      printf("%d\n", dato);
    }
    pthread_mutex_unlock(&m -> modificarB);

    if (m -> cantidadDeDatos == m -> capacidadBuffer - 1) {
      pthread_mutex_lock(&m -> mutexLleno);
      pthread_cond_signal(&m -> bufferLleno);
      sleep(1);
      pthread_mutex_unlock(&m -> mutexLleno);
    }
    //pthread_cond_signal(&m -> bufferLleno);



  }
  pthread_mutex_unlock(&m -> mutex);

  pthread_exit(0);
}

void procesarDatos(monitor *m){

  int i, dato, pos = 0;

  for(i = 0; i < 200; i++){

    pthread_mutex_lock(&m -> mutex);
    //printf("hebra2 cantidad de datos: %d\n", m -> cantidadDeDatos);
    if(m -> cantidadDeDatos == 0) {
      pthread_mutex_lock(&m -> mutexLleno);
      pthread_mutex_unlock(&m -> mutex);
      pthread_cond_wait(&m -> bufferLleno, &m -> mutexLleno);
      pthread_mutex_unlock(&m -> mutexLleno);
    }
    pthread_mutex_lock(&m -> modificarA);
    dato = m -> buffer[pos];
    //printf("hebra2 lee dato: %d\n", dato);
    pos = (pos + 1);
    m -> cantidadDeDatos = m -> cantidadDeDatos - 1;
    pthread_mutex_unlock(&m -> modificarA);

    if (m -> cantidadDeDatos == 0) {
      pthread_mutex_lock(&m -> mutexVacio);
      pthread_cond_signal(&m -> bufferVacio);
      //sleep(1);
      pthread_mutex_unlock(&m -> mutexVacio);
    }
    //pthread_cond_signal(&m -> bufferVacio);
    printf("Hebra2 consume: %d\n", dato);

    pthread_mutex_unlock(&m -> mutex);

  }

  pthread_exit(0);
}

void *funcion (void* entrada)
{
    printf("\nENTRE");
}

//--------------------------------------------------------------------------------

//MAIN
int main(int argc, char const *argv[])
{
    //entrada* entradas = analizarEntradas(argc, argv);


    int numeroHebras = 10;
    monitor *m;
    m -> capacidadBuffer = 20;
    m -> cantidadDeDatos = 0;
    pthread_t h1, h2;

    pthread_mutex_init(&m -> mutex, NULL);
    pthread_mutex_init(&m -> mutexLleno, NULL);
    pthread_mutex_init(&m -> modificarA, NULL);
    pthread_mutex_init(&m -> modificarB, NULL);
    pthread_mutex_init(&m -> mutexVacio, NULL);
    pthread_cond_init(&m -> bufferLleno, NULL);
    pthread_cond_init(&m -> bufferVacio, NULL);


    pthread_create(&h1, NULL, (void *)&escribirDatos, m);
    pthread_create(&h2, NULL, (void *)&procesarDatos, m);
    pthread_join(h1, NULL);
    //printf("hebra1\n");
    pthread_join(h2, NULL);
  //  printf("hebra2\n");
    pthread_mutex_destroy(&m -> mutex);
    pthread_cond_destroy(&m -> bufferLleno);
    pthread_cond_destroy(&m -> bufferVacio);
    exit(0);


    //Crear hebras por cantidad de discos
    //Crear monitor por cantidad de hebras
        //Cada monitor se enlazara a una hebra
    //Cada monitor almacenara una cantidad BUFFER de datos (relacionados con su disco)
        //Cuando el BUFFER se llene, le mandara los datos a la hebra.

    //Leer archivo.
        //A medida que se lea se les asigna los datos a las hebras por medio del monitor.

    //Escribir resultados.

    return 0;
}
