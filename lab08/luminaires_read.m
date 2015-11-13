function lux= luminaires_read(r)
% r : struct : room
% lux : Nx1 : current illumination in the room
lux= zeros(length(r.luminaires),1);
for i = 1:length(r.luminaires);
    lux(i) = read_luminaire(r,i);
end
