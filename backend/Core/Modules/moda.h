/**
 * @file moda.h
 * @brief Header file for mode calculation functions.
 *
 * This file contains declarations for functions related to statistical calculations,
 * specifically focusing on finding the mode and auxiliary functions like finding
 * minimum and maximum values and slicing arrays.
 */
#ifndef MODE_CALCULATION_H
#define MODE_CALCULATION_H

#include <stdio.h>
#include <stdlib.h>

#include <stddef.h> // For size_t type

// Función para calcular el valor mínimo en un vector de doubles
double find_min(double *array, size_t size);

double find_max(double *array, int lower_index, int upper_index);

// Function prototype for calculating the mode
double calculate_mode(double data[], int size);

double* slice(double* array, int lower_index, int upper_index);

#endif // MODE_CALCULATION_H