
ax = polaraxes;
ax.RAxis.Label.String = 'Elevation';
ax.ThetaAxis.Label.String = 'Azimuth';
ax.ThetaZeroLocation = 'bottom';
hold on;
polarscatter(double(points(:,1))./180.0 * pi,points(:,2),20)
thetalim([0 360])
rticks(0:22.5:90)
hold on;
polarscatter(double(points(best_way,1))./180.0 * pi,points(best_way,2), 30,'r','filled')
legend('All stars', 'Selected stars')



ax = polaraxes;
ax.RAxis.Label.String = 'Elevation axis';
ax.ThetaAxis.Label.String = 'Azimuth axis';
ax.ThetaZeroLocation = 'bottom';
hold on;
polarscatter(double(points(:,1))./180.0 * pi, 90-points(:,2),20)
thetalim([0 360])
rticks(0:22.5:90)
hold on;
polarscatter(double(points(best_way,1))./180.0 * pi, 90-points(best_way,2), 30,'filled','MarkerFaceColor','#FF8000')
legend('All stars', 'Selected stars')
ax.ThetaTickLabel = string(ax.ThetaTickLabel) + char(176)
rticks([0 22.5 45  67.5 90])                        % 在r = 50,100,200处显示刻度
rticklabels({'90','67.5','45','22.5','0'})            % 在刻度线处加标记
ax.RTickLabel = string(ax.RTickLabel) + char(176)
ax.RAxisLocation = 90;




ax = polaraxes;
ax.RAxis.Label.String = '高度轴';
ax.ThetaAxis.Label.String = '方位轴';
ax.ThetaZeroLocation = 'bottom';
hold on;
polarscatter(double(points(best_way,1))./180.0 * pi, 90-points(best_way,2), 20)
thetalim([0 360])
rticks(0:22.5:90)
hold on;

ax.ThetaTickLabel = string(ax.ThetaTickLabel) + char(176)
rticks([0 22.5 45  67.5 90])                        % 在r = 50,100,200处显示刻度
rticklabels({'90','67.5','45','22.5','0'})            % 在刻度线处加标记
ax.RTickLabel = string(ax.RTickLabel) + char(176)
ax.RAxisLocation = 90;
