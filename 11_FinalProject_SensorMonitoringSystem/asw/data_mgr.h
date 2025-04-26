#ifndef DATA_MGR_H
#define DATA_MGR_H

/**
 * @brief Thread function to process sensor data from shared memory,
 * log temperature status, and maintain running averages.
 *
 * @param arg Pointer to thread_args_t containing FIFO name and shared memory buffer.
 * @return NULL
 */
void *data_mgr_run(void *arg);

#endif // DATA_MGR_H
