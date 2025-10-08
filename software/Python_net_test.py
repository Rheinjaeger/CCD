import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.settimeout(4)
sock.sendto(b"ping", ("172.21.96.2", 6000))
try:
    data, addr = sock.recvfrom(512)
    print(f"Received: {data} from {addr}")
except socket.timeout:
    print("No response received.")

#172.29.82.64