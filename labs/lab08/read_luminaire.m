function l = read_luminaire(room,index)
% Reads the illuminance level at the location of a certain luminaire


x = room.luminaires(index).x;
y = room.luminaires(index).y;
l = get_global_illuminance(room, x, y);