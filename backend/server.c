// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <math.h>

#define PORT 8080
#define DATA_POINTS 4096

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    double x;
    double y;
} DataPoint;

// Genera la señal como la suma de dos senos con ruido
void generate_signal(DataPoint *data, double freq1, double freq2) {
    const double sample_rate = 4096.0; // 4096 muestras por segundo (1 segundo total)
    for (int i = 0; i < DATA_POINTS; i++) {
        double t = (double)i / sample_rate;
        double sine1 = sin(2.0 * M_PI * freq1 * t);
        double sine2 = sin(2.0 * M_PI * freq2 * t);
        // Ruido aleatorio: valor entre -0.1 y 0.1
        double noise = (((double)rand() / RAND_MAX) - 0.5) * 0.2;
        data[i].x = t;
        data[i].y = sine1 + sine2 + noise;
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    // Inicializar la semilla para números aleatorios
    srand(time(NULL));

    // Crear socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket falló");
        exit(EXIT_FAILURE);
    }

    // Configurar opciones del socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular el socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind falló");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor C listo en el puerto %d\n", PORT);

    while (1) {
        // Aceptar conexión
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Leer las frecuencias enviadas desde Node (16 bytes: 2 doubles)
        double freqs[2];
        ssize_t bytesRead = read(new_socket, freqs, sizeof(freqs));
        if (bytesRead != sizeof(freqs)) {
            fprintf(stderr, "Error al leer las frecuencias\n");
            close(new_socket);
            continue;
        }
        printf("Frecuencias recibidas: %f Hz, %f Hz\n", freqs[0], freqs[1]);

        DataPoint data[DATA_POINTS];
        generate_signal(data, freqs[0], freqs[1]);

        // Enviar los datos de manera confiable
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
