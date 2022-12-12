本项目是用qt来实现当前时间当前观测站点经纬度下 观测的天顶的恒星情况

也可参考本人近期博客

文件夹中的j1991.25是由hip2根据星等筛选得来的

j2000是由j1991.25 由novas函数库转换得来的 留着备用

在文件夹下还有my天文库用来计算j2000到视赤道坐标，以及视赤道坐标转为地平坐标



qt项目由一个表格显示hip序号、方位角、高度角以及星等，另一边是qt charts 显示polar charts，显示方位角和高度角，高度角的坐标轴问题 根据 https://stackoverflow.com/questions/55614512/reverse-axis-of-qpolarchart 解决



---

最后显示一下效果



2等星

![image-20221212201307020](https://images-1312692717.cos.ap-nanjing.myqcloud.com/img/image-20221212201307020.png)

3等星

![image-20221212201202828](https://images-1312692717.cos.ap-nanjing.myqcloud.com/img/image-20221212201202828.png)

和下方对比 差不多

![image-20221212201154933](https://images-1312692717.cos.ap-nanjing.myqcloud.com/img/image-20221212201154933.png)

4等星

![image-20221212201741568](https://images-1312692717.cos.ap-nanjing.myqcloud.com/img/image-20221212201741568.png)

5等星

![image-20221212201732161](https://images-1312692717.cos.ap-nanjing.myqcloud.com/img/image-20221212201732161.png)

画到五等星还算较快，后面就很慢了 可以尝试用qpen来画

6等星

![image-20221212201933313](https://images-1312692717.cos.ap-nanjing.myqcloud.com/img/image-20221212201933313.png)

6.5等星

![image-20221212202045109](https://images-1312692717.cos.ap-nanjing.myqcloud.com/img/image-20221212202045109.png)