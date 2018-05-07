#include <QtCore>
#include <iostream>
#include "BuildingTextureRDF.h"
using namespace std;



int main(int argc, char *argv[])
{
	QHash<QString, QList<Building>> vHashBuildings;
	QHash<QString, int> vTest;
	vTest.insert("0xx01", 0);
	vTest.insert("0xx02", 1);
	vTest.insert("0xx01", 2);
	vTest.insert("0xx01", 3);	//Key具有唯一性, 除非采用insertMulti
	vTest.insert("0xx02", 2);
	vTest.insert("0xx03", -1);

	cout << vTest.size() << endl;
	qDebug() << vTest.keys() << endl;
	vTest["0xx01"] = 4;

	//如果不存在，则Insert一个默认值
	qDebug() << vTest["0xx04"] << endl;
	cout << vTest.size() << endl;


	return 0;
}
