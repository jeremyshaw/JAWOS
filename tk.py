import tkinter

root = tkinter.Tk()
label = tkinter.Label(root)
label.pack()
tkimg = [None]  # This, or something like it, is necessary because if you do not keep a reference to PhotoImage instances, they get garbage collected.

delay = 500   # in milliseconds
def loopCapture():
    try:
        tkimg[0] = tkinter.PhotoImage(file = "ball.gif")
    except:
        tkimg[0] = tkinter.PhotoImage(file = "ball1.gif")
    label.config(image=tkimg[0])
    root.update_idletasks()
    root.after(delay, loopCapture)

loopCapture()
root.mainloop()