function lum = get_background_illuminance(room, x, y)
% Computes the illuminance due to background lighting at a certain point x, y in a room

background_model = room.background;
E0 = background_model(1);
Ex = background_model(2);
Ey = background_model(3);
Exx = background_model(4);
Exy = background_model(5);
Eyy = background_model(6);
lum = E0+x*Ex+y*Ey+2*x*y*Exy+x^2*Exx+y^2*Eyy;
if lum < 0,
    lum = 0;
end;