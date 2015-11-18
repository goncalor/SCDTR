function view_room(room, x_min, x_max, y_min, y_max, img_num);
% Displays the illuminance map of a room in the are [x_min, x_max]x[y_min y_max]
% Writes the value of the illuminance measured by each light sensor, at its
% location.

%Discretize map in 100 steps
disc_steps = 100;
%disc_steps = 20;
x_disc = x_min: (x_max-x_min)/disc_steps: x_max;
y_disc =y_min: (y_max-y_min)/disc_steps: y_max;

%affine map for plots
sx = length(x_disc)/(x_max-x_min);
tx = -sx*x_min;
sy = length(y_disc)/(y_max-y_min);
ty = -sy*y_min;

m = length(y_disc);
n = length(x_disc);
map = zeros(m,n);
for i = 1:m
    for j = 1:n
        map(i,j) = get_global_illuminance(room, x_disc(j), y_disc(i));
    end;
end;

figure(img_num); clf
if 1, % select 1 to show as an image, select 0 to show as mesh
    imshow(map,[]);
    for i = 1:length(room.luminaires);
        x = room.luminaires(i).x;
        y = room.luminaires(i).y;
        l(i) = read_luminaire(room,i);
        text(x*sx+tx,y*sy+ty,num2str(l(i),'%2.1f'),'Color', [1 0 0]);
    end;
else
    mesh(map)
end
