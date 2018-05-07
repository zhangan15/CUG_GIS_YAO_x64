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
	float dlat, dlon;
	float dPerFloorArea;
	float dDemButtom;	//DEM�߳�
	float dBuildingHeight;
	float dTotalArea;
};

struct ObservedSphere
{
	float dlat, dlon;
	float dCenterHeight;
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
	//���nFloorCol = -1, ��ֻ����Height
	//���nHeight = -1, ��ֻ����Height = nFloor * nPerFloorHeight
	//���nFloorCol & nHeight = -1, ����false
	//��Ҫ������������0��ʼ����
	bool loadBuildingData(char* sfilename, int nLatCol, int nLonCol, int nPerFloorAreaCol, \
		int nFloorCol = -1, int nHeightCol = -1, double dPerFloorHeight = 3.0f,\
		int nGeoHashScale = 8);
	//����RDF value
	bool calculateRdfValues(ObservedSphere& obs);

public:
	//�ж�ĳ���������������ڵ�����Ͱٷֱ�
	bool buildingSpaceSizeInSphere(float dlat, float dlon, float dCenterHeight, float dR, \
		Building bd, float& dRatio, float& dSpaceSize);
	//�ж�ĳ��������������ڵ�����Ͱٷֱ�(�������)
	bool buildingSpaceSizeInSphereShell(float dlat, float dlon, float dCenterHeight, float dMinR, \
		float dMaxR, Building bd, float& dRatio, float& dSpaceSize);

private:
	void setAllGeoHashEnable();
	double calculateSphereSegmentVol(double dDisHeight, double dR);

protected:
	QHash<QString, QList<Building>> mvHashBuildings;	//�洢���еؿ��Building����
	QHash<QString, bool> mvHashEnable;		//���õؿ��Ƿ���������
	QStringList msHashList;		//GeoHash list
	int mnGeoHashScale;
};






#endif


