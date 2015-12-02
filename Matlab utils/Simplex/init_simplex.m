function [ N,B,A,b,c,v] = init_simplex( A,b,c )
% [ N,B,A,b,c,v] = INIT_SIMPLEX( A,b,c )
% Basic init simplex to check if first solution is
% feasible. Based on the pseudo code present in 
% "Introduction to Alorithms" 3rd Edition by
% Cormen, page 887.

b_min = b(1);
k=1;
v=0;
[m,n]=size(A);
for i=1:length(b)
    if b(i)<b_min
        b_min=b(i);
        k=i;
    end
end

if b(k)>0
    N=1:n;
    B=n+1:n+m;
    return
else
    error('ERROR: Basic solution Unfeasible!!')
end
end

