#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <time.h>

#define ERROR           -1
#define ONE             1U
#define TWO             2U
#define THREE           3U
#define PERM_RW_ALL     0666
#define DELAY_100MS     100000UL

#define CLEAR_ARRAY(arr, len)         \
    do {                              \
        for (size_t i = 0; i < (len); i++) { \
            (arr)[i] = 0;              \
        }                             \
    } while(0)

/**
 * Converts a timestamp to a human-readable string
 * @param timestamp Unix timestamp
 * @return Pointer to timestamp string (must be freed by caller)
 */
char* get_timestamp_string(time_t timestamp);

#endif // COMMON_H
