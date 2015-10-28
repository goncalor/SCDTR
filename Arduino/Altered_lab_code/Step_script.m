%% Arduino Step Response Script
clear
clc

file='output.mat';

%% Init
arduino_ini()
pause(1) % Pause so arduino can start

% Send step, save data
arduino_cmd({'s','<'});

arduino_end()
%% load the data and plot in ADC values

load('output_t.mat');
% Plot just the step
p=figure(); 
hold on
title('Step Response of the office in ADC values')
xlabel('Time in milliseconds')
ylabel('Capacitor readings 0 to 1024')
figure(p)
plot((y(:,end)-y(1,end))/10,y(:,2), '.-')    
%  Plot the tau intersect
figure(p)
plot((y(:,end)-y(1,end))/10, ...
    ones(1,length(y))*0.63212*max(y(:,2)))


%% load the data and plot in Voltage 

load(file); 
% Plot just the step
p=figure(); 
hold on
title('Step Response of the office in Voltage values')
xlabel('Time in milliseconds')
ylabel('Capacitor readings 0 to 5 V')
figure(p)
plot((y(:,end)-y(1,end))/10,y(:,2)/1024*5, '.-')    
%  Plot the tau intersect
figure(p)
plot((y(:,end)-y(1,end))/10, ...
    ones(1,length(y))*0.63212*max(y(:,2))/1024*5)

%% load the data and plot in LDR Resistance

load(file); 

y_ldr= arrayfun(@(x) LDR_Voltage2Ohms(x),y(:,2)/1024*5);

% Plot just the step
p=figure(); 
hold on
title('Step Response of the office in LDR Ohms')
xlabel('Time in milliseconds')
ylabel('LDR Resistance Ohms')
figure(p)
plot((y(:,end)-y(1,end))/10,y_ldr, '.-')
set(gca,'yscale','log');


%% Now plot two files


load('output_t.mat');
% Plot just the step
p=figure(); 
hold on
title('Step Response of the office in ADC values')
xlabel('Time in milliseconds')
ylabel('Capacitor readings 0 to 1024')
figure(p)
plot((y(:,end)-y(1,end))/10,y(:,2), '.-')   

load('output_cap.mat');
figure(p)
plot((y(:,end)-y(1,end))/10,y(:,2), 'r.-')   
%  Plot the tau intersect
figure(p)
plot((y(:,end)-y(1,end))/10, ...
    ones(1,length(y))*0.63212*max(y(:,2)))





