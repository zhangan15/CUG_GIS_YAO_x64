/*
Author: Yao
Version 1.1.0

How to Use:
#include "GDALVectorManager.h"
#include <iostream>
using namespace std;

void testVectorManager()
{
	CGDALVectorManager vm;
	vm.LoadFrom("F:/data/SYSU_SHP/BuildingSpot.shp");

	cout<<vm.GetLayerCount()<<endl;

	int i, j, k;
	for (i=0; i<vm.GetLayerCount(); i++)
	{
		cout<<"-------------------------------------------------------------------------------"<<endl;
		cout<<"Layer No."<<i+1<<endl;
		CGDALVectorLayer* poLayer = vm.GetpLayer(i);
		for (j=0; j<vm.GetpLayer(i)->GetFeaturesCount(); j++)
		{
			cout<<"Feature No."<<j+1<<endl;

			cout<<"Dimension = "<<poLayer->GetFeatureDimension(j)<<endl;
			cout<<"Geometry Type = "<<poLayer->GetFeatureGeometryType()<<endl;

			OGREnvelope rect = poLayer->GetFeatureEnvelope(j);
			cout<<"Envelope = "<<"( "<<rect.MinX<<", "<<rect.MinY<<" ) -> "<<"( "<<rect.MaxX<<", "<<rect.MaxY<<" )"<<endl;

			//��ȡ������Ϣ
			OGRFeature* poFeature = poLayer->GetpoFeature(j);
			cout<<"+ Attribute Count = "<<poLayer->GetFieldCount()<<endl;
			for (k=0; k<poLayer->GetFieldCount(); k++)
			{
				//���������Ϣ
				cout<<"- "<<poLayer->GetFieldDefine(k)->GetNameRef()<<" = "<<poLayer->GetFieldAttributeAsString(j, k)<<endl;
			}

			cout<<"-------------------------------------------------------------------------------"<<endl;

		}		
	}

}

void testVectorMaker()
{
	CGDALVectorMaker maker;
	if (!maker.CreateDataSource("f:/data"))
	{
		cout<<"make datasource error!"<<endl;
		return;
	}

	//���ò�����
	char* layerName = "test_shape";

	//����������Ϣ
	OGRFieldDefn f1("NAME", OFTString);
	f1.SetWidth(32);
	OGRFieldDefn f2("ID", OFTInteger);
	OGRFieldDefn f3("HEIGHT", OFTReal);
	f3.SetPrecision(6);

	OGRFeatureDefn fdefn;
	fdefn.AddFieldDefn(&f1);
	fdefn.AddFieldDefn(&f2);
	fdefn.AddFieldDefn(&f3);

	int i=0;
	//����Feature��Ϣ
	vector<OGRFeature*> vf1;
	for (i=0; i<1000; i++)
	{
		OGRFeature* f = OGRFeature::CreateFeature(&fdefn);
		f->SetField("NAME", "PT");
		f->SetField("ID", 1000+i);
		f->SetField("HEIGHT", 1000*rand()/RAND_MAX);

		OGRPoint pt;
		pt.setX(1000*rand()/RAND_MAX);
		pt.setY(1000*rand()/RAND_MAX);

		f->SetGeometry(&pt);
		vf1.push_back(f);
	}
	maker.AddLayer(layerName, &fdefn,  vf1);
	CGDALVectorMaker::RemoveVectorOGRFeatures(vf1);

	maker.CloseDataSource();
}

*/


#ifndef GDALVECTORMANAGER_H
#define GDALVECTORMANAGER_H
#include "hapheader.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include <vector>
using namespace std;
USING_NAMESPACE(HAPLIB)

