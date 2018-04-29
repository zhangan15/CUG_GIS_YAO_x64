#include "InfoClient.h"
#include <QtCore>
#include <QMessageBox>

InfoClient::InfoClient(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	mpUdpSocket = new QUdpSocket(this);
	bool brlt = mpUdpSocket->bind(40016);	

	if (!brlt)
	{
		QMessageBox::critical(this, "exit", "Program can not listen port 40016.");
		exit(0);
	}

	connect(mpUdpSocket, &QUdpSocket::readyRead, this, &InfoClient::receiveInfo);


// 	mpTimer = new QTimer(this);
// 	connect(mpTimer, &QTimer::timeout, this, &InfoClient::receiveInfo);
// 	
// 	mpTimer->start(100);

	
}

InfoClient::~InfoClient()
{
	mpUdpSocket->close();
	mpUdpSocket->deleteLater();
}

void InfoClient::receiveInfo()
{
	while (mpUdpSocket->hasPendingDatagrams())
	{
		QByteArray block;
		block.resize(mpUdpSocket->pendingDatagramSize());
		mpUdpSocket->readDatagram(block.data(), block.size());

		QDataStream in(&block, QIODevice::ReadOnly);
		QString msg;
		int nval;
		in >> msg;
		in >> nval;

		msg = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss: \r\n")+msg+QString("\r\nDanger index: %1").arg(nval);
		ui.text->append("****** NEW ******");
		ui.text->append(msg);
		ui.progressBar->setValue(nval);
		ui.label->setText(QString("DI: %1%").arg(nval));
		//ui.lcdNumber->display((double)nval / 100.0);


		QString sParentAddress;
		qint16 nParentPort;
		in >> sParentAddress >> nParentPort;
		ui.text->append("local: " + mpUdpSocket->localAddress().toString() + ":" + QString::number(mpUdpSocket->localPort()));
		ui.text->append("from: " + sParentAddress + ":" + QString::number(nParentPort));

	}
}
