/**
 * @file CS8toIQ.c
 * @brief Implementación optimizada de conversión de datos CS8 a formato IQ complejo.
 *
 * Este archivo contiene la implementación de las funciones declaradas en cs8_to_iq.h
 * para el procesamiento eficiente de datos CS8 (Complex Signed 8-bit) y su conversión
 * a formato IQ (In-phase and Quadrature) complejo.
 * 
 * Optimizaciones implementadas:
 * - Uso opcional de mapeo de memoria para entrada/salida eficiente
 * - Procesamiento por bloques para manejo de archivos grandes
 * - Validación exhaustiva de parámetros y manejo de errores
 * - Acceso optimizado a memoria
 */

#include "CS8toIQ.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
    // Windows no tiene mmap nativo, usamos un enfoque de lectura en bloques
    #define USE_MMAP 0
#else
    // En sistemas POSIX, usamos mmap para entrada/salida más eficiente
    #define USE_MMAP 1
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

// Tamaño de bloque óptimo para procesamiento (ajustable según requisitos)
#define OPTIMAL_BLOCK_SIZE (1024 * 1024)  // 1MB de datos por defecto

// Mensajes de error
static const char* error_messages[] = {
    "Operación exitosa",
    "Error al abrir el archivo",
    "Tamaño de archivo inválido (debe ser múltiplo de 2)",
    "Error de asignación de memoria",
    "Error de lectura del archivo",
    "Parámetros inválidos",
    "Error en mapeo de memoria"
};

const char* cs8_iq_error_string(int error_code) {
    error_code = -error_code;  // Convertimos a positivo para indexar el array
    if (error_code >= 0 && error_code < (int)(sizeof(error_messages) / sizeof(error_messages[0]))) {
        return error_messages[error_code];
    } else {
        return "Error desconocido";
    }
}

/**
 * @brief Convierte datos CS8 a formato IQ con validación de límites.
 * 
 * Esta función interna realiza la conversión de bytes CS8 a números complejos
 * garantizando que no ocurran desbordamientos.
 * 
 * @param raw_data Datos CS8 de entrada.
 * @param size_bytes Tamaño en bytes (debe ser par).
 * @param output_buffer Buffer de salida para los datos IQ.
 * @param max_samples Capacidad máxima del buffer de salida.
 * @return Número de muestras convertidas o código de error negativo.
 */
int cs8_to_iq_convert(const int8_t* raw_data, size_t size_bytes, 
                    complex double* output_buffer, size_t max_samples) {
    // Validación de parámetros
    if (!raw_data || !output_buffer || size_bytes % 2 != 0) {
        return CS8_IQ_ERROR_PARAM;
    }
    
    size_t num_samples = size_bytes / 2;
    if (num_samples > max_samples) {
        num_samples = max_samples;  // Limitamos al tamaño del buffer
    }
    
    // Convertimos los datos (I, Q) -> complejo
    for (size_t i = 0; i < num_samples; i++) {
        // Creamos un número complejo donde:
        // - La parte real es el componente I (in-phase)
        // - La parte imaginaria es el componente Q (quadrature)
        output_buffer[i] = raw_data[2 * i] + raw_data[2 * i + 1] * I;
    }
    
    return (int)num_samples;
}

