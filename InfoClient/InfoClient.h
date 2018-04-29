#pragma once

#include <QtWidgets/QDialog>
#include "ui_InfoClient.h"
#include <QUdpSocket>

class InfoClient : public QDialog
{
	Q_OBJECT

public:
	InfoClient(QWidget *parent = Q_NULLPTR);
	~InfoClient();

public slots:
	void receiveInfo();

private:
	Ui::InfoClientClass ui;
	QUdpSocket* mpUdpSocket;
	//QTimer* mpTimer;

};
