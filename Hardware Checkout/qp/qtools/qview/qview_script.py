# qview_script.py
# Built for the CoreH743I_SparxController project.

# awong

import sys
from pathlib import Path

# Globally add paths to qview so that we can access classes in other locations.  This
# allows us to maintain a common set of qview support along with application specific
# extensions.
sys.path.insert(0, str(Path("../qtools").resolve()))
sys.path.insert(0, str(Path("../common/qtools/sparx_common").resolve()))
sys.path.insert(0, str(Path("../common/qtools/sparx_common/qview").resolve()))

print(sys.path)

import project_signals as sigs
import project_common_signals as common_sigs

import max7315_frame as max7315
import max7317_frame as max7317
import tmc429_frame as tmc429
import digital_output_frame as digital_output
import bldc_frame as bldc

class QViewScript:
    # We use 2 MAX7317 IO expanders in a daisy chain on the valve PCB for this application.
    NumberOfMax7317sDaisyChained = 2
            
    def __init__(self):
        reset_target()

        QView.show_canvas()
        QView.canvas.configure(width=1400, height=600)
        
        # Filters:
        self.configure_user_filter_column(QView.canvas, 10, 10)

        self._tmc429 = tmc429.TMC429Frame(QView, QView.canvas, publish)
        self._tmc429.create_motor_command_frame(210, 
                                                10, 
                                                common_sigs.Signals.TMC429_SET_MOTOR_POSITION_SIG.value,
                                                common_sigs.Signals.TMC429_SET_MOTOR_VELOCITY_SIG.value,
                                                common_sigs.Signals.TMC429_SET_MODE_SIG.value)
        self._tmc429.create_motor_status_frame(410, 10)

        self._max7315 = max7315.MAX7315Frame(QView, QView.canvas, publish)
        self._max7315.create_output_pin_command_labelframe(1010, 10, sigs.Signals.IOEXP2_SET_OUTPUT_SIG.value)

        self._max7317 = max7317.MAX7317Frame(QView, QView.canvas, publish)
        self._max7317.create_output_pin_command_labelframe(1210, 10, sigs.Signals.IOEXP_SET_OUTPUT_SIG.value, self.NumberOfMax7317sDaisyChained)

        self._digital_output = digital_output.DigitalOutputFrame(QView, QView.canvas, publish)
        self._digital_output.create_output_pin_command_labelframe(810, 10, sigs.Signals.SET_DIGITAL_OUTPUT_SIG.value)

        self._bldc = bldc.BLDCFrame(QView, QView.canvas, publish)
        self._bldc.create_command_labelframe(610, 10, sigs.Signals.SET_BLDC_SPEED_PERCENTAGE_SIG.value)

    def configure_user_filter_column(self, tk_parent, x_coor: int, y_coor: int) -> None:
        frame_root = LabelFrame(tk_parent, text='QP User Filter')

        self._user_filter_enable_list = []
        self._user_filter_checkbox_list = []

        row_index = 0
        for common_user_message in common_sigs.QSUserMessages:
            if common_user_message != common_sigs.QSUserMessages.LastCommonUserMessage:
                filter_var = BooleanVar(value=False)
                checkbutton = Checkbutton(frame_root, text=common_user_message.name, variable=filter_var)
                checkbutton.grid(row=row_index, column=0)
                self._user_filter_enable_list.append(filter_var)
                self._user_filter_checkbox_list.append(checkbutton)
                row_index += 1

        for project_user_message in sigs.QSUserMessages:
            if project_user_message != sigs.QSUserMessages.LastUserMessage:
                filter_var = BooleanVar(value=False)
                checkbutton = Checkbutton(frame_root, text=project_user_message.name, variable=filter_var)
                checkbutton.grid(row=row_index, column=0)
                self._user_filter_enable_list.append(filter_var)
                self._user_filter_checkbox_list.append(checkbutton)
                row_index += 1

        Button(frame_root, text='Select All', command=self.user_filter_select_all_event).grid(row=row_index, column=0)
        row_index += 1
        Button(frame_root, text='Deselect All', command=self.user_filter_deselect_all_event).grid(row=row_index, column=0)
        row_index += 1
        Button(frame_root, text='Send', command=self.send_user_filter_event).grid(row=row_index, column=0)
        
        QView.canvas.create_window(x_coor, y_coor, anchor=NW, window=frame_root)

    def user_filter_select_all_event(self):
        for filter_var in self._user_filter_enable_list:
            filter_var.set(1)
            
    def user_filter_deselect_all_event(self):
        for filter_var in self._user_filter_enable_list:
            filter_var.set(0)
            
    def send_user_filter_event(self):
        user_filter_list = []
        for i in range(0, len(self._user_filter_enable_list)):
            if self._user_filter_enable_list[i].get() != 0:
                user_filter_list.append(common_sigs._QP_QS_USER + i)
            else:
                user_filter_list.append(-(common_sigs._QP_QS_USER + i))
            
        glb_filter(*user_filter_list)

    # Plumbed TMC429MgrMsg (user 3) to motor status processing.
    def QS_USER_03(self, packet):
        self._tmc429.motor_status_parse(packet)


QView.customize(QViewScript())
