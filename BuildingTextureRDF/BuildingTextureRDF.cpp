#include "BuildingTextureRDF.h"
#include <GeoHash.h>
#include <iostream>
#include <QtCore>
using namespace std;

BuildingTextureRDF::BuildingTextureRDF()
{
	mvHashBuildings.clear();
	mnGeoHashScale = 8;
	msHashList.clear();
}


BuildingTextureRDF::~BuildingTextureRDF()
{
	mvHashBuildings.clear();
	
}

bool BuildingTextureRDF::loadBuildingData(char* sfilename, int nFIDCol, int nLatCol, int nLonCol, int nPerFloorAreaCol,\
	int nFloorCol, int nHeightCol /*= -1*/, int nDemCol, double dPerFloorHeight /*= 3.0f*/,\
	int nGeoHashScale /*= 8*/)
{
	if (nFloorCol < 0 && nHeightCol < 0)
	{
		logLn("ERROR: missing floor or height data.");
		//cout << "ERROR: missing floor or height data." << endl;
		return false;
	}

	mnGeoHashScale = nGeoHashScale;
	logLn(QString("GeoHash Scale = %1").arg(mnGeoHashScale));
	//cout << "GeoHash Scale = " << mnGeoHashScale << endl;

	//重置所有数据
	mvHashBuildings.clear();
	mvHashEnable.clear();
	msHashList.clear();
	
	//载入时计算Hash并存储
	QFile _f(sfilename);
	if (!_f.open(QIODevice::ReadOnly))
	{
		logLn("ERROR: load building data fail.");
		//cout << "ERROR: load building data fail." << endl;
		return false;
	}
	QTextStream _in(&_f);

	// remove titles
	_in.readLine();
	int nBuildingCount = 0;
	while (!_in.atEnd())
	{
		QString s = _in.readLine();
		QStringList slist = s.split(",");

		Building bld;
		bld.nFID = slist[nFIDCol].trimmed().toLong();
		bld.dlat = slist[nLatCol].trimmed().toDouble();
		bld.dlon = slist[nLonCol].trimmed().toDouble();
		bld.dPerFloorArea = slist[nPerFloorAreaCol].trimmed().toDouble();

		if (nHeightCol >= 0)
			bld.dBuildingHeight = slist[nHeightCol].trimmed().toDouble();
		else
			bld.dBuildingHeight = slist[nFloorCol].trimmed().toDouble() * dPerFloorHeight;

		bld.dTotalArea = bld.dBuildingHeight * bld.dPerFloorArea;

		if (nDemCol >= 0)
			bld.dDemButtom = slist[nDemCol].trimmed().toDouble();
		else
			bld.dDemButtom = 0;
		
		// change to geohash
		string sHa = GeoHash::EncodeLatLon(bld.dlat, bld.dlon, mnGeoHashScale);
		QString sHash = QString::fromStdString(sHa).toLower();

		//if new buildings
		if (!mvHashBuildings.contains(sHash))
		{
			mvHashBuildings.insert(sHash, QList<Building>());
			mvHashEnable.insert(sHash, true);
			msHashList.append(sHash);
		}		
		
		mvHashBuildings[sHash].append(bld);		
		nBuildingCount++;

		if (nBuildingCount % 1000 == 0)
			logLn(QString("\tloading %1 building now ...").arg(nBuildingCount));
			//cout << "\tloading " << nBuildingCount << " building now ..." << endl;
		
	}

	logLn(QString("Load file success. Total Number of Buildings = %1.").arg(nBuildingCount));
	logLn(QString("GeoHash Count = %1.").arg(mvHashBuildings.keys().size()));
	//cout << "Load file success. Building Count = " << nBuildingCount << endl;
	//cout << "GeoHash Count = " << mvHashBuildings.keys().size() << endl;

// 	long ntotalcount = 0;
// 	foreach(QString s, msHashList)
// 		ntotalcount += mvHashBuildings[s].size();
// 	cout << "ntotal count = " << ntotalcount << endl;

	_f.close();

	return true;
}

