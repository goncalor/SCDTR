data = dlmread('incstep_response.csv', ',');
data = data(2:end,:);    % get read of first line
data(:,1) = data(:,1) - data(1);    % subtract initial time from time collumn
data(:,1) = data(:,1)/1000;    % convert from us to ms
data(:,2) = data(:,2)/1024*5;  % convert from PWM to Volt

%plot(data(:,1), data(:,2), data(:,1), data(:,3))
plot(data(:,1), data(:,2))
xlabel('Time (ms)');
ylabel('Voltage at pin A0 (V)');
axis([0, data(end,1)]);
grid on
print('incstep_response', '-depsc2', '-mono', '-tight');
