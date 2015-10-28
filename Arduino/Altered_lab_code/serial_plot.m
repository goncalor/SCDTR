


fclose(ser_port)
delete(ser_port)
clear ser_port

ser_port = serial('COM3');
set(ser_port, 'BaudRate', 115200);

fopen(ser_port)
pause(1)
ser_port.RecordName = 'myfile.txt';
record(ser_port)
fprintf(ser_port, 'L\n');
data = plot_real_time(ser_port, 499,5000)


fclose(ser_port)
delete(ser_port)


clear ser_port
