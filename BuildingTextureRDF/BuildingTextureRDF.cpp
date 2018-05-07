#include "BuildingTextureRDF.h"
#include <GeoHash.h>
#include <iostream>
using namespace std;

BuildingTextureRDF::BuildingTextureRDF()
{
	mvHashBuildings.clear();
	mnGeoHashScale = 12;
}


BuildingTextureRDF::~BuildingTextureRDF()
{
	mvHashBuildings.clear();
	
}

bool BuildingTextureRDF::loadBuildingData(char* sfilename, int nGeoHashScale)
{
	mnGeoHashScale = nGeoHashScale;

	//载入时计算Hash并存储

	return true;
}

bool BuildingTextureRDF::calculateRdfValues(ObservedSphere& obs)
{
	return true;
}

bool BuildingTextureRDF::buildingSpaceSizeInSphere(float dlat, float dlon, float dR, Building bd, float& dRatio, float& dSpaceSize)
{
	return true;
}

bool BuildingTextureRDF::buildingSpaceSizeInSphereShell(float dlat, float dlon, float dMinR, float dMaxR, Building bd, float& dRatio, float& dSpaceSize)
{
	return true;
}
