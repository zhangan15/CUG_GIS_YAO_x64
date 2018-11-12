#ifndef GLOBAL_INCLUDE_DEFINE
#define GLOBAL_INCLUDE_DEFINE
//#include <armadillo>

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <time.h> 
#include <memory.h>
#include <string.h>
#include <omp.h>
#include <vector>

#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/PatternLayout.hh>

#include "cxml2config.h"
//#include <opencv2/opencv.hpp>
using namespace std;

#include "global_define.h"

class CGDALFileManager;

//�ⲿxml�ļ�
extern char* XML_CONFIG;
extern int MAX_THREAD_NUM;

extern char* SLCFILEPATH;	
extern char* SAUXFILEPATH;
extern char* SOPFILEPATH;

//д��log�ļ�
 void global_ilog(char* sMessage, char* sInstance = "info");
 void global_ilog(QString sMessage, char* sInstance = "info");

 //src image => dst image, projection & coordinate transform
 bool _Src2DstCoordTrans( double src_x, double src_y, double& dst_x, double& dst_y, CGDALFileManager* pSrcImg, CGDALFileManager* pDstImg );
 
 bool str_connect_begin(char* filepath, char* filename);


/****************************************************************
* ��γ�Ⱥ����������ת��
****************************************************************/
// 
// int _World2Pixel(double lat, double lon, double *x, double *y, double* geoTransform, const char* lpszSpatialReferrence);
// int _Pixel2World(double *lat, double *lon, double x, double y, double* geoTransform, const char* lpszSpatialReferrence);
// 
// /****************************************************************
// * �����������������ת��
// ****************************************************************/
// int _Pixel2Ground(double x,double y,double* pX,double* pY,double* geoTransform);
// int _Ground2Pixel(double X,double Y,double* px,double* py,double* geoTransform);
// 
// //�ж�һ����������ڲ���ͼ����
// //��ͼ���ڷ���true��ͬʱ�������������xy�У����򷵻�0��xyΪ-1
// /******************************************************************
// * gX, gY ��������
// * poDataset ң��Ӱ��
// * x, y ң��Ӱ�����꣬��0��ʼ
// ******************************************************************/
// //HIPAPI bool IsGroudXYInImg(double gX, double gY, GDALDataset* poDataset, double& x, double& y);
// bool _IsGroudXYInImg(double gX, double gY, CGDALFileManager* pImg, double& x, double& y);


//������ֵȨ��
/******************************************************************
* x ����ֵ
* x_min, x_max, x_mean ��Сֵ�����ֵ�;�ֵ
* isNearerBetter �Ƿ�Խ��Խ��
******************************************************************/
//double _calXPower(double x, double x_min, double x_max, double x_mean, bool isNearerBetter=true);

// #ifndef FEATRUE_POINT
// #define FEATRUE_POINT
// struct FeaturePoint
// {
// 	long x, y;		//��ͨ���꣬��ʱ��
// 	float lon;		//����
// 	float lat;		//γ��
// 	DistAuxiDT prob_DistCapitanCity;
// 	DistAuxiDT prob_DistPrefectureCity;
// 	DistAuxiDT prob_DistCountyTown;
// 	DistAuxiDT prob_DistMainRoad;
// 	DistAuxiDT prob_DistMainRailway;
// 	LandCoverDT class_code;	
// };
// #endif

#endif
