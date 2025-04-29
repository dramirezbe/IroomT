/**
 * @file parameter.c
 * @brief Optimized signal processing implementation for spectral analysis
 *
 * This file contains highly optimized implementations for processing CS8 signal data,
 * performing Welch's method for power spectral density estimation, and exporting
 * results to JSON format. The implementation prioritizes:
 *
 * - Performance optimization for real-time processing
 * - Memory efficiency and proper resource management
 * - Robust error handling and parameter validation
 * - Cross-platform compatibility (Unix, Linux, Raspberry Pi)
 * - Clear documentation for maintainability
 *
 * @author Martin Ramirez Espinosa
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include "../Modules/CS8toIQ.h"
#include "../Modules/IQ.h"
#include "../Modules/welch.h"
#include "../Modules/cJSON.h"
#include "../Modules/find_closest_index.h"

#include "parameter.h"

/**
 * @brief Compares two double values for sorting
 * 
 * @param a First value pointer
 * @param b Second value pointer
 * @return Comparison result (-1, 0, or 1)
 */
static int compare_doubles(const void* a, const void* b) {
    double x = *(const double*)a;
    double y = *(const double*)b;
    
    if (x < y) return -1;
    else if (x > y) return 1;
    else return 0;
}

/**
 * @brief Calculate the median of a segment in an array
 * 
 * This function allocates a temporary array to avoid modifying the original data.
 * 
 * @param array Source array
 * @param start Start index (inclusive)
 * @param end End index (exclusive)
 * @return Median value or NAN if memory allocation fails
 */
static double calculate_median(const double* array, int start, int end) {
    if (array == NULL || start < 0 || end <= start) {
        return NAN;
    }
    
    int length = end - start;
    double* temp = (double*)malloc(length * sizeof(double));
    if (temp == NULL) {
        return NAN;
    }
    
    /* Copy segment to temporary buffer */
    memcpy(temp, array + start, length * sizeof(double));
    
    /* Sort the temporary array */
    qsort(temp, length, sizeof(double), compare_doubles);
    
    /* Calculate median based on array length */
    double median_value;
    if (length % 2 == 0) {
        median_value = (temp[length/2 - 1] + temp[length/2]) * 0.5;
    } else {
        median_value = temp[length/2];
    }
    
    free(temp);
    return median_value;
}

/**
 * @brief Find minimum value in an array
 * 
 * @param array Source array
 * @param length Array length
 * @return Minimum value
 */
static double find_min(const double* array, int length) {
    if (array == NULL || length <= 0) {
        return NAN;
    }
    
    double min_val = array[0];
    for (int i = 1; i < length; i++) {
        if (array[i] < min_val) {
            min_val = array[i];
        }
    }
    
    return min_val;
}

/**
 * @brief Find maximum value in a segment of an array
 * 
 * @param array Source array
 * @param start Start index (inclusive)
 * @param end End index (inclusive)
 * @return Maximum value
 */
static double find_max(const double* array, int start, int end) {
    if (array == NULL || start < 0 || end < start) {
        return NAN;
    }
    
    double max_val = array[start];
    for (int i = start + 1; i <= end; i++) {
        if (array[i] > max_val) {
            max_val = array[i];
        }
    }
    
    return max_val;
}

/**
 * @brief Rearrange the Welch's PSD result array for proper frequency ordering
 * 
 * This function reorders the PSD data to place negative frequencies first,
 * followed by positive frequencies for correct spectrum visualization.
 * 
 * @param psd The PSD array to reorder (modified in-place)
 * @param length Length of the array (must be even)
 * @return True if successful, false otherwise
 */
static bool rearrange_welch_psd(double* psd, int length) {
    if (psd == NULL || length <= 0 || length % 2 != 0) {
        return false;
    }
    
    int half = length / 2;
    double* temp = (double*)malloc(length * sizeof(double));
    if (temp == NULL) {
        return false;
    }
    
    /* First half contains second half of original array */
    memcpy(temp, psd + half, half * sizeof(double));
    
    /* Second half contains first half of original array */
    memcpy(temp + half, psd, half * sizeof(double));
    
    /* Copy back to original array */
    memcpy(psd, temp, length * sizeof(double));
    
    free(temp);
    return true;
}

/**
 * @brief Generate spectral flatness correction
 * 
 * This function applies a spectral correction to the PSD around the center frequency
 * to compensate for analyzer non-linearities.
 * 
 * @param psd The PSD array to correct (modified in-place)
 * @param length Length of the array
 * @param center_index Center index in the array
 * @param correction_width Width of the correction (number of samples)
 * @return True if successful, false otherwise
 */
