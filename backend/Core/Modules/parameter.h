/**
 * @file parameter.h
 * @author Martin Ramirez Espinosa, David Ramírez Betancourth
 * @brief Signal spectrum processing library interface
 * @ingroup signal_processor
 *
 * Defines types and function prototypes for:
 * - Configuring spectrum analysis parameters
 * - Processing signal data to detect transmissions
 * - Generating JSON output for visualization
 * - Reporting errors in a structured manner
 */

#ifndef SIGNAL_PROCESSOR_H
#define SIGNAL_PROCESSOR_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include "../Modules/CS8toIQ.h"
#include "../Modules/IQ.h"
#include "../Modules/welch.h"
#include "../Modules/cJSON.h"
#include "../Modules/find_closest_index.h"

/**
 * @enum SPErrorCode
 * @brief Error codes returned by signal processing functions
 *
 * Provides detailed status of operations:
 * - SP_SUCCESS: Operation succeeded
 * - SP_ERROR_NULL_POINTER: A required pointer was NULL
 * - SP_ERROR_MEMORY_ALLOC: Memory allocation failed
 * - SP_ERROR_FILE_IO: File input/output failure
 * - SP_ERROR_INVALID_PARAMETER: Invalid argument provided
 * - SP_ERROR_DATA_PROCESSING: Error during data analysis
 */
typedef enum {
    SP_SUCCESS                 =  0, /**< Success */
    SP_ERROR_NULL_POINTER      = -1, /**< Null pointer argument */
    SP_ERROR_MEMORY_ALLOC      = -2, /**< Allocation failure */
    SP_ERROR_FILE_IO           = -3, /**< File I/O error */
    SP_ERROR_INVALID_PARAMETER = -4, /**< Invalid parameter */
    SP_ERROR_DATA_PROCESSING   = -5  /**< Data processing error */
} SPErrorCode;

/**
 * @struct SignalProcessorConfig
 * @brief Configuration for spectrum processing
 *
 * Holds all parameters required to perform spectrum analysis:
 * - input_file_path: Path to the raw signal data file
 * - central_freq:    Center frequency in Hertz
 * - nperseg_large:   Segment size for coarse analysis
 * - nperseg_small:   Segment size for fine analysis
 * - threshold:       Detection threshold in dB
 * - canalization:    Array of channel center frequencies (MHz)
 * - bandwidth:       Array of channel bandwidths (MHz)
 * - canalization_length: Number of channels defined
 * - output_json_path: Path where JSON results will be written
 * - use_mmap:        Enable memory-mapped file access
 * - verbose_output:  Enable detailed console logging
 */
typedef struct {
    const char* input_file_path;
    uint64_t    central_freq;
    int         nperseg_large;
    int         nperseg_small;
    int         threshold;
    double*     canalization;
    double*     bandwidth;
    int         canalization_length;
    const char* output_json_path;
    bool        use_mmap;
    bool        verbose_output;
} SignalProcessorConfig;

/**
 * @brief Analyze signal spectrum and produce JSON output.
 *
 * Processes input data according to the provided configuration:
 * 1. Reads the signal file (optionally via mmap)
 * 2. Splits data into overlapping segments
 * 3. Computes spectral power and applies thresholding
 * 4. Detects active channels and timestamps
 * 5. Writes results to a JSON file
 *
 * @param config Pointer to a fully populated SignalProcessorConfig
 * @return SP_SUCCESS on success, or an SPErrorCode on failure
 */
int process_signal_spectrum(const SignalProcessorConfig* config);

/**
 * @brief Retrieve a human-readable message for an error code.
 *
 * Maps SPErrorCode values to descriptive strings:
 * e.g., SP_ERROR_FILE_IO → "File input/output error"
 *
 * @param error_code Numeric error code returned by library functions
 * @return Constant string describing the error
 */
const char* get_signal_processor_error(int error_code);

#endif // SIGNAL_PROCESSOR_H
