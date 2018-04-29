#pragma execution_character_set("utf-8")

#include <QtCore>
#include <iostream>
#include "GDALRead.h"
using namespace std;

#define GDT_MF_DT GDT_UInt16
#define MF_DATATYPE unsigned short
#define NODATAVAL 65535

// #define GDT_MF_DT GDT_Byte
// #define MF_DATATYPE unsigned char
// #define NODATAVAL 255

#define GDT_FILE_DT GDT_Float32
#define FILE_DATATYPE float


// return max count
long StatiscalImageFromMask(const char* sinfile, const char* smaskfile, double*& pdProbs)
{
	CGDALRead* pFile = new CGDALRead;
	CGDALRead* pMask = new CGDALRead;
	if (!pFile->loadFrom(sinfile) || pFile->datatype() != GDT_FILE_DT)
	{
		cout << "load image file fail." << endl;
		delete pFile;
		delete pMask;
		return -1;
	}
	cout << "load image file success" << endl;

	if (!pMask->loadFrom(smaskfile) || pMask->datatype() != GDT_MF_DT)
	{
		cout << "load mask file fail." << endl;
		pFile->close();
		delete pFile;
		delete pMask;
		return -2;
	}
	cout << "load mask file success" << endl;
		
	// max mask count
	long nMaxCount = 0;
	int i, j, k;
	for (i=0; i<pMask->rows(); i++)
	{
		for (j=0; j<pMask->cols(); j++)
		{
			MF_DATATYPE val = *((MF_DATATYPE*)pMask->read(i, j, 0));
			if (val == NODATAVAL)
				continue;

			if (val > nMaxCount)
				nMaxCount = val + 1;
		}
	}
	cout << "Max Count of MASK = " << nMaxCount << endl;

	double dsum = 0;
	for (i = 0; i < pFile->rows(); i++)
	{
		for (j = 0; j < pFile->cols(); j++)
		{
			FILE_DATATYPE dval = *((FILE_DATATYPE*)pFile->read(i, j, 0));
			if (dval <= 0)
				continue;
			dsum += dval;
		}
	}
	cout << "total user count = " << QString("%1").arg(dsum, 0, 'f', 6).toLocal8Bit().data() << endl;

	//开始统计
	pdProbs = new double[nMaxCount];
	memset(pdProbs, 0, sizeof(double)*nMaxCount);

	double dLat, dLon;
	double _drow, _dcol;
	int _nrow, _ncol;
	for (i=0; i<pFile->rows(); i++)
	{
		if ((i + 1) % 500 == 0)
			cout << "processing " << i + 1 << " / " << pFile->rows() << " ..." << endl;

		for (j=0; j<pFile->cols(); j++)
		{
			FILE_DATATYPE dval = *((FILE_DATATYPE*)pFile->read(i, j, 0));
			if (dval <= 0)
				continue;

			pFile->pixel2World(&dLat, &dLon, j, i);
			pMask->world2Pixel(dLat, dLon, &_dcol, &_drow);
			_ncol = int(_dcol + 0.5);
			_nrow = int(_drow + 0.5);

			if (_ncol < 0 || _nrow < 0 || _ncol >= pMask->cols() || _nrow >= pMask->rows())
				continue;

			MF_DATATYPE nId = *((MF_DATATYPE*)pMask->read(_nrow, _ncol, 0));
			if (nId >= nMaxCount)
				continue;

			pdProbs[nId] += dval;
		}
	}

	//statistical, save probs
	/*
	double dsum = 0;
	for (i = 0; i < nMaxCount; i++)
		dsum += pdProbs[i];

	if (dsum <= 0)
		cout << "all values are zero." << endl;
	else
	{
		for (i = 0; i < nMaxCount; i++)
			pdProbs[i] /= dsum;
	}
	*/
	cout << "statistical success." << endl;


	pFile->close();
	pMask->close();
	return nMaxCount;
}


