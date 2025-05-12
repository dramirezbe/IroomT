#include "script_utils.h" // Include our own header file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h> // Required for dirname in some implementations (though manual parsing is used)
#include <limits.h>
// #include <string.h> // Duplicate include removed
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>   // Required for strerror and errno
#include <signal.h>  // Required for kill

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// Define a reasonable buffer size for reading lines in .env file
#define ENV_LINE_BUFFER_SIZE (PATH_MAX * 2)

// --- Helper function to get executable path (internal to this .c file) ---
// NOTE: This implementation is Linux-specific using /proc/self/exe
static void get_executable_path_internal(char *buffer, size_t size) {
    ssize_t len = readlink("/proc/self/exe", buffer, size - 1);
    if (len < 0 || (size_t)len >= size - 1) {
        perror("readlink");
        // Use the exit code defined in the header
        exit(EXIT_PATH_READ);
    }
    buffer[len] = '\0';
}

// --- Helper function to resolve a path (internal to this .c file) ---
static int resolve_path_internal(const char *input, char *resolved, size_t size) {
    if (!realpath(input, resolved)) {
        // realpath fails if the path doesn't exist or other errors
        // errno is set by realpath
        return -1;
    }
    return 0;
}

// --- Helper function to parse .env for a specified key (internal to this .c file) ---
// Assumes 'out' buffer has a size of 'len'
static int parse_env_key_internal(FILE *fp, const char *key, char *out, size_t len) {
    char line[ENV_LINE_BUFFER_SIZE]; // Buffer for reading lines
    size_t key_len = strlen(key);

    if (len == 0) return -1; // Cannot write to a buffer of size 0

    rewind(fp); // Start reading from the beginning of the file

    while (fgets(line, sizeof(line), fp)) {
        // Remove leading whitespace before checking key
        char *trimmed_line = line;
        while(*trimmed_line == ' ' || *trimmed_line == '\t') {
            trimmed_line++;
        }

        // Check if the line starts with the key followed by '='
        if (strncmp(trimmed_line, key, key_len) == 0 && trimmed_line[key_len] == '=') {
            char *val = trimmed_line + key_len + 1; // Pointer to the value part
            // Find the length of the value before newline, carriage return, or comment '#'
            size_t vlen = strcspn(val, "\r\n#");

            // Handle potential trailing whitespace in the value
            while (vlen > 0 && (val[vlen-1] == ' ' || val[vlen-1] == '\t')) {
                 vlen--;
            }

            // Ensure the value length does not exceed the output buffer size minus null terminator
            if (vlen >= len) {
                 vlen = len - 1; // Truncate value to fit buffer
                 // Optionally, add a warning here about truncation
                 // fprintf(stderr, "Warning: Value for key '%s' in .env truncated.\n", key);
            }

            strncpy(out, val, vlen);
            out[vlen] = '\0'; // Null-terminate the string
            return 0; // Key found and parsed successfully
        }
    }
    return -1; // Key not found
}


