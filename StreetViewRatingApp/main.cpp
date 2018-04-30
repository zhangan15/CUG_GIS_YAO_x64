#include "StreetViewRatingApp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	StreetViewRatingApp w;
	w.show();
	return a.exec();
}
