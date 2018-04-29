#include <QtCore>
#include <iostream>
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "GDALRead.h"

#include "alglib/statistics.h"
#include "alglib/optimization.h"
#include "alglib/dataanalysis.h"
#include "alglib/alglibmisc.h"

#include "RFWeights.h"
using namespace std;
using namespace alglib;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	GDALAllRegister();
	OGRRegisterAll();
	cout << "GDAL and OGR Register Success!" << endl;
	

	cout << "hello world!" << endl;






	return a.exec();
}
