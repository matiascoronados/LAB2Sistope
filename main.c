#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

#include <pthread.h>
#include <math.h>

int padreleyendoA=1;
//OJO: Separar funciones de main despues.
//OJO: Debe reconocer errores de entrada.

//CONSTANTES
char* parametros[6] = {"-i","-o","-n","-d","-s","-b"};
#define FLOATMAX 20
#define BUFFERLECTURA 100

datos *crearDato(){
    datos *nuevo = malloc(sizeof(datos));
    return nuevo;
}

entrada* crearEntrada()
{
    entrada *p_entrada = malloc(sizeof(entrada));
    p_entrada->archivoV = "null";
    p_entrada->archivoS = "null";
    p_entrada->ancho = -1;
    p_entrada->ndiscos = -1;
    p_entrada->buffer = -1;
    p_entrada->bandera = 0;
    return p_entrada;
}

void inicializarMonitor(monitor *mon, entrada *entradas){

  //monitor *nuevo = (monitor*)malloc(sizeof(monitor));
  pthread_mutex_init(&mon -> mutex, NULL);
  pthread_cond_init(&mon -> bufferLleno, NULL);
  pthread_cond_init(&mon -> bufferVacio, NULL);
  mon -> capacidadBuffer = entradas -> buffer;
  mon -> cantidadDeDatos = 0;
  mon -> buffer = (datos*)malloc(sizeof(datos)*entradas -> buffer);
}

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
        if(p_entrada->ancho == -1 ||p_entrada->ndiscos == -1 || p_entrada->buffer == -1 || strncmp(p_entrada->archivoV,"null",3) == 0 || strncmp(p_entrada->archivoS,"null",3) == 0){
            printf("Faltan parametros, verifique los datos de entrada\n");
            exit(0);
        }
        else {return p_entrada;}
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

// -Descripcion: A razon del tipo de dato entrado, se procedera a transformar el string de entrada en uno de los
//               posibles datos, para luego almacenarlo.
// -Entrada: Puntero a una tabla de datos, entero que indica un tipo del mismo, y un char del dato tipo string
// -Salida: -

/*
void escribirDatos(monitor *m){

  int i, dato, pos = 0;

  for(i = 0; i < 200; i++){

    dato = i;

    if(m -> cantidadDeDatos == m -> capacidadBuffer) {

      pthread_mutex_lock(&m -> mutex);
      pthread_cond_wait(&m -> bufferVacio, &m -> mutex);
      pthread_mutex_unlock(&m -> mutex);
    }

    m -> buffer[pos] = i;
    pos = (pos + 1);
    m -> cantidadDeDatos = m -> cantidadDeDatos + 1;
    if (m -> cantidadDeDatos < 19) {
      printf("%d ", dato);
    }
    else{
      printf("%d\n", dato);
    }

    if (m -> cantidadDeDatos == m -> capacidadBuffer - 1) {
      pthread_mutex_lock(&m -> mutex);
      pthread_cond_signal(&m -> bufferLleno);
      pthread_mutex_unlock(&m -> mutex);
    }
    //pthread_cond_signal(&m -> bufferLleno);

  }

  pthread_exit(0);

}
void escribirDatosA(monitor *m){

  int i, dato, pos = 0;

  for(i = 0; i < 200; i++){

    dato = i;
    if(m->bufferLLeno) {
      pthread_cond_signal(&m -> bufferVacio, &m -> mutex);
      pthread_cond_wait(&m -> bufferVacio, &m -> mutex);
    }

    if(m -> cantidadDeDatos == m -> capacidadBuffer) {
      pthread_mutex_lock(&m -> mutex);
      pthread_mutex_unlock(&m -> mutex);
    }

    m -> buffer[pos] = i;
    pos = (pos + 1);
    m -> cantidadDeDatos = m -> cantidadDeDatos + 1;

    if (m -> cantidadDeDatos < 19) {
      printf("%d ", dato);
    }
    else{
      printf("%d\n", dato);
    }

    if (m -> cantidadDeDatos == m -> capacidadBuffer) {
      pthread_mutex_lock(&m -> mutex);
      pthread_cond_signal(&m -> bufferLleno);
      pthread_mutex_unlock(&m -> mutex);
    }
    //pthread_cond_signal(&m -> bufferLleno);

  }

  pthread_exit(0);
}
*/
//-------------------------------------------------------------------------------- PSEUDO CODIGO
void *funcion (void* entrada)
{
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
        if(padreleyendoA == 0){
          printf("\nAHHH LIBERADOB\n");
        }
      }
      if(padreleyendoA == 1)
      {
        printf("\n Entree!!");
      for(int i = 0; i < monitor->cantidadDeDatos; i++)
      {
        datos dato = monitor->buffer[i];
        acumMedia = mediaReal(&dato, acumMedia);
        acumMediana = medianaImaginaria(&dato, acumMediana);
        acumRuido = ruidoTotal(&dato, acumRuido);
        acumPoten = potencia(&dato, acumPoten);
        printf("\n%f %f",acumMedia, dato.u);
      }
      //ya calcule, debo decir que el buffer esta bufferVacio
      monitor->cantidadDeDatos = 0;
      pthread_cond_signal(&monitor->bufferVacio);
      sleep(1);
      }
    }
    printf("%d\n", padreleyendoA);
    //puede que el buffer tenga datos pero no esta bufferLLeno
    for(int i = 0; i < monitor->cantidadDeDatos; i++){
      datos dato = monitor->buffer[i];
      acumMedia = mediaReal(&dato, acumMedia);
      acumMediana = medianaImaginaria(&dato, acumMediana);
      acumRuido = ruidoTotal(&dato, acumRuido);
      acumPoten = potencia(&dato, acumPoten);
    }
  //  printf("\n Media = %f \n",acumMedia);
}