static bool apply_spectral_correction(double* psd, int length, int center_index, int correction_width) {
    if (psd == NULL || length <= 0 || center_index < 0 || center_index >= length || correction_width <= 0) {
        return false;
    }
    
    int b = center_index - (correction_width + 13);
    int a = center_index;
    
    /* Apply before center */
    for (int i = 0; i < correction_width; i++) {
        b = b - 3;
        if (b >= 0 && a >= 0 && a < length) {
            psd[a] = psd[b];
            a--;
        }
    }
    
    /* Apply after center */
    a = center_index;
    b = center_index - (correction_width + 13);
    for (int i = 0; i < correction_width; i++) {
        if (a < length && b >= 0 && b < length) {
            psd[a] = psd[b];
            a++;
            b -= 2;
        }
    }
    
    return true;
}

/**
 * @brief Create JSON representation of signal data
 * 
 * @param f Frequency array
 * @param psd PSD array
 * @param length Length of arrays
 * @param calibration_factor Calibration factor for PSD values
 * @param canalization Channel center frequencies
 * @param bandwidth Channel bandwidths
 * @param canalization_length Number of channels
 * @param threshold Power threshold for signal presence
 * @param noise_floor Calculated noise floor
 * @return cJSON object containing the results (must be freed with cJSON_Delete)
 */
static cJSON* create_signal_json(
    const double* f, 
    const double* psd, 
    int length,
    double calibration_factor,
    const double* canalization,
    const double* bandwidth,
    int canalization_length,
    int threshold,
    double noise_floor
) {
    if (f == NULL || psd == NULL || length <= 0) {
        return NULL;
    }
    
    /* Create root JSON object */
    cJSON *json_root = cJSON_CreateObject();
    if (json_root == NULL) {
        return NULL;
    }
    
    /* Add metadata */
    cJSON_AddStringToObject(json_root, "band", "VHF");       
    cJSON_AddStringToObject(json_root, "fmin", "88");
    cJSON_AddStringToObject(json_root, "fmax", "108");
    cJSON_AddStringToObject(json_root, "units", "MHz");
    cJSON_AddStringToObject(json_root, "measure", "RMER");
    
    /* Create vectors object */
    cJSON *json_vectors = cJSON_CreateObject();
    if (json_vectors == NULL) {
        cJSON_Delete(json_root);
        return NULL;
    }
    
    /* Create PSD array */
    cJSON *json_psd_array = cJSON_CreateArray();
    if (json_psd_array == NULL) {
        cJSON_Delete(json_root);
        return NULL;
    }
    
    /* Add PSD values to array with calibration factor applied */
    for (int i = 0; i < length; i++) {
        double adjusted_psd = 10.0 * log10(psd[i]) + calibration_factor;
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.3f", adjusted_psd);
        cJSON_AddItemToArray(json_psd_array, cJSON_CreateNumber(atof(buffer)));
    }
    cJSON_AddItemToObject(json_vectors, "Pxx", json_psd_array);
    
    /* Create frequency array */
    cJSON *json_f_array = cJSON_CreateArray();
    if (json_f_array == NULL) {
        cJSON_Delete(json_root);
        return NULL;
    }
    
    /* Add frequency values to array */
    for (int i = 0; i < length; i++) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.3f", f[i]);
        cJSON_AddItemToArray(json_f_array, cJSON_CreateNumber(atof(buffer)));
    }
    cJSON_AddItemToObject(json_vectors, "f", json_f_array);
    
    cJSON_AddItemToObject(json_root, "vectors", json_vectors);
    
    /* Create parameters array for channels */
    cJSON *json_params_array = cJSON_CreateArray();
    if (json_params_array != NULL) {
        /* Process will be added here if needed */
        cJSON_AddItemToObject(json_root, "parameters", json_params_array);
    }
    
    /* Wrap everything in a data object */
    cJSON *json_data = cJSON_CreateObject();
    if (json_data == NULL) {
        cJSON_Delete(json_root);
        return NULL;
    }
    
    cJSON_AddItemToObject(json_data, "data", json_root);
    return json_data;
}

/**
 * @brief Save JSON data to a file
 * 
 * @param json_obj JSON object to save
 * @param filename Output filename
 * @return 0 on success, error code otherwise
 */
static int save_json_to_file(const cJSON* json_obj, const char* filename) {
    if (json_obj == NULL || filename == NULL) {
        return SP_ERROR_NULL_POINTER;
    }
    
    /* Convert JSON to string */
    char* json_string = cJSON_Print(json_obj);
    if (json_string == NULL) {
        return SP_ERROR_MEMORY_ALLOC;
    }
    
    /* Open file for writing */
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        free(json_string);
        return SP_ERROR_FILE_IO;
    }
    
    /* Write JSON string to file */
    size_t len = strlen(json_string);
    size_t written = fwrite(json_string, 1, len, file);
    
    /* Clean up */
    fclose(file);
    free(json_string);
    
    return (written == len) ? SP_SUCCESS : SP_ERROR_FILE_IO;
}