int main(int argc, char *argv[])
{
// 	if (argc <= 3)
// 	{
// 		cout << "plz input params as follow: data_dir mask_file output_csv" << endl;
// 		return -1;
// 	}

	GDALAllRegister();
	double* pdProbs = NULL;
	//QStringList strList;

	QFileInfoList filist = QDir("G:\\data\\腾讯lbs全球数据\\处理后_China_per_1hours\\geotiff_sum_clip").entryInfoList(QStringList(QString("*.tif")), QDir::Files, QDir::Name);
	wcout << "file count = " << filist.size() << endl;

	QStringList inMaskFileList;
	inMaskFileList << "E:\\Data\\Lambert中国1-400万数据\\mask\\new_prefectural_city_100m(with static).tif" << "E:\\Data\\Lambert中国1-400万数据\\mask\\prefectural_city_100m.tif" << "E:\\Data\\Lambert中国1-400万数据\\mask\\county_100m.tif";
	foreach(QString inMaskFile, inMaskFileList)
	{
		cout << "cur mask filename = " << inMaskFile.toLocal8Bit().data() << endl;

	//Mask File
	//QString inMaskFile = "E:\\Data\\Lambert中国1-400万数据\\mask\\province_100m.tif";

		//save to file
		QString soutfilename = QString("./sum_count/%1_statisc.csv").arg(QFileInfo(inMaskFile).completeBaseName());
		cout << "output filename = " << soutfilename.toLocal8Bit().data() << endl;

		int nSaveTitleFlag = 1;	

		QFile csvfile(soutfilename);
		QTextStream _in;
		QStringList sProcessedFileNameList;
		if (QFile::exists(soutfilename))
		{
			csvfile.open(QIODevice::ReadOnly);
			_in.setDevice(&csvfile);
			while (!_in.atEnd())
			{
				QString s = _in.readLine();
				QStringList slist = s.split(",");
				if (slist.size() <= 1) continue;
				sProcessedFileNameList << slist[0].trimmed().toLower();
			}
			csvfile.close();

			if (sProcessedFileNameList.size() == 0)
				nSaveTitleFlag = -1;

			csvfile.setFileName(soutfilename);
			if (!csvfile.open(QIODevice::Append))
			{
				cout << "make csv writable file error." << endl;
				return -3;
			}
			_in.setDevice(&csvfile);
		
		}
		else
		{
			if (!csvfile.open(QIODevice::WriteOnly))
			{
				cout << "make csv writable file error." << endl;
				return -3;
			}
			_in.setDevice(&csvfile);
		}

		foreach (QFileInfo fi, filist)
		{
			if (sProcessedFileNameList.contains(fi.completeBaseName().trimmed().toLower()))
			{
				cout << "Processed filename = " << fi.absoluteFilePath().toLocal8Bit().data() << endl;
				continue;
			}

			//_in.flush();
		
			QString inImgFile = fi.absoluteFilePath();
			cout << "processing filename = " << inImgFile.toLocal8Bit().data() << endl;

			long nCount = StatiscalImageFromMask(inImgFile.toStdString().data(), inMaskFile.toStdString().data(), pdProbs);
			if (nCount <= 0) continue;

			//保存首行
			if (nSaveTitleFlag > 0)
			{
				QString stitle = "filename";
				for (int i = 0; i < nCount; i++)
					stitle += QString(", %1").arg(i);
				_in << stitle << "\r\n";
				nSaveTitleFlag = -1;
			}

			QString tmp_str;
			tmp_str.append(QFileInfo(inImgFile).completeBaseName());
			for (int i = 0; i < nCount; i++)
				tmp_str += QString(", %1").arg(pdProbs[i], 0, 'f', 6);

			//strList.push_back(tmp_str);
			_in << tmp_str << "\r\n";
			cout << tmp_str.toStdString().data() << endl;
			_in.flush();

			delete[]pdProbs;
		}

	

		_in.flush();
		csvfile.flush();
		csvfile.close();

	}

	return 0;
}
