#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "Drivers/bacn_RF.h"
#include "Modules/IQ.h"
#include "Modules/parameter.h"
#include "Modules/handle_web.h"

#define LOFreq 88000000
#define CeFreq 98000000
#define HIFreq 108000000

/* Variable global para controlar la ejecución del bucle principal */
volatile sig_atomic_t running = 1;


int main(void) {

    /* Iniciar el servicio web */
    if (start_web() != 0) {
        fprintf(stderr, "Error al iniciar el servicio web.\n");
        exit(EXIT_FAILURE);
    }
    
    int totalSamples;
    int bands_length;
    double canalisation[250];
    double bandwidth[250];
    
    /* Bucle principal que mantiene el programa en ejecución */
    while (running) {
        bands_length = load_bands(canalisation, bandwidth);
        printf("Bands length: %d\r\n", bands_length);
        totalSamples = getSamples(LOFreq, HIFreq);
        printf("Total files: %d\r\n", totalSamples);
        parameter(-30, canalisation, bandwidth, bands_length, CeFreq);
        //sleep(0.5);
    }
    
    printf("Deteniendo servicio web...\n");
    if (stop_web() != 0) {
        fprintf(stderr, "Error al detener el servicio web.\n");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}
