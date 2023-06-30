#!/bin/bash

# Check if IP, port, and password are provided as parameters
if [[ $# -ne 3 ]]; then
    echo "Usage: $0 <IP> <port> <password>"
    exit 1
fi

ip=$1
port=$2
password=$3

# Number of connections to test
num_connections=2000

# Function to test the maximum number of connections
test_connections() {
    local i=0
    local success=0
    local failure=0

    while [[ $i -lt $num_connections ]]; do
        if nc -w 1 -z "$ip" "$port" >/dev/null 2>&1; then
            # Connect to the IRC server and send the password
            (
            sleep 2 # Wait for the connection to establish
            echo "PASS $password"
            echo "PING :keepalive" # Send a PING command periodically
            while true; do
                sleep 60 # Wait for 60 seconds before sending the next PING command
                echo "PING :keepalive"
            done
            ) | nc "$ip" "$port" >/dev/null 2>&1 &
            
            success=$((success + 1))
        else
            failure=$((failure + 1))
        fi

        i=$((i + 1))
        sleep 1
    done

    echo "Success: $success"
    echo "Failure: $failure"
}

# Call the function to test connections
test_connections