// --- get_paths implementation (exposed via the header) ---
// Assumes env_path_t contains fixed-size char arrays of sufficient size
void get_paths(env_path_t *paths) {
    char exe_path[PATH_MAX+1];
    char exe_dir_raw[PATH_MAX+1]; // Raw directory path before resolving
    char dir[PATH_MAX+1];         // Resolved directory of the executable
    char up1[PATH_MAX+1];        // One level up from the executable's directory (resolved)
    char up2[PATH_MAX+1];        // Two levels up from the executable's directory (resolved)
    char env_file[PATH_MAX+1];    // Path to the found .env file
    FILE *fp = NULL;

    // 1. Get the path of the executable using the internal helper
    get_executable_path_internal(exe_path, sizeof(exe_path));

    // --- Logic to find executable directory ---
    // Need a copy as dirname might modify the string (though we do it manually here)
    // Using strncpy with size-1 and manual null termination is safer
    strncpy(exe_dir_raw, exe_path, sizeof(exe_dir_raw) - 1);
    exe_dir_raw[sizeof(exe_dir_raw)-1]='\0'; // Ensure null termination

    char *slash = strrchr(exe_dir_raw, '/');
    if (slash) {
        *slash = '\0'; // Null-terminate the string at the last slash to get the directory
    } else {
         // If no slash, the executable is in the current directory relative to the process
         strncpy(exe_dir_raw, ".", sizeof(exe_dir_raw) - 1); // Assume current directory
         exe_dir_raw[sizeof(exe_dir_raw)-1]='\0';
    }

    // 2. Resolve executable directory using the internal helper
    if (resolve_path_internal(exe_dir_raw, dir, sizeof(dir)) < 0) {
        // Use fprintf for more detailed error reporting including the specific path
        fprintf(stderr, "Error resolving executable directory '%s': %s\n", exe_dir_raw, strerror(errno));
        exit(EXIT_PATH_READ); // Exit if we can't resolve the executable's directory
    }

    // 3. Compute one and two levels up using the internal helper
    char temp_up_input[PATH_MAX+1]; // Use a single temp buffer

    // One level up
    // Start from the resolved directory
    snprintf(temp_up_input, sizeof(temp_up_input), "%s/..", dir);
    if (resolve_path_internal(temp_up_input, up1, sizeof(up1)) < 0) {
        // If realpath fails (e.g., already at root), clear the buffer
        // Note: If dir was "/", resolving "/.." correctly yields "/"
        // This failure might indicate permission issues or other errors, but clearing is chosen here.
        // A more robust approach might check if dir was "/" before attempting "..".
        up1[0] = '\0';
    }

    // Two levels up
    // Start from the result of one level up (up1)
    if (up1[0] != '\0') { // Only attempt if one level up was successful/not root
        snprintf(temp_up_input, sizeof(temp_up_input), "%s/..", up1);
        if (resolve_path_internal(temp_up_input, up2, sizeof(up2)) < 0) {
             // If realpath fails, clear the buffer
            up2[0] = '\0';
        }
    } else {
        up2[0] = '\0'; // Cannot go two levels up if one level up was root or failed
    }


    // 4. Try finding and opening the .env file
    const char *locations[] = { dir, up1, up2 };
    int found_location_index = -1;

    for (int i = 0; i < 3; i++) {
        if (locations[i][0] == '\0') continue; // Skip if path is empty

        // Construct the full path to the .env file
        if (snprintf(env_file, sizeof(env_file), "%s/.env", locations[i]) >= sizeof(env_file)) {
             // Path construction failed (buffer too small)
             fprintf(stderr, "Warning: Path to .env buffer too small for location '%s'\n", locations[i]);
             continue; // Try the next location
        }

        fp = fopen(env_file, "r");
        if (fp) {
            found_location_index = i;
            break; // Found the .env file
        }
    }

    if (!fp) {
        fprintf(stderr, "Error: Could not find .env in any of the checked locations:\n");
        fprintf(stderr, " - Executable dir: '%s'%s\n", dir, (locations[0][0] == '\0' ? " (Resolution Failed)" : ""));
        fprintf(stderr, " - One level up:   '%s'%s\n", up1[0] != '\0' ? up1 : "(N/A)", (up1[0] == '\0' && locations[1][0] != '\0' ? " (Resolution Failed)" : ""));
        fprintf(stderr, " - Two levels up:  '%s'%s\n", up2[0] != '\0' ? up2 : "(N/A)", (up2[0] == '\0' && locations[2][0] != '\0' ? " (Resolution Failed)" : ""));
        exit(EXIT_PATH_READ);
    }

    // Print which file was found for debugging/information
    printf("Found .env file at: %s\n", env_file);


    // 5. Parse the required keys using the internal helper function
    // NOTE: Assumes paths->members are char arrays and sizeof() works as expected.
    if (parse_env_key_internal(fp, "ROOT_PATH", paths->root_path, sizeof(paths->root_path)) < 0) {
         fprintf(stderr, "Error: Missing required key 'ROOT_PATH' in %s\n", env_file);
         fclose(fp);
         exit(EXIT_PATH_READ);
    }
     if (parse_env_key_internal(fp, "CORE_SAMPLES_PATH", paths->core_samples_path, sizeof(paths->core_samples_path)) < 0) {
         fprintf(stderr, "Error: Missing required key 'CORE_SAMPLES_PATH' in %s\n", env_file);
         fclose(fp);
         exit(EXIT_PATH_READ);
    }
     if (parse_env_key_internal(fp, "CORE_JSON_PATH", paths->core_json_path, sizeof(paths->core_json_path)) < 0) {
         fprintf(stderr, "Error: Missing required key 'CORE_JSON_PATH' in %s\n", env_file);
         fclose(fp);
         exit(EXIT_PATH_READ);
    }

    // You might want to add parsing for optional keys here

    fclose(fp); // Close the file after successfully parsing

    // Optional: Add validation that the parsed paths exist and are accessible
    // e.g., check if ROOT_PATH is a valid directory
}

