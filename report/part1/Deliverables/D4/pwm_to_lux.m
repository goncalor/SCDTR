# Octave

data = dlmread('pwm_to_lux.csv', ',');

#figure('visible', 'off');
plot(data(:,1), data(:,4));
xlabel('PWM value (0--255)');
ylabel('');
axis([0, 255]);
grid on
print('pwm_to_lux', '-depslatex', '-mono');
