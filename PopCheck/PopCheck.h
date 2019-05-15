#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PopCheck.h"
#include <QtCore>

class PopCheck : public QMainWindow
{
	Q_OBJECT

public:
	PopCheck(QWidget *parent = Q_NULLPTR);

protected:
	void makeInNameList();
	void makeCheckNameList();
	void compOutNameList();
	void display();

private:
	Ui::PopCheckClass ui;
	QString msInNames;
	//QString msOutNames;
	QString msChNames;
	QStringList msInNameList;
	QStringList msOutNameList;
	QStringList msChNameList;

};
