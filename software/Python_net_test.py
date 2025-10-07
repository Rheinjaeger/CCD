import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.settimeout(2)
sock.sendto(b"ping", ("192.168.1.123", 5000))
try:
    data, addr = sock.recvfrom(512)
    print(f"Received: {data} from {addr}")
except socket.timeout:
    print("No response received.")
