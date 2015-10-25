mode(0)
clear

// get a file location
filename = uigetfile('*.csv', pwd(), 'Choose a file');
separator = ',';

// get data from csv-file with using some separator. Ignore the first line
data_values = csvRead(filename, separator, [], [], [], [], [], 1);

// get the column names based on the first line
col_names = strsplit(mgetl(filename, 1), separator);
scale = evstr(x_mdialog('Enter scaling factors for each column', col_names(2:$), string(ones(col_names(2:$)))));

// define colors and descriptions for columns of data
colors = {'r-','g-','b-', 'c-', 'm-', 'k-', 'w-'};
descriptions = col_names(2:$);

// Create figure and loop through all columns and plot all data agains first column.
f = scf();
for i = 1:size(data_values,2)-1,
    // plot columns, with different colors
    plot(data_values(:,1), data_values(:,i+1)*scale(i), colors(i));
end

//
title("")
xlabel("Time (µs)");
ylabel("Volgage (V)");
h = legend(descriptions, 4);
xgrid()
xs2pdf(0, fileparts(filename, 'path') + fileparts(filename, 'fname'));
