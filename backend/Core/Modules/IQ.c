#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <unistd.h>
#include "IQ.h"

int load_bands(double* frequencies, double* bandwidths, env_path_t *paths)
{

    char temp_buffer[MAX_BAND_SIZE];
    char *token;
    int num_rows = 0;
    int i = 0;
    //const char *file_band = NULL;

    char csv_relative_path[256] = "/VHF1.csv";
    char file_band[256];
    snprintf(file_band, sizeof(paths->core_bands_path), "%s%s", paths->core_bands_path, csv_relative_path);


    FILE *file = fopen(file_band, "r");
    if (!file) {
        printf("Error: Unable to open file %s\n", file_band);
        return 0;
    }

    while (fgets(temp_buffer, MAX_BAND_SIZE, file) != NULL) {
        num_rows++;
    }

    fseek(file, 0, SEEK_SET);
    fgets(temp_buffer, MAX_BAND_SIZE, file);

    while (fgets(temp_buffer, MAX_BAND_SIZE, file) != NULL) {
        token = strtok(temp_buffer, ",");
        frequencies[i] = atof(token);
        token = strtok(NULL, "\n");
        bandwidths[i] = atof(token);
        i++;
    }

    fclose(file);
    return num_rows;
}