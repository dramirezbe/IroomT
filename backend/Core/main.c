#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "Modules/handle_web.h"

// Función para detener el proceso web y salir
void cleanup_and_exit(int signum) {
    printf("\n[WEB] Señal %d recibida. Deteniendo el proceso web...\n", signum);
    if (stop_web() != 0) {
        fprintf(stderr, "[WEB] Error al detener el proceso web.\n");
    }
    exit(EXIT_SUCCESS);
}

int main() {
    // Configurar los manejadores de señales para capturar la terminación
    struct sigaction sa;
    sa.sa_handler = cleanup_and_exit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    // Capturar señales SIGINT (Ctrl+C) y SIGTERM
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    
    // Iniciar el proceso web (npm start)
    if (start_web() != 0) {
        fprintf(stderr, "[WEB] Error al iniciar el proceso web.\n");
        // En caso de error, asegurarse de detener el proceso web si fuera necesario
        stop_web();
        exit(EXIT_FAILURE);
    }
    
    printf("[WEB] Proceso web iniciado.\n");
    
    // Bucle principal que mantiene el programa en ejecución
    while (1) {
        sleep(1);
    }
    
    // Código no alcanzado, pero se incluye la detención del proceso web por seguridad.
    if (stop_web() != 0) {
        fprintf(stderr, "[WEB] Error al detener el proceso web.\n");
    }
    
    return 0;
}
