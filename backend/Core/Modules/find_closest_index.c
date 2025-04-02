/**
 * @file find_closest_index.c
 * @brief Función para encontrar el índice más cercano a un valor en un arreglo.
 * 
 * Este archivo contiene la declaración de la función `find_closest_index`, que se utiliza para 
 * encontrar el índice del elemento más cercano a un valor dado en un arreglo de números de punto flotante.
 */
#include <stdio.h>
#include <math.h>

int find_closest_index(double* array, int length, double value) {
    int min_index = 0;
    double min_diff = fabs(array[0] - value);
    for (int i = 1; i < length; i++) {
        double diff = fabs(array[i] - value);
        if (diff < min_diff) {
            min_diff = diff;
            min_index = i;
        }
    }
    return min_index;
}