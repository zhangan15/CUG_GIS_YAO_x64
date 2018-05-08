#include <QtCore>
#include <iostream>
#include "BuildingTextureRDF.h"
using namespace std;


int run(int argc, char *argv[])
{
	if (argc < 15)
	{
		cout << "Please execute program like this:" << endl;
		cout << "\tBuildingTextureRDF.exe input_building_attribute_csv input_calculate_position_csv" << \
			" output_csv FID_col_num(int) Lat_col_num(int) Lon_col_num(int) Per_Area_col_num(int) Floor_count_col(int. if not, set to -1)" << \
			" Height_col(int. if not, set to -1) Dem_col(int. if not, set to -1) Per_Floor_Height(float, unit:m) GeoHashScale(int. 6-10)"<< \
			" step_radius(float) max_radius(float)" << endl;
		cout << "Example: " << endl;
		cout << "Example: BuildingTextureRDF.exe ./data/eerduosi.csv ./data/input_lat_lon.csv ./data/output_result.csv 0 2 3 4 1 -1 -1 3.0 8 50.0 1000.0" << endl;
		cout << "Exit." << endl;
		return -1;
	}

	// 读取参数
	QString inputBuildingAttibuteFile = QString(argv[1]);
	QString inputLatLonFile = QString(argv[2]);
	QString OutputResultFile = QString(argv[3]);
	int nFidCol = atoi(argv[4]);
	int nLatCol = atoi(argv[5]);
	int nLngCol = atoi(argv[6]);
	int nPerAreaCol = atoi(argv[7]);
	int nFloorCol = atoi(argv[8]);
	int nHeightCol = atoi(argv[9]);
	int nDemCol = atoi(argv[10]);
	float dFloorHeight = atof(argv[11]);
	int nHashScale = atoi(argv[12]);
	float dStepRadius = atof(argv[13]);
	float dMaxRadius = atof(argv[14]);

	//输出参数显示
	cout << "Input Building File: " << inputBuildingAttibuteFile.toLocal8Bit().data() << endl;
	cout << "Input Lat Lon File: " << inputLatLonFile.toLocal8Bit().data() << endl;
	cout << "Output Result File: " << OutputResultFile.toLocal8Bit().data() << endl;
	cout << "FID COLUMN = " << nFidCol << endl;
	cout << "LATITUDE COLUMN = " << nLatCol << endl;
	cout << "LONGITUDE COLUMN = " << nLngCol << endl;
	cout << "PER FLOOR AREA COLUMN = " << nPerAreaCol << endl;
	cout << "FLOORS COUNT COLUMN = " << nFloorCol << endl;
	cout << "BUILDING HEIGHT COLUMN = " << nHeightCol << endl;
	cout << "DEM HEIGHT COLUMN = " << nDemCol << endl;
	cout << "PER FLOOR HEIGHT = " << dFloorHeight << endl;
	cout << "* GEO-HASH SCALE = " << nHashScale << endl;
	cout << "* STEP RADIUS = " << dStepRadius << endl;
	cout << "* MAXIMUM RADIUS = " << dMaxRadius << endl;

	//判断
	if (dMaxRadius < dStepRadius)
	{
		cout << "Error: input max raidus < step radius." << endl;
		return -5;
	}


	// 读取经纬度信息文件
	QList<ObservedSphere> vObs;
	QFile latlonFile(inputLatLonFile);
	if (!latlonFile.open(QIODevice::ReadOnly))
	{
		cout << "Error: load lat lon file fail." << endl;
		return -1;
	}
	QTextStream latlonIn(&latlonFile);
	QString s = latlonIn.readLine();
	QStringList slist = s.split(",");
	int nLatLonFileSize = slist.size();

	if (nLatLonFileSize < 4)
	{
		latlonFile.close();
		cout << "Error: unkown lat_lon file format." << endl;
		return -2;
	}

	while (!latlonIn.atEnd())
	{
		s = latlonIn.readLine();
		slist = s.split(",");

		if (slist.size() != nLatLonFileSize)
			continue;

		ObservedSphere obs;
		obs.nFID = slist[0].trimmed().toLong();
		obs.dlat = slist[1].trimmed().toDouble();
		obs.dlon = slist[2].trimmed().toDouble();
		obs.dStepRadius = dStepRadius;
		obs.dMaxRadius = dMaxRadius;
		obs.dCenterHeight = slist[3].trimmed().toDouble();
		obs.dEntropy = 0;
		obs.vdRdfValues.clear();

		vObs.append(obs);
	}
	cout << "Load lat lon number: " << vObs.size() << endl;
	latlonFile.close();

	if (vObs.size() == 0)
	{
		cout << "Error: load lat lon fail." << endl;
		return -3;
	}

	//计算
	BuildingTextureRDF btr;
	btr.loadBuildingData(inputBuildingAttibuteFile.toLocal8Bit().data(), \
		nFidCol, nLatCol, nLngCol, nPerAreaCol, nFloorCol, \
		nHeightCol, nDemCol, dFloorHeight, nHashScale);
	btr.calculateSeveralRfdValues(vObs);

	//输出文件
	QFile _f(OutputResultFile);
	if (!_f.open(QIODevice::WriteOnly))
	{
		cout << "Error: create output file fail." << endl;
		return -1;
	}
	QTextStream _in(&_f);

	
	_in << "nFID, lat, lon, entropy";
	double dStepR = dStepRadius;
	double dMaxR = dMaxRadius;
	for (double dR = dStepR; dR <= dMaxR; dR += dStepR)
		_in << QString(", R%1").arg(dR, 0, 'f', 3);
	_in << "\r\n";


	for (int i=0; i<vObs.size(); i++)
	{
		_in << QString("%1, %2, %3, %4").arg(vObs[i].nFID).arg(vObs[i].dlat, 0, 'f', 6).arg(vObs[i].dlon, 0, 'f', 6).arg(vObs[i].dEntropy, 0, 'f', 6);
		foreach(float val, vObs[i].vdRdfValues)
			_in << QString(", %1").arg(val, 0, 'f', 6);
		_in << "\r\n";
	}
	
	_in.flush();
	_f.flush();
	_f.close();

	cout << "PROCESS FINISH." << endl;

	return 0;
}


