#ifndef CLASS_URBAN_CHANGE_PROBABILITY_ANALYSIS_VERSION_1
#define CLASS_URBAN_CHANGE_PROBABILITY_ANALYSIS_VERSION_1


#include <QtCore/QList>
#include <QtCore/QString>

#include "gdalfilemanager.h"
#include "cxml2config.h"
#include "alglib/statistics.h"
#include "alglib/dataanalysis.h"
#include "alglib/alglibmisc.h"
#include "LogicRegression/LR.h"

#include "global_define.h"
class HAPBEGBase;


/**
加入新辅助数据时：
1 辅助图像CGDALFileManager（数据）, StaticBag（统计）, HAPBEGBase（IO）;
2 更新构造函数和析构函数（close()）, 更新FeaturePoint类;
3 更新loadAuxiliaryData();
4 更新makeRandomPointFiles(), 每个随机点添加变量;
5 更新writeRandomPointsToFile(), 写入控制点文件更新;
6 更新CalWholeImageDevelpedProbability()
**/

//定义特征点
#ifndef FEATRUE_POINT_V1
#define FEATRUE_POINT_V1
class FeaturePoint_V1
{
public:
	FeaturePoint_V1()
	{
		x=0, y=0;		//普通坐标，临时用
		lon=0;		//经度
		lat=0;		//纬度
		prob_DistCapitanCity=0;
		prob_DistPrefectureCity=0;
		prob_DistCountyTown=0;
		prob_DistMainRoad=0;
		prob_DistMainRailway=0;
		class_code=0;	
	}

	~FeaturePoint_V1(){}

	bool isValid()
	{
		if (class_code==0 || prob_DistCapitanCity<=0 || prob_DistPrefectureCity<=0\
			|| prob_DistCountyTown<=0 || prob_DistMainRoad<=0 || prob_DistMainRailway<=0)
		{
			return false;
		}

		return true;
	}

	friend bool operator<(const FeaturePoint_V1& pt1, const FeaturePoint_V1& pt2)
	{
		return pt1.x<pt2.x;
	}

public:
	int x, y;		//普通坐标，临时用
	double lon;		//经度
	double lat;		//纬度
	DistAuxiDT prob_DistCapitanCity;
	DistAuxiDT prob_DistPrefectureCity;
	DistAuxiDT prob_DistCountyTown;
	DistAuxiDT prob_DistMainRoad;
	DistAuxiDT prob_DistMainRailway;
	LandCoverDT class_code;	
};
#endif


class StaticBag_V1 
{
public:
	StaticBag_V1()
	{
		minVal = maxVal = meanVal = 0;
	}

	~StaticBag_V1(){}

	//返回归化概率
	/******************************************************************
	* x 输入值
	* x_min, x_max, x_mean 最小值，最大值和均值
	* isNearerBetter 是否越近越好
	******************************************************************/
	double calXPower(double x, bool isNearerBetter=true)
	{
		if (maxVal <= minVal)
			return -1;

		double _p;
		if (x<minVal)
			_p = 0;
		else if (x>maxVal)
			_p = 1;
		else if (x>minVal && x<meanVal)
			_p = 0.5*(x-minVal)/(meanVal-minVal);
		else if (x>=meanVal && x<maxVal)
			_p = 0.5+0.5*(x-meanVal)/(maxVal-meanVal);

		if (isNearerBetter)
			return 1-_p;
		else
			return _p;
	}

public:
	double minVal;
	double maxVal;
	double meanVal;
};

class UrbanChangeProbabilityAnalysis_V1
{
public:
	UrbanChangeProbabilityAnalysis_V1(void);
	~UrbanChangeProbabilityAnalysis_V1(void);

public:
	bool run();
	void close();

protected:
	//载入图像文件，返回0执行成功，<0 失败
	//-1: 解析xml失败
	//-2: 载入图像失败
	//-3: IO失败
	int loadUrbanChangeData();

	//载入辅助数据，返回0执行成功，<0失败
	//-1: 解析xml文件信息失败
	//-301, -302, -303: 解析统计值失败
	//-2: 载入图像失败
	//-4: IO失败
	int loadAuxiliaryData();

	//在图像上获取随机像素函数，存储入文本文件，返回0成功，<0失败
	//-1: 解析xml随机文件位置节点失败
	//-2: 解析xml坐标数目节点失败
	//-5: 制作随机坐标文件失败
	int makeRandomPointFiles();

protected:
	//mnClassifiMode = 0
	//训练随机森林分类器。返回0成功，<0失败
	//-1: 解析xml失败
	//-2: 打开随机点位文件失败
	//-3: 训练失败
	int trainRandomForestClassifier();	

