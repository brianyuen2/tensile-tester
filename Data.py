import serial
import matplotlib.pyplot as plt
import numpy as np
import msvcrt
import time

port = 'COM11' # Change this to the com the arduino uses.
rate = 9600;
ser = serial.Serial(port, rate, timeout = 5)

timestr = time.strftime("%d%m%Y-%H%M%S")
f = open(timestr+'.csv','w') # This will create a csvfile in the project directory.

print("Attach the sample and enter any number in the console to begin.")
input();
msvcrt.getch()

print("Enter '1' for tensile test or '2' for compression test")
x = input();
msvcrt.getch()
ser.write(x.encode('utf-8'))

while (1):
    line = ser.readline();
    line = line.decode("utf-8") #ser.readline returns a binary, convert to string
    if line:
        f.write(line)
        print(line);
    else:
        break
    # pressing 's' stops the arduino.
    if msvcrt.kbhit():
        letter = msvcrt.getch()
        if letter == b's':
            ser.write('s'.encode('utf-8'))


'''f.close()

# Plots graph.
x, y = np.loadtxt(timestr +'.csv', delimiter=',',skiprows=12, unpack=True)
plt.plot(x,y, label='Loaded from file!')

plt.xlabel('Distance (mm)')
plt.ylabel('Force (N)')
plt.title('Distance v Force Graph')
plt.legend()
plt.show()'''