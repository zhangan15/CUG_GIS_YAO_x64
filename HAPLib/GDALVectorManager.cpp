#include "GDALVectorManager.h"
#include <iostream>
using namespace std;

CGDALVectorManager::CGDALVectorManager(void)
{
	mpoDS = NULL;
	init();	
}

CGDALVectorManager::CGDALVectorManager( const char* filename )
{
	mpoDS = NULL;
	init();	
	LoadFrom(filename);
}

CGDALVectorManager::~CGDALVectorManager(void)
{
	init();
}

void CGDALVectorManager::init()
{	
	strcpy(msFilename, "");
	mnLayerCount = 0;	

	for (int i=0; i<mvpLayers.size(); i++)
	{
		delete mvpLayers[i];
	}
	mvpLayers.clear();

	if (mpoDS!=NULL)
	{
		GDALClose(mpoDS);
		mpoDS = NULL;
	}
}

GDALDataset* CGDALVectorManager::GetpoDS()
{
	return mpoDS;
}

bool CGDALVectorManager::LoadFrom( const char* filename )
{
	//注册
	if(GDALGetDriverCount() == 0)
	{
		GDALAllRegister();
		CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	}

	OGRRegisterAll();

	init();
	//load from data
	mpoDS = static_cast<GDALDataset*>(GDALOpenEx( filename, GDAL_OF_VECTOR, NULL, NULL, NULL));

	if (mpoDS == NULL)
	{
		cout<<"CGDALVectorManager::LoadFrom: load vector failed!"<<endl;
		return false;
	}

	strcpy(msFilename, filename);
	mnLayerCount = mpoDS->GetLayerCount();

	for (int i=0; i<mnLayerCount; i++)
	{
		CGDALVectorLayer* layer = new CGDALVectorLayer;
		layer->SetLayer(i, mpoDS->GetLayer(i));
		if (layer->IsValid())
		{
			//cout<<"CGDALVectorManager::LoadFrom: load band - "<<i+1<<" success."<<endl;
			mvpLayers.push_back(layer);
		}
	}
	
	return true;
}

CGDALVectorLayer* CGDALVectorManager::GetpLayer( int nLayerIndex )
{
	return mvpLayers[nLayerIndex];
}

int CGDALVectorManager::GetLayerCount()
{
	return mnLayerCount;
}

bool CGDALVectorManager::IsValid()
{
	if (mpoDS == NULL || mnLayerCount == 0 || mvpLayers.size() == 0)
	{
		return false;
	}
	
	return true;
}



CGDALVectorLayer::CGDALVectorLayer()
{
	mpFeature =NULL;
	latLongSR = NULL;
	poTransform = poTransformT = NULL;
	init();
}

CGDALVectorLayer::CGDALVectorLayer( int nID, OGRLayer* poLayer )
{
	mpFeature =NULL;
	latLongSR = NULL;
	poTransform = poTransformT = NULL;
	init();

	SetLayer(nID, poLayer);
}

CGDALVectorLayer::~CGDALVectorLayer()
{
	init();
}

void CGDALVectorLayer::init()
{
	mnID = -1;
	mpLayer = NULL;
	strcpy(msLayerName, "");
	mvFID.clear();
	mwkbGeoType = wkbPoint;
	mpoSpatialRef = NULL;

	if (mpFeature!=NULL)
	{
		OGRFeature::DestroyFeature(mpFeature);
		mpFeature = NULL;
	}

	if (latLongSR!=NULL)
	{
		OSRDestroySpatialReference(latLongSR);
		latLongSR = NULL;
	}

	if (poTransform!=NULL)
	{
		OCTDestroyCoordinateTransformation(poTransform);
		poTransform = NULL;
	}

	if (poTransformT!=NULL)
	{
		OCTDestroyCoordinateTransformation(poTransformT);
		poTransformT = NULL;
	}
}


bool CGDALVectorLayer::IsValid()
{
	if (mnID == -1 || mpLayer == 0 || mvFID.size() == 0)
	{
		return false;
	}

	return true;
}

