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
	//buildCurves(); //����ͼ
	//buildBars();     //��ֱ��״ͼ
	//buildLines();   //����ͼ ������������ͼ��ͬ
	//buildScatter();   //ɢ��ͼ
	buildPie();       //��ͼ

}

void QChartDemo::buildCurves()
{
	QSplineSeries *series=new QSplineSeries();

	//�����������ѡ�񴰿ڲ���
	QHBoxLayout *layout;
	QValueAxis *axisX;
	QValueAxis *axisY;

	chart = new QChart;
	chartview = new QChartView(chart);

	//����������
	axisX = new QValueAxis;
	axisX->setRange(0, 20);//���÷�Χ
	axisX->setLabelFormat("%u");//���ÿ̶ȵĸ�ʽ

	axisX->setGridLineVisible(true);   //�����߿ɼ�
	axisX->setTickCount(10);       //���ö��ٸ����
	axisX->setMinorTickCount(1);   //����ÿ���������С�̶��ߵ���Ŀ
	axisX->setTitleText("<h1>X</h1>");  //��������

	axisY = new QValueAxis;
	axisY->setRange(0, 10);
	axisY->setLabelFormat("%u");

	axisY->setGridLineVisible(true);
	axisY->setTickCount(10);
	axisY->setMinorTickCount(1);
	axisY->setTitleText("<h1>Y</h1>");

	chart->addAxis(axisX, Qt::AlignBottom); //�£�Qt::AlignBottom  �ϣ�Qt::AlignTop
	chart->addAxis(axisY, Qt::AlignLeft);   //��Qt::AlignLeft    �ң�Qt::AlignRight

	//��������Դ
	series->setPen(QPen(Qt::red, 1, Qt::SolidLine));//����������ʾЧ��
	series->append(0, 6); //������������ݵ����ַ�ʽ
	series->append(2, 4);
	series->append(3, 8);
	series->append(6, 4);
	series->append(8, 6);
	series->append(10, 5);
	*series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

	chart->setTitle(u8"����ͼʵ��");
	chart->setAnimationOptions(QChart::SeriesAnimations);//�������߶���ģʽ
	chart->legend()->hide(); //����ͼ��
	chart->addSeries(series);//��������
	chart->setAxisX(axisX, series);
	chart->setAxisY(axisY, series);

	layout = new QHBoxLayout;
	layout->addWidget(chartview);
	ui.widget->setLayout(layout);
	chartview->setRenderHint(QPainter::Antialiasing);//��Ⱦ ��ֹͼ������
	
}
void QChartDemo::buildBars()
{
	chart = new QChart;
	chartview = new QChartView(chart);
	//����������ʵ�����������
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

	//��������ӵ���ϵ����
	QBarSeries *series = new QBarSeries();
	series->append(set0);
	series->append(set1);
	series->append(set2);
	series->append(set3);
	series->append(set4);

	chart->addSeries(series);//��������
	chart->setTitle(u8"����ͼʵ��");
	chart->setAnimationOptions(QChart::SeriesAnimations);//��Ӷ���

	QStringList categories;
	categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";

	QBarCategoryAxis *axis = new QBarCategoryAxis();//������״ͼʱ����
	axis->append(categories); //���ú�������ֵ����������qstring
	chart->createDefaultAxes(); //����ͼ��������
	chart->setAxisX(axis, series);// ��������������Ϊͼ��ĺ������ᣬ������������ϵ����
	
	chart->legend()->setVisible(true);//����Ϊͼ���ɼ�
	chart->legend()->setAlignment(Qt::AlignBottom);

	QHBoxLayout *layout;
	layout = new QHBoxLayout;
	layout->addWidget(chartview);
	ui.widget->setLayout(layout);
	chartview->setRenderHint(QPainter::Antialiasing);//��Ⱦ ��ֹͼ������

}
void QChartDemo::buildLines()
{
	QLineSeries* series = new QLineSeries();
	//�����������ѡ�񴰿ڲ���
	QHBoxLayout *layout;
	QValueAxis *axisX;
	QValueAxis *axisY;

	chart = new QChart;
	chartview = new QChartView(chart);

	axisX = new QValueAxis;
	axisX->setRange(0, 20);//���÷�Χ
	axisX->setLabelFormat("%u");//���ÿ̶ȵĸ�ʽ

	axisX->setGridLineVisible(true);   //�����߿ɼ�
	axisX->setTickCount(10);       //���ö��ٸ����
	axisX->setMinorTickCount(1);   //����ÿ���������С�̶��ߵ���Ŀ
	axisX->setTitleText("<h1>X</h1>");  //��������

	axisY = new QValueAxis;
	axisY->setRange(0, 10);
	axisY->setLabelFormat("%u");

	axisY->setGridLineVisible(true);
	axisY->setTickCount(10);
	axisY->setMinorTickCount(1);
	axisY->setTitleText("<h1>Y</h1>");

	chart->addAxis(axisX, Qt::AlignBottom); //�£�Qt::AlignBottom  �ϣ�Qt::AlignTop
	chart->addAxis(axisY, Qt::AlignLeft);   //��Qt::AlignLeft    �ң�Qt::AlignRight

	//��������Դ
	series->setPen(QPen(Qt::red, 1, Qt::SolidLine));//����������ʾЧ��
	series->append(0, 6); //������������ݵ����ַ�ʽ
	series->append(2, 4);
	series->append(3, 8);
	series->append(6, 4);
	series->append(8, 6);
	series->append(10, 5);
	*series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

	chart->setTitle(u8"����ͼʵ��");
	chart->setAnimationOptions(QChart::SeriesAnimations);//�������߶���ģʽ
	chart->legend()->hide(); //����ͼ��
	chart->addSeries(series);//��������
	chart->setAxisX(axisX, series);
	chart->setAxisY(axisY, series);

	layout = new QHBoxLayout;
	layout->addWidget(chartview);
	ui.widget->setLayout(layout);
	chartview->setRenderHint(QPainter::Antialiasing);//��Ⱦ ��ֹͼ������

	

}
void QChartDemo::buildScatter()
{
	QScatterSeries* series = new QScatterSeries();

	//�����������ѡ�񴰿ڲ���
	QHBoxLayout *layout;
	QValueAxis *axisX;
	QValueAxis *axisY;

	chart = new QChart;
	chartview = new QChartView(chart);

	axisX = new QValueAxis;
	axisX->setRange(0, 20);//���÷�Χ
	axisX->setLabelFormat("%u");//���ÿ̶ȵĸ�ʽ

	axisX->setGridLineVisible(true);   //�����߿ɼ�
	axisX->setTickCount(10);       //���ö��ٸ����
	axisX->setMinorTickCount(1);   //����ÿ���������С�̶��ߵ���Ŀ
	axisX->setTitleText("<h1>X</h1>");  //��������

	axisY = new QValueAxis;
	axisY->setRange(0, 10);
	axisY->setLabelFormat("%u");

	axisY->setGridLineVisible(true);
	axisY->setTickCount(10);
	axisY->setMinorTickCount(1);
	axisY->setTitleText("<h1>Y</h1>");

	chart->addAxis(axisX, Qt::AlignBottom); //�£�Qt::AlignBottom  �ϣ�Qt::AlignTop
	chart->addAxis(axisY, Qt::AlignLeft);   //��Qt::AlignLeft    �ң�Qt::AlignRight

	//��������Դ
	series->setPen(QPen(Qt::red, 1, Qt::SolidLine));//����������ʾЧ��
	series->append(1, 6); //������������ݵ����ַ�ʽ
	series->append(2, 4);
	series->append(3, 8);
	series->append(6, 4);
	series->append(8, 6);
	series->append(10, 5);
	*series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(19, 2);

	chart->setTitle(u8"ɢ��ͼʵ��");
	chart->setAnimationOptions(QChart::SeriesAnimations);//�������߶���ģʽ
	chart->legend()->hide(); //����ͼ��
	chart->addSeries(series);//��������
	chart->setAxisX(axisX, series);
	chart->setAxisY(axisY, series);

	layout = new QHBoxLayout;
	layout->addWidget(chartview);
	ui.widget->setLayout(layout);
	chartview->setRenderHint(QPainter::Antialiasing);//��Ⱦ ��ֹͼ������
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
	chart->setTitle(u8"��ͼʵ��");
	chart->legend()->hide();

	chartview->setRenderHint(QPainter::Antialiasing);

	layout = new QHBoxLayout;
	layout->addWidget(chartview);
	ui.widget->setLayout(layout);
	chartview->setRenderHint(QPainter::Antialiasing);//��Ⱦ ��ֹͼ������
}
