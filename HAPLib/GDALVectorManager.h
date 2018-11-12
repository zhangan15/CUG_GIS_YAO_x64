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

			//获取属性信息
			OGRFeature* poFeature = poLayer->GetpoFeature(j);
			cout<<"+ Attribute Count = "<<poLayer->GetFieldCount()<<endl;
			for (k=0; k<poLayer->GetFieldCount(); k++)
			{
				//输出属性信息
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

	//设置波段名
	char* layerName = "test_shape";

	//设置属性信息
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
	//设置Feature信息
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
	//获取特征
	OGRFeature* GetpoFeature(int nFeaturesIndex);	//获取特征信息GetFieldAs...
	//获取形状
	OGRGeometry* GetpoFeatureGeometry(int nFeatureIndex);
	//获取形状的外接矩形
	OGREnvelope GetFeatureEnvelope(int nFeatureIndex);
	//获取几何对象的维度，0-点，1-线，2-面
	int GetFeatureDimension(int nFeatureIndex);
	//获取几何对象类型
	OGRwkbGeometryType GetFeatureGeometryType();
	OGRSpatialReference* GetpoSpatialReference();

public:
	//获取属性表头信息
	OGRFieldDefn* GetFieldDefine(int nFieldIndex);
	const char* GetFieldName(int nFieldIndex);

public:
	//获取表信息
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

//主类
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
	int mnLayerCount;	//层数
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