/* Variable estática para almacenar el PID del proceso que ejecuta npm start. */
static pid_t npm_pid = -1;

int start_web(env_path_t *paths) {
    /* Verificar si ya se inició npm start. */
    // Check if npm_pid is in a state indicating a running process
    if (npm_pid > 0) {
        fprintf(stderr, "start_web() Web already running with PID %d\n", npm_pid);
        return -1; // Indicate already running
    }

    /* Cambiar al directorio raíz especificado */
    // Use paths->root_path directly, assuming it's a valid null-terminated path
    if (chdir(paths->root_path) != 0) {
        // Corrected error reporting using fprintf and strerror
        fprintf(stderr, "start_web() Error changing directory to '%s': %s\n", paths->root_path, strerror(errno));
        return 1; // Indicate error
    }

    /* Crear un proceso hijo que ejecute "npm start". */
    npm_pid = fork();
    if (npm_pid < 0) {
        perror("start_web() fork error"); // Correct use of perror for fork errors
        return 1; // Indicate error
    } else if (npm_pid == 0) {
        /* Proceso hijo: ejecutar "npm start" */
        // execlp searches the PATH for the executable "npm"
        execlp("npm", "npm", "start", (char *)NULL);

        // If execlp returns, an error occurred
        perror("start_web() Error executing npm start");
        exit(1); // Child process exits with error status
    } else {
        /* Proceso padre: se almacena el PID del proceso hijo. */
        printf("start_web() Web started with PID: %d\n", npm_pid);
        return 0; // Indicate success
    }
}

int stop_web(void) {
    // Check if the web process is considered running
    if (npm_pid <= 0) { // Check for initial state (-1) or potential failed fork (0, though child exits)
        fprintf(stderr, "stop_web() Web process is not running (PID: %d).\n", npm_pid);
        return -1; // Indicate not running
    }

    /* Attempt graceful shutdown by sending SIGTERM */
    // SIGTERM asks the process to terminate gracefully
    if (kill(npm_pid, SIGTERM) == -1) {
        perror("stop_web() Error sending SIGTERM");
        // Depending on requirements, you might try SIGKILL here if SIGTERM fails,
        // but SIGTERM failure often means the process is already gone or something is very wrong.
        // Continuing to waitpid might still work if the process is exiting.
    } else {
         printf("stop_web() Sent SIGTERM to process with PID %d.\n", npm_pid);
    }


    /* Wait for the process to terminate */
    // NOTE: waitpid(..., 0) will block indefinitely until the child exits.
    // If the child process doesn't handle SIGTERM and exit, this function will hang.
    // For production systems, consider a timeout using waitpid with WNOHANG
    // in a loop, and eventually sending SIGKILL if the timeout expires.
    int status;
    pid_t terminated_pid = waitpid(npm_pid, &status, 0);

    if (terminated_pid == npm_pid) {
       printf("stop_web() Web process with PID %d terminated.\n", npm_pid);
    } else if (terminated_pid == -1) {
        perror("stop_web() Error waiting for process");
    } else {
        // This case shouldn't typically happen with waitpid(npm_pid, ...) unless
        // a signal interrupted the wait.
         fprintf(stderr, "stop_web() waitpid returned unexpected PID %d for target %d.\n", terminated_pid, npm_pid);
    }


    npm_pid = -1; // Reset the PID state
    return 0; // Indicate success (process waited for)
}