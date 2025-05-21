/**
 * @file
 * @author Martin Ramirez Espinosa, David Ramírez Betancourth
 * 
 * @brief Implementation of CS8 to IQ conversion library
 * @ingroup cs8_iq
 *
 * This file implements efficient functions for converting CS8 (Complex Signed 8-bit)
 * data to IQ (In-phase and Quadrature) complex format. The library handles both
 * complete file processing and incremental block-wise conversion, with optimizations
 * for various platforms.
 *
 * Key features:
 * - Memory mapping for efficient I/O on supported platforms
 * - Block processing for handling large files
 * - Robust error handling and parameter validation
 * - Cross-platform compatibility (Windows/POSIX)
 */
#include "CS8toIQ.h"


#ifdef _WIN32
    /**
     * @brief Disable memory mapping on Windows platforms
     */
    #define USE_MMAP 0
#else
    /**
     * @brief Enable memory mapping on POSIX platforms
     */
    #define USE_MMAP 1
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

/**
 * @brief Optimal block size for processing
 *
 * Defines the recommended chunk size for block-wise processing to balance
 * memory usage and processing efficiency.
 */
#define OPTIMAL_BLOCK_SIZE (1024 * 1024)  // 1MB default block size

/**
 * @brief Error message array for human-readable error reporting
 */
static const char* error_messages[] = {
    "Success",
    "Error opening file",
    "Invalid file size (must be a multiple of 2)",
    "Memory allocation error",
    "File read error",
    "Invalid parameters",
    "Memory mapping error"
};

/**
 * @brief Get a textual description of an error code
 * 
 * Translates numeric error codes to human-readable descriptions.
 * 
 * @param error_code Error code to describe
 * @return String with the error description
 */
const char* cs8_iq_error_string(int error_code) {
    error_code = -error_code;  // Convert to positive to index the array
    if (error_code >= 0 && error_code < (int)(sizeof(error_messages) / sizeof(error_messages[0]))) {
        return error_messages[error_code];
    } else {
        return "Unknown error";
    }
}

/**
 * @brief Convert raw CS8 data to IQ format
 *
 * Core conversion function that transforms interleaved I/Q bytes into complex
 * numbers. Handles validation and buffer size limits.
 *
 * @param raw_data Input CS8 buffer containing interleaved I/Q samples
 * @param size_bytes Size in bytes of raw_data (must be even)
 * @param output_buffer Output buffer for complex samples
 * @param max_samples Maximum capacity of output_buffer in complex samples
 * @return Number of complex samples converted, or negative error code
 */
int cs8_to_iq_convert(const int8_t* raw_data, size_t size_bytes, 
                    complex double* output_buffer, size_t max_samples) {
    if (!raw_data || !output_buffer || size_bytes % 2 != 0) {
        return CS8_IQ_ERROR_PARAM;
    }
    
    size_t num_samples = size_bytes / 2;
    if (num_samples > max_samples) {
        num_samples = max_samples;  // Limit to buffer size
    }
    
    for (size_t i = 0; i < num_samples; i++) {
        // Create complex sample: real (I) + imaginary (Q)
        output_buffer[i] = raw_data[2 * i] + raw_data[2 * i + 1] * I;
    }
    
    return (int)num_samples;
}

/**
 * @brief Initialize a context for block processing
 *
 * Prepares a processing context for a given input file with appropriate
 * access strategy (memory-mapped or file I/O) based on parameters and platform.
 *
 * @param ctx Context structure to initialize
 * @param filename Path to CS8 binary data file
 * @param use_mmap Whether to use memory mapping when available
 * @return CS8_IQ_SUCCESS on success, negative error code otherwise
 */
int cs8_iq_init_context(CS8_IQ_Context* ctx, const char* filename, bool use_mmap) {
    if (!ctx || !filename) {
        return CS8_IQ_ERROR_PARAM;
    }
    
    // Initialize context with safe values
    memset(ctx, 0, sizeof(CS8_IQ_Context));
    
#if USE_MMAP
    if (use_mmap) {
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            ctx->error_code = CS8_IQ_ERROR_FILE_OPEN;
            return ctx->error_code;
        }
        
        // Get file size
        struct stat st;
        if (fstat(fd, &st) == -1) {
            close(fd);
            ctx->error_code = CS8_IQ_ERROR_FILE_OPEN;
            return ctx->error_code;
        }
        
        ctx->file_size = st.st_size;
        
        // Verify valid size (multiple of 2)
        if (ctx->file_size % 2 != 0) {
            close(fd);
            ctx->error_code = CS8_IQ_ERROR_INVALID_SIZE;
            return ctx->error_code;
        }
        
        // Map entire file to memory
        ctx->mapped_memory = mmap(NULL, ctx->file_size, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);  // Can close file descriptor after mapping
        
        if (ctx->mapped_memory == MAP_FAILED) {
            ctx->mapped_memory = NULL;
            ctx->error_code = CS8_IQ_ERROR_MMAP;
            return ctx->error_code;
        }
        
        ctx->use_mmap = true;
    } else {
#endif
        // Traditional file I/O approach
        ctx->file = fopen(filename, "rb");
        if (!ctx->file) {
            ctx->error_code = CS8_IQ_ERROR_FILE_OPEN;
            return ctx->error_code;
        }
        
        // Determine file size
        fseek(ctx->file, 0, SEEK_END);
        ctx->file_size = ftell(ctx->file);
        rewind(ctx->file);
        
        // Verify valid size (multiple of 2)
        if (ctx->file_size % 2 != 0) {
            fclose(ctx->file);
            ctx->file = NULL;
            ctx->error_code = CS8_IQ_ERROR_INVALID_SIZE;
            return ctx->error_code;
        }
        
        ctx->use_mmap = false;
#if USE_MMAP
    }
