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
	unsigned char* readL(long _row, long _col, size_t _band);	//����ȡ����Ϊ�˴�������Ե����
	double linearRead(double _row, double _col, size_t _band);	//֧��˫���Բ�ֵ��������������ͷ���-9999

protected:
	// ģ�庯��������
	template<class TT> double linRead(double _row, double _col, size_t _band);

public:
	void close();
	bool isValid();

public:
	GDALDataset* poDataset();	//��ȡGDAL���ݼ�ָ��
	size_t rows();				//��ȡ���ݼ����� rows
	size_t cols();				//��ȡ���ݼ����� cols
	size_t bandnum();			//��ȡ���ݼ�������
	size_t datalength();		//��ȡ���ݼ��ֽڳ���
	double invalidValue();		//��ȡ���ݼ���Ч��ֵ(�������)
	unsigned char* imgData();	//��ȡ���ݼ�����ͷָ��
	GDALDataType datatype();	//��ȡ���ݼ�����
	double* geotransform();		//��ȡ���ݼ�����ת������	
	char* projectionRef();		//��ȡ���ݼ�ͶӰwkt�ַ���
	size_t perPixelSize();		//��ȡ���ݼ�����size

public:
	//��γ��lat,lonת��������
	bool world2Pixel(double lat, double lon, double *pcol, double *prow);
	//��������ת��γ��lat,lon
	bool pixel2World(double *lat, double *lon, double col, double row);
	// ��������תͶӰ����XY
	bool pixel2Ground(double col,double row,double* pX,double* pY);
	// ͶӰ����XYת��������
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



