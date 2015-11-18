function r = luminaires_actuate(r,pwm)
% r : struct : room
% pwm : Nx1 : actuation of the luminaires
for i = 1:length(r.luminaires);
    r = actuate_luminaire(r,i,pwm(i));
end
