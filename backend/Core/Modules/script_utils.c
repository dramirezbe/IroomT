#include "script_utils.h"  // Function declarations and shared types

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

#define ENV_LINE_BUFFER_SIZE (PATH_MAX * 2)

/* Static PID of the child process running “npm start” */
static pid_t npm_pid = -1;

/**
 * @brief Read the full path of the current executable.
 *
 * Uses the Linux-specific /proc/self/exe link.
 * Exits with EXIT_PATH_READ if the path cannot be read or is too long.
 *
 * @param buffer Destination buffer for the executable path.
 * @param size   Size of the destination buffer.
 */
static void get_executable_path_internal(char *buffer, size_t size) {
    ssize_t len = readlink("/proc/self/exe", buffer, size - 1);
    if (len < 0 || (size_t)len >= size - 1) {
        perror("get_executable_path_internal: readlink failed");
        exit(EXIT_PATH_READ);
    }
    buffer[len] = '\0';
}

/**
 * @brief Resolve a filesystem path to its canonical form.
 *
 * Wraps realpath(); returns 0 on success or -1 on failure (errno set).
 *
 * @param input    Input path to resolve.
 * @param resolved Destination buffer for the real path.
 * @param size     Size of the destination buffer.
 * @return 0 on success, -1 on error.
 */
static int resolve_path_internal(const char *input, char *resolved, size_t size) {
    if (!realpath(input, resolved)) {
        return -1;
    }
    return 0;
}

/**
 * @brief Parse a single key=value pair from an open .env file.
 *
 * Scans from the start of the file for a line beginning with `key=`,
 * then extracts the value up to newline, carriage return, or comment.
 *
 * @param fp  Open FILE pointer to a .env file (rewound internally).
 * @param key Name of the environment variable to find.
 * @param out Output buffer to receive the value.
 * @param len Size of the output buffer.
 * @return 0 if the key was found and copied, or -1 on failure.
 */
static int parse_env_key_internal(FILE *fp, const char *key, char *out, size_t len) {
    char line[ENV_LINE_BUFFER_SIZE];
    size_t key_len = strlen(key);
    rewind(fp);

    while (fgets(line, sizeof(line), fp)) {
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (strncmp(p, key, key_len) == 0 && p[key_len] == '=') {
            char *val = p + key_len + 1;
            size_t vlen = strcspn(val, "\r\n#");
            while (vlen > 0 && (val[vlen-1] == ' ' || val[vlen-1] == '\t')) vlen--;
            if (vlen >= len) vlen = len - 1;
            memcpy(out, val, vlen);
            out[vlen] = '\0';
            return 0;
        }
    }
    return -1;
}

/**
 * @brief Locate, open, and parse the .env file to populate env_path_t.
 *
 * Searches in the executable’s directory and up to two parent levels.
 * Exits with EXIT_PATH_READ on failure (not found or missing keys).
 *
 * @param paths Pointer to an env_path_t structure to fill.
 */