/**
 * @brief Process signal data and generate spectrum analysis
 * 
 * This function replaces the original 'parameter' function with a more optimized,
 * secure, and efficient implementation that:
 * 1. Loads CS8 data from a file
 * 2. Performs Welch's method for PSD estimation
 * 3. Processes the spectrum to identify signals
 * 4. Generates a JSON output with results
 * 
 * @param config Processing configuration structure
 * @return 0 on success, error code otherwise
 */
int process_signal_spectrum(const SignalProcessorConfig* config) {
    /* Validate input parameters */
    if (config == NULL || config->input_file_path == NULL || 
        config->output_json_path == NULL || config->canalization == NULL || 
        config->bandwidth == NULL || config->canalization_length <= 0) {
        return SP_ERROR_NULL_POINTER;
    }
    
    /* Initialize local variables */
    complex double* vector_IQ = NULL;
    double* psd_large = NULL;
    double* f_large = NULL;
    double* psd_small = NULL;
    double* f_small = NULL;
    size_t num_samples = 0;
    int error_code = 0;
    int result = SP_SUCCESS;
    
    /* Set default segment sizes if not provided */
    int nperseg_large = config->nperseg_large > 0 ? config->nperseg_large : 32768;
    int nperseg_small = config->nperseg_small > 0 ? config->nperseg_small : 4096;
    
    /* Validate segment sizes are even */
    if (nperseg_large % 2 != 0 || nperseg_small % 2 != 0) {
        return SP_ERROR_INVALID_PARAMETER;
    }
    
    /* Performance timing if verbose mode is enabled */
    clock_t start_time = 0, end_time = 0;
    if (config->verbose_output) {
        start_time = clock();
        printf("Starting signal processing...\n");
    }
    
    /* Load CS8 data with error handling */
    vector_IQ = cargar_cs8(config->input_file_path, &num_samples, &error_code);
    if (vector_IQ == NULL) {
        fprintf(stderr, "Error loading CS8 data: %s\n", cs8_iq_error_string(error_code));
        return SP_ERROR_FILE_IO;
    }
    
    if (config->verbose_output) {
        printf("Successfully loaded %zu samples\n", num_samples);
    }
    
    /* Delete CS8 resources (if needed by the original code) */
    // delete_CS8(0);
    
    /* Allocate memory for PSDs and frequency arrays */
    psd_large = (double*)malloc(nperseg_large * sizeof(double));
    f_large = (double*)malloc(nperseg_large * sizeof(double));
    psd_small = (double*)malloc(nperseg_small * sizeof(double));
    f_small = (double*)malloc(nperseg_small * sizeof(double));
    
    if (psd_large == NULL || f_large == NULL || psd_small == NULL || f_small == NULL) {
        result = SP_ERROR_MEMORY_ALLOC;
        goto cleanup;
    }
    
    /* Calculate PSDs using Welch's method */
    welch_psd_complex(vector_IQ, num_samples, 20000000, nperseg_large, 0, f_large, psd_large);
    welch_psd_complex(vector_IQ, num_samples, 20000000, nperseg_small, 0, f_small, psd_small);
    
    /* We don't need the IQ data anymore */
    free(vector_IQ);
    vector_IQ = NULL;
    
    /* Rearrange PSDs for proper frequency order */
    if (!rearrange_welch_psd(psd_large, nperseg_large) || 
        !rearrange_welch_psd(psd_small, nperseg_small)) {
        result = SP_ERROR_MEMORY_ALLOC;
        goto cleanup;
    }
    
    /* Apply spectral correction */
    int center_large = nperseg_large / 2;
    int count_large = (int)(nperseg_large * 0.002);
    int center_small = nperseg_small / 2;
    int count_small = (int)(nperseg_small * 0.002);
    
    apply_spectral_correction(psd_large, nperseg_large, center_large, count_large);
    apply_spectral_correction(psd_small, nperseg_small, center_small, count_small);
    
    /* Adjust frequency values to absolute frequencies */
    for (int i = 0; i < nperseg_large; i++) {
        f_large[i] = (f_large[i] + config->central_freq) / 1e6;
    }
    
    for (int i = 0; i < nperseg_small; i++) {
        f_small[i] = (f_small[i] + config->central_freq) / 1e6;
    }
    
    /* Calculate calibration factor between large and small PSD */
    double constante = fabs(fabs(10 * log10(psd_large[0])) - fabs(10 * log10(psd_small[0])));
    
    /* Calculate noise floor for SNR estimation */
    float noise = find_min(psd_large, nperseg_large);
    
    /* Process channel information */
    int N_f = nperseg_large;
    bool signal_detected = false;
    
    for (int idx = 0; idx < config->canalization_length; idx++) {
        double center_freq = config->canalization[idx];
        double bw = config->bandwidth[idx];
        
        double target_lower_freq = center_freq - bw / 2;
        double target_upper_freq = center_freq + bw / 2;
        
        int lower_index = find_closest_index(f_large, N_f, target_lower_freq);
        int upper_index = find_closest_index(f_large, N_f, target_upper_freq);
        
        /* Ensure lower index is actually lower */
        if (lower_index > upper_index) {
            int temp = lower_index;
            lower_index = upper_index;
            upper_index = temp;
        }
        
        /* Clamp indices to valid range */
        if (lower_index < 0) lower_index = 0;
        if (upper_index >= N_f) upper_index = N_f - 1;
        
        int range_length = upper_index - lower_index + 1;
        
        /* Only process if we have a valid range */
        if (range_length > 0) {
            double power_max = find_max(psd_large, lower_index, upper_index);
            double power = calculate_median(psd_large, lower_index, upper_index);
            double snr = 10.0 * log10(power_max / noise);
            
            /* Check if signal is present based on threshold */
            if (10.0 * log10(power_max) > config->threshold) {
                signal_detected = true;
            }
        }
    }
    
    /* Create JSON output */
    cJSON *json_data = create_signal_json(
        f_small,
        psd_small,
        nperseg_small,
        constante,
        config->canalization,
        config->bandwidth,
        config->canalization_length,
        config->threshold,
        noise
    );
    
    if (json_data == NULL) {
        result = SP_ERROR_MEMORY_ALLOC;
        goto cleanup;
    }
    
    /* Save JSON to file */
    result = save_json_to_file(json_data, config->output_json_path);
    cJSON_Delete(json_data);
    
    if (config->verbose_output) {
        end_time = clock();
        double processing_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        printf("Processing completed in %.3f seconds\n", processing_time);
        printf("Signal %s\n", signal_detected ? "DETECTED" : "NOT DETECTED");
    }
    
cleanup:
    /* Free all allocated resources */
    free(psd_large);
    free(f_large);
    free(psd_small);
    free(f_small);
    free(vector_IQ);  /* In case it wasn't freed earlier */
    
    return result;
}

