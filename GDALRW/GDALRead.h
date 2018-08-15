/************************************************************************/
/* This class is wrote for supercomputer to read image use parallel.
/* Not support Block read & write
/* But support multi-thread
/* Author: Y. Yao
/* E-mail: yaoy@cug.edu.cn
/* Version: v4.5
/************************************************************************/

#ifndef CLASS_GDAL_READ
#define CLASS_GDAL_READ

#include "gdal_priv.h"
#include "ogr_core.h"
#include "ogr_spatialref.h"

class CGDALRead
{
public:
	CGDALRead(void);
	~CGDALRead(void);

public:
	bool loadFrom(const char* _filename);
	unsigned char* read(size_t _row, size_t _col, size_t _band);
	unsigned char* readL(long _row, long _col, size_t _band);	//超限取镜像，为了处理卷积边缘问题
	double linearRead(double _row, double _col, size_t _band);	//支持双线性插值，错误的数据类型返回-9999

protected:
	// 模板函数，慎用
	template<class TT> double linRead(double _row, double _col, size_t _band);

public:
	void close();
	bool isValid();

public:
	GDALDataset* poDataset();	//获取GDAL数据集指针
	size_t rows();				//获取数据集行数 rows
	size_t cols();				//获取数据集列数 cols
	size_t bandnum();			//获取数据集波段数
	size_t datalength();		//获取数据集字节长度
	double invalidValue();		//获取数据集无效数值(如果设置)
	unsigned char* imgData();	//获取数据集数据头指针
	GDALDataType datatype();	//获取数据集类型
	double* geotransform();		//获取数据集坐标转换参数	
	char* projectionRef();		//获取数据集投影wkt字符串
	size_t perPixelSize();		//获取数据集像素size

public:
	//经纬度lat,lon转像素坐标
	bool world2Pixel(double lat, double lon, double *pcol, double *prow);
	//像素坐标转经纬度lat,lon
	bool pixel2World(double *lat, double *lon, double col, double row);
	// 像素坐标转投影坐标XY
	bool pixel2Ground(double col,double row,double* pX,double* pY);
	// 投影坐标XY转像素坐标
	bool ground2Pixel(double X,double Y,double* pcol,double* prow);

protected:
	template<class TT> bool readData();

protected:
	GDALDataset* mpoDataset;	//=>
	size_t mnRows;					//
	size_t mnCols;					//
	size_t mnBands;				//
	unsigned char* mpData;		//=>
	GDALDataType mgDataType;	//
	size_t mnDatalength;			//=>
	double mpGeoTransform[6];	//
	char msProjectionRef[2048];	//
	char msFilename[2048];		//
	double mdInvalidValue;
	size_t mnPerPixSize;			//=>

public:
	OGRSpatialReferenceH srcSR;
	OGRSpatialReferenceH latLongSR;
	OGRCoordinateTransformationH poTransform;		//pixel->world
	OGRCoordinateTransformationH poTransformT;		//world->pixel
};

#endif



