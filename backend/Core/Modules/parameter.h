
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>

double median(double* array, int start, int end);

int compare_doubles(const void* a, const void* b);

void parameter(int threshold, double* canalization, double* bandwidth, int canalization_length, uint64_t central_freq);

#endif // PARAMETER_ANALYSIS_H