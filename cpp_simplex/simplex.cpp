#include "simplex.hpp"



pivot_struct pivot(pivot_struct in, int e, int l){
    pivot_struct out(in);

    out.b[e]=in.b[l]/in.A[l][e];

    
    return out;
}

/*

function [N,B,A,b,c,v] = pivot(N,B,A,b,c,v,l,e)
[N,B,A,b,c,v] = PIVOT(N,B,A,b,c,v,l,e)
Pivot method used by the simplex algorithm based on the pseudo code present in "Introduction to Alorithms" 3rd Edition by Cormen, page 869.
*/


// on the new matrices i_e becomes i_l and vice versa


//Compute the coefficients of the equation for the new  basic variable x(e)

/*
% Get index of e
for i = 1:length(N)
if N(i) == e
i_e = i;
end
end

% Get index of l
for i = 1:length(B)
if B(i) == l
i_l = i;
end
end

b(i_l) = b(i_l)/A(i_l,i_e);
for i = 1:length(N)
if N(i) ~= e
A(i_l,i) = A(i_l,i)/A(i_l,i_e);
end
end

A(i_l,i_e) = 1/A(i_l,i_e);

% Compute the coefficients of the remaining constrains

for i = 1:length(B)
if B(i) ~= l
b(i) = b(i) - A(i,i_e)*b(i_l);
for j = 1 : length(N)
if N(j) ~= e
A(i,j) = A(i,j) - A(i,i_e)*A(i_l,j);
end
end
A(i,i_e) = - A(i,i_e) * A(i_l,i_e);
end
end

% Compute the objective function

v=v+c(e)*b(i_l);
for j = 1 : length(N)
if N(j) ~= e
c(j) = c(j) - c(e)*A(i_l,j);
end
end

c(i_e) = -c(i_e)*A(i_l,i_e);

% Compute new sets of basic and nonbasic variables


N(i_e)=l;
B(i_l)=e;
%  for j = 1 : length(N)
%      if N(j) == e
%         N(j) = l;
%      end
%  end
%
%   for j = 1 : length(B)
%      if N(j) == l
%         N(j) = e;
%      end
%  end
%

end
*/
