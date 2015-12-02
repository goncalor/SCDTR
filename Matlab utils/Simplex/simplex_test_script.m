%% Script to test simplex
clear
clc
% Test based on example page 865

% Input:
A = [1, 1, 3;
     2, 2, 5;
     4, 1, 2];
b = [30;24;36];
c = [3,1,2];
 

[ x] = simplex( A,b,c ) 
% it's working... maybe

% Expected Output:
N = [8,4,0];

%% Now test using lab08 example
clc
clear
load('lab08_simplex_test')
% add upper bounds as constrains
A=[A;eye(9)];
b=[b;ub];   
clear('ub')
% transform minimization problem in maximization
% min c'.x <=> max -c'.x
c=-f;
clear('f')
[ x] = simplex( A,b,c ) 
