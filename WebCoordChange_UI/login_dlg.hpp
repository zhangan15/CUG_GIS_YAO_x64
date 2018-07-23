#pragma once
#include <QDialog>
#include "ui_login_dlg.h"

class login_dlg : public QDialog {
	Q_OBJECT

public:
	login_dlg(QDialog * parent = Q_NULLPTR);
	~login_dlg();

private:
	Ui::login_dlg ui;

private slots:
	void checkname();
};
