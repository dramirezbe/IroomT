/**
 * @file CS8toIQ.h
 * @brief Conversión optimizada de datos CS8 a formato IQ complejo.
 *
 * Este archivo contiene las declaraciones de funciones para procesar datos
 * en formato CS8 (Complex Signed 8-bit) y convertirlos a formato IQ (In-phase and Quadrature).
 * Incluye funciones para procesamiento completo de archivos y procesamiento por bloques
 * para aplicaciones de tiempo real o con restricciones de memoria.
 *
 */

#ifndef CS8_TO_IQ_H
#define CS8_TO_IQ_H

#include <stdint.h>
#include <complex.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Estructura para manejar el contexto de conversión CS8 a IQ.
 * 
 * Esta estructura mantiene el estado del procesamiento para permitir
 * el procesamiento por bloques.
 */
typedef struct CS8_IQ_Context {
    FILE* file;              ///< Puntero al archivo de entrada
    size_t file_size;        ///< Tamaño total del archivo
    size_t processed_bytes;  ///< Bytes procesados hasta el momento
    void* mapped_memory;     ///< Puntero al mapeo de memoria (si se utiliza)
    bool use_mmap;           ///< Indica si se está usando mapeo de memoria
    int error_code;          ///< Código de error (0 si no hay error)
} CS8_IQ_Context;

/**
 * @brief Códigos de error para las funciones de conversión.
 */
enum CS8_IQ_ErrorCodes {
    CS8_IQ_SUCCESS = 0,             ///< Operación exitosa
    CS8_IQ_ERROR_FILE_OPEN = -1,    ///< Error al abrir el archivo
    CS8_IQ_ERROR_INVALID_SIZE = -2, ///< Tamaño de archivo inválido
    CS8_IQ_ERROR_MEMORY = -3,       ///< Error de asignación de memoria
    CS8_IQ_ERROR_READ = -4,         ///< Error de lectura del archivo
    CS8_IQ_ERROR_PARAM = -5,        ///< Parámetros inválidos
    CS8_IQ_ERROR_MMAP = -6          ///< Error en mapeo de memoria
};

/**
 * @brief Inicializa un contexto para procesamiento por bloques.
 * 
 * @param ctx Puntero a una estructura de contexto a inicializar.
 * @param filename Nombre del archivo binario a procesar.
 * @param use_mmap Indica si se debe usar mapeo de memoria (recomendado para mejor rendimiento).
 * 
 * @return 0 en caso de éxito, código de error negativo en caso contrario.
 */
int cs8_iq_init_context(CS8_IQ_Context* ctx, const char* filename, bool use_mmap);

/**
 * @brief Procesa un bloque de datos del archivo abierto en el contexto.
 * 
 * Esta función es ideal para procesamiento en tiempo real donde se necesita 
 * procesar los datos por bloques sin cargar todo el archivo en memoria.
 * 
 * @param ctx Contexto de procesamiento inicializado.
 * @param output_buffer Buffer donde se almacenarán los datos IQ convertidos.
 * @param max_samples Tamaño máximo del buffer de salida (en número de muestras complejas).
 * @param samples_read Puntero donde se almacenará el número de muestras leídas.
 * 
 * @return 0 en caso de éxito, código de error negativo en caso contrario.
 *         Retorna 1 cuando se ha llegado al final del archivo.
 */
int cs8_iq_process_block(CS8_IQ_Context* ctx, complex double* output_buffer, 
                        size_t max_samples, size_t* samples_read);

/**
 * @brief Libera los recursos asignados al contexto.
 * 
 * @param ctx Contexto a liberar.
 */
void cs8_iq_close_context(CS8_IQ_Context* ctx);

/**
 * @brief Carga y convierte todos los datos IQ desde un archivo binario con formato CS8.
 * 
 * Esta función es una versión optimizada de la original con mejoras de rendimiento
 * y seguridad. Para archivos grandes o procesamiento en tiempo real, considere usar
 * las funciones de procesamiento por bloques.
 * 
 * @param filename Nombre del archivo binario que contiene los datos en formato CS8.
 * @param num_samples Puntero a una variable donde se almacenará el número de muestras leídas.
 * @param error_code Puntero opcional donde se almacenará el código de error (puede ser NULL).
 * 
 * @return Un puntero a un arreglo de números complejos (`complex double`), 
 *         que contiene las muestras IQ. Retorna `NULL` en caso de error.
 * 
 * @note Es responsabilidad del usuario liberar la memoria asignada para el arreglo devuelto
 *       usando la función `free()`.
 */
complex double* cargar_cs8(const char* filename, size_t* num_samples, int* error_code);

/**
 * @brief Versión compatible con la interfaz original.
 * 
 * @param filename Nombre del archivo binario que contiene los datos en formato CS8.
 * @param num_samples Puntero a una variable donde se almacenará el número de muestras leídas.
 * 
 * @return Un puntero a un arreglo de números complejos (`complex double`), 
 *         que contiene las muestras IQ. Retorna `NULL` en caso de error.
 */
complex double* cargar_cs8_og(const char* filename, size_t* num_samples);

/**
 * @brief Convierte un buffer de datos CS8 a formato IQ complejo.
 * 
 * @param raw_data Puntero a los datos en formato CS8.
 * @param size_bytes Tamaño del buffer en bytes (debe ser par).
 * @param output_buffer Buffer donde se almacenarán los datos convertidos.
 * @param max_samples Tamaño máximo del buffer de salida.
 * 
 * @return Número de muestras complejas convertidas, o valor negativo en caso de error.
 */
int cs8_to_iq_convert(const int8_t* raw_data, size_t size_bytes, 
                    complex double* output_buffer, size_t max_samples);

/**
 * @brief Obtiene una descripción textual de un código de error.
 * 
 * @param error_code Código de error a describir.
 * @return Cadena con la descripción del error.
 */
const char* cs8_iq_error_string(int error_code);

#endif  // CS8_TO_IQ_H
