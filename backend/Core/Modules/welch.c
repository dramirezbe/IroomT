/**
 * @file welch.c
 * @brief Compute the Power Spectral Density (PSD) of a complex signal and generate its frequency bins.
 *
 * Implements Welch’s method: splits the signal into overlapping segments,
 * applies a Hamming window, performs FFT on each segment, averages the periodograms,
 * and outputs PSD values with associated frequencies.
 */

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <fftw3.h>
#include <math.h>
#include <string.h>

#include "welch.h"

#define PI 3.14159265358979323846

/**
 * @brief Generate a Hamming window.
 *
 * @param window         Array to fill with window coefficients (length = segment_length).
 * @param segment_length Number of samples in each segment/window.
 */
void generate_hamming_window(double* window, int segment_length) {
    for (int n = 0; n < segment_length; n++) {
        window[n] = 0.54 - 0.46 * cos((2.0 * PI * n) / (segment_length - 1));
    }
}

/**
 * @brief Compute the PSD of a complex signal using Welch’s method.
 *
 * Splits the input into overlapping segments, windows each segment,
 * executes the FFT, accumulates and averages the spectral power,
 * and fills output arrays with PSD values and corresponding frequencies.
 *
 * @param signal         Pointer to input complex signal array (length = N_signal).
 * @param N_signal       Total number of samples in the input signal.
 * @param fs             Sampling rate in Hz.
 * @param segment_length Number of samples per segment.
 * @param overlap        Fractional overlap between segments (0 ≤ overlap < 1).
 * @param f_out          Output array for frequency bins (length = segment_length).
 * @param P_welch_out    Output array for PSD values (length = segment_length).
 */
void welch_psd_complex(complex double* signal, size_t N_signal, double fs, 
                       int segment_length, double overlap, 
                       double* f_out, double* P_welch_out) {
    int step = (int)(segment_length * (1.0 - overlap));
    int K = ((int)N_signal - segment_length) / step + 1;
    size_t psd_size = segment_length;

    /* Allocate and prepare the window */
    double window[segment_length];
    generate_hamming_window(window, segment_length);

    /* Compute window normalization factor */
    double U = 0.0;
    for (int i = 0; i < segment_length; i++) {
        U += window[i] * window[i];
    }
    U /= segment_length;

    /* Allocate FFT input/output buffers and plan */
    complex double* segment = fftw_alloc_complex(segment_length);
    complex double* X_k     = fftw_alloc_complex(segment_length);
    fftw_plan plan = fftw_plan_dft_1d(segment_length, segment, X_k, FFTW_FORWARD, FFTW_ESTIMATE);

    /* Initialize PSD accumulator */
    memset(P_welch_out, 0, psd_size * sizeof(double));

    /* Loop over each segment */
    for (int k = 0; k < K; k++) {
        int start = k * step;

        /* Apply window to the current segment */
        for (int i = 0; i < segment_length; i++) {
            segment[i] = signal[start + i] * window[i];
        }

        /* Perform FFT */
        fftw_execute(plan);

        /* Accumulate spectral power */
        for (size_t i = 0; i < psd_size; i++) {
            double mag = cabs(X_k[i]);
            P_welch_out[i] += (mag * mag) / (fs * U);
        }
    }

    /* Average over all segments */
    for (size_t i = 0; i < psd_size; i++) {
        P_welch_out[i] /= K;
    }

    /* Generate frequency bins (from –fs/2 to +fs/2) */
    double df = fs / segment_length;
    for (size_t i = 0; i < psd_size; i++) {
        f_out[i] = -fs / 2 + i * df;
    }

    printf("[welch] PSD computation complete.\n");

    /* Clean up FFT resources */
    fftw_destroy_plan(plan);
    fftw_free(segment);
    fftw_free(X_k);
}
