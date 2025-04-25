#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <time.h>

#define ERROR       -1
#define ONE         1U
#define TWO         2U
#define THREE       3U

/**
 * Converts a timestamp to a human-readable string
 * @param timestamp Unix timestamp
 * @return Pointer to timestamp string (must be freed by caller)
 */
char* get_timestamp_string(time_t timestamp);

#endif // COMMON_H
