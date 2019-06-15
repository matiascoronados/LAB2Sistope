#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "constantes.h"
#include "funciones.h"
#include "funciones.c"

#include <pthread.h>
#include <math.h>


// -Descripcion: Funcion principal que da inicio al programa.
// -Entradas: Argumentos de entrada.
// -Salidas: Entero que indica si el proceso fallo o si tubo exito.

int main(int argc, char const *argv[])
{
    padreleyendoA = 1;
    entrada* entradas = analizarEntradas(argc, argv);
    pthread_t *hebras = malloc(sizeof(pthread_t)*entradas->ndiscos);
    resultadosExperimento = malloc(sizeof(resultados)*entradas->ndiscos);
    monitor **monitoresUWU;
    monitoresUWU = (monitor**)malloc(entradas->ndiscos*sizeof(monitor*));

    for(int i=0;i<entradas->ndiscos;i++){
	    monitoresUWU[i] = (monitor*)malloc(sizeof(monitor));
        inicializarMonitor(monitoresUWU[i],entradas,i);
    }

    for(int i = 0; i < entradas -> ndiscos; i++){
      pthread_create(&hebras[i], NULL, funcion, (void*)monitoresUWU[i]);
    }

    procesarDatos(monitoresUWU, entradas);

    for(int i = 0; i < entradas -> ndiscos; i++){
      pthread_join(hebras[i], NULL);
    }

    if(entradas->bandera == 1){
        for(int i = 0 ; i < entradas->ndiscos;i++){
            resultados a = resultadosExperimento[i];
            printf("\nSoy la hebra %d, proces%c %d visibilidades",a.numeroDisco,130,a.cantidadVisibilidades);
        }
    }

    FILE *file = fopen(entradas->archivoS,"w");
    for(int i = 0 ; i < entradas->ndiscos;i++)
    {
        resultados a = resultadosExperimento[i];
        fprintf(file, "Disco: %d\n",a.numeroDisco+1);
        fprintf(file, "Media real: %f\n",a.acumMedia);
        fprintf(file, "Media imaginaria: %f\n",a.acumMediana);
        fprintf(file, "Potencia: %f\n",a.acumPoten);
        fprintf(file, "Ruido total: %f\n",a.acumRuido);
    }
    fclose(file);

    for(int i = 0; i < entradas -> ndiscos; i++){
      pthread_mutex_destroy(&(monitoresUWU[i]->mutex));
      pthread_cond_destroy(&(monitoresUWU[i]-> bufferLleno));
      pthread_cond_destroy(&(monitoresUWU[i]-> bufferVacio));
    }

    exit(0);
    return 0;
  }
