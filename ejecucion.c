/*-----------------------------------------------------+      
 |     E J E C U C I O N . C                           |
 +-----------------------------------------------------+
 |     Asignatura :  SOP-GIIROB                        |
 |     Descripcion:                                    |
 +-----------------------------------------------------*/
#include "defines.h"
#include "redireccion.h"
#include "ejecucion.h"
#include <signal.h>
#include "profe.h"
#include <unistd.h>
#include <sys/wait.h>


int ejecutar (int nordenes , int *nargs , char **ordenes , char ***args , int bgnd)
{
    pid_t aux;
    for(int i=0; i<nordenes; i++){
        aux = fork();
        if(aux < 0){
            perror("fork");
            return ERROR;
        }
        if(aux==0){ //Hijo
           if(redirigir_salida(i) == ERROR){
               perror("redirigir_salida");
               _exit(EXIT_FAILURE);
           }
           if(redirigir_entrada(i) == ERROR){
               perror("redirigir_entrada");
               _exit(EXIT_FAILURE);
           }
           cerrar_fd();

           execvp(ordenes[i],args[i]);
           perror("execvp");
           _exit(EXIT_FAILURE);
        }
    }
    /* padre: cerrar ficheros y esperar a todos los hijos */
    cerrar_fd();
    while(wait(NULL) > 0) ; /* esperar hasta que no queden hijos */
    //wait() -> PID del hijo terminado (>0) cuando hay un hijo reaparecido
    return OK;

} // Fin de la funcion "ejecutar"
