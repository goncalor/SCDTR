
mode(0)
pt1 = [1, 60e3];  // lux, LDR resistance
pt2 = [100, 2.5e3];

pt1_log = log10(pt1);
pt2_log = log10(pt2);


// solve   log10(R) = a * log10(lux) + b   for a and b
ab = inv([pt1_log(1) 1; pt2_log(1) 1]) * [pt1_log(2); pt2_log(2)];

a = ab(1)
b = ab(2)

lux = logspace(pt1_log(1), pt2_log(1));

r = 10 ** (a * log10(lux) + b);

scf(0);
plot2d('ll', lux, r)
xtitle('LDR Characteristic', 'Illuminance (lx)', 'Resistance (Ω)');
xgrid()
xs2pdf(0,'LDR_model');
