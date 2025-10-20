/*-----------------------------------------------------+      
 |     R E D I R E C C I O N . C                       |
 +-----------------------------------------------------+
 |     Version    :                        
 |     Autor :   
 |     Asignatura :  SOP-GIIROB                                                       
 |     Descripcion: 
 +-----------------------------------------------------*/
#include "defines.h"
#include "analizador.h"
#include "redireccion.h"
#include <string.h>

REDIRECCION_ORDENES red_ordenes;


void redireccion_ini(void)
{
    for (int i = 0; i < PIPELINE; i++) {
        red_ordenes[i].entrada = 0;  // STDIN_FILENO
        red_ordenes[i].salida = 1;   // STDOUT_FILENO
    }

}//Inicializar los valores de la estructura cmdfd



int pipeline(int nordenes, char * infile, char * outfile, int append, int bgnd)
{
    redireccion_ini();
    int fds[2];
    for(int i = 0; i< nordenes - 1; i++){
        pipe(fds);
        red_ordenes[i].salida = fds[1];
        red_ordenes[i+1].entrada = fds[0];
    }

    if(bgnd && strcmp(infile, "")!= 0){
        int fd_bg = open("/dev/null", O_RDONLY); 
        red_ordenes[0].entrada = fd_bg;
    }

    if(strcmp(infile,"")!=0){
        int fd_in = open(infile, O_RDONLY);
        red_ordenes[0].entrada = fd_in;
    }

    if(strcmp(outfile,"")!=0){
        
        int flags = O_APPEND | O_CREAT;//debe ser uno u otro
        if(append){ //append
             flags = O_APPEND; 
        }
        else { //trunk
            flags = O_CREAT; 
        }

        int fd_out = open(outfile, flags ,0644); 

        red_ordenes[nordenes -1].salida = fd_out; 
    }

    





} // Fin de la funci�n "pipeline"



int redirigir_entrada(int i)
{
    if(red_ordenes[i].entrada != 0){
        if(dup2(red_ordenes[i].entrada, 0) == -1){
            return ERROR;
        }
    }

    return OK;
} // Fin de la funci�n "redirigir_entrada"



int redirigir_salida(int i)
{

    if(red_ordenes[i].salida != 1){
        if(dup2(red_ordenes[i].salida, 1) == -1){
            return ERROR;
        }
    }

    return OK;

} // Fin de la funci�n "redirigir_salida"


int cerrar_fd()
{
    for(int i = 0; i< PIPELINE; i++){
        if(red_ordenes[i].entrada != 0){
            close(red_ordenes[i].entrada);
        }
        if(red_ordenes[i].salida != 1){
            close(red_ordenes[i].salida);
        }
    }

    return OK;
} // Fin de la funci�n "cerrar_fd"