bool CGDALVectorLayer::SetLayer( int nID, OGRLayer* poLayer )
{
	if (poLayer == NULL || poLayer->GetFeatureCount() == 0)
		return false;
	
	init();
	mnID = nID;
	mpLayer = poLayer;
	strcpy(msLayerName, poLayer->GetName());
	mwkbGeoType = poLayer->GetGeomType();
	mpoSpatialRef = poLayer->GetSpatialRef();

	poLayer->ResetReading();
	for (int i=0; i<poLayer->GetFeatureCount(); i++)
	{
		OGRFeature* poFeature = poLayer->GetNextFeature();
		//mvpFeatures.push_back(poFeature);	
		mvFID.push_back(poFeature->GetFID());
	}

	//cout<<"CGDALVectorLayer::SetLayer: load features - "<<poLayer->GetFeatureCount()<<" success."<<endl;

	//trans
	OGRSpatialReference* pSrcSR = poLayer->GetSpatialRef();
	latLongSR=(OGRSpatialReference*)OSRCloneGeogCS(pSrcSR);
	poTransform = OGRCreateCoordinateTransformation(pSrcSR, latLongSR);
	poTransformT = OGRCreateCoordinateTransformation(latLongSR, pSrcSR);	


	return true;
	
}

int CGDALVectorLayer::GetID()
{
	return mnID;
}

int CGDALVectorLayer::GetFeaturesCount()
{
	return mvFID.size();
}

OGRFeature* CGDALVectorLayer::GetpoFeature( int nIndex )
{
	if (mpFeature!=NULL)
	{
		OGRFeature::DestroyFeature(mpFeature);
		mpFeature = NULL;
	}

	mpFeature = mpLayer->GetFeature(mvFID[nIndex]);
	return mpFeature;
}

OGRGeometry* CGDALVectorLayer::GetpoFeatureGeometry( int nFeatureIndex )
{
	return GetpoFeature(nFeatureIndex)->GetGeometryRef();
}

int CGDALVectorLayer::GetFieldCount()
{
	if (mvFID.size() == 0)
	{
		return 0;
	}

	return  GetpoFeature(0)->GetFieldCount();
}

OGREnvelope CGDALVectorLayer::GetFeatureEnvelope( int nFeatureIndex )
{
	OGRGeometry* poGeometry =  GetpoFeature(nFeatureIndex)->GetGeometryRef();
	
	OGREnvelope rect;
	poGeometry->getEnvelope(&rect);

	return rect;
}

int CGDALVectorLayer::GetFeatureDimension( int nFeatureIndex )
{
	OGRGeometry* poGeometry =  GetpoFeature(nFeatureIndex)->GetGeometryRef();
	return poGeometry->getDimension();	
}

OGRwkbGeometryType CGDALVectorLayer::GetFeatureGeometryType()
{
	//OGRGeometry* poGeometry =  GetpoFeature(nFeatureIndex)->GetGeometryRef();

	OGRwkbGeometryType _gt = mpLayer->GetGeomType();

	return wkbFlatten(_gt);

}


OGRFieldDefn* CGDALVectorLayer::GetFieldDefine( int nFieldIndex )
{
	//获取feature信息
	OGRFieldDefn* poFieldDefn = mpLayer->GetLayerDefn()->GetFieldDefn(nFieldIndex);
	return poFieldDefn;
}

const char* CGDALVectorLayer::GetFieldAttributeAsString( int nFeatureIndex, int nFieldIndex )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsString(nFieldIndex);
}

const char* CGDALVectorLayer::GetFieldAttributeAsString( int nFeatureIndex, const char *pszFieldName )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsString(pszFieldName);
}

const char* CGDALVectorLayer::GetFieldName( int nFieldIndex )
{
	return GetFieldDefine(nFieldIndex)->GetNameRef();
}

int CGDALVectorLayer::GetFieldAttributeAsInteger( int nFeatureIndex, int nFieldIndex )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsInteger(nFieldIndex);
}

