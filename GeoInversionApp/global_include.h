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

//外部xml文件
extern char* XML_CONFIG;
extern int MAX_THREAD_NUM;

extern char* SLCFILEPATH;	
extern char* SAUXFILEPATH;
extern char* SOPFILEPATH;

//写入log文件
 void global_ilog(char* sMessage, char* sInstance = "info");
 void global_ilog(QString sMessage, char* sInstance = "info");

 //src image => dst image, projection & coordinate transform
 bool _Src2DstCoordTrans( double src_x, double src_y, double& dst_x, double& dst_y, CGDALFileManager* pSrcImg, CGDALFileManager* pDstImg );
 
 bool str_connect_begin(char* filepath, char* filename);


/****************************************************************
* 经纬度和像素坐标的转变
****************************************************************/
// 
// int _World2Pixel(double lat, double lon, double *x, double *y, double* geoTransform, const char* lpszSpatialReferrence);
// int _Pixel2World(double *lat, double *lon, double x, double y, double* geoTransform, const char* lpszSpatialReferrence);
// 
// /****************************************************************
// * 大地坐标和像素坐标的转变
// ****************************************************************/
// int _Pixel2Ground(double x,double y,double* pX,double* pY,double* geoTransform);
// int _Ground2Pixel(double X,double Y,double* px,double* py,double* geoTransform);
// 
// //判断一个大地坐标在不在图像内
// //在图像内返回true，同时将像素坐标存入xy中，否则返回0，xy为-1
// /******************************************************************
// * gX, gY 地面坐标
// * poDataset 遥感影像
// * x, y 遥感影像坐标，从0开始
// ******************************************************************/
// //HIPAPI bool IsGroudXYInImg(double gX, double gY, GDALDataset* poDataset, double& x, double& y);
// bool _IsGroudXYInImg(double gX, double gY, CGDALFileManager* pImg, double& x, double& y);


//计算数值权重
/******************************************************************
* x 输入值
* x_min, x_max, x_mean 最小值，最大值和均值
* isNearerBetter 是否越近越好
******************************************************************/
//double _calXPower(double x, double x_min, double x_max, double x_mean, bool isNearerBetter=true);

// #ifndef FEATRUE_POINT
// #define FEATRUE_POINT
// struct FeaturePoint
// {
// 	long x, y;		//普通坐标，临时用
// 	float lon;		//经度
// 	float lat;		//纬度
// 	DistAuxiDT prob_DistCapitanCity;
// 	DistAuxiDT prob_DistPrefectureCity;
// 	DistAuxiDT prob_DistCountyTown;
// 	DistAuxiDT prob_DistMainRoad;
// 	DistAuxiDT prob_DistMainRailway;
// 	LandCoverDT class_code;	
// };
// #endif

#endif
