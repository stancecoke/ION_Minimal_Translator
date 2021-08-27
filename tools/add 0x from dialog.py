import matplotlib.pyplot as plt
import numpy as np
import tkinter as tk
from tkinter import filedialog
from tkinter import simpledialog
from tkinter import messagebox
root = tk.Tk()
root.withdraw()


answer = simpledialog.askstring("Input", "Bitte gib die Nachricht ein!",
                                parent=root)

print( answer)

output = "{"

for zahl in range(0,len(answer),3):
    output = output +("0x"+answer[zahl:zahl+2]+", ")
output = output[0:len(output)-2]+"},"

root.clipboard_clear()
root.clipboard_append(output)
root.update()

messagebox.showinfo("Ergebnis wurde in die Zwischenablage kopiert!",output)
print (output)

