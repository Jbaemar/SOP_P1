/*-----------------------------------------------------+
 | U S H. C
 +-----------------------------------------------------+
 | Versión : |
 | Autor : [Tu nombre aquí] |
 | Asignatura : SOP-GIIROB |
 | Descripción : Shell básico con funciones de lectura y visualización |
 +-----------------------------------------------------*/
#include "defines.h"
#include "analizador.h"
#include "redireccion.h"
#include "ejecucion.h"
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "profe.h"
#include <stdio.h>
#include <stdlib.h>

//
// Declaraciones de funciones locales
//
void visualizar( void );
int leerLinea( char *linea, int maxLinea );

//
// Prog. ppal.
//
int main(int argc, char * argv[])
{
    char line[255];
    int res;
    char **m_ordenes;
    char ***m_argumentos;
    int *m_num_arg;
    int m_n;
    
    while(1)
    {
        do
        {
            res=leerLinea(line,MAXLINE);  // Cambiado a usar nuestra implementación
            if (res==-2) {
                fprintf(stdout,"ush: logout\n");
                exit(0);
            }
            if (res==-1){
                fprintf(stdout,"linea muy larga\n");
            }
        }while(res<=0);
        
        if (analizar(line)==OK){
            m_n=num_ordenes();
            m_num_arg=num_argumentos();
            m_ordenes=get_ordenes();
            m_argumentos=get_argumentos();
            
            if(m_n>0)
            {
                if (pipeline_profe(m_n,fich_entrada(),fich_salida(),es_append(),es_background())==OK)
                    ejecutar(m_n,m_num_arg,m_ordenes,m_argumentos,es_background());
            }
            visualizar();  // Cambiado a usar nuestra implementación
        }
    }
    return 0;
}

/****************************************************************/
/* leerLinea
 --------------------------------------------------------------
 DESCRIPCIÓN:
 Obtiene la línea de órdenes para el mShell.
 ENTRADA:
 linea - puntero a un vector de carácteres donde se almancenas los caracteres
 leídos del teclado
 maxLinea - tamaño máximo del vector anterior
 SALIDA:
 -- linea - si termina bien, contiene como último carácter el retorno de carro.
 -- leerLinea - Entero que representa el motivo de finalización de la función:
 > 0 - terminación correcta, número de caracteres leídos, incluído '\n'
 -1 - termina por haber alcanzado el número máximo de caracteres
 -2 - termina por haber leído fin de fichero (EOF).
*/
/****************************************************************/
int leerLinea( char *linea, int maxLinea )
{
    char cwd[1024];  // Buffer para el directorio actual
    int c;           // Carácter leído
    int i = 0;       // Contador de caracteres
    
    // 1. Obtener el directorio actual de trabajo
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        strcpy(cwd, "unknown");  // Si falla, usar "unknown"
    }
    
    // 2. Mostrar el prompt: directorio + PROMPT
    printf("%s%s", cwd, PROMPT);
    fflush(stdout);  // Asegurar que se muestre inmediatamente
    
    // 3. Leer caracteres uno a uno
    while ((c = getchar()) != '\n') {
        // Comprobar si es EOF (Ctrl+D)
        if (c == EOF) {
            return -2;
        }
        
        // Comprobar si excedemos el tamaño máximo
        if (i >= maxLinea - 1) {  // -1 para dejar espacio al '\0'
            // Consumir el resto de la línea hasta encontrar '\n' o EOF
            while ((c = getchar()) != '\n' && c != EOF);
            return -1;
        }
        
        // Almacenar el carácter en la línea
        linea[i] = c;
        i++;
    }
    
    // 4. Añadir el carácter '\n' al final
    linea[i] = '\n';
    i++;
    
    // 5. Añadir el terminador de cadena
    linea[i] = '\0';
    
    // 6. Retornar el número de caracteres leídos (incluyendo '\n')
    return i;
}

/****************************************************************/
/* visualizar */
/*--------------------------------------------------------------*/
/* DESCRIPCIÓN: */
/* Visualiza los distintos argumentos de la orden analizada. */
/* ENTRADA: void */
/* SALIDA: void */
/****************************************************************/
void visualizar( void )
{
    int n_ordenes = num_ordenes();
    char **ordenes = get_ordenes();
    char ***argumentos = get_argumentos();
    int *num_args = num_argumentos();
    
    printf("\n=== ANÁLISIS DE LA LÍNEA DE ÓRDENES ===\n");
    
    // 1. Mostrar número de órdenes
    printf("Número de órdenes: %d\n", n_ordenes);
    
    // 2. Mostrar cada orden y sus argumentos
    for (int i = 0; i < n_ordenes; i++) {
        printf("Orden %d: '%s'\n", i + 1, ordenes[i]);
        printf("  Número de argumentos: %d\n", num_args[i]);
        
        // Mostrar cada argumento
        for (int j = 0; j < num_args[i]; j++) {
            printf("    Argumento %d: '%s'\n", j, argumentos[i][j]);
        }
    }
    
    // 3. Redirección de entrada
    char *entrada = fich_entrada();
    if (entrada != NULL && strlen(entrada) > 0) {
        printf("Redirección de entrada: < %s\n", entrada);
    } else {
        printf("Sin redirección de entrada\n");
    }
    
    // 4. Redirección de salida
    char *salida = fich_salida();
    if (salida != NULL && strlen(salida) > 0) {
        if (es_append()) {
            printf("Redirección de salida (APPEND): >> %s\n", salida);
        } else {
            printf("Redirección de salida (TRUNK): > %s\n", salida);
        }
    } else {
        printf("Sin redirección de salida\n");
    }
    
    // 5. Ejecución en primer o segundo plano
    if (es_background()) {
        printf("Ejecución: BACKGROUND (segundo plano)\n");
    } else {
        printf("Ejecución: FOREGROUND (primer plano)\n");
    }
    
    printf("==========================================\n\n");
    
} // Fin de "visualizar"