#include "InfoClient.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	InfoClient w;
	w.show();
	return a.exec();
}
