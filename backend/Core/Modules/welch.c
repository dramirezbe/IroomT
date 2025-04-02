/**
 * @file welch.c
 * @brief Función que calcula la Densidad Espectral de Potencia de un array complejo y genera sus frecuencias asociadas.
 *
 * Esta función permite almacenar datos de frecuencias y densidades espectrales de potencia (PSD)
 * en un archivo CSV con formato adecuado para análisis posterior.
 */

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <fftw3.h>
#include <math.h>
#include <string.h>

#include "welch.h"

#define PI 3.14159265358979323846


void generate_hamming_window(double* window, int segment_length) {
    for (int n = 0; n < segment_length; n++) {
        window[n] = 0.54 - 0.46 * cos((2.0 * PI * n) / (segment_length - 1));
    }
}


void welch_psd_complex(complex double* signal, size_t N_signal, double fs, 
                       int segment_length, double overlap, double* f_out, double* P_welch_out) {
    int step = (int)(segment_length * (1.0 - overlap));
    int K = ((N_signal - segment_length) / step) + 1;
    size_t psd_size = segment_length;

    // Inicializar ventana
    double window[segment_length];
    generate_hamming_window(window, segment_length);

    // Calcular normalización de la ventana
    double U = 0.0;
    for (int i = 0; i < segment_length; i++) {
        U += window[i] * window[i];
    }
    U /= segment_length;

    // Reservar memoria para segmentos y resultados FFT
    complex double* segment = fftw_alloc_complex(segment_length);
    complex double* X_k = fftw_alloc_complex(segment_length);
    fftw_plan plan = fftw_plan_dft_1d(segment_length, segment, X_k, FFTW_FORWARD, FFTW_ESTIMATE);

    // Inicializar PSD
    memset(P_welch_out, 0, psd_size * sizeof(double));

    // Procesar cada segmento
    for (int k = 0; k < K; k++) {
        int start = k * step;

        // Aplicar ventana al segmento
        for (int i = 0; i < segment_length; i++) {
            segment[i] = signal[start + i] * window[i];
        }

        // Ejecutar FFT en el segmento
        fftw_execute(plan);

        // Acumular la potencia espectral
        for (size_t i = 0; i < psd_size; i++) {
            double abs_X_k = cabs(X_k[i]);
            P_welch_out[i] += (abs_X_k * abs_X_k) / (fs * U);
        }
    }

    // Promediar sobre los segmentos
    for (size_t i = 0; i < psd_size; i++) {
        P_welch_out[i] /= K;
    }

    // Generar frecuencias asociadas
    double val = fs / segment_length;
    for (size_t i = 0; i < psd_size; i++) {
        f_out[i] = -fs / 2 + i * val;
    }

    printf("Finish");

    // Liberar memoria
    fftw_destroy_plan(plan);
    fftw_free(segment);
    fftw_free(X_k);
}
