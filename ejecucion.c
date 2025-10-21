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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/* Simple tabla de trabajos en background para mapear PID -> linea de comando */
#define MAX_BG_JOBS 64
static pid_t bg_pids[MAX_BG_JOBS];
static char *bg_cmds[MAX_BG_JOBS];

/* Registrar un trabajo en background (copia la cadena cmd) */
static void register_bg_job(pid_t pid, const char *cmd)
{
    for (int i = 0; i < MAX_BG_JOBS; ++i) {
        if (bg_pids[i] == 0) {
            bg_pids[i] = pid;
            if (cmd) {
                bg_cmds[i] = strdup(cmd);
            } else {
                bg_cmds[i] = NULL;
            }
            return;
        }
    }
}

/* Eliminar un trabajo en background y devolver la cadena registrada */
static char *unregister_bg_job(pid_t pid)
{
    for (int i = 0; i < MAX_BG_JOBS; ++i) {
        if (bg_pids[i] == pid) {
            bg_pids[i] = 0;
            char *cmd = bg_cmds[i];
            bg_cmds[i] = NULL;
            return cmd; /* caller must free */
        }
    }
    return NULL;
}


/* Manejador de señal SIGCHLD para recolectar procesos zombies y
   mostrar estado similar a bash/ksh para trabajos en segundo plano. */
void sigchld_handler(int sig)
{
    int status;
    pid_t pid;

    /* Recolectar todos los procesos hijos terminados sin bloquear */
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        char *cmd = unregister_bg_job(pid);

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            if (cmd) {
                if (exit_code == 0) {
                    fprintf(stderr, "\n[%d] Done    %s\n", pid, cmd);
                } else {
                    fprintf(stderr, "\n[%d] Exit %d %s\n", pid, exit_code, cmd);
                }
            } else {
                if (exit_code == 0) {
                    fprintf(stderr, "\n[%d] Done\n", pid);
                } else {
                    fprintf(stderr, "\n[%d] Exit %d\n", pid, exit_code);
                }
            }
        } else if (WIFSIGNALED(status)) {
            int signal_num = WTERMSIG(status);
            if (cmd) {
                fprintf(stderr, "\n[%d] Killed by signal %d    %s\n", pid, signal_num, cmd);
            } else {
                fprintf(stderr, "\n[%d] Killed by signal %d\n", pid, signal_num);
            }
        }

        if (cmd) free(cmd);
    }
}


int ejecutar (int nordenes , int *nargs , char **ordenes , char ***args , int bgnd)
{
    pid_t aux;
    pid_t first_pid = -1;

    /* Configurar el manejador de señal SIGCHLD para evitar zombies */
    signal(SIGCHLD, sigchld_handler);

    /* Construir una linea de comando compacta para registrar si bgnd */
    char cmdline[1024];
    cmdline[0] = '\0';
    if (bgnd) {
        /* Concatenar las ordenes y argumentos en una sola cadena */
        for (int i = 0; i < nordenes; ++i) {
            if (i > 0) strncat(cmdline, " | ", sizeof(cmdline)-strlen(cmdline)-1);
            strncat(cmdline, ordenes[i], sizeof(cmdline)-strlen(cmdline)-1);
            if (nargs[i] > 0 && args[i]) {
                for (int j = 1; j < nargs[i]; ++j) {
                    strncat(cmdline, " ", sizeof(cmdline)-strlen(cmdline)-1);
                    if (args[i][j]) strncat(cmdline, args[i][j], sizeof(cmdline)-strlen(cmdline)-1);
                }
            }
        }
    }

    for(int i=0; i<nordenes; i++){
        aux = fork();
        if(aux < 0){
            perror("fork");
            return ERROR;
        }
        if(aux==0){
           /* Proceso hijo */
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
        
        /* Proceso padre: guardar el PID del primer proceso */
        if (i == 0) {
            first_pid = aux;
        }
    }
    
    /* Padre: cerrar ficheros */
    cerrar_fd();
    
    if (bgnd) {
        /* Ejecución en segundo plano: registrar y no esperar */
        if (first_pid > 0) {
            register_bg_job(first_pid, cmdline);
            fprintf(stderr, "[Background] PID: %d\n", first_pid);
        }
    } else {
        /* Ejecución en primer plano: esperar a todos los hijos */
        while(wait(NULL) > 0) ; /* esperar hasta que no queden hijos */
    }
    
    return OK;

} // Fin de la funcion "ejecutar"
