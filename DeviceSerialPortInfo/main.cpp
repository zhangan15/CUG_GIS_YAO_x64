#pragma execution_character_set("utf-8")

#include <QtCore>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	wcout.imbue(locale("chs"));

	//obtain all COM port info
	QList<QSerialPortInfo> COM_infos = QSerialPortInfo::availablePorts();
	cout << COM_infos.size() << endl;
	foreach(QSerialPortInfo cinfo, COM_infos)
		wcout << cinfo.portName().toStdWString() << ", number = " << cinfo.serialNumber().toStdWString() << ", isvalid = " << cinfo.isValid() << endl;


	QList<qint32> COM_Rates = QSerialPortInfo::standardBaudRates();
	qDebug() << COM_Rates;

	if (COM_infos.isEmpty())
		return -1;


	//GPS
	QSerialPort serial;
	serial.setPort(COM_infos[0]);
	serial.open(QIODevice::ReadWrite);
	serial.setBaudRate(QSerialPort::Baud4800);
	//serial.setBaudRate(QSerialPort::Baud9600);
	//serial.setBaudRate(QSerialPort::Baud115200);
	QByteArray GPSBuffer = serial.readAll();
	serial.close();


	qDebug() << GPSBuffer << endl;




	return a.exec();
}
