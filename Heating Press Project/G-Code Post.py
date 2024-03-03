import os
from tkinter import Tk
from tkinter.filedialog import askopenfilename

# function to create new G-code file for each M6 command
def split_gcode(file):
    # read G-code file
    with open(file, 'r') as gcode:
        lines = gcode.readlines()

    # initialize variables
    current_file = None
    file_num = 1

    # iterate through lines of G-code
    for line in lines:
        # check if line contains M6 command
        if "M6" in line:
        

            # close current file
            if current_file is not None:
            
                #Write the end of program
                current_file.write(endofprogram)
            
                current_file.close()

            # create new file with name "file_num.gcode"
            current_file = open(os.path.splitext(file)[0] + "_" + str(file_num) + ".nc", "w")
            file_num += 1
            
            #Write header
            current_file.write(programheader)
            
        # write line to current file
        if current_file is not None:
            current_file.write(line)

    # close last file
    current_file.close()

# create GUI to select file
root = Tk()
root.withdraw()
file = askopenfilename()


#Program Header
programheader = "(" + file + " Post Processed G-code)\n" \
                "(Program has been seperated by tool changes)\n" \
                "(Move to safe Z to avoid workholding)\n" \
                "G90 \n" \
                "G21 \n" \
                "G53G0Z-5.000 \n" \
                

#End of program text
endofprogram = "M02"

# run function on selected file
split_gcode(file)
