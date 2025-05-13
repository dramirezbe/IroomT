/**
 * @file welch.h
 * @brief Function definitions for computing the Power Spectral Density (PSD) of a complex signal using Welch's method.
 *
 * Provides functions to compute the PSD and its associated frequency bins,
 * and optionally export the results to a CSV file.
 */

#ifndef WELCH_H
#define WELCH_H

#include <stddef.h>
#include <complex.h>

#define PI 3.14159265358979323846

/**
 * @brief Generate a Hamming window.
 *
 * Fills the given array with Hamming window values of specified length.
 *
 * @param window Pointer to the array where the window values will be stored.
 * @param segment_length Length of the Hamming window.
 */
void generate_hamming_window(double* window, int segment_length);

/**
 * @brief Compute the Power Spectral Density (PSD) of a complex signal using Welch's method.
 *
 * This function segments the input signal, applies a Hamming window to each segment,
 * performs the FFT, and averages the periodograms to estimate the PSD.
 *
 * @param signal Pointer to the input complex signal.
 * @param N_signal Number of samples in the input signal.
 * @param fs Sampling frequency.
 * @param segment_length Length of each segment.
 * @param overlap Overlap factor between segments (range: 0 to 1).
 * @param f_out Output array for frequency bins (must be of size segment_length).
 * @param P_welch_out Output array for the PSD values (must be of size segment_length).
 *
 * @note Ensure to free FFT resources using `fftw_destroy_plan` and `fftw_free`.
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
