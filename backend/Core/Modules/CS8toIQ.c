
#include <stdio.h>
#include <stdlib.h>
#include "CS8toIQ.h"


complex double* cargar_cs8(const char* filename, size_t* num_samples) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Error: No se pudo abrir el archivo de datos CS8");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size % 2 != 0) {
        fprintf(stderr, "Error: Tamaño del archivo inválido. Debe ser múltiplo de 2.\n");
        fclose(file);
        return NULL;
    }

    *num_samples = file_size / 2;
    int8_t* raw_data = (int8_t*)malloc(file_size);
    complex double* IQ_data = (complex double*)malloc(*num_samples * sizeof(complex double));

    if (!raw_data || !IQ_data) {
        perror("Error: No se pudo reservar memoria");
        free(raw_data);
        free(IQ_data);
        fclose(file);
        return NULL;
    }

    if (fread(raw_data, 1, (size_t)file_size, file) != (size_t)file_size) {
        perror("Error: Lectura incompleta del archivo");
        free(raw_data);
        free(IQ_data);
        fclose(file);
        return NULL;
    }

    for (size_t i = 0; i < *num_samples; i++) {
        IQ_data[i] = raw_data[2 * i] + raw_data[2 * i + 1] * I;
        //IQ_data[i] = CMPLX(raw_data[2 * i], raw_data[2 * i + 1]); 
    }

    free(raw_data);
    fclose(file);

    return IQ_data;
}
