#include "QChartDemo.h"
#include<QHBoxLayout>
#include<qvalueaxis.h>
#include<qbarseries.h>
#include<QBarCategoryAxis>
#include<qbarset.h>
#include<qsplineseries.h>
#include<qscatterseries.h>
#include<qpieseries.h>



QChartDemo::QChartDemo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//buildCurves(); //曲线图
	//buildBars();     //垂直柱状图
	//buildLines();   //折线图 基本上与曲线图相同
	//buildScatter();   //散点图
	buildPie();       //饼图

}

void QChartDemo::buildCurves()
{
	QSplineSeries *series=new QSplineSeries();

	//建立坐标轴和选择窗口布局
	QHBoxLayout *layout;
	QValueAxis *axisX;
	QValueAxis *axisY;

	chart = new QChart;
	chartview = new QChartView(chart);

	//建立坐标轴
	axisX = new QValueAxis;
	axisX->setRange(0, 20);//设置范围
	axisX->setLabelFormat("%u");//设置刻度的格式

	axisX->setGridLineVisible(true);   //网格线可见
	axisX->setTickCount(10);       //设置多少个大格
	axisX->setMinorTickCount(1);   //设置每个大格里面小刻度线的数目
	axisX->setTitleText("<h1>X</h1>");  //设置描述

	axisY = new QValueAxis;
	axisY->setRange(0, 10);
	axisY->setLabelFormat("%u");

	axisY->setGridLineVisible(true);
	axisY->setTickCount(10);
	axisY->setMinorTickCount(1);
	axisY->setTitleText("<h1>Y</h1>");

	chart->addAxis(axisX, Qt::AlignBottom); //下：Qt::AlignBottom  上：Qt::AlignTop
	chart->addAxis(axisY, Qt::AlignLeft);   //左：Qt::AlignLeft    右：Qt::AlignRight

	//创建数据源
	series->setPen(QPen(Qt::red, 1, Qt::SolidLine));//设置曲线显示效果
	series->append(0, 6); //这里是添加数据的两种方式
	series->append(2, 4);
	series->append(3, 8);
	series->append(6, 4);
	series->append(8, 6);
	series->append(10, 5);
	*series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

	chart->setTitle(u8"曲线图实例");
	chart->setAnimationOptions(QChart::SeriesAnimations);//设置曲线动画模式
	chart->legend()->hide(); //隐藏图例
	chart->addSeries(series);//输入数据
	chart->setAxisX(axisX, series);
	chart->setAxisY(axisY, series);

	layout = new QHBoxLayout;
	layout->addWidget(chartview);
	ui.widget->setLayout(layout);
	chartview->setRenderHint(QPainter::Antialiasing);//渲染 防止图形走样
	
}
void QChartDemo::buildBars()
{
	chart = new QChart;
	chartview = new QChartView(chart);
	//创建条集合实例并填充数据
	QBarSet *set0 = new QBarSet("Jane");
	QBarSet *set1 = new QBarSet("John");
	QBarSet *set2 = new QBarSet("Axel");
	QBarSet *set3 = new QBarSet("Mary");
	QBarSet *set4 = new QBarSet("Samantha");

	*set0 << 1 << 2 << 3 << 4 << 5 << 6;
	*set1 << 5 << 0 << 0 << 4 << 0 << 7;
	*set2 << 3 << 5 << 8 << 13 << 8 << 5;
	*set3 << 5 << 6 << 7 << 3 << 4 << 5;
	*set4 << 9 << 7 << 5 << 3 << 1 << 2;

	//条集合添加到条系列中
	QBarSeries *series = new QBarSeries();
	series->append(set0);
	series->append(set1);
	series->append(set2);
	series->append(set3);
	series->append(set4);

	chart->addSeries(series);//输入数据
	chart->setTitle(u8"柱形图实例");
	chart->setAnimationOptions(QChart::SeriesAnimations);//添加动画

	QStringList categories;
	categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";

	QBarCategoryAxis *axis = new QBarCategoryAxis();//创建柱状图时间轴
	axis->append(categories); //设置横轴坐标值，这里是用qstring
	chart->createDefaultAxes(); //创建图表坐标轴
	chart->setAxisX(axis, series);// 将该坐标轴设置为图表的横坐标轴，并将它与条联系起来
	
	chart->legend()->setVisible(true);//设置为图例可见
	chart->legend()->setAlignment(Qt::AlignBottom);

	QHBoxLayout *layout;
	layout = new QHBoxLayout;
	layout->addWidget(chartview);
	ui.widget->setLayout(layout);
	chartview->setRenderHint(QPainter::Antialiasing);//渲染 防止图形走样

}
void QChartDemo::buildLines()
{
	QLineSeries* series = new QLineSeries();
	//建立坐标轴和选择窗口布局
	QHBoxLayout *layout;
	QValueAxis *axisX;
	QValueAxis *axisY;

	chart = new QChart;
	chartview = new QChartView(chart);

	axisX = new QValueAxis;
	axisX->setRange(0, 20);//设置范围
	axisX->setLabelFormat("%u");//设置刻度的格式

	axisX->setGridLineVisible(true);   //网格线可见
	axisX->setTickCount(10);       //设置多少个大格
	axisX->setMinorTickCount(1);   //设置每个大格里面小刻度线的数目
	axisX->setTitleText("<h1>X</h1>");  //设置描述

	axisY = new QValueAxis;
	axisY->setRange(0, 10);
	axisY->setLabelFormat("%u");

	axisY->setGridLineVisible(true);
	axisY->setTickCount(10);
	axisY->setMinorTickCount(1);
	axisY->setTitleText("<h1>Y</h1>");

	chart->addAxis(axisX, Qt::AlignBottom); //下：Qt::AlignBottom  上：Qt::AlignTop
	chart->addAxis(axisY, Qt::AlignLeft);   //左：Qt::AlignLeft    右：Qt::AlignRight

	//创建数据源
	series->setPen(QPen(Qt::red, 1, Qt::SolidLine));//设置曲线显示效果
	series->append(0, 6); //这里是添加数据的两种方式
	series->append(2, 4);
	series->append(3, 8);
	series->append(6, 4);
	series->append(8, 6);
	series->append(10, 5);
	*series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

	chart->setTitle(u8"折线图实例");
	chart->setAnimationOptions(QChart::SeriesAnimations);//设置曲线动画模式
	chart->legend()->hide(); //隐藏图例
	chart->addSeries(series);//输入数据
	chart->setAxisX(axisX, series);
	chart->setAxisY(axisY, series);

	layout = new QHBoxLayout;
	layout->addWidget(chartview);
	ui.widget->setLayout(layout);
	chartview->setRenderHint(QPainter::Antialiasing);//渲染 防止图形走样

	

}
void QChartDemo::buildScatter()
{
	QScatterSeries* series = new QScatterSeries();

	//建立坐标轴和选择窗口布局
	QHBoxLayout *layout;
	QValueAxis *axisX;
	QValueAxis *axisY;

	chart = new QChart;
	chartview = new QChartView(chart);

	axisX = new QValueAxis;
	axisX->setRange(0, 20);//设置范围
	axisX->setLabelFormat("%u");//设置刻度的格式

	axisX->setGridLineVisible(true);   //网格线可见
	axisX->setTickCount(10);       //设置多少个大格
	axisX->setMinorTickCount(1);   //设置每个大格里面小刻度线的数目
	axisX->setTitleText("<h1>X</h1>");  //设置描述

	axisY = new QValueAxis;
	axisY->setRange(0, 10);
	axisY->setLabelFormat("%u");

	axisY->setGridLineVisible(true);
	axisY->setTickCount(10);
	axisY->setMinorTickCount(1);
	axisY->setTitleText("<h1>Y</h1>");

	chart->addAxis(axisX, Qt::AlignBottom); //下：Qt::AlignBottom  上：Qt::AlignTop
	chart->addAxis(axisY, Qt::AlignLeft);   //左：Qt::AlignLeft    右：Qt::AlignRight

	//创建数据源
	series->setPen(QPen(Qt::red, 1, Qt::SolidLine));//设置曲线显示效果
	series->append(1, 6); //这里是添加数据的两种方式
	series->append(2, 4);
	series->append(3, 8);
	series->append(6, 4);
	series->append(8, 6);
	series->append(10, 5);
	*series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(19, 2);

	chart->setTitle(u8"散点图实例");
	chart->setAnimationOptions(QChart::SeriesAnimations);//设置曲线动画模式
	chart->legend()->hide(); //隐藏图例
	chart->addSeries(series);//输入数据
	chart->setAxisX(axisX, series);
	chart->setAxisY(axisY, series);

	layout = new QHBoxLayout;
	layout->addWidget(chartview);
	ui.widget->setLayout(layout);
	chartview->setRenderHint(QPainter::Antialiasing);//渲染 防止图形走样
}
void QChartDemo::buildPie()
{
	QHBoxLayout *layout = new QHBoxLayout;
	chart = new QChart;
	chartview = new QChartView(chart);

	QPieSeries *series = new QPieSeries();
	series->append("Jane", 1);
	series->append("Joe", 2);
	series->append("Andy", 3);
	series->append("Barbara", 4);
	series->append("Axel", 5);

	QPieSlice *slice = series->slices().at(3);
	slice->setExploded();
	slice->setLabelVisible();
	slice->setPen(QPen(Qt::darkGreen, 2));
	slice->setBrush(Qt::green);

	chart->addSeries(series);
	chart->setTitle(u8"饼图实例");
	chart->legend()->hide();

	chartview->setRenderHint(QPainter::Antialiasing);

	layout = new QHBoxLayout;
	layout->addWidget(chartview);
	ui.widget->setLayout(layout);
	chartview->setRenderHint(QPainter::Antialiasing);//渲染 防止图形走样
}
