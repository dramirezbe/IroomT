/**
 * @file welch.h
 * @brief Definición de funciones que calcula la Densidad Espectral de Potencia de un array complejo y genera sus frecuencias asociadas.
 *
 * Esta función permite almacenar datos de frecuencias y densidades espectrales de potencia (PSD)
 * en un archivo CSV con formato adecuado para análisis posterior.
 */


#ifndef WELCH_H
#define WELCH_H

#include <stddef.h>   // Para size_t
#include <complex.h>  // Para el tipo double complex

#define PI 3.14159265358979323846

/**
 * @brief Genera una ventana de Hamming.
 * 
 * Esta función genera una ventana de Hamming de longitud `segment_length` y
 * almacena los valores en el arreglo `window`.
 *
 * @param window Un puntero al arreglo donde se almacenarán los valores de la ventana de Hamming.
 * @param segment_length La longitud del segmento para la ventana de Hamming.
 */

void generate_hamming_window(double* window, int segment_length);


/**
 * @brief Calcula la Densidad Espectral de Potencia (PSD) de Welch para señales complejas.
 * 
 * Esta función aplica el método de Welch para calcular la PSD de una señal compleja de entrada.
 * Divide la señal en segmentos superpuestos, aplica una ventana de Hamming a cada segmento,
 * realiza la Transformada de Fourier en cada segmento, y promedia las potencias espectrales.
 *
 * @param signal Puntero a la señal de entrada de tipo `complex double`.
 * @param N_signal Tamaño de la señal de entrada.
 * @param fs Frecuencia de muestreo de la señal de entrada.
 * @param segment_length Longitud de cada segmento en el que se divide la señal.
 * @param overlap Factor de solapamiento entre segmentos (0 a 1).
 * @param f_out Puntero al arreglo donde se almacenarán las frecuencias de salida.
 * @param P_welch_out Puntero al arreglo donde se almacenarán los valores calculados de la PSD.
 * 
 * @note Es necesario liberar la memoria reservada para la FFT usando `fftw_destroy_plan` y `fftw_free`.
 *
 * @example
 * @code
 * size_t N_signal = 1024;
 * complex double signal[N_signal];
 * double fs = 1000.0;
 * int segment_length = 256;
 * double overlap = 0.5;
 * double f_out[segment_length];
 * double P_welch_out[segment_length];
 * welch_psd_complex(signal, N_signal, fs, segment_length, overlap, f_out, P_welch_out);
 * @endcode
 */
void welch_psd_complex(complex double* signal, size_t N_signal, double fs, 
                       int segment_length, double overlap, double* f_out, double* P_welch_out);

#endif // WELCH_H
