#include "login_dlg.hpp"

login_dlg::login_dlg(QDialog * parent) : QDialog(parent) {
	ui.setupUi(this);
	QObject::connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(checkname()));
}

login_dlg::~login_dlg() {
	
}

void login_dlg::checkname()
{
	if (ui.lineEdit->text() == "liupenghua")
		accept();
	else
		reject();
}
