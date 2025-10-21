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
#include <unistd.h>


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
    
    
    for(int i = 0; i < nordenes - 1; i++){
        if(pipe(fds) == -1) return ERROR;
        red_ordenes[i].salida = fds[1];
        red_ordenes[i+1].entrada = fds[0];
    }

   
    // Si es background SIN archivo de entrada → usar /dev/null
    if(bgnd && strcmp(infile, "") == 0){
        int fd_bg = open("/dev/null", O_RDONLY); 
        if(fd_bg == -1) return ERROR;
        red_ordenes[0].entrada = fd_bg;
    }
    // Si HAY archivo de entrada especificado
    else if(strcmp(infile, "") != 0){
        int fd_in = open(infile, O_RDONLY);
        if(fd_in == -1) return ERROR;
        red_ordenes[0].entrada = fd_in;
    }

   
    if(strcmp(outfile, "") != 0/* outfile != NULL */){ 
        int flags;
        printf("he entrado en redireccion outfile: %s\n", outfile);
        if(append){ 
            // Append: >> (añadir al final)
            flags = O_WRONLY | O_CREAT | O_APPEND;
        }
        else { 
            // Trunk: > (sobrescribir)
            flags = O_WRONLY | O_CREAT | O_TRUNC;
        }

        int fd_out = open(outfile, flags, 0644); 
        if(fd_out == -1) return ERROR;
        red_ordenes[nordenes - 1].salida = fd_out; 
    }

    return OK;  
} //Fin de la función pipeline



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


