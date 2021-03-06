%% Arduino Steady State Script
clear
clc

file='output.mat';

%% Init
arduino_ini('/dev/ttyACM0')
pause(1) % Pause so arduino can start

% Send step, save data
arduino_cmd({'<'});

arduino_end()
%%
load('output_t.mat')

% Plot steady state
p=figure(); 
hold on
title('Steady State characteristic of the office in ADC values')
xlabel('PWM val')
ylabel('Capacitor readings 0 to 1024')
figure(p)
plot(y(:,1),y(:,2), '.-')
