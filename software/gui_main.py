import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'tests')))

import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
from PIL import Image, ImageTk

# Import from FakeImage
from FakeImage import generate_fake_image, send_image
from utils import np_array_to_tkimage

class CCDGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("CCD Small Satellite Readout System")
        self.root.geometry("850x750")
        self.image_data = None
        self.tk_image = None

        # UI Elements
        self.title_label = ttk.Label(root, text="CCD Image Control", font=("Helvetica", 18))
        self.title_label.pack(pady=10)

        self.image_label = ttk.Label(root, text="Generated Image Preview Below")
        self.image_label.pack(pady=5)

        self.canvas = tk.Canvas(root, width=512, height=512, bg="black")
        self.canvas.pack(pady=10)

        self.generate_button = ttk.Button(root, text="Generate Image", command=self.generate_image)
        self.generate_button.pack(pady=10)

        self.send_button = ttk.Button(root, text="Send Image Over Network", command=self.send_image)
        self.send_button.pack(pady=10)

    def generate_image(self):
        try:
            self.image_data = generate_fake_image()
            pil_img = Image.open("fake_image.png").resize((512, 512))
            self.tk_image = ImageTk.PhotoImage(pil_img)
            self.canvas.create_image(0, 0, anchor="nw", image=self.tk_image)
            self.image_label.config(text="Image Generated")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to generate image:\n{e}")

    def send_image(self):
        if self.image_data is None:
            messagebox.showwarning("Warning", "Please generate an image first.")
            return
        try:
            send_image(self.image_data)
            messagebox.showinfo("Success", "Image sent successfully.")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to send image:\n{e}")

if __name__ == "__main__":
    root = tk.Tk()
    app = CCDGUI(root)
    root.mainloop()