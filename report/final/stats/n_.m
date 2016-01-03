data = dlmread('n_.csv', ',');
data = data(2:end,:);    % get rid of first line

sample_period = 6/40;
plot(0:sample_period:6-sample_period, data(:,1), 0:sample_period:6-sample_period, data(:,2), 0:sample_period:6-sample_period, data(:,3))
%plot(0:sample_period:6-sample_period, data(:,2), '.')
xlabel('Time (s)');
ylabel('Comfort error (lx)');
axis([0, 6-sample_period]);
grid on
legend ({'Lum 1', 'Lum 2', 'Lum 3'}, 'location', 'east');
print('n_', '-depsc2', '-tight');
