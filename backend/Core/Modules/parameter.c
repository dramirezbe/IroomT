
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <time.h>
#include <unistd.h>
#include "CS8toIQ.h"
#include "IQ.h"
#include "welch.h"
#include "cJSON.h"
#include "moda.h"
#include "find_closest_index.h"

int compare_doubles(const void* a, const void* b) {
    double x = *(const double*)a;
    double y = *(const double*)b;
    if (x < y) return -1;
    else if (x > y) return 1;
    else return 0;
}

double median(double* array, int start, int end) {
    int length = end - start;
    double* temp = (double*)malloc(length * sizeof(double));
    if (temp == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < length; i++) {
        temp[i] = array[start + i];
    }
    qsort(temp, length, sizeof(double), compare_doubles);

    double med;
    if (length % 2 == 0) {
        med = 0.5 * (temp[length / 2 - 1] + temp[length / 2]);
    } else {
        med = temp[length / 2];
    }
    free(temp);
    return med;
}

void parameter(int threshold, double* canalization, double* bandwidth, int canalization_length, uint64_t central_freq)
{
    size_t num_samples;

    char file_sample_str[60];
    sprintf(file_sample_str, "backend/Core/Samples/0"); 

    complex double* vector_IQ = cargar_cs8(file_sample_str, &num_samples);

    printf("Total samples: %lu\r\n", num_samples);

    delete_CS8(0);
    //delete_JSON(0);
       
    double* Pxx = NULL;
    double* f = NULL;
    double* Pxx1 = NULL;
    double* f1 = NULL;
    int nperseg = 32768;
    int presence;
    int N_f=nperseg;

    size_t psd_size = nperseg;
    size_t psd_size1 = 4096;
    Pxx = (double*) malloc(psd_size * sizeof(double));
    f = (double*) malloc(psd_size * sizeof(double));
    Pxx1 = (double*) malloc(psd_size1 * sizeof(double));
    f1 = (double*) malloc(psd_size1 * sizeof(double));
    
    welch_psd_complex(vector_IQ, num_samples, 20000000, nperseg, 0, f, Pxx);
    welch_psd_complex(vector_IQ, num_samples, 20000000, 4096, 0, f1, Pxx1);
    free(vector_IQ);

    if (nperseg % 2 != 0) {
        printf("La longitud del vector debe ser par.\n");
        return;
    }

    // -----------Acomodar potencias de welch-----------
    int half = nperseg / 2;
    int half1 = 2048;
    double* temp = (double*)malloc(nperseg * sizeof(double));
    double* temp1 = (double*)malloc(4096 * sizeof(double));

    for (int i = 0; i < half; i++) {
        temp[i] = Pxx[half + i];        
    }

    for (int i = 0; i < half1; i++) {
        temp1[i] = Pxx1[half1 + i];        
    }

    for (int i = 0; i < half; i++) {
        temp[half + i] = Pxx[i];
    }
    for (int i = 0; i < half1; i++) {
        temp1[half1 + i] = Pxx1[i];
    }

    for (int i = 0; i < nperseg; i++) {
        Pxx[i] = temp[i];
    } 
    for (int i = 0; i < 4096; i++) {
        Pxx1[i] = temp1[i];
    }

    free(temp);
    free(temp1);

    // save_to_file(f, Pxx, nperseg, "data.csv");

    for (int i = 0; i < N_f; i++) {
        f[i] = (f[i] + central_freq) / 1e6;
    }
    for (int i = 0; i < 4096; i++) {
        f1[i] = (f1[i] + central_freq) / 1e6;
    }

    int index = nperseg/2 ;
    int count = nperseg*0.002;
    int index1 = 2048 ;
    int count1 = 4096*0.002;
    int b=index-(count+13);
    int a=index;

    for (int i=0; i<count; i++)
    {   
        b=b-3;
        Pxx[a]=Pxx[b];
        a=a-1;        
    }
    a=index;
    for (int i=0; i<count; i++)
    {   
        Pxx[i]=Pxx[b];
        a=a+1;
        b=b-2;        
    }
    a=index1;
    b=index1-(count1+5);
    for (int i=0; i<count1; i++)
    {   
        b=b-3;
        Pxx1[a]=Pxx1[b];
        a=a-1; 
    }
    a=index1;
    b=index1-(count1+5);
    for (int i=0; i<count; i++)
    {            
        Pxx1[a]=Pxx1[b];
        a=a+1;
        b=b-2;        
    }

    //datos visualizacion
    char tempu[50];

     // ---------------Cálculo de parámetros para cada canal--------------------
    cJSON *json_root = cJSON_CreateObject();

    cJSON_AddStringToObject(json_root, "band", "VHF");
    cJSON_AddStringToObject(json_root, "fmin", "88");
    cJSON_AddStringToObject(json_root, "fmax", "108");
    cJSON_AddStringToObject(json_root, "units", "MHz");
    cJSON_AddStringToObject(json_root, "measure", "RMER");

    cJSON *json_vectors = cJSON_CreateObject();
    double constante=abs((abs(10*log10(Pxx[0])))-abs((10*log10(Pxx1[0]))));

    cJSON *json_Pxx_array = cJSON_CreateArray();
    
    for (int i = 0; i < 4096; i++) {
        memset(tempu, 0, sizeof(tempu));
        sprintf(tempu, "%0.3f", 10*log10(Pxx1[i])+constante); //Hacer algo aca
        cJSON_AddItemToArray(json_Pxx_array, cJSON_CreateNumber(atof(tempu)));
        //cJSON_AddItemToArray(json_Pxx_array, cJSON_CreateNumber(10*log10(constante*Pxx1[i])));
    }
    cJSON_AddItemToObject(json_vectors, "Pxx", json_Pxx_array);

    cJSON *json_f_array = cJSON_CreateArray();
    for (int i = 0; i < 4096; i++) {
        memset(tempu, 0, sizeof(tempu));
        sprintf(tempu, "%0.3f", f1[i]);
        cJSON_AddItemToArray(json_f_array, cJSON_CreateNumber(atof(tempu)));
        //cJSON_AddItemToArray(json_f_array, cJSON_CreateNumber(f1[i]));
    }
    cJSON_AddItemToObject(json_vectors, "f", json_f_array);

    cJSON_AddItemToObject(json_root, "vectors", json_vectors);

    cJSON *json_params_array = cJSON_CreateArray();

    float noise = find_min(Pxx, nperseg);

    for (int idx = 0; idx < canalization_length; idx++) {
        double center_freq = canalization[idx];
        double bw = bandwidth[idx];

        double target_lower_freq = center_freq - bw / 2;
        double target_upper_freq = center_freq + bw / 2;

        int lower_index = find_closest_index(f, N_f, target_lower_freq);
        int upper_index = find_closest_index(f, N_f, target_upper_freq);


        if (lower_index > upper_index) {
            int temp = lower_index;
            lower_index = upper_index;
            upper_index = temp;
        }
        if (lower_index < 0) lower_index = 0;
        if (upper_index >= N_f) upper_index = N_f - 1;

        int range_length = upper_index - lower_index + 1;
        
        double power_max = find_max(Pxx, lower_index, upper_index);

        double power = median(Pxx, lower_index, upper_index);

        double snr = 10.0 * log10(power_max / noise );

        if (10.0 * log10(power_max) > threshold){
            presence = 1;
        } else {
            presence = 0;
        }
    }
  
    cJSON *json_data = cJSON_CreateObject();
    cJSON_AddItemToObject(json_data, "data", json_root);
    
    char *json_string = cJSON_Print(json_data);

    char filename[60];
    memset(filename, 0, 20);
    sprintf(filename, "backend/Core/JSON/%d", 0);

    // Abrir el archivo en modo "r+" (lectura y escritura)
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        printf("Error al abrir el archivo para escritura.\n");
        cJSON_Delete(json_root);
        free(json_string);
        free(f);
        free(Pxx);
        free(f1);
        free(Pxx1);
        free(vector_IQ);
        return;
    }

    // Truncar el archivo para borrar el contenido previo
    if (ftruncate(fileno(file), 0) != 0) {
        printf("Error al truncar el archivo.\n");
        fclose(file);
        cJSON_Delete(json_root);
        free(json_string);
        free(f);
        free(Pxx);
        free(f1);
        free(Pxx1);
        free(vector_IQ);
        return;
    }
    fprintf(file, "%s", json_string);
    fclose(file);
    
    //cJSON_Delete(json_data);
    cJSON_Delete(json_root);
    free(json_string);
    free(f);
    free(Pxx);
    free(f1);
    free(Pxx1);
}
