%% Start LP without initial solution

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
%%
if b(k)<0

    %error('ERROR: Basic solution Unfeasible!!')
    
    c_old = c;
    A = [ones(m,1),A];
    c = [-1 , zeros(1,n)];
    
    % x_0 becomes x_1 and x_i becomes x_i+1
    N = 1:n+1;
    B = n+2:n+m+1;
    l = n+1 + k;

    e = 1;
    
    [N,B,A,b,c,v] = pivot(N,B,A,b,c,v,l,e);
    
    
    
    
    n = length(c);
    delta = zeros(1,n);
    % while there are positive coeffincients in c
    while any(c>0) 
    % Now choose e using bland's rule (page 877)
        e=N(1);
        e_i = 1;
        for tmp = 1:length(N)
            if N(tmp)<e && c(e_i)>0
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
        min = delta(1);
        for tmp = 1:length(B)
            if delta(tmp)<min
                l = B(tmp);
                l_i = tmp;
                min = delta(tmp);
            end
        end
% Detect if unbounded
        disp('delta min:')
        disp(l_i)

        if delta(l_i) == 1e+10;
            disp('Unbounded')
            x = [];
            return;
        end
        disp('e and l:')
        disp(e)
        disp(l)
        [N,B,A,b,c,v] = pivot(N,B,A,b,c,v,l,e);
        disp('v:')
        disp(v)
        pause();
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