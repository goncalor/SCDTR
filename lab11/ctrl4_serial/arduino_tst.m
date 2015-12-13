function arduino_tst

%arduino_ini; arduino_cmd({'C', 's', '<'}); arduino_end
%arduino_ini; arduino_cmd({'C', 'r0', 's', '<'}); arduino_end
%arduino_ini; arduino_cmd({'r100', 's', '<'}); arduino_end
%arduino_ini; arduino_cmd({'r900', 's', '<'}); arduino_end
%arduino_ini; arduino_cmd({'r900', 'x', '<'}); arduino_end

cLst= {'r0', 's', 'r200', 's', '<'};
cLst= {'o0', 'D1000', 'r200', 's', '<'};
cLst= {'o0', 'D1000', 'c02', 'c10', 'c30', 'C', 'r800', 'x', '<'};
% cLst= {'o0', 'D1000', 'c00.35', 'c1-0.314', 'c3-1', 'C', 'r800', 'x', '<'};

arduino_ini; arduino_cmd(cLst); arduino_end
