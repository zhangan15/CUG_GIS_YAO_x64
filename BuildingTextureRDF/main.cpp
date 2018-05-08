#include <QtCore>
#include <iostream>
#include "BuildingTextureRDF.h"
using namespace std;



int main(int argc, char *argv[])
{
	BuildingTextureRDF btr;

	//输入建筑物数据: csv, FIDColNum, LatColNum, LonColNum, PerAreaCol, FloorCountCol, HeightCol, DemCol, PerFloorHeight, GeoHashScale
	btr.loadBuildingData("./data/eerduosi.csv", 0, 2, 3, 4, 1, -1, -1, 3.0, 8);

	//输入经纬度数据集和球参数 center_lat, center_lon, step_radius, max_radius, height, output_filename

	ObservedSphere test_obs;
	test_obs.dlon = 109.95455734200;
	test_obs.dlat = 39.80956918330;
	test_obs.dCenterHeight = 0.0f;
	test_obs.dMaxRadius = 10000.0f;
	test_obs.dStepRadius = 50.0f;

	btr.calculateRdfValues(test_obs);

	//输出结果
	int nCount = 0;
	foreach(float val, test_obs.vdRdfValues)
	{
		cout << test_obs.dStepRadius + nCount*test_obs.dStepRadius << "," << val << endl;
		nCount++;
	}

	cout << "Entropy: " << test_obs.dEntropy << endl;

	return 0;
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
