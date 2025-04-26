#include "storage_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include "shm_buffer.h"
#include "utils.h"
#include "common.h"

#define SQL_SIZE 128U

/**
 * @brief Initialize the database by creating the "sensors" table if it doesn't exist.
 *
 * @param db Pointer to the SQLite database object.
 * @param fifo_name Name of the FIFO to log events.
 * @return 0 on success, or -1 if table creation fails.
 */
static int init_db(sqlite3 *db, const char *fifo_name) {
    const char *sql = "CREATE TABLE IF NOT EXISTS sensors (id INTEGER, temperature REAL, timestamp INTEGER);";
    char *err_msg = NULL;
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        log_event(fifo_name, "Failed to create table");
        sqlite3_free(err_msg);
        return ERROR;
    }
    log_event(fifo_name, "New table sensors created");
    return 0;
}

/**
 * @brief Thread function to manage storage by connecting to an SQLite database and inserting sensor data.
 *
 * @param arg Pointer to the thread arguments (thread_args_t).
 * @return NULL.
 */
void *storage_mgr_run(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    sqlite3 *db = NULL;
    sensor_data data;
    int retry_count = 0;

    while (ONE) {
        // Connect to SQLite database
        if (!db) {
            if (sqlite3_open("sensors.db", &db) != SQLITE_OK) {
                log_event(args->fifo_name, "Unable to connect to SQL server");
                if (++retry_count >= 3) {
                    log_event(args->fifo_name, "Connection to SQL server failed after 3 attempts. Shutting down.");
                    exit(EXIT_FAILURE);
                }
                sleep(ONE);
                continue;
            }
            log_event(args->fifo_name, "Connection to SQL server established");
            if (init_db(db, args->fifo_name) != 0) {
                sqlite3_close(db);
                db = NULL;
                continue;
            }
            retry_count = 0;
        }

        // Process data
        if (shm_buffer_remove(args->buffer, &data) == 0) {
            char sql[SQL_SIZE];
            snprintf(sql, sizeof(sql), "INSERT INTO sensors (id, temperature, timestamp) VALUES (%u, %u, %ld);",
                     data.id, data.temperature, data.timestamp);
            char *err_msg = NULL;
            if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
                log_event(args->fifo_name, "Connection to SQL server lost");
                sqlite3_free(err_msg);
                sqlite3_close(db);
                db = NULL;
                continue;
            }
        }
        usleep(DELAY_100MS); // Prevent busy-waiting
    }
    sqlite3_close(db);
    return NULL;
}
