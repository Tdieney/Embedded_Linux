#ifndef LOG_PROCESS_H
#define LOG_PROCESS_H

#include <stdint.h>

/**
 * @brief Continuously read log messages from a FIFO and write them to a log file.
 *
 * @param fifo_name Name of the FIFO to read log messages from.
 * @return EXIT_SUCCESS on success, or EXIT_FAILURE if an error occurs.
 */
uint8_t log_process_run(const char *fifo_name);

#endif // LOG_PROCESS_H