int CGDALVectorLayer::GetFieldAttributeAsInteger( int nFeatureIndex, const char *pszFieldName )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsInteger(pszFieldName);
}

double CGDALVectorLayer::GetFieldAttributeAsDouble( int nFeatureIndex, int nFieldIndex )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsDouble(nFieldIndex);
}

double CGDALVectorLayer::GetFieldAttributeAsDouble( int nFeatureIndex, const char *pszFieldName )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsDouble(pszFieldName);
}

const int* CGDALVectorLayer::GetFieldAttributeAsIntegerList( int nFeatureIndex, int nFieldIndex, int *pnCount )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsIntegerList(nFieldIndex, pnCount);
}

const int* CGDALVectorLayer::GetFieldAttributeAsIntegerList( int nFeatureIndex, const char *pszFieldName, int *pnCount )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsIntegerList(pszFieldName, pnCount);
}

const double* CGDALVectorLayer::GetFieldAttributeAsDoubleList( int nFeatureIndex, int nFieldIndex, int *pnCount )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsDoubleList(nFieldIndex, pnCount);
}

const double* CGDALVectorLayer::GetFieldAttributeAsDoubleList( int nFeatureIndex, const char *pszFieldName, int *pnCount )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsDoubleList(pszFieldName, pnCount);
}

char** CGDALVectorLayer::GetFieldAttributeAsStringList(int nFeatureIndex, int nFieldIndex)
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsStringList(nFieldIndex);
}

char** CGDALVectorLayer::GetFieldAttributeAsStringList(int nFeatureIndex, const char *pszFieldName)
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsStringList(pszFieldName);
}


GByte* CGDALVectorLayer::GetFieldAttributeAsBinary( int nFeatureIndex, int nFieldIndex, int *pnCount )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsBinary(nFieldIndex, pnCount);
}

/*
GByte* CGDALVectorLayer::GetFieldAttributeAsBinary( int nFeatureIndex, const char *pszFieldName, int *pnCount )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsBinary(pszFieldName, pnCount);
}
*/

int CGDALVectorLayer::GetFieldAttributeAsDateTime( int nFeatureIndex, int nFieldIndex, int *pnYear, int *pnMonth, int *pnDay, int *pnHour, int *pnMinute, int *pnSecond, int *pnTZFlag )
{
	return GetpoFeature(nFeatureIndex)->GetFieldAsDateTime(nFieldIndex, pnYear, pnMonth, pnDay, pnHour, pnMinute, pnSecond, pnTZFlag);
}

OGRSpatialReference* CGDALVectorLayer::GetpoSpatialReference()
{
	return mpoSpatialRef;
}

OGRLayer* CGDALVectorLayer::GetpLayer()
{
	return mpLayer;
}

bool CGDALVectorLayer::ground2World( double* lat, double* lon, double x, double y )
{
	if( poTransform == NULL )  
	{
		return false;
	}
	else
	{
		*lon = x;
		*lat = y;		
		poTransform->Transform(1, lon, lat);

		return true;
	}	
}

bool CGDALVectorLayer::world2Gound( double lat, double lon, double* x, double* y )
{
	if( poTransformT == NULL )  
	{

		return false;
	}
	else
	{
		*x = lon;
		*y = lat;		
		poTransformT->Transform(1, x, y);

		return true;

	}	
}

// bool CGDALVectorLayer::CreateLayer( int nID, char* layerName, OGRSpatialReference *poSpatialRef /*= NULL*/, OGRwkbGeometryType eGType /*= wkbUnknown*/, char ** papszOptions /*= NULL*/ )
// {
// 	init();
// }

// bool CGDALVectorLayer::CreateLayer( OGRDataSource* poDS, char* layerName, OGRSpatialReference *poSpatialRef /*= NULL*/, OGRwkbGeometryType eGType /*= wkbUnknown*/, char ** papszOptions /*= NULL*/ )
// {
// 	init();
// 	mnID = 0;
// 	msLayerName = layerName;
// 	mpoSpatialRef = poSpatialRef;
// 	mwkbGeoType = eGType;
// 
// 	mpLayer = poDS->CreateLayer(layerName, poSpatialRef, eGType, papszOptions);
// 
// 	if (mpLayer == NULL)
// 		return false;
// 	else
// 		return true;
// 
// }


