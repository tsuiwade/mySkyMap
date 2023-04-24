
% function  requation()

clc;clear;
load data1.csv

azimuth=data1(:,1)/180*pi; %#ok<*NODEF>
altitude=data1(:,2)/180*pi;
deltaA=0-data1(:,3)/3600.0; % °
deltaH=0-data1(:,4)/3600.0; 

X=[0.*altitude+1 0.*altitude -cos(azimuth).*tan(altitude) -sin(azimuth).*tan(altitude) sec(altitude) -tan(altitude) sin(azimuth) cos(azimuth) 0.*altitude sin(2.*azimuth) cos(2.*azimuth) 0.*altitude 0.*altitude 0.*altitude 0.*altitude sin(2.*azimuth).*sec(altitude) cos(2.*azimuth).*sec(altitude) 0.*altitude 0.*altitude];
y=[0.*altitude 0.*altitude+1 sin(azimuth) -cos(azimuth) 0.*altitude 0.*altitude 0.*altitude 0.*altitude cot(altitude) 0.*altitude 0.*altitude sin(azimuth) cos(azimuth) altitude.*sin(azimuth) altitude.*cos(azimuth) 0.*altitude 0.*altitude sin(2.*azimuth) cos(2.*azimuth)];

A=[X;y];
Y=[deltaA;deltaH]; % °

%Res 为方程系数 Y° = A(弧度) * Res -> 
Res = inv(A'*A)*A'*Y;
%E 为残差
E = (A*Res - Y)*3600;
%MSE 为平均均方误差
RMS = sqrt(mse(E))
%输出 到TXT文件 科学计算法
%save ResultXISHU.txt Res -ascii; 
file1=fopen('ResultXISHU.txt','w');
for i=1:length(Res)
   fprintf(file1,'%5.5f\r\n',Res(i));
end
EA = E(1:64);
EE = E(65:end);
RMSA = sqrt(mse(EA));

RMSE = sqrt(mse(EE));
% aa = 

fclose(file1);
%输出 到TXT文件 科学计算法
%save ResultRMS.txt RMS -ascii; 

% end


% for i = 1 :64
%     if data1(i,1) < 0 
%         data1(i,1) = data1(i,1) + 360;
%     end
% end


subplot(2,1,1,'Position',[0.1 0.55 0.8 0.4]);
title('模型校正前的高度角偏差')
grid on;
box on; 
% axis equal;
xlim([0 90]);
ylim([0 300]);
xticks([0 15 30 45 60 75 90]);
% yticks([0 30 60 90])
xlabel('高度轴');
ylabel('误差（角秒）');


ax = gca;
ax.XAxis.MinorTick = 'on';
ax.XAxis.MinorTickValues = ax.XAxis.Limits(1):5:ax.XAxis.Limits(2);
ax.YAxis.MinorTick = 'on';
ax.YAxis.MinorTickValues = ax.YAxis.Limits(1):50:ax.YAxis.Limits(2);
grid minor;


hold on;

scatter(data1(:,2), data1(:,4), 28)

% scatter(points(best_way,1), points(best_way,2), 20,'filled','MarkerFaceColor','#FF8000')
% xtickformat('usd')
% legend('All stars', 'Selected stars')
xtickformat('degrees')
% ytickformat('degrees')


subplot(2,1,2,'Position',[0.1 0.1 0.8 0.35]);
title('模型校正后的高度角残差')
box on; 
% axis equal;
xlim([0 90]);
ylim([-20 20]);
xticks([0 15 30 45 60 75 90]);
% yticks([0 30 60 90])
xlabel('高度轴');
ylabel('残差（角秒）');



ax = gca;
ax.XAxis.MinorTick = 'on';
ax.XAxis.MinorTickValues = ax.XAxis.Limits(1):5:ax.XAxis.Limits(2);
ax.YAxis.MinorTick = 'on';
ax.YAxis.MinorTickValues = ax.YAxis.Limits(1):5:ax.YAxis.Limits(2);
grid minor;


hold on;

scatter(data1(:,2), EE, 28)

% scatter(points(best_way,1), points(best_way,2), 20,'filled','MarkerFaceColor','#FF8000')
% xtickformat('usd')
% legend('All stars', 'Selected stars')
xtickformat('degrees')
% ytickformat('degrees')