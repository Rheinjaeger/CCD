import logging
import socket
from PIL import Image

DEF_PORT = 40020
BUFFER_SIZE = 1024
WIDTH, HEIGHT = 1024, 1024

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(message)s',
    handlers=[logging.StreamHandler()]
)

def start_receiver(port=DEF_PORT, filename = "greyscale_img.png"):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind(('0.0.0.0', port))
    server_socket.listen()

    logging.info(f"Receiver started, listening on port {port}...")

    while True:
        peer_socket, peer_address = server_socket.accept()
        logging.info(f"Accepted connection from {peer_address}")

        logging.info(f"Receiving image data...")
        image_data = b""

        while True:
            data = peer_socket.recv(BUFFER_SIZE)
            if not data:
                break
            image_data += data

        with open(filename, 'wb') as f:
            f.write(image_data)

        image = Image.frombytes('L', (WIDTH, HEIGHT),image_data)
        image.save("greyscale_img.png")

        logging.info(f"Image received and saved as {filename}")
        peer_socket.close()

if __name__ == '__main__':
    start_receiver(port=DEF_PORT, filename="greyscale_img.png")


