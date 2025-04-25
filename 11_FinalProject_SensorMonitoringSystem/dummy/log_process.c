#include "log_process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "common.h"

void log_process_run(const char *fifo_name) {
    int fd = open(fifo_name, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open FIFO");
        exit(1);
    }

    FILE *log_file = fopen("gateway.log", "a");
    if (!log_file) {
        perror("Failed to open log file");
        close(fd);
        exit(1);
    }

    char buffer[256];
    int seq_num = 1;

    while (read(fd, buffer, sizeof(buffer)) > 0) {
        time_t now = time(NULL);
        char *time_str = get_timestamp_string(now);
        
        fprintf(log_file, "%s - Seq #%d - Received buffer: %s\n", time_str, seq_num++, buffer);
        
        free(time_str);

        fflush(log_file);
    }

    fclose(log_file);
    close(fd);
}
