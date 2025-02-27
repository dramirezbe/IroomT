#ifndef HANDLE_DATA_H
#define HANDLE_DATA_H

#include <math.h>

#ifndef DATA_POINTS
#define DATA_POINTS 4096
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Estructura para un punto de datos
typedef struct {
    double x;
    double y;
} DataPoint;

// Función para generar la señal como la suma de dos senos con ruido
void generate_signal(DataPoint *data, double freq1, double freq2);

#endif // HANDLE_DATA_H
