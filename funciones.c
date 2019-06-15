#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "funciones.h"

#include <pthread.h>
#include <math.h>

// -Descripcion: Se inicializa y retorna un puntero a una estructura de datos.
// -Entradas: N/A
// -Salidas: Puntero a una estructura de datos inicializada.
datos *crearDato(){
    datos *nuevo = malloc(sizeof(datos));
    return nuevo;
}

// -Descripcion: Se le asigna los valores de los resultados a una estrucutra del mismo nombre.
// -Entradas: Acumuladores para los valores de la media real y imaginaria, la potencia, ruido, visibilidades y numero de disco.
// -Salidas: N/A
void asignarResultados(float acumMedia, float acumMediana, float acumRuido, float acumPoten, int cantidadVisibilidades,int numeroDisco){
    resultadosExperimento[numeroDisco].acumMedia = acumMedia;
    resultadosExperimento[numeroDisco].acumMediana = acumMediana;
    resultadosExperimento[numeroDisco].acumRuido = acumRuido;
    resultadosExperimento[numeroDisco].acumPoten = acumPoten;
    resultadosExperimento[numeroDisco].cantidadVisibilidades = cantidadVisibilidades;
    resultadosExperimento[numeroDisco].numeroDisco = numeroDisco;
}

// -Descripcion: Se inicializa y retorna un puntero a una entrada
// -Entradas: N/A
// -Salidas: Puntero a una estructura de entrada inicializada.
entrada* crearEntrada(){
    entrada *p_entrada = malloc(sizeof(entrada));
    p_entrada->archivoV = "null";
    p_entrada->archivoS = "null";
    p_entrada->ancho = -1;
    p_entrada->ndiscos = -1;
    p_entrada->buffer = -1;
    p_entrada->bandera = 0;
    return p_entrada;
}

// -Descripcion: Funcion que sirve para inicializar un puntero de un monitor.
// -Entradas: Puntero a una estrucuta de monitor, otra de entradas y un entero que indica el numero de disco que
//            pertenece el monitor.
// -Salidas: N/A
void inicializarMonitor(monitor *mon, entrada *entradas, int numeroDisco){
  pthread_mutex_init(&mon -> mutex, NULL);
  pthread_cond_init(&mon -> bufferLleno, NULL);
  pthread_cond_init(&mon -> bufferVacio, NULL);
  mon -> capacidadBuffer = entradas -> buffer;
  mon -> cantidadDeDatos = 0;
  mon -> numeroDisco = numeroDisco;
  mon -> buffer = (datos*)malloc(sizeof(datos)*entradas -> buffer);
}

// -Descripcion: Analiza las entradas del main, en busca de los parametros solicitados, en los casos
//               de que estos sean validos, se retornara un punto a una estrucuta entrada con los
//               respectivos valores, en caso contrario se detendra la ejecucion del programa.
// -Entradas: Entero argc que indica la cantida de elementos que tiene el arreglo argv
// -Salidas: Puntero a una estructura de entrada inicializada.
entrada* analizarEntradas(int argc,char const *argv[])
{
    entrada *p_entrada = crearEntrada();
    if(argc < 11){
        printf("Faltan parametros, verifique los datos de entrada\n");
        exit(0);
    }
    else if(argc > 12){
        printf("Demasiados parametros, verifique los datos de entrada\n");
        exit(0);
    }
    else{
        for(int i = 1; i < argc ; i++){
            for(int j = 0; j < 6;j++){
                char* tipoEntrada = parametros[j];
                if(strncmp(tipoEntrada,argv[i],2) == 0){
                    if(j == 0){p_entrada->archivoV = argv[i+1];}
                    else if(j == 1){p_entrada->archivoS = argv[i+1];}
                    else if(j == 2){p_entrada->ndiscos = atoi(argv[i+1]);}
                    else if(j == 3){p_entrada->ancho = atoi(argv[i+1]);}
                    else if(j == 4){p_entrada->buffer = atoi(argv[i+1]);}
                    else {p_entrada->bandera = 1;}
                }
            }
        }
        FILE* archivo = fopen(p_entrada -> archivoV,"r");
        if(p_entrada->ancho == -1 ||p_entrada->ndiscos == -1 || p_entrada->buffer == -1 || strncmp(p_entrada->archivoV,"null",3) == 0 || strncmp(p_entrada->archivoS,"null",3) == 0){
            printf("Verifique los datos de entrada\n");
            exit(0);
        }
        else if(p_entrada->ancho < 1 || p_entrada->ndiscos < 1 || p_entrada->buffer < 1)
        {
            printf("Verifique los datos de entrada\n");
            exit(0);
        }
        else if(archivo == NULL){
            printf("Ingrese un archivo de entrada valido\n");
            exit(0);
        }
        else {
            fclose(archivo);
            return p_entrada;
        }
    }
}

