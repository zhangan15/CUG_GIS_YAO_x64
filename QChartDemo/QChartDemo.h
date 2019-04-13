#pragma once

#include <QtWidgets/QWidget>
#include "ui_QChartDemo.h"
//添加QCharts必须的头文件
#include<qchart.h>
#include<qchartview.h>

//使用QtCharts必须先声明的它的命名空间QT_CHARTS_USE_NAMESPACE，当然也可以这样声明using namespace QT_CHARTS_NAMESPACE;
using namespace QT_CHARTS_NAMESPACE;

class QChartDemo : public QWidget
{
	Q_OBJECT

public:
	QChartDemo(QWidget *parent = Q_NULLPTR);
	void buildCurves();//创建曲线图
	void buildBars();//创建柱状图
	void buildLines();//创建折线图
	void buildScatter();//创建散点图
	void buildPie();//创建饼图

private:
	Ui::QChartDemoClass ui;

	QChart * chart;
	QChartView *chartview;
};
