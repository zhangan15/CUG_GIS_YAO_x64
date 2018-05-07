#include <QtCore>
#include <iostream>
#include "BuildingTextureRDF.h"
using namespace std;



int main(int argc, char *argv[])
{
	BuildingTextureRDF btr;

	//输入建筑物数据: csv, LatColNum, LonColNum, PerAreaCol, FloorCountCol, HeightCol, PerFloorHeight, GeoHashScale
	btr.loadBuildingData("./data/eerduosi.csv", 2, 3, 4, 1, -1, 3.0, 7);

	ObservedSphere test_obs;
	test_obs.dlon = 109.98684045800;
	test_obs.dlat = 39.82008127450;
	test_obs.dCenterHeight = 50.0f;
	test_obs.dMaxRadius = 10000.0f;
	test_obs.dStepRadius = 100.0f;

	btr.calculateRdfValues(test_obs);

	//输出结果
	int nCount = 0;
	foreach(float val, test_obs.vdRdfValues)
	{
		cout << test_obs.dStepRadius + nCount*test_obs.dStepRadius << "," << val << endl;
		nCount++;
	}

	return 0;
}
