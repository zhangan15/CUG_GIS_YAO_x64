#ifndef CLASS_URBAN_CHANGE_PROBABILITY_ANALYSIS_VERSION_2
#define CLASS_URBAN_CHANGE_PROBABILITY_ANALYSIS_VERSION_2

#include <QtCore/QList>
#include <QtCore/QString>

#include "gdalfilemanager.h"
#include "cxml2config.h"
#include "alglib/statistics.h"
#include "alglib/dataanalysis.h"
#include "alglib/alglibmisc.h"
#include "LogicRegression/LR.h"

#include "ImmuneAlgorithm/AIS.h"
#include "ImmuneAlgorithm/AG.h"
#include "ImmuneAlgorithm/immune.h"

#include "global_define.h"
class HAPBEGBase;

//定义特征点
#ifndef FEATRUE_POINT_V2
#define FEATRUE_POINT_V2
class FeaturePoint_V2
{
public:
	FeaturePoint_V2()
	{
		x=0, y=0;		//普通坐标，临时用
		lon=0;		//经度
		lat=0;		//纬度
		probs.clear();
		class_code=0;	
	}

	~FeaturePoint_V2(){}

	bool isValid()
	{
		//?????????????????????????????????maybe not right
// 		foreach (DistAuxiDT _t, probs)
// 		{
// 			if (_t <= 0)
// 				return false;
// 		}
		if (probs.size() <= 0)
			return false;

		return true;
	}

	friend bool operator<(const FeaturePoint_V2& pt1, const FeaturePoint_V2& pt2)
	{
		return pt1.y<pt2.y;
	}

	bool clear()
	{
		x=0, y=0;		//普通坐标，临时用
		lon=0;		//经度
		lat=0;		//纬度
		probs.clear();
		class_code=0;	
	}

	void addProbValue(DistAuxiDT val)
	{
		probs.append(val);
	}

public:
	int x, y;		//普通坐标，临时用
	double lon;		//经度
	double lat;		//纬度
	QList<DistAuxiDT> probs;
	LandCoverDT class_code;	
};
#endif

#ifndef STATIC_BAG_V2
#define STATIC_BAG_V2
class StaticBag_V2
{
public:
	StaticBag_V2()
	{
		minVal = maxVal = meanVal = 0;
		isNearBetter = true;
	}

	~StaticBag_V2(){}

	//返回归化概率
	/******************************************************************
	* x 输入值
	* x_min, x_max, x_mean 最小值，最大值和均值
	* isNearerBetter 是否越近越好
	******************************************************************/
	double calXPower(double x)
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

		if (isNearBetter)
			return 1-_p;
		else
			return _p;
	}

public:
	double minVal;
	double maxVal;
	double meanVal;
	bool isNearBetter;
};
#endif

class UrbanChangeProbabilityAnalysis_V2
{
public:
	UrbanChangeProbabilityAnalysis_V2(void);
	~UrbanChangeProbabilityAnalysis_V2(void);


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

	//mnCLassifiMode = 3
	//训练免疫算法分类器。返回0成功，<0失败
	//-1: 解析xml失败
	//-2: 打开随机点位文件失败
	int trainAGClassifier();

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

	//辅助概率文件
// 	bool mbIsUseAddProb;
// 	CGDALFileManager* mpAddProb;
// 	HAPBEGBase* mpIO_AddProb;

	QList<CGDALFileManager*> mvpMgr;
	QList<StaticBag_V2>	msbData;
	QList<HAPBEGBase*>	mvpIO;

	//随机像素点
	QList<FeaturePoint_V2> m_lpts;

private:
	int mnClassifiMode;	//分类模式，0-随机森林，1-逻辑回归，2-神经网络, 3-免疫算法
	int mnDataCount;	//数据数目
	int mnTotalDataCount;	//数据总数目

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

	//随机免疫算法
	bool mbIsAgTraining;
	ag m_ag;	//抗原类的构造，参数分别为：1.抗原集强制转化的二维指针。2.抗原的个数。3.抗原的维数
	ais* m_Fais;	//抗体集合, size = abnumber -
	int m_agNumJ;	//分类器阶数 -
	int m_agNumber; //抗原数组阶数（变量数+一个结果）, 文件得出
	int m_numAg;	//训练数据，文件得出
	int m_numAb;	//抗体数目 -
	float m_fVariationPg;	//变异概率 -
	float m_fExchangePg;	//交换概率 -
	int m_nClassNum;		//类别数目, 文件得出
	int m_nAgIteration;		//训练循环次数	-


private:
	inline void _ilog(char* sMessage, char* sInstance= "info" );
	inline void _ilog( QString sMessage, char* sInstance = "info" );

	//载入辅助信息
	//-1: 解析xml文件信息失败
	//-301, -302, -303: 解析统计值失败
	//-2: 载入图像失败
	//-4: IO失败
	int loadAuxiliaryData(int nID, CGDALFileManager* &pmgr, HAPBEGBase* &pIO, StaticBag_V2* sbVal);

	//获取FeaturePoint的像素
	inline bool calFeaturePixelValue(DistAuxiDT& _prob, double _lat, double _lon, int nID);

	//将控制点写入文件
	bool writeRandomPointsToFile(char* sfilename);

private:
	inline DistAuxiDT bilinearGetData(HAPBEGBase* pIO, float dx, float dy, int bands);
	template<class TT> inline DistAuxiDT _getdata(HAPBEGBase* pIO, float dx, float dy, int bands);
};

#endif

