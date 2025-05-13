/**
 * @file
 * @author Martin Ramirez Espinosa, David Ramírez Betancourth
 * 
 * @brief Optimized conversion of CS8 data to complex IQ format.
 * @defgroup cs8_iq CS8 to IQ Conversion Library
 * @{
 * 
 * This library provides APIs for converting binary CS8 (Complex Signed 8-bit) 
 * samples to in-phase and quadrature (IQ) pairs as complex numbers. It supports 
 * both complete file processing and block-based conversion for large files, 
 * with optimizations for performance and cross-platform compatibility.
 */
#ifndef CS8_TO_IQ_H
#define CS8_TO_IQ_H

#include <stdint.h>
#include <complex.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Processing context for block-by-block CS8 to IQ conversion.
 * 
 * Holds state information for incremental processing of CS8 data files.
 */
typedef struct CS8_IQ_Context {
    FILE* file;              /**< File pointer for input or NULL if memory-mapped */
    size_t file_size;        /**< Total size in bytes of input data */
    size_t processed_bytes;  /**< Number of bytes processed so far */
    void* mapped_memory;     /**< Pointer to memory-mapped data (if used) */
    bool use_mmap;           /**< Indicates memory mapping usage */
    int error_code;          /**< Last error code (0 if no error, negative otherwise) */
} CS8_IQ_Context;

/**
 * @brief Error codes for CS8 IQ conversion operations
 * 
 * Standard error codes used across all library functions
 */
enum CS8_IQ_ErrorCodes {
    CS8_IQ_SUCCESS = 0,             /**< Operation succeeded */
    CS8_IQ_ERROR_FILE_OPEN = -1,    /**< Unable to open file for reading */
    CS8_IQ_ERROR_INVALID_SIZE = -2, /**< File size invalid (not divisible by 2) */
    CS8_IQ_ERROR_MEMORY = -3,       /**< Failed to allocate memory */
    CS8_IQ_ERROR_READ = -4,         /**< Error reading input file */
    CS8_IQ_ERROR_PARAM = -5,        /**< Invalid input parameters */
    CS8_IQ_ERROR_MMAP = -6          /**< Memory mapping error */
};

/**
 * @brief Initialize a block-wise file conversion context
 *
 * Prepares a context for processing an input file containing CS8 data.
 * Sets up appropriate file or memory access mechanism based on parameters.
 *
 * @param ctx Pointer to context structure to initialize
 * @param filename Path to CS8 binary data file
 * @param use_mmap Attempt memory mapping if supported for improved throughput
 * @return CS8_IQ_SUCCESS on success, negative error code on failure
 *
 * @see cs8_iq_process_block
 * @see cs8_iq_close_context
 */
int cs8_iq_init_context(CS8_IQ_Context* ctx, const char* filename, bool use_mmap);

/**
 * @brief Convert a data block from the opened CS8 file to IQ samples
 *
 * Reads and converts up to max_samples from the current position in the file
 * and writes the IQ pairs into output_buffer.
 *
 * @param ctx Initialized processing context
 * @param output_buffer Buffer for resulting IQ pairs (complex double)
 * @param max_samples Maximum number of IQ samples to write to output_buffer
 * @param samples_read Pointer to variable updated with actual samples converted
 * @return 0 on success with more data available, 1 at end of file, negative on error
 * 
 * @see cs8_iq_init_context
 * @see cs8_iq_close_context
 */
int cs8_iq_process_block(CS8_IQ_Context* ctx, complex double* output_buffer, 
                        size_t max_samples, size_t* samples_read);

/**
 * @brief Release all resources associated with a CS8_IQ_Context
 *
 * Closes file handles, unmaps memory, and resets the context state.
 * Safe to call multiple times.
 *
 * @param ctx Context to clear and free
 * 
 * @see cs8_iq_init_context
 */
void cs8_iq_close_context(CS8_IQ_Context* ctx);

/**
 * @brief Convert an entire CS8-format file into an IQ sample array
 *
 * Loads and converts a complete file to an array of complex samples.
 * Suitable for files that comfortably fit in memory.
 *
 * @param filename Path to the CS8 binary file
 * @param num_samples Pointer set to the total number of samples loaded
 * @param error_code Optional pointer for detailed error reporting (can be NULL)
 * @return Pointer to allocated IQ sample array, or NULL on error
 * 
 * @note Caller MUST free() the returned memory
 * @see cs8_iq_error_string
 */
complex double* load_iq_data(const char* filename, size_t* num_samples, int* error_code);

/**
 * @brief Convert a raw CS8 buffer directly to IQ samples
 *
 * Low-level function to convert interleaved int8_t I/Q byte pairs into complex numbers
 * with boundary checking. Does not perform file I/O.
 *
 * @param raw_data Pointer to input CS8 buffer (interleaved I/Q samples)
 * @param size_bytes Size in bytes of raw_data (must be even)
 * @param output_buffer Pointer to the output complex array
 * @param max_samples Maximum samples to write into output_buffer
 * @return Number of complex samples converted, or negative error code
 */
int cs8_to_iq_convert(const int8_t* raw_data, size_t size_bytes, 
                    complex double* output_buffer, size_t max_samples);

/**
 * @brief Get a textual description of an error code
 * 
 * Translates numeric error codes to human-readable descriptions.
 * 
 * @param error_code Error code to describe
 * @return String with the error description
 */
const char* cs8_iq_error_string(int error_code);

/** @} */ /* End of cs8_iq group */

#endif // CS8_TO_IQ_H
