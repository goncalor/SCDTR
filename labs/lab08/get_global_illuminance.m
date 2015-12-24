function lum = get_global_illuminance(room, x, y)
% Computes the luminance at a certain point x, y in the room, both due to
% background illuminance and the irradiance of all luminaires.

lum = get_background_illuminance(room,x,y);
num_luminaires = length(room.luminaires);
for i = 1:num_luminaires,
    lum = lum + get_luminaire_illuminance(room,i,x,y);
end;