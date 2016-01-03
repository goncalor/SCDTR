data = dlmread('f_.csv', ',');
data = data(2:end,:);    % get rid of first line

sample_period = 6/40;

%graphics_toolkit('gnuplot');
plot(0:sample_period:6-sample_period, data(:,1), 0:sample_period:6-sample_period, data(:,2), 0:sample_period:6-sample_period, data(:,3))
%plot(0:sample_period:6-sample_period, data(:,2), '.')
xlabel('Time (s)');
ylabel('Confort Variance (lx/s^2)');
axis([0, 6-sample_period]);
grid on
legend ({'Dev 1', 'Dev 2', 'Dev 3'}, 'location', 'east');
print('f_', '-depsc2', '-tight', '-FHelvetica');
