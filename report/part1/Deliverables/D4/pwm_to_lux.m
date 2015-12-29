# Octave

data = dlmread('pwm_to_lux.csv', ',');

#figure('visible', 'off');
plot(data(:,1), data(:,4));
xlabel('xx');
ylabel('yy');
axis([0, 255]);
grid on
print('pwm_to_lux', '-deps2', '-tight');