// -Descripcion: De los datos de entrada se obtiene el dato real para luego sumarlo con un acumulador y retornarlo.
// -Entradas: Datos de entrada, flotante acumulador
// -Salidas: Flotante del calculo de media real
float mediaReal(datos* datos, float acumulador){
    float real = datos->real;
    return real + acumulador;
}

// -Descripcion: De los datos de entrada se obtiene el dato real para luego sumarlo con un acumulador y retornarlo.
// -Entrada: Datos de entrada, flotante acumulador
// -Salida: Flotante del calculo de mediana imaginaria
float medianaImaginaria(datos* datos, float acumulador){
    float imag = datos->imag;
    return imag + acumulador;
}

// -Descripcion: De los datos de entrada se obtiene el dato real para luego sumarlo con un acumulador y retornarlo.
// -Entrada: Datos de entrada, flotante acumulador
// -Salida: Flotante del calculo de ruido total
float ruidoTotal(datos* datos, float acumulador){
    float ruido = datos->ruido;
    return ruido + acumulador;
}

// -Descripcion: De los datos de entrada se obtiene el dato real e imaginario para procesarlo, y luego retornarlo.
// -Entrada: Datos de entrada, flotante acumulador
// -Salida: Flotante del calculo de la potencia
float potencia(datos* datos, float acumulador){
    float real = datos->real;
    float imag = datos->imag;
    float resultado = real*real + imag*imag;
    return sqrtf(resultado) + acumulador;
}

// -Descripcion: Inicializa y asigna memoria a la estructura datos
// -Entrada: -
// -Salida: Puntero a una tabla de datos
datos* crearTabla(){
    datos *p_datos = malloc(sizeof(datos));
    p_datos->u = 0.0;
    p_datos->v = 0.0;
    p_datos->real = 0.0;
    p_datos->imag = 0.0;
    p_datos->ruido = 0.0;
    return p_datos;
}

// -Descripcion: Funcion en que trabaja las hebras creadas. Dentro de esta se encuentran las estructuras y elementos
//               necesarios para que se lea continuamente datos de la hebra "padre", los cuales se almacenaran en
//               acumuladores de la misma funcion.
//               Al final, se escriben los resultados dentro de una estructura que comparten todas las hebras.
// -Entrada: Entrada void que se setea a un monitor de la hebra.
// -Salida: N/A
void *funcion (void* entrada){
    monitor *monitor = malloc(sizeof(monitor));
    monitor = entrada;
    int cantidadVisibilidades = 0;
    float acumMedia = 0;
    float acumMediana = 0;
    float acumRuido = 0;
    float acumPoten= 0;
    while(padreleyendoA == 1){
      if(monitor->capacidadBuffer != monitor->cantidadDeDatos){
        pthread_cond_wait(&monitor->bufferLleno, &monitor->mutex);
        //printf("la cantidad de datos en mi buffer es: %d\n", monitor -> cantidadDeDatos);
      }
      for(int i = 0; i < monitor->cantidadDeDatos; i++)
      {
        datos dato = monitor->buffer[i];
        acumMedia = mediaReal(&dato, acumMedia);
        acumMediana = medianaImaginaria(&dato, acumMediana);
        acumRuido = ruidoTotal(&dato, acumRuido);
        acumPoten = potencia(&dato, acumPoten);
        cantidadVisibilidades++;
      }
      monitor->cantidadDeDatos = 0;
      //printf("vacie mi buffer\n");
      pthread_cond_signal(&monitor->bufferVacio);
    }
    int numeroDisco = monitor->numeroDisco;
    if(acumMedia != 0 ){acumMedia = acumMedia / cantidadVisibilidades;}
    if(acumMediana != 0){acumMediana = acumMediana / cantidadVisibilidades;}
    asignarResultados(acumMedia,acumMediana,acumRuido,acumPoten,cantidadVisibilidades,numeroDisco);
    //printf("termine mi ejecucion\n");
    pthread_cond_signal(&monitor -> bufferVacio);
}

