# max7315_frame.py
# Common frame for the MAX7315 used for qview.

# awong

import tkinter as tk
import struct

class MAX7315Frame:
    def __init__(self, qview, tk_parent, publish_callback) -> None:
        self._qview = qview
        self._tk_parent = tk_parent
        self._publish_callback = publish_callback

    def create_output_pin_command_labelframe(self, x_coor: int, y_coor: int, qp_signal: int) -> None:
        self._signal = qp_signal
        frame_root = tk.LabelFrame(self._tk_parent, text='Set MAX7315 Output Pin')
        
        tk.Label(frame_root, text='MAX7315 Output Pin Index').grid(row=0, column=0)
        self._pin_index_entry = tk.Entry(frame_root, textvariable=tk.IntVar(value=0))
        self._pin_index_entry.grid(row=1, column=0)
        self._pin_enable = tk.IntVar(value=False)
        self._pin_checkbox = tk.Checkbutton(frame_root, text='Enable', variable=self._pin_enable)
        self._pin_checkbox.grid(row=2, column=0)
        tk.Button(frame_root, text='Send', command=self.send_out_pin_event).grid(row=3, column=0)
        
        self._qview.canvas.create_window(x_coor, y_coor, anchor=tk.NW, window=frame_root)

    # Callback from button press.
    def send_out_pin_event(self) -> None:
        pin_index = int(self._pin_index_entry.get())
        enable = self._pin_enable.get()

        self._publish_callback(self._signal, struct.pack('<iB', pin_index, enable))
