import serial
import matplotlib.pyplot as plt
import numpy as np

port = 'COM9' # Change this to the com the arduino uses.
rate = 9600;
ser = serial.Serial(port, rate, timeout = 5)

f = open('csvfile.csv','w') # This will create a csvfile in the project directory.

print("Enter any number in the console to begin.")
input();
print("Attach the sample and enter another number to begin.")
x = input();
ser.write(x.encode('utf-8'))
print("Starting now...")


while True:
    line = ser.readline();
    line = line.decode("utf-8") #ser.readline returns a binary, convert to string
    if line:
        f.write(line)
        print(line);
    else:
        break
f.close()

x, y = np.loadtxt('csvfile.csv', delimiter=',',skiprows=12, unpack=True)
plt.plot(x,y, label='Loaded from file!')

plt.xlabel('Distance (mm)')
plt.ylabel('Force (N)')
plt.title('Distance v Force Graph')
plt.legend()
plt.show()