float distanciaVisibilidad(datos *datos)
{
    float u = datos->u;
    float v = datos->v;
    float resultado = u*u + v*v;
    return sqrtf(resultado);
}

int calcularDisco(entrada *entradas, datos *dato){

  float distancia = distanciaVisibilidad(dato);
  float limInferior = 0;
  float limSuperior = entradas -> ancho;
  int datoPosicionado = 0;
  int discoDelDato = 0;
  //Funcion para posicionar.

  while(datoPosicionado == 0)
  {
      if( (limInferior <= distancia && limSuperior > distancia) || discoDelDato == entradas -> ndiscos-1)
      {
          return discoDelDato;
      }
      else
      {
          limInferior = limSuperior;
          limSuperior = limSuperior + entradas -> ancho;
          discoDelDato++;
      }
  }
}

void procesarDatos(monitor **monitores, entrada *entrada){
  FILE *archivo;
  archivo = fopen(entrada -> archivoV,"r");
  char aux2[50];
  char aux3[50];
  char aux4[50];
  char aux5[50];
  char aux6[50];
  int disco;

  while(fscanf(archivo, " %[^','], %[^','], %[^','], %[^','], %s",aux2, aux3, aux4, aux5, aux6) == 5){
      datos *nuevo = crearDato();
      sscanf(aux2, "%f", &nuevo -> u);
      sscanf(aux3, "%f", &nuevo -> v);
      sscanf(aux4, "%f", &nuevo -> real);
      sscanf(aux5, "%f", &nuevo -> imag);
      sscanf(aux6, "%f", &nuevo -> ruido);
      disco = calcularDisco(entrada, nuevo);

    if(monitores[disco]->cantidadDeDatos == monitores[disco]->capacidadBuffer){
      pthread_cond_signal(&monitores[disco]->bufferLleno);
      pthread_cond_wait(&monitores[disco]->bufferVacio, &monitores[disco]->mutex);
    }

    int cantidad = monitores[disco]->cantidadDeDatos;

    datos auxiliar;
    auxiliar.u = nuevo->u;
    auxiliar.v = nuevo->v;
    auxiliar.real = nuevo->real;
    auxiliar.imag = nuevo->imag;
    auxiliar.ruido = nuevo->ruido;
  //  printf("\nMande un dato: %d",cantidad);
    monitores[disco]->buffer[cantidad] = auxiliar;
    monitores[disco]->cantidadDeDatos = monitores[disco]->cantidadDeDatos + 1;
    }

    padreleyendoA=0;

    for(int i = 0 ; i < entrada->ndiscos;i++)
    {
      sleep(1);
      pthread_cond_signal(&monitores[i]->bufferLleno);
      printf("\nLIBERE AL ZANGANO %d\n",i);
    }


  }





//--------------------------------------------------------------------------------

//MAIN
int main(int argc, char const *argv[])
{
    padreleyendoA = 1;
    entrada* entradas = analizarEntradas(argc, argv);
    //monitor monitores[entradas -> ndiscos];
    pthread_t *hebras = malloc(sizeof(pthread_t)*entradas->ndiscos);
    //[entradas -> ndiscos];
    monitor **monitoresUWU;

        monitoresUWU = (monitor**)malloc(entradas->ndiscos*sizeof(monitor*));
	     for(int i=0;i<entradas->ndiscos;i++){
		     monitoresUWU[i] = (monitor*)malloc(sizeof(monitor));
         inicializarMonitor(monitoresUWU[i],entradas);
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
