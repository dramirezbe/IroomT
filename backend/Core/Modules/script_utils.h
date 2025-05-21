#ifndef PATH_UTILS_H
#define PATH_UTILS_H

#include <limits.h> // For PATH_MAX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// Custom exit codes for path-related errors
typedef enum {
    EXIT_PATH_READ = 2
} error_handler_t;

// Holds application directory paths from the .env file
typedef struct {
    char root_path[PATH_MAX + 1];
    char core_samples_path[PATH_MAX + 1];
    char core_json_path[PATH_MAX + 1];
    char core_bands_path[PATH_MAX + 1];
} env_path_t;

/**
 * @brief Locate and parse the .env file to populate required paths.
 *
 * Searches for “.env” in the executable’s directory and up to two parent levels.
 * Extracts ROOT_PATH, CORE_SAMPLES_PATH, CORE_JSON_PATH, and CORE_BANDS_PATH.
 * On failure (file not found or missing key), prints an error and exits with EXIT_PATH_READ.
 *
 * @param paths Pointer to an env_path_t struct to receive the parsed paths.
 */
void get_paths(env_path_t *paths);

/**
 * @brief Launch the web frontend via “npm start”.
 *
 * Changes the working directory to three levels above the executable’s location,
 * then spawns a child process running “npm start”.
 *
 * @param paths Pointer to an env_path_t struct (uses root_path to locate the web directory).
 * @return 0 on success, non-zero on failure.
 */
int start_web(env_path_t *paths);

/**
 * @brief Stop the web frontend process started by start_web().
 *
 * Sends the appropriate termination signal to the “npm start” process.
 *
 * @return 0 on success, non-zero on failure.
 */
int stop_web(void);

#endif // PATH_UTILS_H
