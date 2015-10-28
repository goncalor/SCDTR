
function [time, vals] = acquire_sample(ser_port)

    stream = [];

    byte = 0;
    while byte ~= 10
        byte = fread(ser_port, 1, '*char');
        stream = strcat(stream, byte);
    end
    
    tmp = str2num(stream);
    time = tmp(1);
    vals = tmp(2:end);
end