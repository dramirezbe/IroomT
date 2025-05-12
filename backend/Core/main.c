#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "Drivers/bacn_RF.h"
#include "Modules/IQ.h"
#include "Modules/parameter.h"
#include "script_utils.h"  // Ensure this matches your header file name

#define LOFreq 88000000
#define CeFreq 98000000
#define HIFreq 108000000

/* Variable global para controlar la ejecución del bucle principal */
volatile sig_atomic_t running = 1;

int main(void) {
    // Declare a structure to hold the environment paths.
    env_path_t paths;

    // Parse the .env file to populate the paths structure.
    // If any required key is missing, get_paths() will print an error and exit.
    get_paths(&paths);

    // Start the web environment.
    if (start_web(&paths) != 0) {
        fprintf(stderr, "Failed to start the web process.\n");
        exit(EXIT_FAILURE);
    }

    int totalSamples;
    int bands_length;
    double canalisation[250];
    double bandwidth[250];

    while (running) {
        bands_length = load_bands(canalisation, bandwidth);
        printf("Bands length: %d\r\n", bands_length);
        totalSamples = getSamples(LOFreq, HIFreq);
        printf("Total files: %d\r\n", totalSamples);
        parameter(-30, canalisation, bandwidth, bands_length, CeFreq);
        //sleep(0.5);
    }


    printf("Deteniendo servicio web...\n");
    // Stop the web service started earlier.
    if (stop_web() != 0) {
        fprintf(stderr, "Failed to stop the web process.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