void get_paths(env_path_t *paths) {
    char exe_path[PATH_MAX + 1], exe_dir[PATH_MAX + 1];
    char up1[PATH_MAX + 1] = "", up2[PATH_MAX + 1] = "";
    char env_file[PATH_MAX + 1];
    FILE *fp = NULL;

    get_executable_path_internal(exe_path, sizeof(exe_path));

    // Derive executable directory
    strncpy(exe_dir, exe_path, sizeof(exe_dir) - 1);
    exe_dir[sizeof(exe_dir)-1] = '\0';
    char *slash = strrchr(exe_dir, '/');
    if (slash) *slash = '\0';
    else strncpy(exe_dir, ".", sizeof(exe_dir));

    // Resolve the executable directory
    if (resolve_path_internal(exe_dir, exe_dir, sizeof(exe_dir)) < 0) {
        fprintf(stderr, "get_paths: Cannot resolve '%s': %s\n", exe_dir, strerror(errno));
        exit(EXIT_PATH_READ);
    }

    // Compute one and two levels up
    {
        char tmp[PATH_MAX + 1];
        snprintf(tmp, sizeof(tmp), "%s/..", exe_dir);
        if (resolve_path_internal(tmp, up1, sizeof(up1)) < 0) up1[0] = '\0';

        if (up1[0]) {
            snprintf(tmp, sizeof(tmp), "%s/..", up1);
            if (resolve_path_internal(tmp, up2, sizeof(up2)) < 0) up2[0] = '\0';
        }
    }

    // Search for .env in exe_dir, up1, up2
    const char *candidates[] = { exe_dir, up1, up2 };
    for (int i = 0; i < 3; i++) {
        if (!candidates[i][0]) continue;
        snprintf(env_file, sizeof(env_file), "%s/.env", candidates[i]);
        fp = fopen(env_file, "r");
        if (fp) break;
    }
    if (!fp) {
        fprintf(stderr, "get_paths: .env not found in '%s', '%s', or '%s'\n",
                exe_dir, up1[0]? up1 : "(n/a)", up2[0]? up2 : "(n/a)");
        exit(EXIT_PATH_READ);
    }
    printf("get_paths: Found .env at %s\n", env_file);

    // Parse required keys
    if (parse_env_key_internal(fp, "ROOT_PATH",         paths->root_path,         sizeof(paths->root_path)) ||
        parse_env_key_internal(fp, "CORE_SAMPLES_PATH", paths->core_samples_path, sizeof(paths->core_samples_path)) ||
        parse_env_key_internal(fp, "CORE_JSON_PATH",    paths->core_json_path,    sizeof(paths->core_json_path)) ||
        parse_env_key_internal(fp, "CORE_BANDS_PATH",   paths->core_bands_path,   sizeof(paths->core_bands_path)))
    {
        fprintf(stderr, "get_paths: Missing required key in %s\n", env_file);
        fclose(fp);
        exit(EXIT_PATH_READ);
    }
    fclose(fp);
}

/**
 * @brief Start the web frontend by running “npm start” in a child process.
 *
 * Changes to paths->root_path and forks a child that execs “npm start”.
 *
 * @param paths Pointer to env_path_t containing root_path.
 * @return 0 on success, non-zero on error.
 */
int start_web(env_path_t *paths) {
    if (npm_pid > 0) {
        fprintf(stderr, "start_web: already running (PID %d)\n", npm_pid);
        return -1;
    }
    if (chdir(paths->root_path) != 0) {
        fprintf(stderr, "start_web: chdir to '%s' failed: %s\n", paths->root_path, strerror(errno));
        return 1;
    }
    npm_pid = fork();
    if (npm_pid < 0) {
        perror("start_web: fork failed");
        return 1;
    }
    if (npm_pid == 0) {
        execlp("npm", "npm", "start", (char *)NULL);
        perror("start_web: execlp failed");
        exit(1);
    }
    printf("start_web: Web started (PID %d)\n", npm_pid);
    return 0;
}

/**
 * @brief Stop the web frontend process started by start_web().
 *
 * Sends SIGTERM, waits for the child to exit, and resets npm_pid.
 *
 * @return 0 on success, non-zero on error.
 */
int stop_web(void) {
    if (npm_pid <= 0) {
        fprintf(stderr, "stop_web: no running web process (PID=%d)\n", npm_pid);
        return -1;
    }
    if (kill(npm_pid, SIGTERM) == -1) {
        perror("stop_web: SIGTERM failed");
    } else {
        printf("stop_web: Sent SIGTERM to PID %d\n", npm_pid);
    }
    int status;
    if (waitpid(npm_pid, &status, 0) == npm_pid) {
        printf("stop_web: Web process %d exited\n", npm_pid);
    } else {
        perror("stop_web: waitpid failed");
    }
    npm_pid = -1;
    return 0;
}
