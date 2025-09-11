import socket

s = socket.create_connection(("192.168.1.2", 5000), timeout=2)
s.sendall(b"PING\n")
print(s.recv(64))
s.close()
