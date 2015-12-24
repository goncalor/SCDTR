function [ Ohms ] = LDR_Voltage2Ohms( Voltage )
%LDR_Voltage2Ohms Converts from voltage at 
%   the capacitor/resistor to LDR resistance based 
%   on the circuit (considering steady state).

Ohms=(5/Voltage-1)*10000;



end

