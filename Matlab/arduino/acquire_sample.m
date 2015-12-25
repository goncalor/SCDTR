
function [vals] = acquire_sample(ser_port)

    stream = [];

    byte = 0;
    while byte ~= 10
        byte = char(fread(ser_port, 1));
        stream = strcat(stream, byte);
    end

    vals = str2num(stream);
%     time = tmp(1);
%     vals = tmp(2:end);
end