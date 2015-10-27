%% Arduino Step Response Scritp
clear
clc

file='output.mat';

%% Init
arduino_ini()
pause(3)

% Send step, save data
arduino_cmd({'s','<'})

arduino_end()
%% load the data...

load(file);

%% plot just the step
p=figure();
hold on
plot(y(:,end)-y(1,end),y(:,2), '.-')

%%  Calculations
figure(p)
plot(y(:,end)-y(1,end),ones(1,length(y))*0.63*max(y(:,2)))


%%