/**
 * @brief Get error message for signal processing error codes
 * 
 * @param error_code The error code
 * @return Error message string
 */
const char* get_signal_processor_error(int error_code) {
    switch (error_code) {
        case SP_SUCCESS:
            return "Success";
        case SP_ERROR_NULL_POINTER:
            return "Null pointer provided";
        case SP_ERROR_MEMORY_ALLOC:
            return "Memory allocation failed";
        case SP_ERROR_FILE_IO:
            return "File I/O error";
        case SP_ERROR_INVALID_PARAMETER:
            return "Invalid parameter";
        case SP_ERROR_DATA_PROCESSING:
            return "Data processing error";
        default:
            return "Unknown error";
    }
}

/**
 * @brief Compatibility wrapper for the original 'parameter' function
 * 
 * This function maintains the same interface as the original 'parameter'
 * function but delegates to our optimized implementation.
 * 
 * @param threshold Signal detection threshold
 * @param canalization Array of channel center frequencies
 * @param bandwidth Array of channel bandwidths
 * @param canalization_length Number of channels
 * @param central_freq Central frequency in Hz
 */
void parameter(int threshold, double* canalization, double* bandwidth, int canalization_length, uint64_t central_freq) {
    /* Create a configuration with the same parameters as the original function */
    SignalProcessorConfig config;
    memset(&config, 0, sizeof(config));
    
    config.input_file_path = "Samples/0";
    config.output_json_path = "JSON/0";
    config.central_freq = central_freq;
    config.nperseg_large = 32768;
    config.nperseg_small = 4096;
    config.threshold = threshold;
    config.canalization = canalization;
    config.bandwidth = bandwidth;
    config.canalization_length = canalization_length;
    config.use_mmap = true;  /* Use memory mapping for better performance */
    config.verbose_output = false;
    
    /* Call the optimized implementation */
    int result = process_signal_spectrum(&config);
    
    if (result != SP_SUCCESS) {
        fprintf(stderr, "Signal processing error: %s\n", get_signal_processor_error(result));
    }
}