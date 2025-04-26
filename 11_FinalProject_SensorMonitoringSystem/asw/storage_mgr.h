#ifndef STORAGE_MGR_H
#define STORAGE_MGR_H

#include <sqlite3.h>

/**
 * @brief Initialize the database by creating the "sensors" table if it doesn't exist.
 *
 * @param db Pointer to the SQLite database object.
 * @param fifo_name Name of the FIFO to log events.
 * @return 0 on success, or -1 if table creation fails.
 */
static int init_db(sqlite3 *db, const char *fifo_name);

/**
 * @brief Thread function to manage storage by connecting to an SQLite database and inserting sensor data.
 *
 * @param arg Pointer to the thread arguments (thread_args_t).
 * @return NULL.
 */
void *storage_mgr_run(void *arg);

#endif // STORAGE_MGR_H
