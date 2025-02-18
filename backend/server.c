#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8080
#define DATA_POINTS 4096

typedef struct {
    double x;
    double y;
} DataPoint;

void generate_data(DataPoint *data) {
    srand(time(NULL));
    for (int i = 0; i < DATA_POINTS; i++) {
        data[i].x = (double)i;
        data[i].y = (double)rand() / RAND_MAX * 100.0;
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    // Crear socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configurar opciones
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("C server ready in %d\n", PORT);

    while (1) {
        // Aceptar conexión
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        DataPoint data[DATA_POINTS];
        generate_data(data);

        // Enviar datos de manera confiable
        size_t total_sent = 0;
        size_t data_size = sizeof(data);
        const char *buffer = (const char*)data;

        while (total_sent < data_size) {
            ssize_t sent = send(new_socket, buffer + total_sent, data_size - total_sent, 0);
            if (sent < 0) {
                perror("send");
                break;
            }
            total_sent += sent;
        }

        close(new_socket);
    }

    close(server_fd);
    return 0;
}