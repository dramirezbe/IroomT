#include "handle_web.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* Variable estática para almacenar el PID del proceso que ejecuta npm start. */
static pid_t npm_pid = -1;

int start_web(void) {
    /* Verificar si ya se inició npm start. */
    if (npm_pid > 0) {
        fprintf(stderr, "start_web() Web already running with PID %d\n", npm_pid);
        return -1;
    }
    
    /* Cambiar al directorio raíz especificado */
    if (chdir("/home/unal/RF-Spectrum-Monitoring") != 0) {
        perror("start_web() Error changing to /home/unal/RF-Spectrum-Monitoring");
        return 1;
    }

    /* Crear un proceso hijo que ejecute "npm start". */
    npm_pid = fork();
    if (npm_pid < 0) {
        perror("start_web() fork error");
        return 1;
    } else if (npm_pid == 0) {
        /* Proceso hijo: ejecutar "npm start" */
        execlp("npm", "npm", "start", (char *)NULL);
        perror("start_web() Error executing npm start");
        exit(1);
    } else {
        /* Proceso padre: se almacena el PID del proceso hijo. */
        printf("start_web() Web started with PID: %d\n", npm_pid);
        return 0;
    }
}

int stop_web(void) {
    if (npm_pid <= 0) {
        fprintf(stderr, "stop_web() Web process is not running.\n");
        return -1;
    }

    /* Eliminar intento de enviar señal; simplemente esperar a que el proceso termine (si es que lo hace solo) */
    int status;
    waitpid(npm_pid, &status, 0);
    printf("stop_web() Web process with PID %d finished.\n", npm_pid);
    npm_pid = -1;
    return 0;
}
