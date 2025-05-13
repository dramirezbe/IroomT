#ifndef IQ_H
#define IQ_H

#include <stdint.h>
#include <complex.h>
#include "script_utils.h"

#define MAX_BAND_SIZE 50 ///< Max size buffer


int load_bands(double* frequencies, double* bandwidths, env_path_t *paths);

#endif // IQ_H