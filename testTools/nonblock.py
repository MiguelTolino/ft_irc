import select
import socket
import time
from colorama import init, Fore, Style


# Initialize colorama
init()

# Server configuration
server_host = 'localhost'
server_port = 6667

# Test case: Check non-blocking behavior
def test_non_blocking_behavior():
    try:
        # Connect to the server
        irc_sockets = []
        for i in range(5):
            irc_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            irc_socket.connect((server_host, server_port))
            irc_sockets.append(irc_socket)

        # Authenticate and join channel for each client
        for irc_socket in irc_sockets:
            send_irc_message(irc_socket, "PASS 123\r\n")
            send_irc_message(irc_socket, "NICK test_user\r\n")
            send_irc_message(irc_socket, "USER test_user 0 * :Test User\r\n")
            time.sleep(1)
            send_irc_message(irc_socket, "JOIN #testchannel\r\n")
            time.sleep(1)

        # Send a large number of messages from multiple clients
        message = "PRIVMSG #testchannel :" + "A" * 10000 + "\r\n"
        for i in range(100):
            for irc_socket in irc_sockets:
                send_irc_message(irc_socket, message)

        # Check for readability on sockets
        readable, _, _ = select.select(irc_sockets, [], [], 5)

        # Verify that all sockets are readable
        if len(readable) == len(irc_sockets):
            print_status("Non-blocking behavior test", "PASS")
        else:
            print_status("Non-blocking behavior test", "FAIL")

        # Receive messages from the channel
        for irc_socket in irc_sockets:
            receive_irc_messages(irc_socket)

        # Close the sockets
        for irc_socket in irc_sockets:
            irc_socket.close()

    except Exception as e:
        print_status("Non-blocking behavior test", "FAIL")
        print("Error during non-blocking behavior test:", e)

# Helper function to send an IRC message
def send_irc_message(sock, message):
    sock.send(message.encode())

# Helper function to receive IRC messages
def receive_irc_messages(sock):
    while True:
        # Check if there are incoming messages
        readable, _, _ = select.select([sock], [], [], 0.1)
        if readable:
            buffer = sock.recv(1024).decode()
            messages = buffer.split("\r\n")
            for msg in messages[:-1]:
                print(Fore.GREEN + "Received message: " + msg + Style.RESET_ALL)
            if not messages[-1].endswith("\r\n"):
                buffer = messages[-1]
            else:
                buffer = ""

def print_status(test_name, status):
    if status == "PASS":
        print(Fore.GREEN + f"[{status}] {test_name}" + Style.RESET_ALL)
    else:
        print(Fore.RED + f"[{status}] {test_name}" + Style.RESET_ALL)

test_non_blocking_behavior()
