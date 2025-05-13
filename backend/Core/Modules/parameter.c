/**
 * @file parameter.c
 * @author Martin Ramirez Espinosa, David Ramírez Betancourth
 * * @brief Implementation of signal spectrum processing library
 * @ingroup signal_processor
 *
 * This file implements functions for analyzing signal spectrum data,
 * detecting transmissions in specified frequency channels, and generating
 * JSON output for visualization. The library supports both large-scale and
 * small-scale spectrum analysis with signal detection capabilities.
 *
 * Key features:
 * - Power spectral density (PSD) calculation using Welch's method
 * - Signal detection in specified frequency channels
 * - Spectrum visualization data generation
 * - JSON output for web interface integration
 * - Robust error handling and reporting
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

// Static helper function (Internal implementation detail)
static int compare_doubles(const void* a, const void* b) {
    double x = *(const double*)a;
    double y = *(const double*)b;
    
    if (x < y) return -1;
    else if (x > y) return 1;
    else return 0;
}

// Static helper function (Internal implementation detail)
static double calculate_median(const double* array, int start, int end) {
    if (array == NULL || start < 0 || end <= start) {
        return NAN;
    }
    
    int length = end - start;
    double* temp = (double*)malloc(length * sizeof(double));
    if (temp == NULL) {
        return NAN;
    }
    
    memcpy(temp, array + start, length * sizeof(double));
    
    qsort(temp, length, sizeof(double), compare_doubles);
    
    double median_value;
    if (length % 2 == 0) {
        median_value = (temp[length/2 - 1] + temp[length/2]) * 0.5;
    } else {
        median_value = temp[length/2];
    }
    
    free(temp);
    return median_value;
}

// Static helper function (Internal implementation detail)
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

// Static helper function (Internal implementation detail)
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

// Static helper function (Internal implementation detail)
static bool rearrange_welch_psd(double* psd, int length) {
    if (psd == NULL || length <= 0 || length % 2 != 0) {
        return false;
    }
    
    int half = length / 2;
    double* temp = (double*)malloc(length * sizeof(double));
    if (temp == NULL) {
        return false;
    }
    
    memcpy(temp, psd + half, half * sizeof(double));
    
    memcpy(temp + half, psd, half * sizeof(double));
    
    memcpy(psd, temp, length * sizeof(double));
    
    free(temp);
    return true;
}

// Static helper function (Internal implementation detail)
static bool apply_spectral_correction(double* psd, int length, int center_index, int correction_width) {
    if (psd == NULL || length <= 0 || center_index < 0 || center_index >= length || correction_width <= 0) {
        return false;
    }
    
    int b = center_index - (correction_width + 13);
    int a = center_index;
    
    for (int i = 0; i < correction_width; i++) {
        b = b - 3;
        if (b >= 0 && a >= 0 && a < length) {
            psd[a] = psd[b];
            a--;
        }
    }
    
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

// Static helper function (Internal implementation detail)
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
    
    cJSON *json_root = cJSON_CreateObject();
    if (json_root == NULL) {
        return NULL;
    }
    
    cJSON_AddStringToObject(json_root, "band", "VHF");      
    cJSON_AddStringToObject(json_root, "fmin", "88");
    cJSON_AddStringToObject(json_root, "fmax", "108");
    cJSON_AddStringToObject(json_root, "units", "MHz");
    cJSON_AddStringToObject(json_root, "measure", "RMER");
    
    cJSON *json_vectors = cJSON_CreateObject();
    if (json_vectors == NULL) {
        cJSON_Delete(json_root);
        return NULL;
    }
    
    cJSON *json_psd_array = cJSON_CreateArray();
    if (json_psd_array == NULL) {
        cJSON_Delete(json_root);
        return NULL;
    }
    
    for (int i = 0; i < length; i++) {
        double adjusted_psd = 10.0 * log10(psd[i]) + calibration_factor;
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.3f", adjusted_psd);
        cJSON_AddItemToArray(json_psd_array, cJSON_CreateNumber(atof(buffer)));
    }
    cJSON_AddItemToObject(json_vectors, "Pxx", json_psd_array);
    
    cJSON *json_f_array = cJSON_CreateArray();
    if (json_f_array == NULL) {
        cJSON_Delete(json_root);
        return NULL;
    }
    
    for (int i = 0; i < length; i++) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.3f", f[i]);
        cJSON_AddItemToArray(json_f_array, cJSON_CreateNumber(atof(buffer)));
    }
    cJSON_AddItemToObject(json_vectors, "f", json_f_array);
    
    cJSON_AddItemToObject(json_root, "vectors", json_vectors);
    
    cJSON *json_params_array = cJSON_CreateArray();
    if (json_params_array != NULL) {
        cJSON_AddItemToObject(json_root, "parameters", json_params_array);
    }
    
    cJSON *json_data = cJSON_CreateObject();
    if (json_data == NULL) {
        cJSON_Delete(json_root);
        return NULL;
    }
    
    cJSON_AddItemToObject(json_data, "data", json_root);
    return json_data;
}

// Static helper function (Internal implementation detail)
static int save_json_to_file(const cJSON* json_obj, const char* filename) {
    if (json_obj == NULL || filename == NULL) {
        return SP_ERROR_NULL_POINTER;
    }
    
    char* json_string = cJSON_Print(json_obj);
    if (json_string == NULL) {
        return SP_ERROR_MEMORY_ALLOC;
    }
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        free(json_string);
        return SP_ERROR_FILE_IO;
    }
    
    size_t len = strlen(json_string);
    size_t written = fwrite(json_string, 1, len, file);
    
    fclose(file);
    free(json_string);
    
    return (written == len) ? SP_SUCCESS : SP_ERROR_FILE_IO;
}

// Implementation for function declared in parameter.h
int process_signal_spectrum(const SignalProcessorConfig* config) {
    if (config == NULL || config->input_file_path == NULL || 
        config->output_json_path == NULL || config->canalization == NULL || 
        config->bandwidth == NULL || config->canalization_length <= 0) {
        return SP_ERROR_NULL_POINTER;
    }
    
    complex double* vector_IQ = NULL;
    double* psd_large = NULL;
    double* f_large = NULL;
    double* psd_small = NULL;
    double* f_small = NULL;
    size_t num_samples = 0;
    int error_code = 0;
    int result = SP_SUCCESS;
    
    int nperseg_large = config->nperseg_large > 0 ? config->nperseg_large : 32768;
    int nperseg_small = config->nperseg_small > 0 ? config->nperseg_small : 4096;
    
    if (nperseg_large % 2 != 0 || nperseg_small % 2 != 0) {
        return SP_ERROR_INVALID_PARAMETER;
    }
    
    clock_t start_time = 0, end_time = 0;
    if (config->verbose_output) {
        start_time = clock();
        printf("Starting signal processing...\n");
    }
    
    // Load IQ data from the input file
    vector_IQ = load_iq_data(config->input_file_path, &num_samples, &error_code);
    if (vector_IQ == NULL) {
        fprintf(stderr, "Error loading CS8 data: %s\n", cs8_iq_error_string(error_code));
        return SP_ERROR_FILE_IO;
    }
    
    if (config->verbose_output) {
        printf("Successfully loaded %zu samples\n", num_samples);
    }
    
    // Allocate memory for PSD and frequency arrays
    psd_large = (double*)malloc(nperseg_large * sizeof(double));
    f_large = (double*)malloc(nperseg_large * sizeof(double));
    psd_small = (double*)malloc(nperseg_small * sizeof(double));
    f_small = (double*)malloc(nperseg_small * sizeof(double));
    
    if (psd_large == NULL || f_large == NULL || psd_small == NULL || f_small == NULL) {
        result = SP_ERROR_MEMORY_ALLOC;
        goto cleanup;
    }
    
    // Calculate power spectral density with different resolutions
    welch_psd_complex(vector_IQ, num_samples, 20000000, nperseg_large, 0, f_large, psd_large);
    welch_psd_complex(vector_IQ, num_samples, 20000000, nperseg_small, 0, f_small, psd_small);
    
    free(vector_IQ);
    vector_IQ = NULL;
    
    // Rearrange PSD arrays for proper visualization
    if (!rearrange_welch_psd(psd_large, nperseg_large) || 
        !rearrange_welch_psd(psd_small, nperseg_small)) {
        result = SP_ERROR_MEMORY_ALLOC;
        goto cleanup;
    }
    
    // Apply spectral correction to remove DC spike artifacts
    int center_large = nperseg_large / 2;
    int count_large = (int)(nperseg_large * 0.002);
    int center_small = nperseg_small / 2;
    int count_small = (int)(nperseg_small * 0.002);
    
    apply_spectral_correction(psd_large, nperseg_large, center_large, count_large);
    apply_spectral_correction(psd_small, nperseg_small, center_small, count_small);
    
    // Convert frequency arrays from relative to absolute frequencies
    for (int i = 0; i < nperseg_large; i++) {
        f_large[i] = (f_large[i] + config->central_freq) / 1e6;
    }
    
    for (int i = 0; i < nperseg_small; i++) {
        f_small[i] = (f_small[i] + config->central_freq) / 1e6;
    }
    
    // Calculate calibration factor between large and small PSDs
    double constante = fabs(fabs(10 * log10(psd_large[0])) - fabs(10 * log10(psd_small[0])));
    
    // Find noise floor
    float noise = find_min(psd_large, nperseg_large);
    
    int N_f = nperseg_large;
    bool signal_detected = false;
    
    // Check each channel for signal presence
    for (int idx = 0; idx < config->canalization_length; idx++) {
        double center_freq = config->canalization[idx];
        double bw = config->bandwidth[idx];
        
        double target_lower_freq = center_freq - bw / 2;
        double target_upper_freq = center_freq + bw / 2;
        
        int lower_index = find_closest_index(f_large, N_f, target_lower_freq);
        int upper_index = find_closest_index(f_large, N_f, target_upper_freq);
        
        if (lower_index > upper_index) {
            int temp = lower_index;
            lower_index = upper_index;
            upper_index = temp;
        }
        
        if (lower_index < 0) lower_index = 0;
        if (upper_index >= N_f) upper_index = N_f - 1;
        
        int range_length = upper_index - lower_index + 1;
        
        if (range_length > 0) {
            double power_max = find_max(psd_large, lower_index, upper_index);
            double power = calculate_median(psd_large, lower_index, upper_index);
            double snr = 10.0 * log10(power_max / noise);
            
            if (10.0 * log10(power_max) > config->threshold) {
                signal_detected = true;
            }
        }
    }
    
    // Create JSON representation of signal data
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
    
    // Save JSON to output file
    result = save_json_to_file(json_data, config->output_json_path);
    cJSON_Delete(json_data);
    
    if (config->verbose_output) {
        end_time = clock();
        double processing_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        printf("Processing completed in %.3f seconds\n", processing_time);
        printf("Signal %s\n", signal_detected ? "DETECTED" : "NOT DETECTED");
    }
    
cleanup:
    free(psd_large);
    free(f_large);
    free(psd_small);
    free(f_small);
    free(vector_IQ); 
    
    return result;
}

// Implementation for function declared in parameter.h
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