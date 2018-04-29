#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_InfoServer.h"

#include <QUdpSocket>

class InfoServer : public QMainWindow
{
	Q_OBJECT

public:
	InfoServer(QWidget *parent = Q_NULLPTR);
	~InfoServer();

public slots:
	void startTimer();
	void sendMessage();

private:
	Ui::InfoServerClass ui;
	QUdpSocket *mudpSocket;
	QTimer* mpTimer;
	int mnSendTime;
};
