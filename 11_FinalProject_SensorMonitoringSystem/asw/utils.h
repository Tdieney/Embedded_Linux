#ifndef UTILS_H
#define UTILS_H

/**
 * @brief Write a log message to a named FIFO with thread safety.
 *
 * @param fifo_name Path to the FIFO (named pipe) to write the log message.
 * @param message The log message to write.
 */
void log_event(const char *fifo_name, const char *message);

#endif // UTILS_H
