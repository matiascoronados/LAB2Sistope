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

typedef struct monitorM { 
       int size, theArray[1];
        int emptyFull;    
        void put(int x) {      
            if (size == ARRAY_SIZE) wait(emptyFull);
                theArray[size] = x;      
                size++;      
            if (size == 1) 
            broadcast(emptyFull);    
            }    
        int get() {      
            if (size == 0) 
                wait(emptyFull);       
                size--;      
            if (size == ARRAY_SIZE-1) 
                broadcast(emptyFull);
            return theArray[size];
            }
            }ahh;

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



void *funcion (void* entrada)
{
    printf("\nENTRE");
}

//--------------------------------------------------------------------------------

//MAIN
int main(int argc, char const *argv[])
{
    entrada* entradas = analizarEntradas(argc, argv);


    int numeroHebras = 10;

    pthread_t *hebras;

    hebras = (pthread_t*)malloc(numeroHebras*sizeof(pthread_t)); 

    for(int i = 0 ; i < numeroHebras;i++){
        pthread_create(&hebras[i], NULL, funcion, (void *)entrada);
    }

    for(int i = 0 ; i < numeroHebras;i++){
        pthread_join(hebras[i], NULL);
    }

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
