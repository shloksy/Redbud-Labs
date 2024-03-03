# bldc_frame.py
# Common frame for BLDC used for qview.

# awong

import tkinter as tk
import struct

class BLDCFrame:
    def __init__(self, qview, tk_parent, publish_callback) -> None:
        self._qview = qview
        self._tk_parent = tk_parent
        self._publish_callback = publish_callback

    def create_command_labelframe(self, x_coor: int, y_coor: int, qp_signal: int) -> None:
        self._signal = qp_signal
        frame_root = tk.LabelFrame(self._tk_parent, text='BLDC Motor')
        
        tk.Label(frame_root, text='Direction (0 or 1)').grid(row=0, column=0)
        self._direction_entry = tk.Entry(frame_root, textvariable=tk.DoubleVar(value=0))
        self._direction_entry.grid(row=1, column=0)
        tk.Label(frame_root, text='BLDC Speed (0-100%)').grid(row=2, column=0)
        self._volt_setpoint_entry = tk.Entry(frame_root, textvariable=tk.DoubleVar(value=0))
        self._volt_setpoint_entry.grid(row=3, column=0)
        tk.Button(frame_root, text='Send', command=self.send_bldc_speed_command).grid(row=4, column=0)
        
        self._qview.canvas.create_window(x_coor, y_coor, anchor=tk.NW, window=frame_root)

    # Callback from button press.
    def send_bldc_speed_command(self):
        volts = self._volt_setpoint_entry.get()
        direction = self._direction_entry.get()
        try:
            volts = int(volts)
            direction = int(direction)
        except ValueError:
            raise
            
        self._publish_callback(self._signal, struct.pack('<2B', direction, volts))
