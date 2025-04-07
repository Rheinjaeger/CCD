from PIL import ImageTk, Image
import numpy as np

def np_array_to_tkimage(np_array, size=(512, 512)):

    #Convert a NumPy array to a Tkinter-compatible image.
    img = Image.fromarray(np_array)
    img = img.resize(size)
    return ImageTk.PhotoImage(img)
