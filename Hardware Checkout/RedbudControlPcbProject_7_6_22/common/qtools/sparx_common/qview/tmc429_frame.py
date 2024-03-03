# tmc429_frame.py
# Common frame for the TMC429 used for qview.

# awong

import sys
from pathlib import Path
sys.path.insert(0, str(Path("../common/qtools/qview").resolve()))

import tkinter as tk
import struct

import qview as qv

class TMC429Frame:
    # From TMC429 datasheet:
    ModeDictionary = {"Ramp Mode": 0, "Soft Mode": 1, "Velocity Mode": 2, "Hold Mode": 3}

    def __init__(self, 
                 qview, 
                 tk_parent,
                 publish_callback) -> None:
        self._qview = qview
        self._tk_parent = tk_parent
        self._publish_callback = publish_callback

    def create_motor_command_frame(self, 
                                   x_coor: int, 
                                   y_coor: int,
                                   x_target_qp_signal: int,
                                   v_target_qp_signal: int,
                                   set_mode_qp_signal: int) -> None:
        self._x_target_qp_signal = x_target_qp_signal
        self._v_target_qp_signal = v_target_qp_signal
        self._set_mode_qp_signal = set_mode_qp_signal

        frame_root = tk.LabelFrame(self._tk_parent, text='TMC429 Stepper Motor')

        self.create_motor_index(frame_root, 0, 0)
        self.create_set_mode(frame_root, 3, 0)
        self.create_x_target(frame_root, 6, 0)
        self.create_v_target(frame_root, 9, 0)
        
        self._qview.canvas.create_window(x_coor, y_coor, anchor=tk.NW, window=frame_root)

    def create_motor_status_frame(self, x_coor: int, y_coor: int) -> None:
        frame_root = tk.LabelFrame(self._tk_parent, text='Stepper Motor Status')
        
        tk.Label(frame_root, text='Interrupt Pending (INT)').pack()
        self._motor_status_interruptpending = tk.BooleanVar(frame_root, value=0)
        self._motor_status_interruptpending_label = tk.Label(frame_root, textvariable=self._motor_status_interruptpending).pack()

        tk.Label(frame_root, text='CDGW').pack()
        self._motor_status_cdgw = tk.BooleanVar(frame_root, value=0)
        self._motor_status_cdgw_label = tk.Label(frame_root, textvariable=self._motor_status_cdgw).pack()

        tk.Label(frame_root, text='Reference Switch (RS)').pack()
        self._motor_status_rs1 = tk.BooleanVar(frame_root, value=0)
        self._motor_status_rs2 = tk.BooleanVar(frame_root, value=0)
        self._motor_status_rs3 = tk.BooleanVar(frame_root, value=0)
        tk.Label(frame_root, textvariable=self._motor_status_rs1).pack()
        tk.Label(frame_root, textvariable=self._motor_status_rs2).pack()
        tk.Label(frame_root, textvariable=self._motor_status_rs3).pack()

        tk.Label(frame_root, text='Position equals Target (xEQt)').pack()
        self._motor_status_xeqt1 = tk.BooleanVar(frame_root, value=0)
        self._motor_status_xeqt2 = tk.BooleanVar(frame_root, value=0)
        self._motor_status_xeqt3 = tk.BooleanVar(frame_root, value=0)
        tk.Label(frame_root, textvariable=self._motor_status_xeqt1).pack()
        tk.Label(frame_root, textvariable=self._motor_status_xeqt2).pack()
        tk.Label(frame_root, textvariable=self._motor_status_xeqt3).pack()

        tk.Label(frame_root, text='Position').pack()
        self._motor_status_position1 = tk.IntVar(frame_root, value=0)
        self._motor_status_position2 = tk.IntVar(frame_root, value=0)
        self._motor_status_position3 = tk.IntVar(frame_root, value=0)
        tk.Label(frame_root, textvariable=self._motor_status_position1).pack()
        tk.Label(frame_root, textvariable=self._motor_status_position2).pack()
        tk.Label(frame_root, textvariable=self._motor_status_position3).pack()

        tk.Label(frame_root, text='Velocity').pack()
        self._motor_status_velocity1 = tk.IntVar(frame_root, value=0)
        self._motor_status_velocity2 = tk.IntVar(frame_root, value=0)
        self._motor_status_velocity3 = tk.IntVar(frame_root, value=0)
        tk.Label(frame_root, textvariable=self._motor_status_velocity1).pack()
        tk.Label(frame_root, textvariable=self._motor_status_velocity2).pack()
        tk.Label(frame_root, textvariable=self._motor_status_velocity3).pack()

        tk.Label(frame_root, text='Int Flags').pack()
        self._motor_status_intflags1 = tk.IntVar(frame_root, value=0)
        self._motor_status_intflags2 = tk.IntVar(frame_root, value=0)
        self._motor_status_intflags3 = tk.IntVar(frame_root, value=0)
        tk.Label(frame_root, textvariable=self._motor_status_intflags1).pack()
        tk.Label(frame_root, textvariable=self._motor_status_intflags2).pack()
        tk.Label(frame_root, textvariable=self._motor_status_intflags3).pack()

        self._qview.canvas.create_window(x_coor, y_coor, anchor=tk.NW, window=frame_root)

    def create_motor_index(self, tk_parent, row_index: int, col_index: int) -> None:
        tk.Label(tk_parent, text='Motor Index').grid(row=row_index, column=col_index)
        self._motor_index_entry = tk.Spinbox(tk_parent, from_=1, to=3, textvariable=tk.IntVar(value=1))
        self._motor_index_entry.grid(row=row_index+1, column=col_index)

    def create_set_mode(self, tk_parent, row_index: int, col_index: int) -> None:
        tk.Label(tk_parent, text='Motor Mode').grid(row=row_index, column=col_index)
        self._motor_mode = tk.StringVar(tk_parent)
        self._motor_mode.set("Ramp Mode")
        self._motor_set_mode_entry = tk.OptionMenu(tk_parent, self._motor_mode, *self.ModeDictionary.keys())
        self._motor_set_mode_entry.grid(row=row_index+1, column=col_index)
        tk.Button(tk_parent, text='Send', command=self.send_motor_set_mode).grid(row=row_index+2, column=col_index)
        
    def create_x_target(self, tk_parent, row_index: int, col_index: int) -> None:
        tk.Label(tk_parent, text='Motor Position Command').grid(row=row_index, column=col_index)
        self._motor_x_target_entry = tk.Entry(tk_parent, textvariable=tk.IntVar(value=0))
        self._motor_x_target_entry.grid(row=row_index+1, column=col_index)
        tk.Button(tk_parent, text='Send', command=self.send_motor_x_target).grid(row=row_index+2, column=col_index)

    def create_v_target(self, tk_parent, row_index: int, col_index: int) -> None:
        tk.Label(tk_parent, text='Motor Velocity Command').grid(row=row_index, column=col_index)
        self._motor_v_target_entry = tk.Entry(tk_parent, textvariable=tk.IntVar(value=0))
        self._motor_v_target_entry.grid(row=row_index+1, column=col_index)
        tk.Button(tk_parent, text='Send', command=self.send_motor_v_target).grid(row=row_index+2, column=col_index)

    def send_motor_x_target(self):
        self._publish_callback(self._x_target_qp_signal, struct.pack('<iI',
                      int(self._motor_index_entry.get()) - 1,
                      int(self._motor_x_target_entry.get())))
        
    def send_motor_v_target(self):
        self._publish_callback(self._v_target_qp_signal, struct.pack('<ih',
                      int(self._motor_index_entry.get()) - 1,
                      int(self._motor_v_target_entry.get())))
                               
    def send_motor_set_mode(self):
        self._publish_callback(self._set_mode_qp_signal, struct.pack('<iB',
                      int(self._motor_index_entry.get()) - 1,
                      self.ModeDictionary[self._motor_mode.get()]))

    # Should be called from application user signal associated with this TMC429.
    # Note: For example, currently TMC429MgrMsg (QS_USER_03).
    def motor_status_parse(self, packet):
        unpacked_data = qv.qunpack("xxTxIxBxBxBxBxBxBxBxBxIxhxB", packet)
        motor_index = unpacked_data[1]
        self._motor_status_interruptpending.set(unpacked_data[2])
        self._motor_status_cdgw.set(unpacked_data[3])
        self._motor_status_rs1.set(unpacked_data[4])
        self._motor_status_rs2.set(unpacked_data[5])
        self._motor_status_rs3.set(unpacked_data[6])
        self._motor_status_xeqt1.set(unpacked_data[7])
        self._motor_status_xeqt2.set(unpacked_data[8])
        self._motor_status_xeqt3.set(unpacked_data[9])
        position = unpacked_data[10]
        velocity = unpacked_data[11]
        int_flags = unpacked_data[12]
        if motor_index == 0:
            self._motor_status_position1.set(position)
            self._motor_status_velocity1.set(velocity)
            self._motor_status_intflags1.set(int_flags)
        elif motor_index == 1:
            self._motor_status_position2.set(position)
            self._motor_status_velocity2.set(velocity)
            self._motor_status_intflags2.set(int_flags)
        elif motor_index == 2:
            self._motor_status_position3.set(position)
            self._motor_status_velocity3.set(velocity)
            self._motor_status_intflags3.set(int_flags)
