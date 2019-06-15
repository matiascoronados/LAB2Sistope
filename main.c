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


//OJO: Separar funciones de main despues.
//OJO: Debe reconocer errores de entrada.
//--------------------------------------------------------------------------------

//MAIN
int main(int argc, char const *argv[])
{
    padreleyendoA = 1;
    entrada* entradas = analizarEntradas(argc, argv);
    //monitor monitores[entradas -> ndiscos];
    pthread_t *hebras = malloc(sizeof(pthread_t)*entradas->ndiscos);


    resultadosExperimento = malloc(sizeof(resultados)*entradas->ndiscos);
    //[entradas -> ndiscos];
    monitor **monitoresUWU;

    monitoresUWU = (monitor**)malloc(entradas->ndiscos*sizeof(monitor*));
	  for(int i=0;i<entradas->ndiscos;i++){
		    monitoresUWU[i] = (monitor*)malloc(sizeof(monitor));
        inicializarMonitor(monitoresUWU[i],entradas,i);
           //init_Foton(f[i], x, y, distMax,i, flag);
    }
    //for(int i = 0; i < entradas -> ndiscos; i++){
    //  monitores[i] = inicializarMonitor(entradas);
    //}

    for(int i = 0; i < entradas -> ndiscos; i++){
      //monitores[i] = inicializarMonitor(entradas);
      pthread_create(&hebras[i], NULL, funcion, (void*)monitoresUWU[i]);
    }

    procesarDatos(monitoresUWU, entradas);

    for(int i = 0; i < entradas -> ndiscos; i++){
      pthread_join(hebras[i], NULL);
    }

    //NUEVO

    if(entradas->bandera == 1){
        for(int i = 0 ; i < entradas->ndiscos;i++){
            resultados a = resultadosExperimento[i];
            printf("\nSoy la hebra %d, proces%c %d visibilidades",a.numeroDisco,130,a.cantidadVisibilidades);
        }
    }

    //NUEVO

    //NUEVO

    FILE *file = fopen(entradas->archivoS,"w");
    for(int i = 0 ; i < entradas->ndiscos;i++)
    {
        resultados a = resultadosExperimento[i];
        fprintf(file, "Disco: %f\n",a.numeroDisco+1.0);
        fprintf(file, "Media real: %f\n",a.acumMedia);
        fprintf(file, "Media imaginaria: %f\n",a.acumMediana);
        fprintf(file, "Potencia: %f\n",a.acumPoten);
        fprintf(file, "Ruido total: %f\n",a.acumRuido);
    }
    fclose(file);

    //NUEVO


    for(int i = 0; i < entradas -> ndiscos; i++){
      pthread_mutex_destroy(&(monitoresUWU[i]->mutex));
      pthread_cond_destroy(&(monitoresUWU[i]-> bufferLleno));
      pthread_cond_destroy(&(monitoresUWU[i]-> bufferVacio));
    }

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
