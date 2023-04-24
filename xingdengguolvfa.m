clear; 
% M = csvread('J2000_5hor.csv');
MM = csvread('J2000_5horzhengfangxing.csv');

% MM = sortrows(M,4);
% MMM = MM(22:85,:);

clf;
% box on;
axis equal;
xticks([0 60 120 180 240 300 360])
yticks([0 30 60 90])
xlim([-10 370]);
ylim([-10 100]);
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
	xt1{k}=sprintf('%d°',xt(k));
end
set(gca,'xticklabel',xt1);
yt=get(gca,'ytick');
for k=1:numel(yt);
	yt1{k}=sprintf('%d°',yt(k));
end
set(gca,'yticklabel',yt1);


xlabel('方位轴','FontWeight','bold');
ylabel('高度轴','FontWeight','bold');
line([0,360],[90,90],'color','k','linewidth',1.5);
line([0,0],[0,90],'color','k','linewidth',1.5);
line([0,360],[0,0],'color','k','linewidth',1.5);
line([360,360],[0,90],'color','k','linewidth',1.5);

scatter(MM(:,2), MM(:,3))
Copy_3_of_totalH(MM(:,2:3))