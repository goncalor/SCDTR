
mode(0)
final_val_LDR = 3.06;   // Volt
final_val_LED = 2.5
step_time = 22350;   // microseconds

[aux, min_index] = min(abs(scaled_values(:,2) - final_val_LDR * (1 - exp(-1))));

tau = scaled_values(min_index, 1) - step_time

k0 = final_val_LED / final_val_LDR


s=poly(0,'s')

G = k0/(1 + s*tau)
// do some magic
G = syslin('c',G)
// bode(G)
// root locus
evans(G);
sgrid;

