#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "moda.h"

double find_min(double *array, size_t size) {
    double min = 1000; // Inicializamos con el máximo valor posible para double
    for (size_t i = 0; i < size; i++) {
        if (array[i] < min) {
            min = array[i];
        }
    }
    return min;
}

double find_max(double *array, int lower_index, int upper_index) {
    int size = upper_index - lower_index;
    double max = array[lower_index];  // Inicializamos max con el primer valor en el rango
    for (size_t i = 0; i < size; i++) {
        if (array[lower_index + i] > max) {
            max = array[lower_index + i];  // Corregimos el acceso a array[lower_index + i]
        }
    }
    return max;
}


// Función para hacer slicing
double* slice(double* array, int lower_index, int upper_index) {
    // Calcula el tamaño del nuevo array
    int size = upper_index - lower_index;
    if(size>9830){
        size=9830;
     }

    // Reserva memoria para el nuevo array
    double* sliced_array = (double*)malloc(size * sizeof(double));
    if (sliced_array == NULL) {
        perror("Error al asignar memoria");
        exit(EXIT_FAILURE);
    }

    // Copia los valores desde el rango especificado
    for (int i = 0; i < size; i++) {
        sliced_array[i] = array[lower_index + i];
        printf("sliced_array: %f\n",sliced_array[i]);
    }
    

    return sliced_array;
}