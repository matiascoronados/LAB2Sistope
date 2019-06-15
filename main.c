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
char* parametros[6] = {"-i","-o","-n","-d","-s","-b"};


//NUEVO
resultados* resultadosExperimento;
//NUEVO


int padreleyendoA=1;


#define FLOATMAX 20
#define BUFFERLECTURA 100

datos *crearDato(){
    datos *nuevo = malloc(sizeof(datos));
    return nuevo;
}



//NUEVO
void asignarResultados(float acumMedia, float acumMediana, float acumRuido, float acumPoten, int cantidadVisibilidades,int numeroDisco){
    resultadosExperimento[numeroDisco].acumMedia = acumMedia;
    resultadosExperimento[numeroDisco].acumMediana = acumMediana;
    resultadosExperimento[numeroDisco].acumRuido = acumRuido;
    resultadosExperimento[numeroDisco].acumPoten = acumPoten;
    resultadosExperimento[numeroDisco].cantidadVisibilidades = cantidadVisibilidades;
    resultadosExperimento[numeroDisco].numeroDisco = numeroDisco;
}
//NUEVO



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

void inicializarMonitor(monitor *mon, entrada *entradas, int numeroDisco){

  //monitor *nuevo = (monitor*)malloc(sizeof(monitor));
  pthread_mutex_init(&mon -> mutex, NULL);
  pthread_cond_init(&mon -> bufferLleno, NULL);
  pthread_cond_init(&mon -> bufferVacio, NULL);
  //pthread_cond_init(&mon -> bufferRestanteL, NULL);
  //pthread_cond_init(&mon -> bufferRestanteV, NULL);
  mon -> capacidadBuffer = entradas -> buffer;
  mon -> cantidadDeDatos = 0;

  //NUEVO
  mon -> numeroDisco = numeroDisco;
  //NUEVO


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
        printf("la cantidad de datos en mi buffer es: %d\n", monitor -> cantidadDeDatos);
      }

      for(int i = 0; i < monitor->cantidadDeDatos; i++)
      {
        datos dato = monitor->buffer[i];
        acumMedia = mediaReal(&dato, acumMedia);
        acumMediana = medianaImaginaria(&dato, acumMediana);
        acumRuido = ruidoTotal(&dato, acumRuido);
        acumPoten = potencia(&dato, acumPoten);

        //NUEVO
        cantidadVisibilidades++;
        //NUEVO
      }
      //ya calcule, debo decir que el buffer esta bufferVacio
      monitor->cantidadDeDatos = 0;
      printf("vacie mi buffer\n");
      pthread_cond_signal(&monitor->bufferVacio);
    }

    //NUEVO
    //ESTO DEBE IR AL FINAL DE TODO EL PROCESAMIENTO.
    int numeroDisco = monitor->numeroDisco;
    if(acumMedia != 0 ){acumMedia = acumMedia / cantidadVisibilidades;}
    if(acumMediana != 0){acumMediana = acumMediana / cantidadVisibilidades;}
    asignarResultados(acumMedia,acumMediana,acumRuido,acumPoten,cantidadVisibilidades,numeroDisco);
    //NUEVO

    /*f (monitor -> cantidadDeDatos != 0) {
      for(int i = 0; i < monitor->cantidadDeDatos; i++){
        datos dato = monitor->buffer[i];
        acumMedia = mediaReal(&dato, acumMedia);
        acumMediana = medianaImaginaria(&dato, acumMediana);
        acumRuido = ruidoTotal(&dato, acumRuido);
        acumPoten = potencia(&dato, acumPoten);
      }
      monitor->cantidadDeDatos = 0;
    }*/

    printf("termine mi ejecucion\n");
    pthread_cond_signal(&monitor -> bufferVacio);
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

        printf("Padre lee el dato: %s\n", aux2);
        fscanf(archivo, " %[^','], %[^','], %[^','], %[^','], %s",aux2, aux3, aux4, aux5, aux6);
      }
      else{
        printf("Padre llena el buffer %d\n", disco);
        pthread_cond_signal(&monitores[disco]->bufferLleno);
        //pthread_cond_wait(&monitores[disco]->bufferVacio, &monitores[disco]->mutex);
      }
    }
    printf("padre deja de leer\n");
    padreleyendoA=0;

    pthread_cond_signal(&monitores[disco] -> bufferLleno);

    for (int i = 0; i < entrada -> ndiscos; i++) {
      //if (monitores[i] -> cantidadDeDatos != 0) {
        printf("quedo un buffer con datos\n");
        pthread_cond_signal(&monitores[i] -> bufferLleno);
        //pthread_cond_wait(&monitores[i] -> bufferVacio, &monitores[i] -> mutex);
        printf("ya se vacio\n");
    //  }
    }
    printf("cago padre\n");
  }





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
