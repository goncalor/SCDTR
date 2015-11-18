function room = create_room(E0,Ex,Ey,Exx,Exy,Eyy);
% Creates and empty room with a bilinear background illuminance pattern:
% E = E0 + x*Ex + y*Ey + x^2*Exx + y^2*Eyy + x*y*Exy
% To add luminaires to the room, use function add_luminaire

room = struct('background', [E0, Ex, Ey, Exx, Exy, Eyy], 'luminaires', []);
