#include <stdlib.h>
#include "handle_data.h"

// Genera la señal como la suma de dos senos con ruido
void generate_signal(DataPoint *data, double freq1, double freq2) {
    const double sample_rate = 4096.0; // 4096 muestras por segundo
    for (int i = 0; i < DATA_POINTS; i++) {
        double t = (double)i / sample_rate;
        double sine1 = sin(2.0 * M_PI * freq1 * t);
        double sine2 = sin(2.0 * M_PI * freq2 * t);
        // Ruido aleatorio: valor entre -0.1 y 0.1
        double noise = (((double)rand() / RAND_MAX) - 0.5) * 0.2;
        data[i].x = t;
        data[i].y = sine1 + sine2 + noise;
    }
}
