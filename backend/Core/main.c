#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "Modules/IQ.h"
#include "Modules/parameter.h"
#include "Modules/script_utils.h"


#define LOWER_FREQ      88000000    /* 88MHz */ 
#define CENTRAL_FREQ    98000000    /* 98MHz */    
#define UPPER_FREQ      108000000   /* 108MHz */   

#define NPERSEG_LARGE   32768       /* High resolution for large-scale analysis */
#define NPERSEG_SMALL   4096        /* Low resolution for small-scale analysis */
#define THRESHOLD       -30         /* dB */         

#define MAX_FILE_NUM    10  


int main(void) {
    // Declare a structure to hold the environment paths.
    env_path_t paths;

    // Parse the .env file to populate the paths structure.
    // If any required key is missing, get_paths() will print an error and exit.
    get_paths(&paths);

    printf("PATH: %s\n\r", paths.root_path);
    printf("PATH: %s\n\r", paths.core_samples_path);
    printf("PATH: %s\n\r", paths.core_json_path);


    strcat(paths.core_json_path, "/0");


    // Start web service for visualization interface
    if (start_web(&paths) != 0) {
        fprintf(stderr, "Error initializing Web Service\n");
        exit(EXIT_FAILURE);
    }

    // Load channel frequency bands from configuration
    int canalization_length;
    double canalization[250];
    double bandwidth[250];
    canalization_length = load_bands(canalization, bandwidth, &paths);
    //printf("Canalization length: %d\r\n", canalization_length);

    // Initialize signal processing configuration
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
    
    // Main processing loop - continuously monitors and processes files
    while (1) {
        for (int file_num = 0; file_num <= MAX_FILE_NUM; file_num++) {
            // Construct path to current input file
            snprintf(input_file_path, sizeof(input_file_path), "%s%d", paths.core_samples_path, file_num);
            
            config.input_file_path = input_file_path;
            
            printf("[Processing] File: %s\n", input_file_path);
            
            // Process the current file
            int result = process_signal_spectrum(&config);
            
            if (result != SP_SUCCESS) {
                fprintf(stderr, "[Processing] Error in: %d: %s\n", 
                        file_num, get_signal_processor_error(result));
                exit(EXIT_FAILURE); // Continue with next file on error
            }
            
            printf("[Processing] File:%d processed\n", file_num);
        }
    }


    printf("Stopping web service...\n");
    // Stop the web service started earlier.
    if (stop_web() != 0) {
        fprintf(stderr, "Failed to stop the web process.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
