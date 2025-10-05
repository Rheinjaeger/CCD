import sys
import os
import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
from PIL import Image, ImageTk

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'tests')))
from FakeImage import generate_fake_image

# Folder to store transmitted images
IMAGE_SAVE_FOLDER = "ccd_images"
os.makedirs(IMAGE_SAVE_FOLDER, exist_ok=True)

class CCDGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("CCD Small Satellite Readout System")
        self.root.geometry("900x600")
        self.tk_image = None
        self.captured_images = []

        # === Main Layout Frame ===
        main_frame = ttk.Frame(root)
        main_frame.pack(fill="both", expand=True, padx=10, pady=10)

        # === Left Side: Image + Log ===
        left_frame = ttk.Frame(main_frame)
        left_frame.pack(side="left", fill="both", expand=True)

        self.title_label = ttk.Label(left_frame, text="CCD Image Control", font=("Helvetica", 16))
        self.title_label.pack(pady=10)

        self.image_label = ttk.Label(left_frame, text="Image Preview")
        self.image_label.pack()

        self.canvas = tk.Canvas(left_frame, width=384, height=384, bg="black")
        self.canvas.pack(pady=10)

        # === Logging Panel ===
        log_label = ttk.Label(left_frame, text="System Logs")
        log_label.pack(pady=(10, 0))

        self.log_text = tk.Text(left_frame, height=10, width=60, state="disabled", wrap="word", bg="#f0f0f0")
        self.log_text.pack(pady=5, padx=10, fill="both", expand=True)

        # === Right Side: Command Buttons + Temps + Preview Loader ===
        right_frame = ttk.LabelFrame(main_frame, text="System Commands")
        right_frame.pack(side="right", fill="y", padx=20)

        button_specs = [
            ("Capture Image", self.capture_image),
            ("Transmit Images", self.transmit_images),
            ("Reset System", self.reset_system),
            ("Clear Flash Memory", self.clear_memory),
            ("Ping", self.ping_device),
            ("Reset CCD sensor", self.reset_CCD)
        ]

        for text, cmd in button_specs:
            ttk.Button(right_frame, text=text, command=cmd).pack(pady=10, padx=10, fill="x")

        # === Temperature Status Frame ===
        temp_frame = ttk.LabelFrame(right_frame, text="Temperature Status")
        temp_frame.pack(pady=20, padx=10, fill="x")

        self.pcb_temp_var = tk.StringVar(value="72°F")
        self.env_temp_var = tk.StringVar(value="70°F")

        ttk.Label(temp_frame, text="PCB Temp:").grid(row=0, column=0, sticky="w", padx=5, pady=2)
        ttk.Label(temp_frame, textvariable=self.pcb_temp_var).grid(row=0, column=1, sticky="e", padx=5, pady=2)
        ttk.Label(temp_frame, text="Env Temp:").grid(row=1, column=0, sticky="w", padx=5, pady=2)
        ttk.Label(temp_frame, textvariable=self.env_temp_var).grid(row=1, column=1, sticky="e", padx=5, pady=2)

        # === Image Preview Loader Frame ===
        preview_frame = ttk.LabelFrame(right_frame, text="Load Saved Image")
        preview_frame.pack(pady=10, padx=10, fill="x")

        self.image_list = ttk.Combobox(preview_frame, state="readonly", width=25)
        self.image_list.pack(pady=5, padx=5)

        ttk.Button(preview_frame, text="Preview Image", command=self.load_selected_image).pack(pady=5)

        self.update_image_dropdown()

    def log_message(self, message):
        self.log_text.config(state="normal")
        self.log_text.insert("end", f"{message}\n")
        self.log_text.see("end")
        self.log_text.config(state="disabled")

    def capture_image(self):
        img_data = generate_fake_image()
        self.captured_images.append(img_data)
        self.log_message("Image captured and stored in memory.")

    def transmit_images(self):
        if not self.captured_images:
            self.log_message("No images to transmit.")
            return

        for idx, img_data in enumerate(self.captured_images, start=1):
            filename = os.path.join(IMAGE_SAVE_FOLDER, f"image_{idx:03d}.png")
            img = Image.fromarray(img_data, mode='L')
            img.save(filename)

        self.log_message(f"{len(self.captured_images)} images transmitted and saved to '{IMAGE_SAVE_FOLDER}/'.")
        self.captured_images.clear()
        self.update_image_dropdown()

    def reset_system(self):
        self.captured_images.clear()
        self.canvas.delete("all")
        self.tk_image = None
        self.image_list.set("")
        self.update_image_dropdown()
        self.log_message("System reset to initial state.")


    def clear_memory(self):
        self.log_message("Flash memory on system cleared.")

    def reset_CCD(self):
        self.log_message("CCD reset")

    def ping_device(self):
        self.log_message("Pinging PCB...")
        # Simulate a short delay or response
        self.root.after(500, lambda: self.log_message("PCB response: System online"))


    def update_image_dropdown(self):
        files = [f for f in os.listdir(IMAGE_SAVE_FOLDER) if f.lower().endswith(".png")]
        files.sort()
        self.image_list["values"] = files
        if files:
            self.image_list.set(files[-1])  # Select latest image by default
        else:
            self.image_list.set("")

    def load_selected_image(self):
        filename = self.image_list.get()
        if not filename:
            self.log_message("No image selected.")
            return

        filepath = os.path.join(IMAGE_SAVE_FOLDER, filename)
        try:
            pil_img = Image.open(filepath).resize((384, 384))
            self.tk_image = ImageTk.PhotoImage(pil_img)
            self.canvas.delete("all")
            self.canvas.create_image(0, 0, anchor="nw", image=self.tk_image)
            self.log_message(f"Previewing '{filename}'.")
        except Exception as e:
            self.log_message(f"Error loading image: {e}")

if __name__ == "__main__":
    root = tk.Tk()
    app = CCDGUI(root)
    root.mainloop()
