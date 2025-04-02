/**
 * @file find_closest_index.h
 * @brief Declaración de la función para encontrar el índice más cercano a un valor en un arreglo.
 * 
 * Este archivo contiene la declaración de la función `find_closest_index`, que se utiliza para 
 * encontrar el índice del elemento más cercano a un valor dado en un arreglo de números de punto flotante.
 */

#ifndef FIND_CLOSEST_INDEX_H
#define FIND_CLOSEST_INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * @brief Encuentra el índice del elemento en el arreglo que está más cerca de un valor dado.
 * 
 * Esta función recorre un arreglo de números de punto flotante (`double`)
 * y calcula la diferencia absoluta entre cada elemento y el valor objetivo `value`.
 * Retorna el índice del elemento cuya diferencia es la menor.
 *
 * @param array Un puntero al arreglo de valores de tipo `double`.
 * @param length La cantidad de elementos en el arreglo.
 * @param value El valor al cual se desea encontrar el elemento más cercano.
 * 
 * @return El índice del elemento en el arreglo que es más cercano al valor dado `value`.
 * 
 * @note Si el arreglo contiene varios elementos a la misma distancia de `value`,
 *       la función retorna el índice del primer elemento encontrado.
 * 
 * @example
 * @code
 * double array[] = {1.2, 3.4, 5.6, 7.8};
 * int closest_index = find_closest_index(array, 4, 4.0);
 * // closest_index será 1, ya que 3.4 es el valor más cercano a 4.0 en el arreglo.
 * @endcode
 */
int find_closest_index(double* array, int length, double value);

#endif // FIND_CLOSEST_INDEX_H
