function lum = get_luminaire_illuminance(room, index, x, y)
% Computes the illuminance at a point x,y due to the light radiated by a certain luminaire

dc = room.luminaires(index).duty_cycle;
A = room.luminaires(index).max_illuminance;
m = room.luminaires(index).lambertian_mode;
x0 = room.luminaires(index).x;
y0 = room.luminaires(index).y;
h = room.luminaires(index).h;
lum = dc*A*(1+ ((x-x0)^2+(y-y0)^2)/(h^2))^(-(m+3)/2);