int cs8_iq_init_context(CS8_IQ_Context* ctx, const char* filename, bool use_mmap) {
    if (!ctx || !filename) {
        return CS8_IQ_ERROR_PARAM;
    }
    
    // Inicializamos el contexto con valores seguros
    memset(ctx, 0, sizeof(CS8_IQ_Context));
    
#if USE_MMAP
    if (use_mmap) {
        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            ctx->error_code = CS8_IQ_ERROR_FILE_OPEN;
            return ctx->error_code;
        }
        
        // Obtenemos tamaño del archivo
        struct stat st;
        if (fstat(fd, &st) == -1) {
            close(fd);
            ctx->error_code = CS8_IQ_ERROR_FILE_OPEN;
            return ctx->error_code;
        }
        
        ctx->file_size = st.st_size;
        
        // Verificamos que el tamaño sea válido (múltiplo de 2)
        if (ctx->file_size % 2 != 0) {
            close(fd);
            ctx->error_code = CS8_IQ_ERROR_INVALID_SIZE;
            return ctx->error_code;
        }
        
        // Mapeamos el archivo completo a memoria
        ctx->mapped_memory = mmap(NULL, ctx->file_size, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);  // Podemos cerrar el file descriptor después de mapear
        
        if (ctx->mapped_memory == MAP_FAILED) {
            ctx->mapped_memory = NULL;
            ctx->error_code = CS8_IQ_ERROR_MMAP;
            return ctx->error_code;
        }
        
        ctx->use_mmap = true;
    } else {
#endif
        // Enfoque de lectura tradicional
        ctx->file = fopen(filename, "rb");
        if (!ctx->file) {
            ctx->error_code = CS8_IQ_ERROR_FILE_OPEN;
            return ctx->error_code;
        }
        
        // Determinamos el tamaño del archivo
        fseek(ctx->file, 0, SEEK_END);
        ctx->file_size = ftell(ctx->file);
        rewind(ctx->file);
        
        // Verificamos que el tamaño sea válido (múltiplo de 2)
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

int cs8_iq_process_block(CS8_IQ_Context* ctx, complex double* output_buffer, 
                        size_t max_samples, size_t* samples_read) {
    if (!ctx || !output_buffer || !samples_read || max_samples == 0) {
        return CS8_IQ_ERROR_PARAM;
    }
    
    *samples_read = 0;
    
    // Verificamos si ya hemos llegado al final
    if (ctx->processed_bytes >= ctx->file_size) {
        return 1;  // Indicamos fin de archivo
    }
    
    // Calculamos cuántos bytes procesar en este bloque
    size_t remaining_bytes = ctx->file_size - ctx->processed_bytes;
    size_t bytes_to_process = max_samples * 2;  // Cada muestra compleja requiere 2 bytes
    
    if (bytes_to_process > remaining_bytes) {
        bytes_to_process = remaining_bytes;
    }
    
    // Aseguramos que bytes_to_process sea par
    bytes_to_process &= ~1UL;  // Forzamos a ser par (eliminamos el bit menos significativo)
    
    if (bytes_to_process == 0) {
        return 1;  // Fin de archivo
    }
    
    int result;
    
    if (ctx->use_mmap) {
#if USE_MMAP
        // Accedemos directamente a la memoria mapeada
        const int8_t* data_ptr = (const int8_t*)ctx->mapped_memory + ctx->processed_bytes;
        result = cs8_to_iq_convert(data_ptr, bytes_to_process, output_buffer, max_samples);
#else
        // No debería llegar aquí si USE_MMAP está desactivado
        return CS8_IQ_ERROR_PARAM;
#endif
    } else {
        // Leemos un bloque del archivo
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
        return result;  // Propagamos el error
    }
}

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
    
    // Limpiamos el contexto para evitar reutilización insegura
    memset(ctx, 0, sizeof(CS8_IQ_Context));
}

complex double* cargar_cs8(const char* filename, size_t* num_samples, int* error_code) {
    int local_error_code = CS8_IQ_SUCCESS;
    
    // Verificar parámetros
    if (!filename || !num_samples) {
        if (error_code) *error_code = CS8_IQ_ERROR_PARAM;
        return NULL;
    }
    
    *num_samples = 0;
    
    // Inicializar el contexto para procesar el archivo
    CS8_IQ_Context ctx;
    local_error_code = cs8_iq_init_context(&ctx, filename, USE_MMAP);
    
    if (local_error_code != CS8_IQ_SUCCESS) {
        if (error_code) *error_code = local_error_code;
        return NULL;
    }
    
    // Calcular tamaño necesario para el buffer
    size_t samples_count = ctx.file_size / 2;
    complex double* IQ_data = (complex double*)malloc(samples_count * sizeof(complex double));
    
    if (!IQ_data) {
        cs8_iq_close_context(&ctx);
        if (error_code) *error_code = CS8_IQ_ERROR_MEMORY;
        return NULL;
    }
    
    // Leer todo el archivo de una vez (esto es preferible para archivos que caben en memoria)
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

// Versión compatible con la interfaz original
complex double* cargar_cs8_og(const char* filename, size_t* num_samples) {
    return cargar_cs8(filename, num_samples, NULL);
}