// -Descripcion: Calcula la distancia de un dato determinado.
// -Entradas: datos* datos.
// -Salidas: Un dato de tipo float correspondiente a la distancia calculada.
float distanciaVisibilidad(datos *datos)
{
    float u = datos->u;
    float v = datos->v;
    float resultado = u*u + v*v;
    return sqrtf(resultado);
}

// -Descripcion: Se encarga de analizar a cual disco le pertenece un dato de entrada, en relacion a las entradas
//               del programa.
// -Enntardas: Puntero a una estructuras de entradas, y otro a datos.
// -Salidas: Entero que indica el numero del disco.
int calcularDisco(entrada *entradas, datos *dato){

  float distancia = distanciaVisibilidad(dato);
  float limInferior = 0;
  float limSuperior = entradas -> ancho;
  int datoPosicionado = 0;
  int discoDelDato = 0;
  while(datoPosicionado == 0){
      if( (limInferior <= distancia && limSuperior > distancia) || discoDelDato == entradas -> ndiscos-1){
          return discoDelDato;
      }
      else{
          limInferior = limSuperior;
          limSuperior = limSuperior + entradas -> ancho;
          discoDelDato++;
      }
  }
}

// -Descripcion: Funcion que se encarga de distribuir los datos del experimento entre las hebras, aplicando
//               el uso de monitores para ello. La funcion entragara los datos a medida que los vaya leyendo
//               del archivo de entrada.
// -Entradas: Puntero doble a un arreglo de monitores, y un puntero a una estructura de entradas.
// -Salidas: N/A
void procesarDatos(monitor **monitores, entrada *entrada){
  FILE *archivo;
  archivo = fopen(entrada -> archivoV,"r");
  char aux2[50];
  char aux3[50];
  char aux4[50];
  char aux5[50];
  char aux6[50];
  int disco;
  fscanf(archivo, " %[^','], %[^','], %[^','], %[^','], %s",aux2, aux3, aux4, aux5, aux6);

  while(!feof(archivo)){
      if (monitores[disco] -> cantidadDeDatos != monitores[disco] -> capacidadBuffer) {

        datos *nuevo = crearDato();
        sscanf(aux2, "%f", &nuevo -> u);
        sscanf(aux3, "%f", &nuevo -> v);
        sscanf(aux4, "%f", &nuevo -> real);
        sscanf(aux5, "%f", &nuevo -> imag);
        sscanf(aux6, "%f", &nuevo -> ruido);
        disco = calcularDisco(entrada, nuevo);

        int cantidad = monitores[disco]->cantidadDeDatos;

        datos auxiliar;
        auxiliar.u = nuevo->u;
        auxiliar.v = nuevo->v;
        auxiliar.real = nuevo->real;
        auxiliar.imag = nuevo->imag;
        auxiliar.ruido = nuevo->ruido;

        monitores[disco]->buffer[cantidad] = auxiliar;
        monitores[disco]->cantidadDeDatos = monitores[disco]->cantidadDeDatos + 1;

        //printf("Padre lee el dato: %s\n", aux2);
        fscanf(archivo, " %[^','], %[^','], %[^','], %[^','], %s",aux2, aux3, aux4, aux5, aux6);
      }
      else{
        //printf("Padre llena el buffer %d\n", disco);
        pthread_cond_signal(&monitores[disco]->bufferLleno);
      }
    }
    //printf("padre deja de leer\n");
    padreleyendoA=0;
    pthread_cond_signal(&monitores[disco] -> bufferLleno);
    for (int i = 0; i < entrada -> ndiscos; i++) {
        //printf("quedo un buffer con datos\n");
        pthread_cond_signal(&monitores[i] -> bufferLleno);
        //printf("ya se vacio\n");
    }
    //printf("murio padre\n");
  }
