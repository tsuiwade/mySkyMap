clear all;  close all;clc;
tic;
M = csvread('J2000_5hor.csv');
points = M(:, 2:4);
jiange = 360.0/16;
ret = [];
x = 0;
y = 15;
while x < 360 && y <= 80
	if x >= -0.0000001
	    ret = [ret; x, y];
 	end
    x = x + jiange;
    if ( x >= 359.9999999 ) 
        x = x - 360 -jiange/2.0;
        y = y + jiange/2.0*sqrt(3);
    end
end

N = 15;                           % ��ʼ��Ⱥ����
d = 2;                             % �ռ�ά�����ο������ĺ�������ʽ��
ger = 10;                         % ����������     
plimit = [0,jiange;0,80-ret(end)];              % ����λ�ò�������(�������ʽ���Զ��?)������2X2����
vlimit = [-1.5, 1.5;-1.5, 1.5];    % �����ٶ�����
w = 0.8;                           % ����Ȩ��,������ʷ�ɼ������ڵ�Ӱ��0.5~1֮��
c1 = 0.5;                          % ����ѧϰ����
c2 = 0.5;                          % Ⱥ��ѧϰ���� 
for i = 1:d
    xx(:,i) = plimit(i, 1) + (plimit(i, 2) - plimit(i, 1)) * rand(N, 1);%��ʼ��Ⱥ��λ��
end                              %rand(N,1)����N��һ�з�Χ��1֮�ڵ������?
                              %��һ��,�ڶ��У�x=0+��20-0��*��1֮�ڵ��������?                                    
v = rand(N, d);                   % ��ʼ��Ⱥ���ٶ�,500��2�зֱ�������ά����
xm = xx;                           % ÿ���������ʷ���λ��
ym = zeros(1, d);                 % ��Ⱥ����ʷ���λ�ã�����ά�ȣ������?0
fxm = ones(N, 1) * (-inf);                % ÿ���������ʷ�����Ӧ�ȣ�����Ϊ0
fym = -inf;                       % ��Ⱥ��ʷ�����Ӧ��?,�����ֵ�����óɸ�����?
kk = 0;
iter = 1;  
n = length(ret);
tmp = ones(n,1)*(-500);
rev = ones(n,2)*(0);

fx = ones(n,1);

while iter <= ger

%     fx = f(xx(:,1),xx(:,2));         % ����x�еĶ�ά���ݣ�������嵱ǰ��Ӧ��?,Ϊ500��1�е�����   

    for i = 1:N                   %��ÿһ���������ж�
        ret1 = ret + xx(i,:);
        ret1(ret1(:,1) > 360) = ret1(ret1(:,1) > 360) - 360;
        
        Mdl = KDTreeSearcher(ret1);
        tmp = ones(n,1)*(-500);
        rev = ones(n,2)*(0);
        for ii = 1 : length(points)
          
            [nn,ddd] = knnsearch(Mdl,points(ii,1:2),'k',kk+1);
            if ddd > jiange 
                tmp(nn) = -2000;
               continue;
            end
            value = -ddd - points(ii,3);
            if value > tmp(nn)
                tmp(nn) = value;
                rev(nn,:) = points(ii,1:2);
            end
        end
        fx(i) = sum(tmp);

        if fxm(i) < fx(i)           %���ÿ���������ʷ�����Ӧ��С�ڸ��嵱ǰ��Ӧ��?
            fxm(i) = fx(i);         % ���¸�����ʷ�����Ӧ��?,��һ�־��ǰ�С��������
            xm(i,:) = xx(i,:);       % ���¸�����ʷ���λ��?
        end 
     end
 

    if fym < max(fxm)                  %��Ⱥ��ʷ�����Ӧ��С�ڸ������������Ӧ�ȵ�����?
            [fym, nmax] = max(fxm);    % ����Ⱥ����ʷ�����Ӧ��?,ȡ�������Ӧ�ȵ�ֵ������������λ��?
            ym = xm(nmax, :);          % ����Ⱥ����ʷ���λ��?
    end
    
     v = v * w + c1 * rand *(xm - xx) + c2 * rand *(repmat(ym, N, 1) - xx); % �ٶȸ��¹�ʽ,repmat������ym���������N��1��
    
     %%�߽��ٶȴ���
     for i=1:d 
            for j=1:N
            if  v(j,i)>vlimit(i,2);      %����ٶȴ��ڱ߽��ٶȣ�����ٶ����ر߽�
                v(j,i)=vlimit(i,2);
            end
            if  v(j,i) < vlimit(i,1)     %����ٶ�С�ڱ߽��ٶȣ�����ٶ����ر߽�
                v(j,i)=vlimit(i,1);
            end
            end
     end      
       
     xx = xx + v;                          % λ�ø���
     
     
      for i=1:d 
            for j=1:N
            if  xx(j,i)>plimit(i,2)
                xx(j,i)=plimit(i,2);
            end
            if  xx(j,i) < plimit(i,1)
                xx(j,i)=plimit(i,1);
            end
            end
      end
      
      record(iter) = fym;            %��¼����?
      
%    if times >= 10
%         cla;                    %�������ͼ��?
%         mesh(x0_1, x0_2, y0);
%         plot3(x(:,1),x(:,2),f(x(:,1),x(:,2)), 'ro');title('״̬λ�ñ仯');
%         pause(0.5);
%        times=0;
%     end
    iter = iter+1;
end
ym
toc;
ym = [0, 0];
ret2 = ret + ym;
ret2(ret2(:,1) > 360) = ret2(ret2(:,1) > 360) - 360;
Mdl = KDTreeSearcher(ret2);
tmp = ones(n,1)*(-500);
rev = ones(n,2)*(0);
for ii = 1 : length(points)
    [nn,ddd] = knnsearch(Mdl,points(ii,1:2),'k',kk+1);
    value = -ddd - points(ii,3);
    if value > tmp(nn)
        tmp(nn) = value;
        rev(nn,:) = points(ii,1:2);
    end
end
aaan = sum(tmp);

Copy_3_of_totalH(rev)



clf;

p1 = scatter(ret2(:,1), ret2(:,2));
hold on
p2 = scatter(points(:,1), points(:,2),5);
hold on
p3 = scatter(rev(:,1), rev(:,2),50,'g');
hold on


% box on;
axis equal;
xticks([0 60 120 180 240 300 360])
yticks([0 30 60 90])
xlim([-10 370]);
ylim([-10 120]);
% grid on;
ax = gca;
ax.XAxis.MinorTick = 'on';
ax.XAxis.MinorTickValues = ax.XAxis.Limits(1):10:ax.XAxis.Limits(2);
ax.YAxis.MinorTick = 'on';
ax.YAxis.MinorTickValues = ax.YAxis.Limits(1):10:ax.YAxis.Limits(2);
grid minor;
hold on;

xt=get(gca,'xtick');
for k=1:numel(xt);
	xt1{k}=sprintf('%d��',xt(k));
end
set(gca,'xticklabel',xt1);
yt=get(gca,'ytick');
for k=1:numel(yt);
	yt1{k}=sprintf('%d��',yt(k));
end
set(gca,'yticklabel',yt1);


xlabel('��λ��');
ylabel('�߶���');
line([0,360],[90,90],'color','k','linewidth',1);
line([0,0],[0,90],'color','k','linewidth',1);
line([0,360],[0,0],'color','k','linewidth',1);
line([360,360],[0,90],'color','k','linewidth',1);

% legend([p1, p2,p3],{'�����������?','�ϰ��������е�','���������?'})
