function tst
% SCDTR Lecture15 slide3 Mr Silva with one surplus var

f= [-7 -5];
A= [+2     1
    +4     3
    -1     0 ];
b= [+100 +240 -20 ]'; % -20 is feasible as solution is (x1,x2)=(30,40)

% x= linprog(f, A, b)   % this would be probl solving in Matlab +LPToolbox

x= linprog_alt(f, A, b) % this is an alternative (freeware) toolbox

% Objective:
%   Use linprog_alt.m for a LP written in standard form (equalities)
%
% Usage:
%   xf= linprog_alt(f, A,b)
% means
%   xf= arg_xf min f'*x,  s.t.  A*x <= b
%
% Usage2:
%   xf= linprog_alt(f, [], [], Aeq,beq)
% means
%   xf= arg_xf min f'*x,  s.t.  Aeq*x=beq
%
% Steps:
% Write the inequalities as equalities using slack/surplus/artificial vars
% See that linprog_alt.m goes ok even with negative entries b(i)
% See that linprog_alt.m goes ok with surplus and artificial vars
