# Sensor Monitoring System

## Project Overview
The Sensor Monitoring System is designed to collect, process, and store temperature data from multiple sensor nodes using a sensor gateway and an SQLite database. The system is implemented in C on an embedded Linux platform, adhering to the requirements outlined in the assignment document. The architecture ensures thread-safe communication, dynamic sensor node connections, and robust logging and data storage mechanisms.

## System Architecture

### Components
1. **Sensor Nodes (Simulated)**:
   - Simulated in software (as per lab 7).
   - Each node establishes a private TCP connection to the sensor gateway.
   - Sends temperature measurements to the gateway.

2. **Sensor Gateway**:
   - Acts as the central hub for data collection, processing, and storage.
   - Consists of a main process and a log process (forked as a child process, Req 1).
   - Main process runs three threads:
     - **Connection Manager**: Listens for incoming TCP connections from sensor nodes, captures data packets, and writes to a shared buffer (Req 3, 4).
     - **Data Manager**: Processes sensor data, calculates running averages for temperature, and determines "too hot/cold" states (Req 5).
     - **Storage Manager**: Reads data from the shared buffer and stores it in an SQLite database, with retry logic for database connection failures (Req 6).
   - Uses a single shared buffer (`sbuffer`) for thread communication, protected by synchronization primitives (Req 2).

3. **SQLite Database**:
   - Stores sensor measurements processed by the storage manager.
   - Supports dynamic table creation and data insertion (Req 6, 9).

4. **Log Process**:
   - Receives log events from the main process via a FIFO (`logFifo`) (Req 7).
   - Writes formatted log messages to `gateway.log` in the format: `<sequence number> <timestamp> <log-event info message>` (Req 8).
   - Supports multiple log event types from all threads (Req 9).

### Data Flow
1. Sensor nodes send temperature measurements over TCP to the sensor gateway.
2. The connection manager thread accepts connections, reads packets, and stores data in the shared buffer (`sbuffer`).
3. The data manager thread reads from the shared buffer, calculates running averages, and logs "too hot/cold" events.
4. The storage manager thread reads from the shared buffer and inserts data into the SQLite database.
5. All threads send log events to the log process via the FIFO, which writes to `gateway.log`.

### Thread Synchronization
- A single `sbuffer` is used for data sharing between the connection, data, and storage manager threads (Req 2).
- Access to `sbuffer` is protected using a **mutex** to ensure thread safety, preventing simultaneous writes or read/write conflicts.
- The FIFO (`logFifo`) is also protected by a mutex to handle concurrent access by multiple threads (Req 7).
- Synchronization ensures:
  - **Correctness**: Only one thread accesses the shared buffer at a time.
  - **Deadlock Avoidance**: Simple lock ordering and no nested locks.
  - **Fairness**: Threads access the buffer immediately if not locked, avoiding starvation.
  - **Efficiency**: Minimal locking overhead with a mutex, suitable for the workload.

### Key Features
- **Dynamic Sensor Connections**: The gateway supports a variable number of sensor nodes, with no maximum limit at startup (Req 3).
- **Thread-Safe Communication**: Shared buffer and FIFO access are protected to prevent race conditions.
- **Robust Database Handling**: The storage manager retries database connections up to three times before shutting down the gateway (Req 6).
- **Comprehensive Logging**: Supports all required log events, written to `gateway.log` in a structured format (Req 8, 9).
- **Debug Mode**: Includes a debug mode for testing with multiple concurrent sensor nodes and small sleep times between measurements.

## Directory Structure
```
sensor_gateway/
├── src/
│   ├── main.c              # Main process and thread initialization
│   ├── connection_mgr.c    # Connection manager thread logic
│   ├── data_mgr.c          # Data manager thread logic
│   ├── storage_mgr.c       # Storage manager thread logic
│   ├── log_process.c       # Log process logic
│   ├── sbuffer.c           # Shared buffer implementation
│   ├── sbuffer.h           # Shared buffer header
│   ├── config.h            # Configuration and constants
│   └── utils.c             # Utility functions (e.g., FIFO, logging)
├── include/                # Header files for external libraries
├── test/                   # Test code for sensor node simulation
├── Makefile                # Build instructions
├── gateway.log             # Log file output
└── README.md               # This file
```

## Build and Run Instructions
1. **Prerequisites**:
   - GCC toolchain
   - SQLite3 development libraries
   - Valgrind for memory leak checking
   - Make tool

2. **Build**:
   ```bash
   make
   ```
   This compiles the source code and links against SQLite3.

3. **Run**:
   ```bash
   ./server <port_number>
   ```
   Example: `./server 1234` to start the gateway listening on port 1234.

4. **Test**:
   - Use the provided test scripts in the `test/` directory to simulate sensor nodes.
   - Enable debug mode by defining the `DEBUG` preprocessor symbol in `config.h`.

5. **Clean**:
   ```bash
   make clean
   ```

## Implementation Notes
- **Code Structure**:
  - Source code is modular, with separate files for each thread and the shared buffer.
  - Logical typedefs and clear variable/function naming are used for readability.
  - Code is indented consistently, avoiding spaghetti code or `goto` statements.

- **Synchronization Choice**:
  - A mutex is chosen for `sbuffer` and FIFO access due to its simplicity and efficiency for this workload.
  - Ensures mutual exclusion, prevents deadlocks, and provides fair access without excessive CPU overhead.
  - Alternative primitives (e.g., semaphores, read/write locks) were considered but deemed unnecessary for the current requirements.

- **Error Handling**:
  - Database connection failures trigger retries with a delay, shutting down after three failed attempts.
  - Invalid sensor data is logged as a specific event (Req 9).

- **Testing**:
  - Thoroughly tested with multiple concurrent sensor nodes using small sleep times.
  - Valgrind used to ensure no memory leaks.
  - Debug mode logs additional information for troubleshooting.

## Deliverables
- **Programming Assignment**:
  - Source code, Makefile, and test scripts uploaded to `labtools.groept.be` by January 10, 2022, 5 PM.
  - Includes a self-review document reflecting on code quality and implementation choices.
  - Compiles and runs, producing meaningful output (database entries, logs).

- **Paper Assignment**:
  - Memory layout drawing (A3 format) for the specified program state.
  - Critical reflection on synchronization primitives (max 2 A4 pages).
  - Submitted as a hard copy during the oral defense.

## Dependencies
- SQLite3 (`libsqlite3-dev`)
- POSIX threads (`pthread`)
- Standard C libraries

## References
- Lab manuals 4–8 for code integration.
- Linux lab manuals for command-line tools and system calls.
- SQLite documentation for database operations.

## Contact
For questions, refer to the assignment document or contact the course instructor via the provided channels.

---
**Note**: This README is designed to align with the minimal requirements and acceptance criteria outlined in the assignment document. Ensure all code is thoroughly tested before submission.