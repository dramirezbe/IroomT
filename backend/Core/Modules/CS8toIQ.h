#ifndef CS8TOIQ_H
#define CS8TOIQ_H

#include <stdint.h>
#include <complex.h>
#include <stddef.h>

complex double* cargar_cs8(const char* filename, size_t* num_samples);

#endif  // CS8TOIQ_H