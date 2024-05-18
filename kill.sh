#!/bin/bash

# Run netstat command and capture output as a string
output=$(netstat -tulpn | grep "./TEST")

# Extract the process ID from each relevant line
listening_pids=$(echo "$output" | awk '{print $NF}' | awk -F'/' '{print $1}')

# Print the results
echo "Listening process IDs: $listening_pids"

# Kill each process with SIGKILL (9)
for pid in $listening_pids
do
    echo "Killing process $pid..."
    kill -9 "$pid"
done

