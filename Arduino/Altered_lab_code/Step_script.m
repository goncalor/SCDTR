%% Arduino Step Response Scritp

arduino_ini();

%%
arduino_cmd('s');

arduino_interf('get_array', 'step.txt')

%%

arduino_end();