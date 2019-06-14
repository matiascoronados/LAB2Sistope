#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

typedef struct tipoMonitor {
  pthread_mutex_t mutex;

  char **buffer;
  int totalBuffer;
  int cantidadBuffer;

  //Condiciones
  int con_lleno;
  int con_vacio;
  int turno;
  int sem;
}monitor; 


monitor auxA;
//--------------------------------------------------------------------------------

//FUNCIONES MONITOR

void monitor_escribir(char* a,monitor b) {
    if(auxA.turno == 1)
    {
        printf("\nENTRE ESPERA escribir");
        wait(&auxA.sem);
    }
    else
    {
        if(auxA.con_vacio == 1)
        {
            pthread_mutex_lock(&auxA.mutex);
            auxA.con_vacio = 0;
            auxA.buffer[auxA.cantidadBuffer] = a;
            auxA.cantidadBuffer = auxA.cantidadBuffer+1;
        }
        else if(auxA.con_vacio == 0 && auxA.con_lleno == 0)
        {
            auxA.buffer[auxA.cantidadBuffer] = a;
            auxA.cantidadBuffer = auxA.cantidadBuffer+1;
            if(auxA.cantidadBuffer == auxA.totalBuffer - 1)
            {
                auxA.con_lleno = 1;
                pthread_mutex_unlock(&auxA.mutex);
                auxA.turno = 0;
                signal(auxA.sem,NULL);
            }
        }
    }
}

void monitor_leer(monitor b) {
    if(auxA.turno == 0)
    {
        printf("\nENTRE ESPERA leer");
        wait(&auxA.sem);
    }
    else
    {
        if(auxA.con_lleno == 1)
        {
            pthread_mutex_lock(&auxA.mutex);
            for(int i = 0 ; i < auxA.cantidadBuffer;i++)
            {
                printf("%s",auxA.buffer[i]);
            }
            auxA.cantidadBuffer = 0;
            auxA.con_lleno = 0;
            auxA.con_vacio = 1;
            pthread_mutex_unlock(&auxA.mutex);
            auxA.turno = 1;
            signal(auxA.sem,NULL);
        }
    }
}




//FUNCIONES GENERALES
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

void inicializarMonitor(monitor *monitor)
{
    pthread_mutex_t aux = monitor->mutex;
    monitor->buffer = (char**)malloc(sizeof(char*)*10);
    for(int i = 0 ; i < 10;i++){
        monitor->buffer[i] = (char*)malloc(sizeof(char*)*1000);
    }
    monitor->con_lleno = 0;
    monitor->con_vacio = 1;
    monitor->totalBuffer = 10;
    monitor->cantidadBuffer = 0;
    monitor->turno = 0;
    monitor->sem = 1;
    pthread_mutex_init(&aux,NULL);
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


void *funcion (void *dato)
{
    int datoA = (int)(long)dato;
    monitor_leer(auxA);
}


//--------------------------------------------------------------------------------

//MAIN
int main(int argc, char const *argv[])
{
    //entrada* entradas = analizarEntradas(argc, argv);

    int numeroHebras = 10;

    pthread_t *hebras;
    monitor *monitores;
    
    pthread_t auxB;

    hebras = (pthread_t*)malloc(numeroHebras*sizeof(pthread_t)); 
    monitores = (monitor*)malloc(numeroHebras*sizeof(monitor));

    for(int i = 0 ; i < numeroHebras;i++){
        inicializarMonitor(&monitores[i]);
    }

    for(int i = 0 ; i < numeroHebras;i++){
        pthread_create(&hebras[i], NULL, funcion, (void *)1);
    }

    for(int i = 0 ; i < numeroHebras;i++){
        pthread_join(hebras[i], NULL);
    }

    //TEST

    inicializarMonitor(&auxA);
    pthread_create(&auxB, NULL, funcion, (void *)1);


    for(int i = 0 ; i < 10 ; i ++)
    {
        monitor_escribir("\nAHHHHHH",auxA);
    }
    for(int i = 0 ; i < 10 ; i ++)
    {
        monitor_escribir("\nUAJDAJSDKAHJ",auxA);
    }
    pthread_join(auxB, NULL);
    



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
