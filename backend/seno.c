#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>  // Para getopt
#include <bits/getopt_core.h>

#define PI 3.141592653589793

int main(int argc, char *argv[]) {
    double duration = 0.0;
    double frequency = 0.0;
    int fs = 0;  // Frecuencia de muestreo
    int opt;

    // Procesamiento de argumentos: -t <duracion> -f <frecuencia> -s <fs>
    while ((opt = getopt(argc, argv, "t:f:s:")) != -1) {
        switch (opt) {
            case 't':
                duration = atof(optarg);
                break;
            case 'f':
                frequency = atof(optarg);
                break;
            case 's':
                fs = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Uso: %s -t <duracion> -f <frecuencia> -s <fs>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Validación de parámetros
    if (duration <= 0 || frequency <= 0 || fs <= 0) {
        fprintf(stderr, "Error: todos los parámetros deben ser mayores a 0.\n");
        exit(EXIT_FAILURE);
    }

    // Ajustar el número de muestras a 4096
    int total_samples = 4096;
    double adjusted_duration = (double)total_samples / fs;

    // Comienzo del JSON
    printf("{\"sine\": [\n");

    // Generación de la señal seno y salida en formato JSON
    for (int i = 0; i < total_samples; i++) {
        double t = (double)i / fs;
        double mag = sin(2 * PI * frequency * t);
        // Si no es el último elemento, añadir coma
        if (i < total_samples - 1)
            printf("  {\"t\": %.6f, \"mag\": %.6f},\n", t, mag);
        else
            printf("  {\"t\": %.6f, \"mag\": %.6f}\n", t, mag);
    }

    // Final del JSON
    printf("]}\n");

    return 0;
}