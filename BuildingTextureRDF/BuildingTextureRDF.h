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
	int nHeightButtom;	//DEM�߳�
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
	//�������ݣ��趨����
	bool loadBuildingData(char* sfilename, int nGeoHashScale = 8);
	//����RDF value
	bool calculateRdfValues(ObservedSphere& obs);

public:
	//�ж�ĳ���������������ڵ�����Ͱٷֱ�
	bool buildingSpaceSizeInSphere(float dlat, float dlon, float dR, \
		Building bd, float& dRatio, float& dSpaceSize);
	//�ж�ĳ��������������ڵ�����Ͱٷֱ�(�������)
	bool buildingSpaceSizeInSphereShell(float dlat, float dlon, float dMinR, \
		float dMaxR, Building bd, float& dRatio, float& dSpaceSize);

protected:
	QHash<QString, QList<Building>> mvHashBuildings;
	int mnGeoHashScale;
};






#endif


