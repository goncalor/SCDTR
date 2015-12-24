function room = add_luminaire(room,x,y,h,max_flux,semi_ang_degrees);
% Adds a luminaire to a room, at position x,y,h, with a lamp of luminous
% flux max_flux and lens aperture semi_ang_degrees

m = -log(2)/log(cos(semi_ang_degrees*pi/180));
max_illuminance = (m+1)*max_flux/(2*pi*h^2);

num_lamps = length(room.luminaires); 
room.luminaires(num_lamps+1).x = x;
room.luminaires(num_lamps+1).y = y;
room.luminaires(num_lamps+1).h = h;
room.luminaires(num_lamps+1).max_flux = max_flux;
room.luminaires(num_lamps+1).semi_ang_degrees = semi_ang_degrees;
room.luminaires(num_lamps+1).lambertian_mode = m;
room.luminaires(num_lamps+1).max_illuminance = max_illuminance;
room.luminaires(num_lamps+1).duty_cycle = 0;
 