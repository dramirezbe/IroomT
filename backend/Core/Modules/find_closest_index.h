/**
 * @file find_closest_index.h
 * @brief Declaration of the function to find the index closest to a value in an array.
 *
 * This file contains the declaration of the `find_closest_index` function, which is used to
 * find the index of the element closest to a given value in an array of floating-point numbers.
 */

#ifndef FIND_CLOSEST_INDEX_H
#define FIND_CLOSEST_INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * @brief Finds the index of the element in the array that is closest to a given value.
 *
 * This function iterates through an array of double-precision floating-point numbers (`double`)
 * and calculates the absolute difference between each element and the target value `value`.
 * It returns the index of the element whose difference is the smallest.
 *
 * @param array A pointer to the array of `double` values.
 * @param length The number of elements in the array.
 * @param value The value to which the closest element is to be found.
 *
 * @return The index of the element in the array that is closest to the given value `value`.
 *
 * @note If the array contains multiple elements at the same distance from `value`,
 * the function returns the index of the first element found.
 *
 * @example
 * @code
 * double array[] = {1.2, 3.4, 5.6, 7.8};
 * int closest_index = find_closest_index(array, 4, 4.0);
 * // closest_index will be 1, since 3.4 is the value closest to 4.0 in the array.
 * @endcode
 */
int find_closest_index(double* array, int length, double value);

#endif // FIND_CLOSEST_INDEX_H