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
    %error('ERROR: Basic solution Unfeasible!!')

    
    A = [ones(m,1),A];
    l = n + k;
    
    
    % while there are positive coeffincients in c
    while any(c>0) 
    % Now choose e using bland's rule (page 877)
        e=N(1);
        e_i = 1;
        for tmp = 1:length(N)
            if N(tmp)<e
                e = N(tmp);
                e_i = tmp;
            end
        end
    % Calculate Delta
        for B_i = 1:length(B)
            if A(B_i,e_i)>0
                delta(B_i) = b(B_i)/ A(B_i,e_i);
            else
                delta(B_i) = 1e+10;
            end
        end
% Select l that minimizes delta taking in account bland
        l = B(1);
        l_i = 1;
        min = delta(1)
        for tmp = 1:length(B)
            if delta(tmp)<min
                l = B(tmp);
                l_i = tmp;
            end
        end
% Detect if unbounded
        if delta(l_i) == 1e+10;
            disp('Unbounded')
            x = [];
            return;
        end
        disp('e and l:')
        disp(e)
        disp(l)
        [N,B,A,b,c,v] = pivot(N,B,A,b,c,v,l,e);
    end
    
    x = zeros(1,n);
    for tmp=1:n
        if any(tmp==B)
            for i=1:length(B)
                if tmp==B(i)
                    break;
                end
            end
            x(tmp)= b(i);
        end
    end

end
end

