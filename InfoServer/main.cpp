#include "InfoServer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	InfoServer w;
	w.show();
	return a.exec();
}