bool BuildingTextureRDF::calculateRdfValues(ObservedSphere& obs)
{
	if (mvHashBuildings.size() == 0)
		return false;

	//寻找到球中心的GeoHash
	string centerHash = GeoHash::EncodeLatLon(obs.dlat, obs.dlon, mnGeoHashScale);
	//QString curHash = QString::fromStdString(centerHash);

	//球中心GeoHash区域生长
	setAllGeoHashEnable();
	QStringList enableHashList;
	enableHashList.clear();

	//暴力遍历, 算法复杂度为O(n)
	foreach(QString curHash, msHashList)
	{
		double dist = GeoHash::distanceBetweenGeoHashes(curHash.toStdString(), centerHash);
		if (dist <= obs.dMaxRadius)
			enableHashList.append(curHash);
	}

	/*
	QStack<QString> stackHashList;
	stackHashList.append(curHash);
	while (!stackHashList.isEmpty())
	{
		curHash = stackHashList.pop();	
		if (!enableHashList.contains(curHash))
		{
			if (mvHashBuildings[curHash].size() != 0)
				enableHashList.append(curHash);
		}
			

		double dist = GeoHash::distanceBetweenGeoHashes(curHash.toStdString(), centerHash);
		string cur_direct[4];
		if (dist <= obs.dMaxRadius)
		{
			cur_direct[0] = GeoHash::CalculateAdjacent(curHash.toStdString(), GeoHash::Top);
			cur_direct[1] = GeoHash::CalculateAdjacent(curHash.toStdString(), GeoHash::Bottom);
			cur_direct[2] = GeoHash::CalculateAdjacent(curHash.toStdString(), GeoHash::Left);
			cur_direct[3] = GeoHash::CalculateAdjacent(curHash.toStdString(), GeoHash::Right);

			for (int i=0; i<4; i++)
			{
				dist = GeoHash::distanceBetweenGeoHashes(cur_direct[i], centerHash);
	
				//有可能某个邻域内没有数据
				if (mvHashEnable.contains(QString::fromStdString(cur_direct[i])))
				{
					if (dist <= obs.dMaxRadius && mvHashEnable[QString::fromStdString(cur_direct[i])])
						stackHashList.append(QString::fromStdString(cur_direct[i]));
				}
				else
				{
					if (dist <= obs.dMaxRadius)
					{
						mvHashBuildings.insert(QString::fromStdString(cur_direct[i]), QList<Building>());
						mvHashEnable.insert(QString::fromStdString(cur_direct[i]), true);
						msHashList.append(QString::fromStdString(cur_direct[i]));
						stackHashList.append(QString::fromStdString(cur_direct[i]));						
					}
				}
					
			}

			mvHashEnable[curHash] = false;
		}		
	}
*/

	//cout << "In The Sphere GeoHash Count = " << enableHashList.size() << endl;

	//计算球的体积和建筑物体积
	//double dSphereVol = 4.0f * PI * obs.dMaxRadius * obs.dMaxRadius * obs.dMaxRadius / 3.0f;
	double dSphereVol = calculateSphereSegmentVol(obs.dMaxRadius-obs.dCenterHeight, obs.dMaxRadius);
	double dBuildingVolInSphere = 0;

	foreach(QString sHa, enableHashList)
	{
		foreach(Building bd, mvHashBuildings[sHa])
		{
			float dRatio, dSpaceSize;
			buildingSpaceSizeInSphere(obs.dlat, obs.dlon, obs.dCenterHeight, obs.dMaxRadius, bd, dRatio, dSpaceSize);
			dBuildingVolInSphere += dSpaceSize;
		}
	}

	double dBuildingVolRatio = dBuildingVolInSphere / dSphereVol;
	
	//把得到的区域内的建筑物取出来分析
	double dCurRadius = 0;
	double dMaxRadius = obs.dMaxRadius;
	obs.vdRdfValues.clear();
	for (dCurRadius = 0; dCurRadius <= dMaxRadius-obs.dStepRadius; dCurRadius += obs.dStepRadius)
	{		
		double dCurMaxRadius = dCurRadius + obs.dStepRadius;

		//获取球壳内建筑物的面积
		double dBuildingVolInSphereShell = 0;
		foreach(QString sHa, enableHashList)
		{
			foreach(Building bd, mvHashBuildings[sHa])
			{
				float dRatio, dSpaceSize;
				buildingSpaceSizeInSphereShell(obs.dlat, obs.dlon, obs.dCenterHeight, \
					dCurRadius, dCurMaxRadius, bd, dRatio, dSpaceSize);
				dBuildingVolInSphereShell += dSpaceSize;
			}
		}

		//计算球壳的体积
		double dSphereShellVol = calculateSphereSegmentVol(obs.dMaxRadius - obs.dCenterHeight, obs.dMaxRadius)\
			- calculateSphereSegmentVol(dCurRadius - obs.dCenterHeight, dCurRadius);
			//4.0f * PI * dCurMaxRadius * dCurMaxRadius * dCurMaxRadius / 3.0f \
			- 4.0f * PI * dCurRadius * dCurRadius * dCurRadius / 3.0f;

		//计算球壳的比例
		double dSphereShellVolRatio = dBuildingVolInSphereShell / dSphereShellVol;

		//得到当前的RDF
		double dRdf = 1.0f / dBuildingVolRatio * dSphereShellVolRatio;

		//加入数据
		obs.vdRdfValues.append(dRdf);
	}


	// 计算熵
	double dEntropy = 0;
	foreach(float dval, obs.vdRdfValues)
	{
		if (dval < 10e-6)
			continue;
		dEntropy += dval*log(dval);
	}
	obs.dEntropy = -dEntropy;


	return true;
}