// int CGDALVectorLayer::GetFieldAttributeAsDateTime( int nFeatureIndex, const char *pszFieldName, int *pnYear, int *pnMonth, int *pnDay, int *pnHour, int *pnMinute, int *pnSecond, int *pnTZFlag )
// {
// 	return GetpoFeature(nFeatureIndex)->GetFieldAsDateTime(pszFieldName, pnYear, pnMonth, pnDay, pnHour, pnMinute, pnSecond, pnTZFlag);
// }

CGDALVectorMaker::CGDALVectorMaker()
{
	mpoDriver = NULL;
	mpoDS = NULL;
	//mpoLayer = NULL;
	init();
}

CGDALVectorMaker::~CGDALVectorMaker()
{
	init();	
}

// bool CGDALVectorMaker::CreateLayer( char* layerName, vector<OGRFieldDefn> voField, OGRSpatialReference *poSpatialRef /*= NULL*/, OGRwkbGeometryType eGType /*= wkbUnknown*/, char ** papszOptions)
// {
// 	//init();
// 	
// 	if (mpoDS == NULL)
// 	{
// 		cout<<"CGDALVectorMaker::CreateLayer: Datasource is not available!"<<endl;
// 		return false;
// 	}
// 
// 	if (mpoLayer!=NULL)
// 	{
// 		delete mpoLayer;
// 		mpoLayer = NULL;
// 	}
// 
// 	mpoLayer = mpoDS->CreateLayer(layerName, poSpatialRef, eGType, papszOptions);
// 
// 	if (mpoLayer == NULL)
// 	{
// 		cout<<"CGDALVectorMaker::CreateLayer: create layer error!"<<endl;
// 		return false;
// 	}
// 
// // 	for (int i=0; i<voField.size(); i++)
// // 	{
// // 		if (mpoLayer->CreateField(&(voField[i]))!=OGRERR_NONE)
// // 		{
// // 			cout<<"CGDALVectorMaker::CreateLayer: Creating Name Filed Failed - Field: "<<voField[i].GetNameRef()<<endl;
// // 			return false;
// // 		}
// // 	}
// 
// 	OGRFieldDefn f1("NAME", OFTString);
// 	mpoLayer->CreateField(&f1);
// 
// 	OGRFieldDefn f2("HEIGHT", OFTReal);
// 	mpoLayer->CreateField(&f2);
// 
// 
// 	/////测试写入
// 	for (int i=0; i<100; i++)
// 	{
// 		OGRFeature* poFeature = OGRFeature::CreateFeature(mpoLayer->GetLayerDefn());
// 		poFeature->SetField("NAME", "POINT1");
// // 		poFeature->SetField("FID", 1000+i);
//  		poFeature->SetField("HEIGHT", 100*rand()/RAND_MAX);
// 
// 		OGRPoint pt;
// 		pt.setX(1000*rand()/RAND_MAX);
// 		pt.setY(1000*rand()/RAND_MAX);
// 		poFeature->SetGeometry(&pt);
// 
// 		if (mpoLayer->CreateFeature(poFeature) != OGRERR_NONE)
// 		{
// 			cout<<"create feature error!"<<endl;
// 			OGRFeature::DestroyFeature(poFeature);
// 			return false;
// 		}
// 
// 		OGRFeature::DestroyFeature(poFeature);
// 	}
// 	
// 	
// 	return true;
// }

void CGDALVectorMaker::init()
{
	strcpy(mpszDriverName, "ESRI Shapefile");
	strcpy(mpszDataSourceName, "");

// 	delete mpoLayer;
// 	mpoLayer = NULL;

	if (mpoDS!=NULL)
		GDALClose(mpoDS);
	mpoDS = NULL;

	delete mpoDriver;
	mpoDriver = NULL;	
}

