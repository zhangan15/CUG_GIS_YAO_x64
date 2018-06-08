
#include "DLPS.h"
int main(int argc, char *argv[])
{
// 	QTextCodec* codec =QTextCodec::codecForLocale();
// 	QTextCodec::setCodecForCStrings(codec);
// 	QTextCodec::setCodecForTr(codec);

	DLPS *dlps_obj = new DLPS();
	dlps_obj->printRight();

	QString infn, outfn, split_flag_field = "split";
	int nMaxIter = 3;
	double dMaxSize = 0, dNTimes = 3.0;
	bool bPrintFlag = false, bLimit = true;
	for (int i = 1; i < argc; i ++)
	{
		if (strcmp(argv[i], "-inputFile") == 0)
			infn = QString(argv[i+1]);
		if (strcmp(argv[i], "-outputFile") == 0)
			outfn = QString(argv[i+1]);
		if (strcmp(argv[i], "-split_flag_field") == 0)
			split_flag_field = QString(argv[i+1]);
		if (strcmp(argv[i], "-maxIteration") == 0)
			nMaxIter = atoi(argv[i+1]);
		if (strcmp(argv[i], "-maxSize") == 0)
			dMaxSize = atof(argv[i+1]);
		if (strcmp(argv[i], "-N") == 0)
			dNTimes = atof(argv[i+1]);
		if (strcmp(argv[i], "-print") == 0)
		{
			if(atoi(argv[i+1])!=0)
				bPrintFlag = true;
		}
		if (strcmp(argv[i], "-limit") == 0)
		{
			if(atoi(argv[i+1])==0)
				bLimit = false;
		}
	}

	if(infn.size() <= 0)
	{
		cout << "[ERROR]  missing parameters(input file name)!\n";
		exit(1);
	}
	if(!QFile::exists(infn))
	{
		 cout << "[ERROR]  input file does not exist!\n";
		 exit(1);

	}
	if(outfn.size() <= 0)
	{
		cout << "[ERROR]  missing parameters(output file name)!\n";
		exit(1);
	}
	if(QFile::exists(outfn))
	{
		cout << "[ERROR]  output file already exists!\n";
		exit(1);
	}

	
	dlps_obj->setParams(infn, outfn, split_flag_field, 
		dMaxSize, dNTimes, nMaxIter, bPrintFlag, bLimit);
	dlps_obj->run();
	dlps_obj->printRight();
	delete dlps_obj;
	dlps_obj = NULL;
	return 0;
}
