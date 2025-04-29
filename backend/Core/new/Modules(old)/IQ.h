#ifndef IQ_H
#define IQ_H

#include <stdint.h>
#include <complex.h>

#define MAX_BAND_SIZE 50 ///< Tamaño máximo para el buffer de bandas

void delete_CS8(uint8_t file_sample);

void delete_JSON(uint8_t file_json);

int load_bands(double* frequencies, double* bandwidths);

#endif // IQ_H