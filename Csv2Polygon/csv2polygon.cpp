
#include <QtCore>
#include <iostream>
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "WebCoordSystemTransform.h"

// 利用csv文件中的边界坐标信息生成polygon
// 这里的csv文件里面包含四个内容，ID,name,address,shape
// 先将shape中的坐标由gcj转成wgs84，然后生成polygon
// shape的坐标组织方式为：lng1;lat1_lng2;lat2...lngn;latn
bool csv2ShpPoly(const char* sCsvFileName, const char* sShpFileName)
{
	GDALAllRegister();
	OGRRegisterAll();
	CPLSetConfigOption("SHAPE_ENCODING", "");
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

	QFile _file(QString::fromLocal8Bit(sCsvFileName));
	if (!_file.open(QIODevice::ReadOnly))
	{
		std::cout << "open csv file " << sCsvFileName << " failed!" << std::endl;
		return false;
	}
	std::cout << "read file " << sCsvFileName << " successful." << std::endl;
	QTextStream _in(&_file);

	//create shp file
	const char* pszDriverName = "ESRI Shapefile";
	GDALDriver* poDriver;
	poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName);

	if (poDriver == NULL)
	{
		std::cout << pszDriverName << " drive not available!" << std::endl;
		return false;
	}

	GDALDataset *poDS;
	poDS = poDriver->Create(sShpFileName, 0, 0, 0, GDT_Unknown, NULL);
	if (poDS == NULL)
	{
		std::cout << "Creation of output file failed!" << std::endl;
		return false;
	}

	OGRSpatialReference oSRS;
	oSRS.SetWellKnownGeogCS("WGS84");
	OGRLayer *poLayer;
	poLayer = poDS->CreateLayer("coordinate", &oSRS, wkbPolygon, NULL);
	if (poLayer == NULL)
	{
		std::cout << "failed to create layer!" << std::endl;
		return false;
	}

	OGRFieldDefn oField2("ID", OFTString);
	oField2.SetWidth(12);
	OGRFieldDefn oField3("name", OFTString);
	//oField3.SetWidth(24);
	OGRFieldDefn oField4("address", OFTString);
	oField4.SetWidth(120);

	if (poLayer->CreateField(&oField2) != OGRERR_NONE)
	{
		std::cout << "create field2 failed" << std::endl;
		return false;
	}
	if (poLayer->CreateField(&oField3) != OGRERR_NONE)
	{
		std::cout << "create field3 failed" << std::endl;
		return false;
	}

	if (poLayer->CreateField(&oField4) != OGRERR_NONE)
	{
		std::cout << "create field4 failed" << std::endl;
		return false;
	}

	_in.readLine();
	int _count = 0;
	while (!_in.atEnd())
	{
		QString smsg = _in.readLine();
		QStringList smsglist = smsg.split(",", QString::KeepEmptyParts);
		OGRFeature *poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

		if (smsglist.size() < 4)
		{
			//std::cout << smsg.toStdString() << std::endl;
			continue;
		}
		
		poFeature->SetField("ID", smsglist[0].trimmed().toStdString().c_str());
		QString sName = smsglist[1].trimmed();
		std::string strName = sName.toLocal8Bit().toStdString();
		poFeature->SetField("name", strName.c_str());

		QString sAddress = smsglist[2];
		for (int i = 3; i < smsglist.size() - 1; i++)
		{
			sAddress += ",";
			sAddress += smsglist[i];
		}

		std::string strAddress = sAddress.toLocal8Bit().toStdString();
		poFeature->SetField("address", strAddress.c_str());

		OGRLinearRing linering;
		QStringList svLocList = smsglist[smsglist.size() - 1].split("_");

		if(svLocList.size() <= 2)
			continue;

		bool bflag = false;
		for (int i = 0; i < svLocList.size(); i++)
		{
			QStringList svOneLoc = svLocList[i].split(";");
			if (svOneLoc.size() < 2)
			{
				bflag = true;
				break;
			}

			double dlng = svOneLoc[0].trimmed().toDouble();
			double dlat = svOneLoc[1].trimmed().toDouble();

			double dwgslng = 0, dwgslat = 0;
			WEBCSTRANSFORM::gcj2wgs(dlat, dlng, dwgslat, dwgslng);
			linering.addPoint(dwgslng, dwgslat);
		}

		if(bflag)
			continue;

		OGRPolygon poly;
		poly.addRing(&linering);
		poFeature->SetGeometry(&poly);

		if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
		{
			std::cout << "failed to create feature " << _count << endl;
			return false;
		}
		OGRFeature::DestroyFeature(poFeature);
		_count++;
	}

	GDALClose(poDS);
	std::cout << "create shp file " << sShpFileName << " successful." << std::endl << std::endl;

	_file.close();

	return true;
}

int main()
{
	const char* sCsvFileName = "./data/wuhan_community.csv";
	const char* sShpFileName = "./data/wuhan_community_shp.shp";

	csv2ShpPoly(sCsvFileName, sShpFileName);
	
	return 0;
}
