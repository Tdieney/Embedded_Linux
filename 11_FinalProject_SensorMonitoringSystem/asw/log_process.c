#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "log_process.h"
#include "common.h"

#define NEWLINE_CHAR    "\n"
#define BUFFER_SIZE     256U
#define LOG_BUFFER_SIZE (256U + 29U)

uint8_t log_process_run(const char *fifo_name) {
    char buffer[BUFFER_SIZE];
    char log_buffer[LOG_BUFFER_SIZE];
    int seq_num = 1;

    int fifo_fd = open(fifo_name, O_RDONLY);
    if (fifo_fd < 0) {
        printf("[ERROR] - Failed to open FIFO\n");
        return EXIT_FAILURE;
    }

    int log_fd = open("gateway.log", O_RDWR | O_CREAT | O_APPEND);
    if (log_fd < 0) {
        printf("[ERROR] - Failed to open log file\n");
        close(fifo_fd);
        return EXIT_FAILURE;
    }

    while (read(fifo_fd, buffer, sizeof(buffer)) > 0) {
        time_t now = time(NULL);
        char *time_str = get_timestamp_string(now);
        
        sprintf(log_buffer, "%s - Seq #%d - Received buffer: %s", time_str, seq_num++, buffer);
        printf("%s\n", log_buffer);
        write(log_fd, log_buffer, sizeof(log_buffer) - ONE);
        write(log_fd, NEWLINE_CHAR, sizeof(NEWLINE_CHAR));

        free(time_str);
    }

    close(log_fd);
    close(fifo_fd);

    return EXIT_SUCCESS;
}
