import serial
from time import sleep

ser = serial.Serial(
        port='/dev/ttyACM0',
        baudrate=19200,
        timeout=2
)

print(ser.isOpen())

sleep(2)    # wait for the Arduino to start after reset made by open
ser.read(3)    # get rid of "D\r\n"

for s in ['e', 'n', 'f']:
    stats = {}
    for i in range(1, 4):
        ser.write(b'r\n')
        ser.read(3)    # get rid of "D\r\n"

        ser.write(b's 1 1\n')
        sleep(0.1)
        ser.write(b's 3 1\n')

        sleep(5.9)
        ser.write(bytes('{} {}\n'.format(s, i).encode('ascii')))
        stats[i] = ser.read(1000).splitlines()

    with open('{}_.csv'.format(s), 'w') as f:
        f.write('luminaire 1, luminaire 2, luminaire 3\n')
        print(str(stats))
        #f.write(str(stats))

        for l1, l2, l3 in zip(stats[1], stats[2], stats[3]):
            f.write('{}, {}, {}\n'.format(l1.decode('ascii'), l2.decode('ascii'), l3.decode('ascii')))

    f.close()
