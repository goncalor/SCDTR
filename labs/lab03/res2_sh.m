function res2_sh

res2  % script filling vars x1, x2, ..., x6

for i=1:6
    x= eval(['x' num2str(i)]);
    subplot(2,3,i)
    %plot(x(:,3)-x(1,3),x(:,1:2),'.-')
    plot(x(:,3)-x(1,3),x(:,2),'.-')
end
