#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <unistd.h>

#include "IQ.h"

void delete_CS8(uint8_t file_sample)
{
    char file_path[60];
    memset(file_path, 0, 20);
    sprintf(file_path, "backend/Core/Samples/%d", file_sample);

    if (remove(file_path) != 0) {
        fprintf(stderr, "Error al eliminar el archivo Sample %s.\n", file_path);
    } else {
        printf("Archivo Sample %s eliminado correctamente.\n", file_path);
    }
}

void delete_JSON(uint8_t file_json)
{
    char file_path[60];
    memset(file_path, 0, 20);
    sprintf(file_path, "backend/Core/JSON/%d", file_json);

    if (remove(file_path) != 0) {
        fprintf(stderr, "Error al eliminar el archivo JSON %s.\n", file_path);
    } else {
        printf("Archivo JSON %s eliminado correctamente.\n", file_path);
    }
}

int load_bands(double* frequencies, double* bandwidths)
{

    char temp_buffer[MAX_BAND_SIZE];
    char *token;
    int num_rows = 0;
    int i = 0;
    const char *file_band = NULL;

    file_band = "backend/Core/bands/VHF1.csv";

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