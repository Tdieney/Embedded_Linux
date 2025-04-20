if [ $# -ne 2 ]; then
    echo "Usage: $0 <port> <num_sensors>"
    exit 1
fi

PORT=$1
NUM_SENSORS=$2
PIDS=()

# Trap Ctrl+C (SIGINT) and TERM signals to clean up child processes
cleanup() {
    echo "Stopping all sensor nodes..."
    for pid in "${PIDS[@]}"; do
        if kill -0 "$pid" 2>/dev/null; then
            kill -TERM "$pid"
            wait "$pid" 2>/dev/null
        fi
    done
    echo "All sensor nodes stopped."
    exit 0
}

trap cleanup SIGINT SIGTERM

# Compile if not already compiled
if [ ! -f sensor ]; then
    make all
fi

echo "Starting $NUM_SENSORS sensor nodes on port $PORT..."

for ((i=1; i<=NUM_SENSORS; i++)); do
    ./_builds/bin/sensor $PORT $((i)) &
    echo "Started sensor node with ID $((i))"
    sleep 0.1  # Small delay to prevent overwhelming the system
done

echo "All sensor nodes started. Press Ctrl+C to stop."
wait