class HAPCLASS CGDALVectorLayer
{
public:
	CGDALVectorLayer();
	~CGDALVectorLayer();

public:
	CGDALVectorLayer(int nID, OGRLayer* poLayer);

public:
	bool IsValid();
	int GetID();
	int GetFeaturesCount();
	int GetFieldCount();

public:
	void init();

public:
	//��ȡ����
	OGRFeature* GetpoFeature(int nFeaturesIndex);	//��ȡ������ϢGetFieldAs...
	//��ȡ��״
	OGRGeometry* GetpoFeatureGeometry(int nFeatureIndex);
	//��ȡ��״����Ӿ���
	OGREnvelope GetFeatureEnvelope(int nFeatureIndex);
	//��ȡ���ζ����ά�ȣ�0-�㣬1-�ߣ�2-��
	int GetFeatureDimension(int nFeatureIndex);
	//��ȡ���ζ�������
	OGRwkbGeometryType GetFeatureGeometryType();
	OGRSpatialReference* GetpoSpatialReference();

public:
	//��ȡ���Ա�ͷ��Ϣ
	OGRFieldDefn* GetFieldDefine(int nFieldIndex);
	const char* GetFieldName(int nFieldIndex);

public:
	//��ȡ����Ϣ
	const char* GetFieldAttributeAsString(int nFeatureIndex, int nFieldIndex);
	int GetFieldAttributeAsInteger(int nFeatureIndex, int nFieldIndex);
	double GetFieldAttributeAsDouble(int nFeatureIndex, int nFieldIndex);
	const int* GetFieldAttributeAsIntegerList(int nFeatureIndex, int nFieldIndex, int *pnCount );
	const double* GetFieldAttributeAsDoubleList(int nFeatureIndex, int nFieldIndex, int *pnCount);
	char** GetFieldAttributeAsStringList(int nFeatureIndex, int nFieldIndex);
	GByte* GetFieldAttributeAsBinary(int nFeatureIndex, int nFieldIndex, int *pnCount);
	int GetFieldAttributeAsDateTime(int nFeatureIndex, int nFieldIndex,  
														int *pnYear, int *pnMonth, int *pnDay,
														int *pnHour, int *pnMinute, int *pnSecond, 
														int *pnTZFlag);

	const char* GetFieldAttributeAsString(int nFeatureIndex, const char *pszFieldName);
	int GetFieldAttributeAsInteger(int nFeatureIndex, const char *pszFieldName);
	double GetFieldAttributeAsDouble(int nFeatureIndex, const char *pszFieldName);
	const int* GetFieldAttributeAsIntegerList(int nFeatureIndex, const char *pszFieldName, int *pnCount );
	const double* GetFieldAttributeAsDoubleList(int nFeatureIndex, const char *pszFieldName, int *pnCount);
	char** GetFieldAttributeAsStringList(int nFeatureIndex, const char *pszFieldName);

public:
	bool SetLayer(int nID, OGRLayer* poLayer);
	OGRLayer* GetpLayer();

public:
	bool world2Gound(double lat, double lon, double* x, double* y);
	bool ground2World(double* lat, double* lon, double x, double y);

	OGRSpatialReference* latLongSR;
	OGRCoordinateTransformation* poTransform;	//G2W
	OGRCoordinateTransformation* poTransformT;	//W2G

private:
	int mnID;
	char msLayerName[1024];
	OGRwkbGeometryType mwkbGeoType;
	OGRSpatialReference* mpoSpatialRef;
	OGRLayer* mpLayer;
	//vector<OGRFeature*> mvpFeatures;
	OGRFeature* mpFeature;

private:
	vector<long> mvFID;


};

//����
class HAPCLASS CGDALVectorManager
{
public:
	CGDALVectorManager(void);
	~CGDALVectorManager(void);

public:
	CGDALVectorManager(const char* filename);

public:
	bool LoadFrom(const char* filename);
	bool IsValid();

public:
	GDALDataset* GetpoDS();
	CGDALVectorLayer* GetpLayer(int nLayerIndex);
	int GetLayerCount();
	

protected:
	void init();

protected:
	char msFilename[1024];
	GDALDataset *mpoDS;
	int mnLayerCount;	//����
	vector<CGDALVectorLayer*> mvpLayers;
};


class HAPCLASS CGDALVectorMaker
{
public:
	CGDALVectorMaker();
	~CGDALVectorMaker();

public:
	bool CreateDataSource(char* pszDataSourceName, char* pszDriverName = "ESRI Shapefile" );
	bool AddLayer(char* layerName, 
							OGRFeatureDefn* poFeatureDefn,
							vector<OGRFeature*> vpoFeature,
							OGRSpatialReference *poSpatialRef = NULL,
							OGRwkbGeometryType eGType = wkbPoint,
							char ** papszOptions = NULL
		);

	bool CloseDataSource();

public:
	static void RemoveVectorOGRFeatures(vector<OGRFeature*>& vpoFeature);

protected:
	void init();

protected:
	char mpszDriverName[1024];
	char mpszDataSourceName[1024];
	GDALDataset* mpoDS;
	GDALDriver* mpoDriver;
	//OGRLayer* mpoLayer;
};

#endif //GDALVECTORMANAGER_H


