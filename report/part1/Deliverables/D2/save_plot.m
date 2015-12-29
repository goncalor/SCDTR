data = dlmread("steadystate.csv", ",");

plot(data(:,1), data(:,3));
#xlabel("PWM value (0--255)");
#ylabel("Voltage at \\texttt{A0} (\\si{\\volt})");
xlabel("xx");
ylabel("yy");
axis([0, 255]);
grid on
#set(gcf, "paperposition", [0 0 8.00000 6.00000]);
#set(gcf, "papersize", [8 6]);
#get(gcf);
#get(gca)
##set(gca,'LooseInset',get(gca,'TightInset'));
#set(gca, 'LooseInset', [0.01, 0.07, 0.01, 0.01]);
print("steady_state", "-deps2", "-tight");
