#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <math.h>

//OJO: Separa funciones de main despues!.
//ESTRUCTURAS
typedef struct inputParameters
{
    const char *archivoV;
    const char *archivoS;
    int ndiscos;
    int ancho;
    int bandera;
}entrada;


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
            for(int j = 0; j < NUMEROENTRADAS;j++){
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

//MAIN
int main(int argc, char const *argv[])
{
    //Obtener datos de entrada.

    //Crear hebras
    //Leer archivo.
    //A medida que se lea se les asigna a las hebras.


    return 0;
}
