/**
 * @file find_closest_index.c
 * @brief Function to find the index closest to a value in an array.
 *
 * This file contains the implementation of the `find_closest_index` function, which is used to
 * find the index of the element closest to a given value in an array of floating-point numbers.
 */
#include <stdio.h>

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