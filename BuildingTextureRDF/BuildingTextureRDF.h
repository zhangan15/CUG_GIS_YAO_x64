#ifndef BUILDING_RDF_TEXTURE_BY_YAO_V10
#define BUILDING_RDF_TEXTURE_BY_YAO_V10


#include <QHash>
#include <QString>
#include <QList>
#include <QStringList>

#define PI 3.141592654

/************************************************************************/
/* In this model, The unit is unified to meter or square meters.        */
/************************************************************************/

struct Building 
{
	long nFID;
	float dlat, dlon;
	float dPerFloorArea;
	float dDemButtom;	//DEM高程
	float dBuildingHeight;
	float dTotalArea;
};

struct ObservedSphere
{
	long nFID = 0;
	float dlat, dlon;
	float dCenterHeight;
	float dMaxRadius;
	float dStepRadius;
	QList<float> vdRdfValues;
	float dEntropy;
};

class BuildingTextureRDF
{
public:
	BuildingTextureRDF();
	~BuildingTextureRDF();

public:
	//载入数据，设定级数
	//如果nFloorCol = -1, 则只考虑Height
	//如果nHeight = -1, 则只考虑Height = nFloor * nPerFloorHeight
	//如果nFloorCol & nHeight = -1, 返回false
	//需要输入列数，从0开始计数
	bool loadBuildingData(char* sfilename, int nFIDCol, int nLatCol, int nLonCol, int nPerFloorAreaCol, \
		int nFloorCol = -1, int nHeightCol = -1, int nDemCol = -1, double dPerFloorHeight = 3.0f, \
		int nGeoHashScale = 8, bool bIsNormal = true);
	//计算RDF value
	bool calculateRdfValues(ObservedSphere& obs);

public:
	//计算若干球形的RDF value
	bool calculateSeveralRfdValues(QList<ObservedSphere>& vObs);

public:
	//判断某个建筑物在球体内的面积和百分比
	bool buildingSpaceSizeInSphere(float dlat, float dlon, float dCenterHeight, float dR, \
		Building bd, float& dRatio, float& dSpaceSize);
	//判断某个建筑物在球壳内的面积和百分比(两者相减)
	bool buildingSpaceSizeInSphereShell(float dlat, float dlon, float dCenterHeight, float dMinR, \
		float dMaxR, Building bd, float& dRatio, float& dSpaceSize);

private:
	void setAllGeoHashEnable();
	double calculateSphereSegmentVol(double dDisHeight, double dR);
	void logLn(QString smsg);

public:
	QHash<QString, QList<Building>> mvHashBuildings;	//存储所有地块的Building数据

private:
	QHash<QString, bool> mvHashEnable;		//设置地块是否允许增长
	QStringList msHashList;		//GeoHash list
	int mnGeoHashScale;
	bool mbIsNormailization;
};






#endif


