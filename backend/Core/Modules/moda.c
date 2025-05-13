#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "moda.h"

double find_min(double *array, size_t size) {
    double min = 1000; // Initialize with a large value (Note: using DBL_MAX from <float.h> is safer for actual maximum value)
    for (size_t i = 0; i < size; i++) {
        if (array[i] < min) {
            min = array[i];
        }
    }
    return min;
}

double find_max(double *array, int lower_index, int upper_index) {
    int size = upper_index - lower_index;
    double max = array[lower_index];  // Initialize max with the first value in the range
    for (size_t i = 0; i < size; i++) {
        if (array[lower_index + i] > max) {
            max = array[lower_index + i];  // Corrected access to array[lower_index + i]
        }
    }
    return max;
}


// Function to perform slicing
double* slice(double* array, int lower_index, int upper_index) {
    // Calculate the size of the new array
    int size = upper_index - lower_index;
    if(size > 9830){
        size = 9830;
    }

    // Allocate memory for the new array
    double* sliced_array = (double*)malloc(size * sizeof(double));
    if (sliced_array == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    // Copy values from the specified range
    for (int i = 0; i < size; i++) {
        sliced_array[i] = array[lower_index + i];
        printf("sliced_array: %f\n",sliced_array[i]);
    }


    return sliced_array;
}