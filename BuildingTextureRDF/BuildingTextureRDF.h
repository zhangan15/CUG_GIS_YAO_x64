#ifndef BUILDING_RDF_TEXTURE_BY_YAO_V10
#define BUILDING_RDF_TEXTURE_BY_YAO_V10


#include <QHash>
#include <QString>
#include <QList>

/************************************************************************/
/* In this model, The unit is unified to meter or square meters.        */
/************************************************************************/

struct Building 
{
	float dlat, dlon;
	float dArea;
	int nHeightButtom;	//DEM高程
	int nHeightTop;
};

struct ObservedSphere
{
	float dlat, dlon;
	float dHeight;
	float dMinRadius;
	float dMaxRadius;
	float dStepRadius;
	QList<float> vdRdfValues;
};

class BuildingTextureRDF
{
public:
	BuildingTextureRDF();
	~BuildingTextureRDF();

public:
	//载入数据，设定级数
	bool loadBuildingData(char* sfilename, int nGeoHashScale = 8);
	//计算RDF value
	bool calculateRdfValues(ObservedSphere& obs);

public:
	//判断某个建筑物在球体内的面积和百分比
	bool buildingSpaceSizeInSphere(float dlat, float dlon, float dR, \
		Building bd, float& dRatio, float& dSpaceSize);
	//判断某个建筑物在球壳内的面积和百分比(两者相减)
	bool buildingSpaceSizeInSphereShell(float dlat, float dlon, float dMinR, \
		float dMaxR, Building bd, float& dRatio, float& dSpaceSize);

protected:
	QHash<QString, QList<Building>> mvHashBuildings;
	int mnGeoHashScale;
};






#endif


