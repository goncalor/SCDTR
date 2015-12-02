%% Script to test init_simplex
clear
clc
% Test based on example page 865

% Input:
A = [1, 1, 3;
     2, 2, 5;
     4, 1, 2];
b = [30;24;36];
c = [3,1,2];
 

[ N,B,A,b,c,v] = init_simplex( A,b,c ) 
% it's working... maybe

% Expected Output:
N = [1,2,3];
B = [4,5,6];
A = [1, 1, 3;
     2, 2, 5;
     4, 1, 2];
b = [30;24;36];
c = [3,1,2];
v = 0;
