/*-----------------------------------------------------+      
 |     R E D I R E C C I O N . C                       |
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
    for(int i=0; i<nordenes; i++){
        int aux=fork();
        if(aux==0){
           redirigir_salida_profe(i);
           redirigir_entrada_profe(i);
           cerrar_fd_profe();
           
           execvp(ordenes[i],args[i]);
        }
        cerrar_fd();
        while(wait(NULL) != -1);//esperar a los procesos hijos
        return OK;
    }

} // Fin de la funcion "ejecutar"
