/**
 * @file parameter.h
 * @brief Optimized signal processing for spectral analysis
 *
 * This header file declares the public API for the optimized signal processing
 * functions that perform CS8 data processing, Welch's method analysis, and
 * JSON output generation. The implementation prioritizes performance, security,
 * and cross-platform compatibility.
 *
 * @author Martin Ramirez Espinosa
 */

#ifndef SIGNAL_PROCESSOR_H
#define SIGNAL_PROCESSOR_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Error codes for signal processing operations
 */
typedef enum {
    SP_SUCCESS = 0,                 ///< Operation completed successfully
    SP_ERROR_NULL_POINTER = -1,     ///< Null pointer provided as argument
    SP_ERROR_MEMORY_ALLOC = -2,     ///< Memory allocation failed
    SP_ERROR_FILE_IO = -3,          ///< File I/O error occurred
    SP_ERROR_INVALID_PARAMETER = -4, ///< Invalid parameter provided
    SP_ERROR_DATA_PROCESSING = -5   ///< Error in data processing
} SPErrorCode;

/**
 * @brief Configuration structure for signal processing
 */
typedef struct {
    /* Input parameters */
    const char* input_file_path;    ///< Path to input CS8 file
    uint64_t central_freq;          ///< Central frequency in Hz
    int nperseg_large;              ///< Segment size for high-resolution PSD (must be even)
    int nperseg_small;              ///< Segment size for visualization PSD (must be even)
    int threshold;                  ///< Power threshold for signal detection in dB
    double* canalization;           ///< Array of channel center frequencies
    double* bandwidth;              ///< Array of channel bandwidths
    int canalization_length;        ///< Number of channels
    
    /* Output parameters */
    const char* output_json_path;   ///< Path to output JSON file
    
    /* Processing flags */
    bool use_mmap;                  ///< Use memory mapping for file I/O if available
    bool verbose_output;            ///< Print verbose progress information
} SignalProcessorConfig;

/**
 * @brief Process signal data and generate spectrum analysis
 * 
 * This function performs the following operations:
 * 1. Loads CS8 data from the specified file
 * 2. Performs Welch's method for PSD estimation at two resolutions
 * 3. Processes the spectrum to identify signals in specified channels
 * 4. Generates a JSON output with results
 * 
 * @param config Pointer to processing configuration structure
 * @return 0 on success, error code otherwise
 */
int process_signal_spectrum(const SignalProcessorConfig* config);

/**
 * @brief Get error message for signal processing error codes
 * 
 * @param error_code The error code
 * @return Error message string
 */
const char* get_signal_processor_error(int error_code);

/**
 * @brief Compatibility wrapper for the original 'parameter' function
 * 
 * This function maintains backward compatibility with the original API,
 * but internally uses the optimized implementation.
 * 
 * @param threshold Signal detection threshold
 * @param canalization Array of channel center frequencies
 * @param bandwidth Array of channel bandwidths
 * @param canalization_length Number of channels
 * @param central_freq Central frequency in Hz
 */
void parameter(int threshold, double* canalization, double* bandwidth, int canalization_length, uint64_t central_freq);

#endif /* SIGNAL_PROCESSOR_H */