bool BuildingTextureRDF::buildingSpaceSizeInSphere(float dlat, float dlon, float dCenterHeight,\
	float dR, Building bd, float& dRatio, float& dSpaceSize)
{
	//计算球心到点的投影距离
	double dr = getPtsDist(dlat, dlon, bd.dlat, bd.dlon) * 1000.0f; //单位为m
	double dt = 0;
	if (dR > dr)
		dt= sqrt(dR*dR - dr*dr);

	double z1 = dCenterHeight - dt;
	double z2 = dCenterHeight + dt;

	//z1 = z1 > bd.dDemButtom ? z1 : bd.dDemButtom;
	z1 = z1 > 0.0f ? z1 : 0.0f;
	z2 = z2 < (bd.dBuildingHeight + bd.dDemButtom) ? z2 : (bd.dBuildingHeight + bd.dDemButtom);

	//
	if (z2 - z1 <= 0)
	{
		dRatio = 0;
		dSpaceSize = 0;
	}
	else
	{
		dRatio = (z2 - z1) / bd.dBuildingHeight;
		dSpaceSize = dRatio * bd.dTotalArea;
	}

	return true;
}

bool BuildingTextureRDF::buildingSpaceSizeInSphereShell(float dlat, float dlon, float dCenterHeight, float dMinR,\
	float dMaxR, Building bd, float& dRatio, float& dSpaceSize)
{
	float dMinRatio, dMinSpaceSize;
	float dMaxRatio, dMaxSpaceSize;
	buildingSpaceSizeInSphere(dlat, dlon, dCenterHeight, dMinR, bd, dMinRatio, dMinSpaceSize);
	buildingSpaceSizeInSphere(dlat, dlon, dCenterHeight, dMaxR, bd, dMaxRatio, dMaxSpaceSize);
	dSpaceSize = dMaxSpaceSize - dMinSpaceSize;
	dRatio = dMaxRatio - dMinRatio;
	return true;
}

void BuildingTextureRDF::setAllGeoHashEnable()
{
	foreach(QString s, msHashList)
		mvHashEnable[s] = true;	
}

double BuildingTextureRDF::calculateSphereSegmentVol(double dDisHeight, double dR)
{
	if (dDisHeight < 0)
		return 4.0f*PI*dR*dR*dR / 3.0f;

	double misSphere = PI*dDisHeight*dDisHeight*(dR - dDisHeight / 3.0f);
	double sphereVal = 4.0f*PI*dR*dR*dR/3.0f;
	return sphereVal - misSphere;
}

void BuildingTextureRDF::logLn(QString smsg)
{
	QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	cout << QString("%1 : %2").arg(timestamp).arg(smsg).toStdString().data() << endl;
}
