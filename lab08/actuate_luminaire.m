function room = actuate_luminaire(room, index, new_duty_cycle)
% Set the duty cycle of a luminiare in a room

if new_duty_cycle < 0,
    new_duty_cycle = 0;
end;

if new_duty_cycle > 1,
    new_duty_cycle = 1;
end;

room.luminaires(index).duty_cycle = new_duty_cycle;