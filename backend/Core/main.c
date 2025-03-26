#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "Modules/handle_web.h"

#define PORT 8080

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    unsigned char is_JSON_ready = 1;  // Booleano "true" como 1 byte

    // Iniciar el proceso web (npm start)
    if (start_web() != 0) {
        fprintf(stderr, "[TCPs] Error initializing the web process.\n");
        exit(EXIT_FAILURE);
    }

    // Crear el descriptor del socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("[TCPs] Error creating socket");
        stop_web();
        exit(EXIT_FAILURE);
    }
    
    // Permitir la reutilización de la dirección y puerto
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[TCPs] Error in setsockopt");
        close(server_fd);
        stop_web();
        exit(EXIT_FAILURE);
    }
    
    // Configurar la dirección del servidor
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Escuchar en todas las interfaces
    address.sin_port = htons(PORT);
    
    // Asociar el socket a la dirección y puerto especificados
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("[TCPs] Bind error");
        close(server_fd);
        stop_web();
        exit(EXIT_FAILURE);
    }
    
    // Escuchar conexiones entrantes
    if (listen(server_fd, 3) < 0) {
        perror("[TCPs] Listen error");
        close(server_fd);
        stop_web();
        exit(EXIT_FAILURE);
    }
    
    printf("[TCPs] Listening on port: %d\n", PORT);
    
    // Aceptar una conexión entrante
    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
        perror("[TCPs] Accept error");
        close(server_fd);
        stop_web();
        exit(EXIT_FAILURE);
    }
    
    printf("[TCPs] Client connected: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
    
    // Enviar cada 10 segundos un byte con valor 1 al cliente
    while (1) {
        ssize_t sent = send(client_fd, &is_JSON_ready, sizeof(is_JSON_ready), 0);
        if (sent != sizeof(is_JSON_ready)) {
            perror("[TCPs] Error sending flag");
            break;
        }
        printf("[TCPs] Sending flag (JSON Ready)\n");
        sleep(10);
    }
    
    close(client_fd);
    close(server_fd);

    // Detener el proceso web (npm start)
    if (stop_web() != 0) {
        fprintf(stderr, "[TCPs] Error killing web process.\n");
    }
    
    return 0;
}
