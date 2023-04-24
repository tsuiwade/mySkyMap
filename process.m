% for i = 1:64
%     hip = unnamed(i,2);
%     xingdeng = find(MM == hip);
%     unnamed1(i,1) = MM(xingdeng,7);
% end

tic
ret = [];
sum = 0;
len = 100000;
for i = 1 : len
    x = 360.*rand(64, 1);
    y = 15.+65.*rand(64, 1);
    sum = sum + Copy_3_of_totalH([x,y]);
end
av = sum / len
toc