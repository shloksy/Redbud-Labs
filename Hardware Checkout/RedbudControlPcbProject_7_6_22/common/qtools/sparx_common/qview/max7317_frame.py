# max7317_frame.py
# Common frame for the MAX7317 used for qview.

# awong

import tkinter as tk
import struct

class MAX7317Frame:
    def __init__(self, 
                 qview, 
                 tk_parent,
                 publish_callback) -> None:
        self._qview = qview
        self._tk_parent = tk_parent
        self._publish_callback = publish_callback

    def create_output_pin_command_labelframe(self,
                                             x_coor: int, 
                                             y_coor: int,
                                             qp_signal: int,
                                             number_of_max7317_daisy_chained: int) -> None:
                                             
        self._signal = qp_signal
        self._num_chained = number_of_max7317_daisy_chained

        frame_root = tk.LabelFrame(self._tk_parent, text='Set MAX7317 Output Pin')

        self._port_entry = []
        self._enable = []

        for i in range(0, number_of_max7317_daisy_chained):
            tk.Label(frame_root, text=f'MAX7317 Index {i}').grid(row=i * 5 + 0, column=0)
            tk.Label(frame_root, text='Pin (0 to 9)').grid(row=i * 5 + 1, column=0)
            entry = tk.Entry(frame_root, textvariable=tk.IntVar(value=0))
            entry.grid(row=i * 5 + 2, column=0)
            self._port_entry.append(entry)

            enable = tk.IntVar(value=False)
            tk.Checkbutton(frame_root, text='Enable', variable=enable).grid(row=i * 5 + 3, column=0)
            self._enable.append(enable)
        
        tk.Button(frame_root, text='Send', command=self.send_out_pin_event).grid(row=i * 5 + 4, column=0)
        
        self._qview.canvas.create_window(x_coor, y_coor, anchor=tk.NW, window=frame_root)  

    # Callback from button press.
    def send_out_pin_event(self) -> None:
        buf_bytes = bytearray(self._num_chained * 5)

        for i in range (0, self._num_chained):
            port = int(self._port_entry[i].get())
            struct.pack_into('<i', buf_bytes, i * 4, port)

        for i in range(0, self._num_chained):
            enable = self._enable[i].get()
            offset = self._num_chained * 4 + i
            struct.pack_into('<B', buf_bytes, offset, enable)

        self._publish_callback(self._signal, buf_bytes)
