#pragma once

#include <QtWidgets/QDialog>
#include "ui_WebCoordChange_UI.h"

class WebCoordChange_UI : public QDialog
{
	Q_OBJECT

public:
	WebCoordChange_UI(QWidget *parent = Q_NULLPTR);

private:
	Ui::WebCoordChange_UIClass ui;
};
