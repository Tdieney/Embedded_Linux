#include "storage_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "shm_buffer.h"
#include "utils.h"

static int init_db(sqlite3 *db, const char *fifo_name) {
    const char *sql = "CREATE TABLE IF NOT EXISTS sensors (id INTEGER, temperature REAL, timestamp INTEGER);";
    char *err_msg = NULL;
    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
        log_event(fifo_name, "Failed to create table");
        sqlite3_free(err_msg);
        return -1;
    }
    log_event(fifo_name, "New table sensors created");
    return 0;
}

void *storage_mgr_run(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    sqlite3 *db = NULL;
    sensor_data_t data;
    int retry_count = 0;

    while (1) {
        // Connect to SQLite database
        if (!db) {
            if (sqlite3_open("sensors.db", &db) != SQLITE_OK) {
                log_event(args->fifo_name, "Unable to connect to SQL server");
                if (++retry_count >= 3) {
                    log_event(args->fifo_name, "Connection to SQL server failed after 3 attempts. Shutting down.");
                    exit(1);
                }
                sleep(1);
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
            char sql[128];
            snprintf(sql, sizeof(sql), "INSERT INTO sensors (id, temperature, timestamp) VALUES (%d, %.2f, %ld);",
                     data.sensor_id, data.temperature, data.timestamp);
            char *err_msg = NULL;
            if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK) {
                log_event(args->fifo_name, "Connection to SQL server lost");
                sqlite3_free(err_msg);
                sqlite3_close(db);
                db = NULL;
                continue;
            }
        }
        usleep(100000); // Prevent busy-waiting
    }
    sqlite3_close(db);
    return NULL;
}
