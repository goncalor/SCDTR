%% Exercise 1
% Criar sala de referência com valores de PWM à sorte
clc
clear


r = create_room(10,1,0,0,0,0);
for i = 0:2:4,
    for j = 0:2:4,
        r = add_luminaire(r,i,j,1,50,85);
    end;
end;
    
random_pwm = zeros(length(r.luminaires),1);

for i = 1:length(r.luminaires);
    random_pwm(i)=rand*0.7;
    r = actuate_luminaire(r,i,random_pwm(i));
end;

lux = zeros(length(r.luminaires),1);

for i = 1:length(r.luminaires);
    lux(i) = read_luminaire(r,i);
end;

view_room(r,-1,5,-1,5,1);
figure 
plot(lux)

%% Get O
for i = 1:length(r.luminaires);
    r = actuate_luminaire(r,i,0);
end;
for i = 1:length(r.luminaires);
    O(i) = read_luminaire(r,i);
end;
figure
plot(O)

%% Get E
for i = 1:length(r.luminaires);
    r = actuate_luminaire(r,i,1);
    for j = 1:length(r.luminaires);
        E(i,j) = read_luminaire(r,j) - O(j);
    end;
    r = actuate_luminaire(r,i,0);
end;
figure
mesh(E)

%view_room(r,-1,5,-1,5,5);



%get lux(i)
%view room

%% Exercise 2
% Identificar matriz de transferência, correr programação linear para forçar sala a
% ter a configuração anterior

% find E, O, show: mesh(E) and plot(O)

