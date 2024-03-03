import serial
import matplotlib.pyplot as plt

ROTATIONS = 50
NUM_DIMENSIONS = 3

port = "/dev/cu.usbmodem141401"
ser = serial.Serial(port, 115200)
cycles3d_list = []


def read_cycle():
    cycle_list = []
    while True:
        data = ser.readline()
        try:
            data = data.decode("utf-8", "ignore")
            data_list = data.split("\t")
            data_list = [float(value) for value in data_list if value.strip() != '']

            if data_list and data_list[0] == 500:
                break

            if len(data_list) == 3:
                try:
                    cycle_list.append(data_list)
                except ValueError:
                    pass
        except UnicodeDecodeError:
            pass
    return cycle_list


for x in range(ROTATIONS):
    cycles3d_list.append(read_cycle())

print(cycles3d_list)

counter = 0
x_vals = []
y_vals = []

for i in range(ROTATIONS):
    counter = 0
    for k in range(NUM_DIMENSIONS):
        counter = 0
        for j in range(len(cycles3d_list[i])):
            x_vals.append(counter)
            y_vals.append(cycles3d_list[i][j][k])
            print('{}, {}'.format(counter, cycles3d_list[i][j][k]))
            counter += 1

        if k == 0:
            plt.plot(x_vals, y_vals, color='red')
        elif k == 1:
            plt.plot(x_vals, y_vals, color='green')
        #elif k == 2:
            #plt.plot(x_vals, y_vals, color='blue')

        x_vals.clear()
        y_vals.clear()


plt.xlabel('Points')
plt.ylabel('Acceleration (m/s)')
plt.title(f'Accelerometer Data Over {ROTATIONS} Rotations')
plt.show()
