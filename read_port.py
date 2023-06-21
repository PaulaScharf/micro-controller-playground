import serial
ser = serial.Serial()
ser.baudrate = 115200
ser.port = '/dev/ttyUSB0'
ser.open()
# ser = serial.Serial('/dev/ttyUSB0', 115200)
while True:
    print(ser.in_waiting)
    s = ser.read(10000)
    print("")
    print(s)
    print("")

