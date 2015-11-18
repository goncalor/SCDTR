%Example script

%function prototypes
%room = create_room(E0,Ex,Ey,Exx,Exy,Eyy);
%room = add_lamp(room,x,y,h,max_flux,semi_ang_degrees);
%room = actuate_luminaire(room, index, new_duty_cycle);
%add_luminaire(room,x,y,h,max_flux,semi_ang_degrees);
    % Adds a luminaire to a room, at position x,y,h, with a lamp of luminous
    % flux max_flux and lens aperture semi_ang_degrees
%l = read_luminaire(room,index)
%view_room(room, x_min, x_max, y_min, y_max, img_num);
    % Displays the illuminance map of a room in the are [x_min, x_max]x[y_min y_max]
    % Writes the value of the illuminance measured by each light sensor, at its
    % location.


r = create_room(10,1,0,0,0,0);
for i = 0:2:4,
    for j = 0:2:4,
        r = add_luminaire(r,i,j,1,50,85);
    end;
end;

for i = 1:length(r.luminaires);
    r = actuate_luminaire(r,i,rand);
end;

for i = 1:length(r.luminaires);
    l(i) = read_luminaire(r,i);
end;

view_room(r,-1,5,-1,5,5);
