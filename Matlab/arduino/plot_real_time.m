
function data = plot_real_time(ser_port, samples_to_display, ...
    total_samples)
%ser_port = serial('/dev/ttyACM0', 'BaudRate', 115200);

figure;


for i=1:total_samples
    data(i,:) = acquire_sample(ser_port);
    if i > samples_to_display
        plot(i-samples_to_display:i, ...
            data(i-samples_to_display:i,2:end));
    else
        plot(1:i,data(1:i,2:end));
    end
    drawnow
end

%fclose(ser)
end