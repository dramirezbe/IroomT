#ifndef PATH_UTILS_H
#define PATH_UTILS_H

#include <limits.h> // Required for PATH_MAX

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// Custom exit codes used by the library functions
typedef enum {
    EXIT_PATH_READ = 2
} error_handler_t;

// Structure to hold environment paths
// Contains only the paths requested: ROOT_PATH, CORE_SAMPLES_PATH, CORE_JSON_PATH
typedef struct {
    char root_path[PATH_MAX+1];
    char core_samples_path[PATH_MAX+1];
    char core_json_path[PATH_MAX+1];
} env_path_t;

/**
 * @brief Finds the .env file in the executable's directory or up to two levels above,
 * parses the required paths (ROOT_PATH, CORE_SAMPLES_PATH, CORE_JSON_PATH),
 * and populates the provided env_path_t structure.
 *
 * If the .env file is not found or any of the required keys are missing,
 * the function prints an error message to stderr and exits the program
 * with the status code EXIT_PATH_READ.
 *
 * @param paths A pointer to an env_path_t structure to be populated with the paths.
 */
void get_paths(env_path_t *paths);

// Inicia el entorno web: cambia de directorio subiendo tres niveles relativos al
// directorio donde se encuentra el ejecutable e inicia "npm start" en un proceso hijo.
// Retorna 0 en caso de éxito o un valor distinto de cero en caso de error.
int start_web(env_path_t *paths);

// Detiene el proceso que ejecuta "npm start" iniciado por start_web().
// Retorna 0 en caso de éxito o un valor distinto de cero en caso de error.
int stop_web(void);

#endif // PATH_UTILS_H
