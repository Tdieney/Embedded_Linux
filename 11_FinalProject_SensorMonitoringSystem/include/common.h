#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <time.h>

/**
 * Converts a timestamp to a human-readable string
 * @param timestamp Unix timestamp
 * @return Pointer to timestamp string (must be freed by caller)
 */
char* get_timestamp_string(time_t timestamp);

#endif // COMMON_H