#endif
    
    ctx->processed_bytes = 0;
    ctx->error_code = CS8_IQ_SUCCESS;
    
    return CS8_IQ_SUCCESS;
}

/**
 * @brief Process a block of data from the context
 *
 * Reads and converts the next chunk of data from the file/memory,
 * handling boundaries and updating progress indicators.
 *
 * @param ctx Initialized processing context
 * @param output_buffer Buffer for the complex samples
 * @param max_samples Maximum number of samples to process
 * @param samples_read Updated with the actual number of samples converted
 * @return 0 on success with more data available, 1 at EOF, negative on error
 */
int cs8_iq_process_block(CS8_IQ_Context* ctx, complex double* output_buffer, 
                        size_t max_samples, size_t* samples_read) {
    if (!ctx || !output_buffer || !samples_read || max_samples == 0) {
        return CS8_IQ_ERROR_PARAM;
    }
    
    *samples_read = 0;
    
    // Check if we've reached the end
    if (ctx->processed_bytes >= ctx->file_size) {
        return 1;  // End of file indicator
    }
    
    // Calculate bytes to process in this block
    size_t remaining_bytes = ctx->file_size - ctx->processed_bytes;
    size_t bytes_to_process = max_samples * 2;  // Each complex sample requires 2 bytes
    
    if (bytes_to_process > remaining_bytes) {
        bytes_to_process = remaining_bytes;
    }
    
    // Ensure bytes_to_process is even
    bytes_to_process &= ~1UL;  // Force to be even (clear least significant bit)
    
    if (bytes_to_process == 0) {
        return 1;  // End of file
    }
    
    int result;
    
    if (ctx->use_mmap) {
#if USE_MMAP
        // Access mapped memory directly
        const int8_t* data_ptr = (const int8_t*)ctx->mapped_memory + ctx->processed_bytes;
        result = cs8_to_iq_convert(data_ptr, bytes_to_process, output_buffer, max_samples);
#else
        // Should not reach here if USE_MMAP is disabled
        return CS8_IQ_ERROR_PARAM;
#endif
    } else {
        // Read a block from the file
        int8_t* buffer = (int8_t*)malloc(bytes_to_process);
        if (!buffer) {
            return CS8_IQ_ERROR_MEMORY;
        }
        
        size_t bytes_read = fread(buffer, 1, bytes_to_process, ctx->file);
        if (bytes_read != bytes_to_process) {
            free(buffer);
            return CS8_IQ_ERROR_READ;
        }
        
        result = cs8_to_iq_convert(buffer, bytes_to_process, output_buffer, max_samples);
        free(buffer);
    }
    
    if (result > 0) {
        ctx->processed_bytes += bytes_to_process;
        *samples_read = result;
        return (ctx->processed_bytes >= ctx->file_size) ? 1 : 0;
    } else {
        return result;  // Propagate error
    }
}

/**
 * @brief Release resources used by a processing context
 *
 * Properly closes files and frees memory resources used by the context.
 * Safe to call multiple times on the same context.
 *
 * @param ctx Context to release
 */
void cs8_iq_close_context(CS8_IQ_Context* ctx) {
    if (!ctx) return;
    
    if (ctx->use_mmap) {
#if USE_MMAP
        if (ctx->mapped_memory) {
            munmap(ctx->mapped_memory, ctx->file_size);
            ctx->mapped_memory = NULL;
        }
#endif
    } else {
        if (ctx->file) {
            fclose(ctx->file);
            ctx->file = NULL;
        }
    }
    
    // Clear context to prevent unsafe reuse
    memset(ctx, 0, sizeof(CS8_IQ_Context));
}

/**
 * @brief Load an entire CS8 file into IQ format
 *
 * Complete file processing function that handles the entire workflow:
 * opening the file, allocating memory, converting data, and error handling.
 *
 * @param filename Path to CS8 binary file
 * @param num_samples Updated with the number of complex samples loaded
 * @param error_code Optional pointer for detailed error reporting
 * @return Pointer to allocated array of complex samples, or NULL on error
 *
 * @note Caller MUST free() the returned memory
 */
complex double* load_iq_data(const char* filename, size_t* num_samples, int* error_code) {
    int local_error_code = CS8_IQ_SUCCESS;
    
    if (!filename || !num_samples) {
        if (error_code) *error_code = CS8_IQ_ERROR_PARAM;
        return NULL;
    }
    
    *num_samples = 0;
    
    // Initialize context to process the file
    CS8_IQ_Context ctx;
    local_error_code = cs8_iq_init_context(&ctx, filename, USE_MMAP);
    
    if (local_error_code != CS8_IQ_SUCCESS) {
        if (error_code) *error_code = local_error_code;
        return NULL;
    }
    
    // Calculate required buffer size
    size_t samples_count = ctx.file_size / 2;
    complex double* IQ_data = (complex double*)malloc(samples_count * sizeof(complex double));
    
    if (!IQ_data) {
        cs8_iq_close_context(&ctx);
        if (error_code) *error_code = CS8_IQ_ERROR_MEMORY;
        return NULL;
    }
    
    // Read the entire file at once
    size_t samples_read = 0;
    int result = cs8_iq_process_block(&ctx, IQ_data, samples_count, &samples_read);
    
    if (result < 0) {
        free(IQ_data);
        cs8_iq_close_context(&ctx);
        if (error_code) *error_code = result;
        return NULL;
    }
    
    *num_samples = samples_read;
    cs8_iq_close_context(&ctx);
    
    if (error_code) *error_code = CS8_IQ_SUCCESS;
    return IQ_data;
}
