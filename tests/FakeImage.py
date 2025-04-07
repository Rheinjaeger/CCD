import numpy as np
from PIL import Image
import socket


# Image Size
WIDTH, HEIGHT = 1024, 1024

# Function to generate a fake image
def generate_fake_image():
    # Generate random pixel values (grayscale, 8-bit per pixel)
    image_data = np.random.randint(0, 256, (HEIGHT, WIDTH), dtype=np.uint8)
    image = Image.fromarray(image_data, mode='L')  # L mode for grayscale
    image.save("fake_image.png")  # Save image
    return image_data

# Function to send image data over Ethernet (TCP)
def send_image(image_data, host='127.0.0.1', port=40020):
    try:
        # Create a TCP socket
        server_address = (host, port)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect(server_address)
            print(f"Connected to {host}:{port}")

            # Convert image data to bytes and send
            sock.sendall(image_data.tobytes())
            print("Image data sent successfully.")
    except Exception as e:
        print(f"Error sending image data: {e}")

if __name__ == "__main__":
    # Generate a fake image
    fake_image_data = generate_fake_image()

    # Simulate sending the image over Ethernet
    #send_image(fake_image_data)