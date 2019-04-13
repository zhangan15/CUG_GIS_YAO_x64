#pragma once

#include <QtWidgets/QWidget>
#include "ui_QChartDemo.h"
//���QCharts�����ͷ�ļ�
#include<qchart.h>
#include<qchartview.h>

//ʹ��QtCharts���������������������ռ�QT_CHARTS_USE_NAMESPACE����ȻҲ������������using namespace QT_CHARTS_NAMESPACE;
using namespace QT_CHARTS_NAMESPACE;

class QChartDemo : public QWidget
{
	Q_OBJECT

public:
	QChartDemo(QWidget *parent = Q_NULLPTR);
	void buildCurves();//��������ͼ
	void buildBars();//������״ͼ
	void buildLines();//��������ͼ
	void buildScatter();//����ɢ��ͼ
	void buildPie();//������ͼ

private:
	Ui::QChartDemoClass ui;

	QChart * chart;
	QChartView *chartview;
};
