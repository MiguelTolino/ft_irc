import socket
import time
import select
from colorama import init, Fore, Style

# Initialize colorama
init()

# Server configuration
server_host = 'localhost'
server_port = 6667

# Test case: Connect to the server
def test_connection():
    try:
        # Connect to the server
        irc_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        irc_socket.connect((server_host, server_port))
        irc_socket.close()
        print_status("Connection test", "PASS")
    except Exception as e:
        print_status("Connection test", "FAIL")
        print("Failed to connect to the server:", e)

# Test case: Authenticate, set nickname, username, join a channel, send and receive messages
def test_basic_functionality():
    try:
        # Connect to the server
        irc_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        irc_socket.connect((server_host, server_port))

        # Authenticate
        send_irc_message(irc_socket, "PASS 123\r\n")
        send_irc_message(irc_socket, "NICK test_user\r\n")
        send_irc_message(irc_socket, "USER test_user 0 * :Test User\r\n")

        # Wait for server messages
        time.sleep(1)

        # Join a channel
        send_irc_message(irc_socket, "JOIN #testchannel\r\n")
        time.sleep(1)

        # Send a message to the channel
        send_irc_message(irc_socket, "PRIVMSG #testchannel :Hello, world from test_user!\r\n")
        time.sleep(1)

        # Receive messages from the channel
        receive_irc_messages(irc_socket)
        
        
        # Send a private message
        send_irc_message(irc_socket, "PRIVMSG target_user :Hello, this is a private message from test_user!\r\n")
        time.sleep(1)

        # Receive private message
        receive_irc_messages(irc_socket)

        # Set channel topic
        send_irc_message(irc_socket, "TOPIC #testchannel :This is a test channel\r\n")
        time.sleep(1)

        # Kick a user from the channel
        send_irc_message(irc_socket, "KICK #testchannel target_user :You have been kicked!\r\n")
        time.sleep(1)

        # Set channel modes
        send_irc_message(irc_socket, "MODE #testchannel +o target_user\r\n")
        time.sleep(1)

        # Set user modes
        send_irc_message(irc_socket, "MODE test_user +i\r\n")
        time.sleep(1)

        irc_socket.close()
        print_status("Basic functionality test", "PASS")
    except Exception as e:
        print_status("Basic functionality test", "FAIL")
        print("Error during basic functionality test:", e)

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
                
# Helper function to print test status with colors
def print_status(test_name, status):
    if status == "PASS":
        print(Fore.GREEN + f"[{status}] {test_name}" + Style.RESET_ALL)
    else:
        print(Fore.RED + f"[{status}] {test_name}" + Style.RESET_ALL)

# Run the test cases
test_connection()
test_basic_functionality()
