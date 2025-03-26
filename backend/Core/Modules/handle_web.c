#include "handle_web.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// Si PATH_MAX no está definido, lo definimos manualmente
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// Variable estática para almacenar el PID del proceso que ejecuta npm start.
static pid_t npm_pid = -1;

int start_web(void) {
    // Verificar si ya se inició npm start.
    if (npm_pid > 0) {
        fprintf(stderr, "[TCPs] Web running with PID %d\n", npm_pid);
        return -1;
    }

    // Obtener la ruta del ejecutable.
    char exePath[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len == -1) {
        perror("[TCPs] Error obtaining executable path");
        return 1;
    }
    exePath[len] = '\0';

    // Obtenemos el directorio del ejecutable usando dirname.
    char exePathDup[PATH_MAX];
    strncpy(exePathDup, exePath, PATH_MAX);
    char *exe_dir = dirname(exePathDup);

    // Cambiar al directorio del ejecutable.
    if (chdir(exe_dir) != 0) {
        perror("[TCPs] Error changing to executable directory");
        return 1;
    }

    // Primer nivel: subir un nivel.
    {
        char temp[PATH_MAX];
        if (getcwd(temp, sizeof(temp)) == NULL) {
            perror("[TCPs] Error obtaining current directory");
            return 1;
        }
        char tempDup[PATH_MAX];
        strncpy(tempDup, temp, PATH_MAX);
        char *father1 = dirname(tempDup);
        if (chdir(father1) != 0) {
            perror("[TCPs] Error changing to first parent directory");
            return 1;
        }
    }

    // Segundo nivel: subir otro nivel.
    {
        char temp[PATH_MAX];
        if (getcwd(temp, sizeof(temp)) == NULL) {
            perror("[TCPs] Error obtaining current directory");
            return 1;
        }
        char tempDup[PATH_MAX];
        strncpy(tempDup, temp, PATH_MAX);
        char *father2 = dirname(tempDup);
        if (chdir(father2) != 0) {
            perror("[TCPs] Error changing to second parent directory");
            return 1;
        }
    }

    // Tercer nivel: subir otro nivel.
    {
        char temp[PATH_MAX];
        if (getcwd(temp, sizeof(temp)) == NULL) {
            perror("[TCPs] Error obtaining current directory");
            return 1;
        }
        char tempDup[PATH_MAX];
        strncpy(tempDup, temp, PATH_MAX);
        char *father3 = dirname(tempDup);
        if (chdir(father3) != 0) {
            perror("[TCPs] Error changing to third parent directory");
            return 1;
        }
    }

    // Crear un proceso hijo que ejecute "npm start".
    npm_pid = fork();
    if (npm_pid < 0) {
        perror("[TCPs] fork error");
        return 1;
    } else if (npm_pid == 0) {
        // Proceso hijo: ejecutar "npm start" usando execlp.
        execlp("npm", "npm", "start", (char *)NULL);
        perror("[TCPs] Error executing web process");
        exit(1);
    } else {
        // Proceso padre: se almacena el PID del proceso hijo.
        printf("[TCPs] web running with PID: %d\n", npm_pid);
        return 0;
    }
}

int stop_web(void) {
    if (npm_pid <= 0) {
        fprintf(stderr, "[TCPs] web process does not respond.\n");
        return -1;
    }
    // Enviar SIGTERM para detener el proceso hijo.
    if (kill(npm_pid, SIGTERM) != 0) {
        perror("[TCPs] Error terminating web process");
        return 1;
    }
    // Esperar a que el proceso hijo termine.
    int status;
    waitpid(npm_pid, &status, 0);
    printf("[TCPs] Web process with PID %d finished.\n", npm_pid);
    npm_pid = -1;
    return 0;
}
