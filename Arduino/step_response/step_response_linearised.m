data = dlmread('step_response.csv', ',');
data = data(2:end,:);    % get read of first line
data(:,1) = data(:,1) - data(1);    % subtract initial time from time collumn
data(:,1) = data(:,1)/1000;    % convert from us to ms
data(:,2) = data(:,2)/1024*5;  % convert from PWM to Volt
% now convert from Volt to lux
a = -0.6901;
b = 4.7782;
r = 10000;
data(:,2) = (10^(-b/a)) .* ((5.*r./data(:,2).-r).^(1/a));

%plot(data(:,1), data(:,2), data(:,1), data(:,3))
plot(data(:,1), data(:,2))
xlabel('Time (ms)');
ylabel('Measured illuminance (lx)');
axis([0, data(end,1)]);
grid on
print('step_response_linearised', '-depsc2', '-mono', '-tight');