bool CGDALVectorMaker::CreateDataSource( char* pszDataSourceName, char* pszDriverName /*= "ESRI Shapefile" */ )
{
	OGRRegisterAll();

	init();	
	strcpy(mpszDriverName, pszDriverName);
	strcpy(mpszDataSourceName, pszDataSourceName);

	//注册
	mpoDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName);
	if (mpoDriver==NULL)
	{
		cout<<"CGDALVectorMaker::CreateDataSource: unknown DriverName"<<endl;
		return false;
	}

	//
	mpoDS = mpoDriver->Create(mpszDataSourceName, 0, 0, 0, GDT_Unknown, NULL);
	if (mpoDS == NULL)
	{
		cout<<"CGDALVectorMaker::CreateDataSource: create Datasource error!"<<endl;
		return false;
	}

	return true;
}

// bool CGDALVectorMaker::AddFeature( OGRFeature* poFeature )
// {
// 	if (mpoLayer == NULL || poFeature == NULL)
// 		return false;
// 	
// 	if (mpoLayer->CreateFeature(poFeature) != OGRERR_NONE)
// 	{
// 		cout<<"CGDALVectorMaker::AddFeature: add feature error."<<endl;
// 		OGRFeature::DestroyFeature(poFeature);
// 		return false;
// 	}
// 
// 	OGRFeature::DestroyFeature(poFeature);
// 	return true;
// }

bool CGDALVectorMaker::CloseDataSource()
{
	init();
	return true;
}

bool CGDALVectorMaker::AddLayer( char* layerName, 
														OGRFeatureDefn* poFeatureDefn,
														vector<OGRFeature*> vpoFeature, 
														OGRSpatialReference *poSpatialRef /*= NULL*/, 
														OGRwkbGeometryType eGType /*= wkbPoint*/, 
														char ** papszOptions /*= NULL */ )
{
	if (mpoDS == NULL)
	{
		cout<<"CGDALVectorMaker::CreateLayer: Datasource is not available!"<<endl;
		return false;
	}

	OGRLayer* poLayer = mpoDS->CreateLayer(layerName, poSpatialRef, eGType, papszOptions);

	if (poLayer == NULL)
	{
		cout<<"CGDALVectorMaker::CreateLayer: create layer error!"<<endl;
		return false;
	}

	int i = 0;
	//添加field
	for (i=0; i<poFeatureDefn->GetFieldCount(); i++)
	{
		if (poLayer->CreateField(poFeatureDefn->GetFieldDefn(i))!=OGRERR_NONE)
		{
			cout<<"CGDALVectorMaker::CreateLayer: Creating Name Filed Failed - Field: "<<poFeatureDefn->GetFieldDefn(i)->GetNameRef()<<endl;
			return false;
		}
	}

	//添加feature
	for (int i=0; i<vpoFeature.size(); i++)
	{
		OGRFeature* poFeature = vpoFeature[i];

		if (poFeature->GetFieldCount() != poFeatureDefn->GetFieldCount() || poFeature->GetGeometryRef() == NULL)
		{
			cout<<"No."<<i+1<<" Feature is unavailable."<<endl;
			continue;
		}

		if (poLayer->CreateFeature(poFeature) != OGRERR_NONE)
		{
			cout<<"No."<<i+1<<" Feature is uncreation."<<endl;
			continue;
		}

		cout<<"No."<<i+1<<" Feature Created Successed."<<endl;
		//OGRFeature::DestroyFeature(poFeature);
	}

	//完成
	return true;
}

void CGDALVectorMaker::RemoveVectorOGRFeatures( vector<OGRFeature*>& vpoFeature )
{
	int i=0;
	for (i=0; i<vpoFeature.size(); i++)
	{
		OGRFeature::DestroyFeature(vpoFeature[i]);
		vpoFeature[i] = NULL;
	}

	vpoFeature.clear();
}
