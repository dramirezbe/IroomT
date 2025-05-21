/**
 * @file main.c
 * @brief Implementation of RF spectrum analyzer using HackRF
 *
 * This application performs real-time RF spectrum analysis in the VHF band,
 * specifically monitoring the 88-108 MHz range. It uses HackRF One for signal
 * acquisition and implements signal processing algorithms for spectral analysis.
 * 
 * Features:
 * - Real-time signal acquisition using HackRF
 * - Welch's method for power spectral density estimation
 * - Signal detection with configurable threshold
 * - JSON output for web interface visualization
 * - Support for both real-time and test modes
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "Drivers/bacn_RF.h"
#include "Modules/IQ.h"
#include "Modules/parameter.h"
#include "Modules/script_utils.h"

/* Define frequency ranges for VHF band scanning */
#define LOWER_FREQ      88000000    /* Lower bound: 88MHz */
#define CENTRAL_FREQ    98000000    /* Center frequency: 98MHz */
#define UPPER_FREQ      108000000   /* Upper bound: 108MHz */

/* Spectral analysis configuration */
#define NPERSEG_LARGE   32768       /* High resolution for large-scale analysis */
#define NPERSEG_SMALL   4096        /* Low resolution for small-scale analysis */
#define THRESHOLD       -30         /* Signal detection threshold in dB */

/* Testing configuration */
#define TESTING_SAMPLES 10          /* Number of samples in TestingSamples directory */

/* Global state variables */
volatile sig_atomic_t running = 1;   /* Controls main processing loop */
bool testmode = false;               /* Enables test mode using pre-recorded samples */

int main(void) {
    /* Initialize environment paths */
    env_path_t paths;
    get_paths(&paths);

    /* Log path configuration */
    printf("PATH: %s\n\r", paths.root_path);
    printf("PATH: %s\n\r", paths.core_samples_path);
    printf("PATH: %s\n\r", paths.core_json_path);
    strcat(paths.core_json_path, "/0");

    /* Initialize web interface */
    if (start_web(&paths) != 0) {
        fprintf(stderr, "[main] Error initializing Web Service\n");
        exit(EXIT_FAILURE);
    }

    /* Initialize frequency band configuration */
    int canalization_length;
    double canalization[250];    /* Array for channel center frequencies */
    double bandwidth[250];       /* Array for channel bandwidths */
    canalization_length = load_bands(canalization, bandwidth, &paths);

    /* Configure signal processing parameters */
    SignalProcessorConfig config;
    memset(&config, 0, sizeof(config));
    config.output_json_path = paths.core_json_path;
    config.central_freq = CENTRAL_FREQ;
    config.nperseg_large = NPERSEG_LARGE;
    config.nperseg_small = NPERSEG_SMALL;
    config.threshold = THRESHOLD;
    config.verbose_output = true;
    config.use_mmap = true;
    config.canalization = canalization;
    config.bandwidth = bandwidth;
    config.canalization_length = canalization_length;

    char input_file_path[256];

    if (testmode) {
        /* Test mode: Process pre-recorded samples */
        while (1) {
            for (int file_num = 0; file_num <= TESTING_SAMPLES; file_num++) {
                /* Process each test file */
                snprintf(input_file_path, sizeof(input_file_path), 
                        "%sTestingSamples/%d", paths.core_samples_path, file_num);
                config.input_file_path = input_file_path;
                
                printf("[main] File: %s\n", input_file_path);
                int result = process_signal_spectrum(&config);
                
                if (result != SP_SUCCESS) {
                    fprintf(stderr, "[main] Error: %d: %s\n", 
                            file_num, get_signal_processor_error(result));
                    exit(EXIT_FAILURE);
                }
                
                printf("[main] SUCCESS: %d processed\n", file_num);
            }
        }
    } else {
        /* Real-time mode: Process live HackRF samples */
        int CS8Samples;
        snprintf(input_file_path, sizeof(input_file_path), 
                "%s%d", paths.core_samples_path, 0);
        config.input_file_path = input_file_path;
        
        while (running) {
            /* Acquire samples from HackRF */
            printf("[main] Getting CS8 samples...\n");
            CS8Samples = getSamples(LOWER_FREQ, UPPER_FREQ);
            printf("[main] errno: %d\n", CS8Samples);
            
            /* Process acquired samples */
            printf("[main] File: %s\n", input_file_path);
            int result = process_signal_spectrum(&config);
            
            if (result != SP_SUCCESS) {
                fprintf(stderr, "[main] ERROR: %s\n", 
                        get_signal_processor_error(result));
                exit(EXIT_FAILURE);
            }
            
            printf("[main] SUCCESS: %s processed\n", input_file_path);
        }
    }

    /* Cleanup and shutdown */
    printf("[main] Stopping web service...\n");
    if (stop_web() != 0) {
        fprintf(stderr, "[main] Failed to stop the web process.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
