# This is Octave

pt1 = [1, 60e3];  % lux, LDR resistance
pt2 = [100, 2.5e3];

pt1_log = log10(pt1);
pt2_log = log10(pt2);


% solve   log10(R) = a * log10(lux) + b   for a and b
ab = inv([pt1_log(1) 1; pt2_log(1) 1]) * [pt1_log(2); pt2_log(2)];

a = ab(1)
b = ab(2)

lux = logspace(pt1_log(1), pt2_log(1));

r = 10.^(a*log10(lux) + b);

#figure('visible', 'off');
loglog(lux, r)
grid on
#title('LDR Characteristic');
#xlabel('Illuminance (lx)');
#ylabel('Resistance ($\Omega$)');
xlabel('xx');
ylabel('yy');
print('LDR_model', '-deps2', '-tight');