	//mnClassifiMode = 1
	//训练逻辑回归分类器。返回0成功，<0失败
	//-1: 解析xml失败
	//-2: 相关参数获取失败
	//-3: 打开随机点位文件失败
	//-4: 训练失败
	//-5: model file载入失败
	//-6: 训练精度太低
	int trainLRClassifier();

	//mnCLassifiMode = 2
	//训练神经网络分类器。返回0成功，<0失败
	//-1: 解析xml失败
	//-2: 打开随机点位文件失败
	int trainNNClassifer();

protected:
	//计算全局开发概率，并输出文件，返回0成功，<0失败
	//-1: 没有分类器
	//-2: 有IO为NULL
	//-3: 设置输出数据失败
	int CalWholeImageDevelpedProbability();




private:
	//输出9波段概率文件
	CGDALFileManager* mpProbFor9;
	bool mbIsOpProbFor9;

	//输出概率文件
	CGDALFileManager* mpProbabilityAll;	//全局概率文件

	//重分类结果
	CGDALFileManager* mpUrbanChange;
	HAPBEGBase* mpIO_UrbanChange;	//输入输出

	//所有辅助图像载入
	CGDALFileManager* mpAux_DistCapitanCity;			//与省会距离
	StaticBag_V1 msbAux_DistCapitanCity;
	HAPBEGBase* mpIn_DistCapitanCity;

	CGDALFileManager* mpAux_DistPrefectureCity;		//与地级市距离
	StaticBag_V1 msbAux_DistPrefectureCity;
	HAPBEGBase* mpIn_DistPrefectureCity;
	
	CGDALFileManager* mpAux_DistCountyTown;			//与城镇中心距离
	StaticBag_V1 msbAux_DistCountyTown;
	HAPBEGBase* mpIn_DistCountyTown;
	
	CGDALFileManager* mpAux_DistMainRoad;				//与主要公路距离
	StaticBag_V1 msbAux_DistMainRoad;
	HAPBEGBase* mpIn_DistMainRoad;
	
	CGDALFileManager* mpAux_DistMainRailway;			//与主要铁路距离
	StaticBag_V1 msbAux_DistMainRailway;
	HAPBEGBase* mpIn_DistMainRailway;

	//随机像素点
	QList<FeaturePoint_V1> m_lpts;

private:
	int mnClassifiMode;	//分类模式，0-随机森林，1-逻辑回归，2-神经网络

private:
	//随机森林分类器
	alglib::decisionforest m_dfClassifier;
	alglib::ae_int_t m_ndfInfo;		//判断其是否分类完毕 =1时候分类器可以使用
	alglib::dfreport m_dfReport;	//分类器报告

	//逻辑回归
	bool mbIsLRTraining;	//是否进行逻辑训练
	LR m_LRClassifier;	//逻辑回归分类器
	char m_smodelfile;	//模型文件地址

	//神经网络NN
	bool mbIsNNTraining;			//是否NN训练
	alglib::mlptrainer m_NNTrainer;	//NN训练器
	alglib::multilayerperceptron m_NNNetwork;	//多层神经网络
	alglib::mlpreport m_NNReport;


private:
	inline void _ilog(char* sMessage, char* sInstance= "info" );
	inline void _ilog( QString sMessage, char* sInstance = "info" );

	//载入辅助信息
	//-1: 解析xml文件信息失败
	//-301, -302, -303: 解析统计值失败
	//-2: 载入图像失败
	//-4: IO失败
	int loadAuxiliaryData(CGDALFileManager* &pmgr, HAPBEGBase* &pIO, char* node_name, char* sComment, StaticBag_V1* sbVal);

// 	inline bool Src2DstCoordTrans(double src_x, double src_y, double* dst_x, double* dst_y, \
// 		CGDALFileManager* pSrcImg, CGDALFileManager* pDstImg);


	//获取FeaturePoint的像素
	inline bool calFeaturePixelValue(DistAuxiDT& _prob, double _lat, double _lon, CGDALFileManager* pMgr, HAPBEGBase* pIO, StaticBag_V1* pSb);

	//将控制点写入文件
	bool writeRandomPointsToFile(char* sfilename);

private:
	/****************************************************************
	* 经纬度和像素坐标的转变
	****************************************************************/
	//int in_World2Pixel(double lat, double lon, double *x, double *y, double* geoTransform, const char* lpszSpatialReferrence);
	//int in_Pixel2World(double *lat, double *lon, double x, double y, double* geoTransform, const char* lpszSpatialReferrence);

	
};



#endif