int main(int argc, char *argv[])
{

	return run(argc, argv);
// 	BuildingTextureRDF btr;
// 
// 	//输入建筑物数据: csv, FIDColNum, LatColNum, LonColNum, PerAreaCol, FloorCountCol, HeightCol, DemCol, PerFloorHeight, GeoHashScale
// 	btr.loadBuildingData("./data/eerduosi.csv", 0, 2, 3, 4, 1, -1, -1, 3.0, 8);
// 
// 	//输入经纬度数据集和球参数 center_lat, center_lon, step_radius, max_radius, height, output_filename
// 
// 	ObservedSphere test_obs;
// 	test_obs.dlon = 109.95455734200;
// 	test_obs.dlat = 39.80956918330;
// 	test_obs.dCenterHeight = 0.0f;
// 	test_obs.dMaxRadius = 10000.0f;
// 	test_obs.dStepRadius = 50.0f;
// 
// 	QList<ObservedSphere> vObs;
// 	vObs.append(test_obs);
// 
// 	//btr.calculateRdfValues(test_obs);
// 
// 
// 	//集群运算
// 	btr.calculateSeveralRfdValues(vObs);
// 
// 	test_obs = vObs[0];
// 
// 	//输出结果
// 	int nCount = 0;
// 	foreach(float val, test_obs.vdRdfValues)
// 	{
// 		cout << test_obs.dStepRadius + nCount*test_obs.dStepRadius << "," << val << endl;
// 		nCount++;
// 	}
// 
// 	cout << "Entropy: " << test_obs.dEntropy << endl;
// 
// 	return 0;
	/*
	//暂不设置obs的中心位置
	ObservedSphere obs;
	obs.dCenterHeight = 0.0f;
	obs.dMaxRadius = 1000.0f;
	obs.dStepRadius = 20.0f;

	//计算所有建筑物的val
	QString sfilename = QString("./Data/all_buildings_step%1_max%2.csv").arg(obs.dStepRadius, 0, 'f', 0).arg(obs.dMaxRadius, 0, 'f', 0);
	QFile _f(sfilename);

	QString sfilename_entropy = QString("./Data/all_buildings_step%1_max%2_entropy.csv").arg(obs.dStepRadius, 0, 'f', 0).arg(obs.dMaxRadius, 0, 'f', 0);
	QFile _f1(sfilename_entropy);

	if (!_f.open(QIODevice::WriteOnly))
	{
		cout << "create output file fail." << endl;
		return -1;
	}

	if (!_f1.open(QIODevice::WriteOnly))
	{
		cout << "create output entropy file fail." << endl;
		return -1;
	}

	QTextStream _in(&_f), _in1(&_f1);

	

	_in << "nFID, lat, lon, CenterHeight";
	double dStepR = obs.dStepRadius;
	double dMaxR = obs.dMaxRadius;
	for (double dR = dStepR; dR <= dMaxR; dR += dStepR)
		_in << QString(", R%1").arg(dR, 0, 'f', 0);
	_in << "\r\n";

	_in1 << "nFID, entropy" << endl;

	int nCount = 0;
	QStringList sHashList = btr.mvHashBuildings.keys();
	foreach(QString sHa, sHashList) {
		foreach(Building bd, btr.mvHashBuildings[sHa]) {
			if ((nCount+1)%100 == 0)
				cout << "\tCalculating building id = " << bd.nFID << " located in No. "<< nCount+1 << " ..." << endl;

			obs.dlon = bd.dlon;
			obs.dlat = bd.dlat;		
			btr.calculateRdfValues(obs);

			_in << QString("%1, %2, %3, %4").arg(bd.nFID).arg(obs.dlat, 0, 'f', 6).arg(obs.dlon, 0, 'f', 6).arg(obs.dCenterHeight, 0, 'f', 6);
			foreach(float val, obs.vdRdfValues)
				_in << QString(", %1").arg(val, 0, 'f', 6);
			_in << "\r\n";

			_in1 << QString("%1, %2").arg(bd.nFID).arg(obs.dEntropy, 0, 'f', 6) << endl;

			nCount++;

		}
	}


	_in1.flush();
	_f1.flush();
	_f1.close();

	_in.flush();
	_f.flush();
	_f.close();
	*/

	/*
	//测试高程
	QString sfilename = QString("./Data/output_%1_%2.csv").arg(test_obs.dlon, 0, 'f', 6).arg(test_obs.dlat, 0, 'f', 6);
	QFile _f(sfilename);
	if (!_f.open(QIODevice::WriteOnly))
	{
		cout << "create output file fail." << endl;
		return -1;
	}

	QTextStream _in(&_f);

	//title
	_in << "lat, lon, CenterHeight";
	double dStepR = test_obs.dStepRadius;
	double dMaxR = test_obs.dMaxRadius;
	for (double dR = dStepR; dR <= dMaxR; dR += dStepR)
		_in << QString(", R%1").arg(dR, 0, 'f', 0);
	_in << "\r\n";

	for (test_obs.dCenterHeight = 0.0f; test_obs.dCenterHeight <= 100.0f; test_obs.dCenterHeight += 20.0f)
	{
		cout << "\tCalculating center height = " << test_obs.dCenterHeight << " ..." << endl;
		btr.calculateRdfValues(test_obs);
		_in << QString("%1, %2, H%3").arg(test_obs.dlat, 0, 'f', 6).arg(test_obs.dlon, 0, 'f', 6).arg(test_obs.dCenterHeight, 0, 'f', 0);
		foreach(float val, test_obs.vdRdfValues)
			_in << QString(", %1").arg(val, 0, 'f', 6);
		_in << "\r\n";

		_in.flush();
	}


	_in.flush();
	_f.flush();
	_f.close();
	*/
	

	return 0;
}
