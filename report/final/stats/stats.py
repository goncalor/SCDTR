import serial
from time import sleep

ser = serial.Serial(
    port='/dev/ttyACM1',
    baudrate=19200,
	timeout=2
)

print(ser.isOpen())

sleep(2)    # wait for the Arduino to start after reset made by open

for i in range(1, 4):
	print('luminaire {}'.format(i))
	ser.write(b'r\n')
	ser.read()
	sleep(5.9)
	ser.write(bytes('e {}\n'.format(i).encode('ascii')))
	stats = ser.read(1000)
	print(stats.decode('ascii'))
