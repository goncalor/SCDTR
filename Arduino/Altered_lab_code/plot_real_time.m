samples_to_display = 100;

figure;
xlim([0 100])
data =zeros(1, 500);
for i=1:500
    
 data(i) = rand();
 
 
 
 if i > samples_to_display
     plot(data(i-samples_to_display:i));
 end
 drawnow
 
end