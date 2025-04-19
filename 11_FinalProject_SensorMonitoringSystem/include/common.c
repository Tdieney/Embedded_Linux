#include <common.h>

/**
 * Converts a timestamp to a human-readable string
 * @param timestamp Unix timestamp
 * @return Pointer to timestamp string (must be freed by caller)
 */
char* get_timestamp_string(time_t timestamp) {
    char *time_str = malloc(30);
    struct tm *tm_info = localtime(&timestamp);
    
    strftime(time_str, 30, "%Y-%m-%d %H:%M:%S", tm_info);
    return time_str;
}
