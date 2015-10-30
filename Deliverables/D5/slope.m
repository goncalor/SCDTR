load('output.mat')

plot(y(1:end,3), y(1:end,2), 'k.-')
hold on

plot(y(1:end-1,3), diff(y(1:end,3))./diff(y(1:end,2)), '.-')

plot(y(1:end-2,3), diff(y(1:end-1,3))./diff(diff(y(1:end,3))./diff(y(1:end,2))), 'r.-')

x1=3700:4300;
y1=1.60833*x1-6312.10833;
plot(x1,y1);