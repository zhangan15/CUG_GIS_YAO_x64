#ifndef CLASS_URBAN_CHANGE_CA_ITERATION
#define CLASS_URBAN_CHANGE_CA_ITERATION

#include <QtCore/QList>
#include <QtCore/QString>

#include "gdalfilemanager.h"
#include "cxml2config.h"
#include "global_define.h"
//#include "UrbanChangeMaker.h"

using namespace std;
class HAPBEGBase;

class UPoint
{
public:
	UPoint() {x=0; y=0;}
	UPoint(int rx, int ry) {x=rx; y=ry;}
	~UPoint() {}

public:
	friend bool operator<(const UPoint& pt1, const UPoint& pt2)
	{
		return pt1.x<pt2.x;
	}

	void operator=(const UPoint& pt2)
	{
		x = pt2.x;
		y = pt2.y;
		//uprob = pt2.uprob;
	}

	friend bool operator==(const UPoint& pt1, const UPoint& pt2)
	{
		return (pt1.x==pt2.x && pt1.y==pt2.y);
	}

public:
	int x;
	int y;
	//float uprob;

};

class UrbanChangeCAIteration
{
public:
	UrbanChangeCAIteration(void);
	~UrbanChangeCAIteration(void);

public:
	bool run();
	void close();

protected:
	//载入参数, =0成功, <0失败
	//-1: 载入最大迭代次数失败
	//-2: 载入两次迭代差值失败
	//-3: 载入最小发展概率失败
	//-4: 载入是否对限制区域发展参数失败
	//-5: 载入输出文件名失败
	//-6: 载入是否邻域计算采用概率图失败
	//-7: 载入是否使用前一次概率图失败
	int loadCAParameters();
	

	//载入分析数据, =0成功, <0失败
	//-101: 解析重分类图像失败
	//-102: 解析土地发展适宜性指数失败
	//-103: 解析概率文件失败
	//-201: 载入重分类图像失败
	//-202: 载入土地发展适宜性图像失败
	//-203: 载入全局发展概率文件失败
	int loadAnalysisData();

protected:
	//启动一次CA分析, =0成功, <0失败
	//-1: 输入图像为空
	//-2: 载入上一张影像失败
	//-301: 设置输入文件重分类图像/上一次输出图像失败
	//-302: 设置输出文件失败
	//-303: 设置输入文件土地适宜性指数失败
	//-304: 设置输入文件全局发展概率文件失败
	//-4: IO可能为空
	int startCA(int nIterNo, long& nChangeCount);	
	int _process(int nIterNo, HAPBEGBase* pIO, /*HAPBEGBase* pSIn,*/ HAPBEGBase* pPIO, HAPBEGBase* pAddPIO, long& nChangeCount);
	inline float _calNeighPrb(int x, int y, HAPBEGBase* pIO);						//计算领域概率
	inline float _calNeighPrb_tan(int x, int y, HAPBEGBase* pIO);						//计算领域概率
	inline float _calNeighPrb2(int x, int y, HAPBEGBase *pPIO, HAPBEGBase *pIO);	//通过频率计算邻域概率, 若为城市权重1, 非城市权重为发展概率


protected:
	//根据要求城市增长的数量开始处理, =0成功, <0失败
	//不输出每次的概率图，写错
	//int _process_ui_wrong(int nIterNum, long nUrbanIncreaseNum, HAPBEGBase *pPIO, HAPBEGBase* pIO);
	int _process_ui(int nIterNo, long nUIPerTime, HAPBEGBase *pPIO, HAPBEGBase* pIO, HAPBEGBase* pAddPIO);
	int startCA_ui();

	//当数据不是很大的时候采用下面这种方法
	//统计符合概率的像素, 存入mlPotentialPts, pPIO-概率IO
	int staticPotentialPts(HAPBEGBase* pPIO, HAPBEGBase* pIO, HAPBEGBase* pAddPIO);
	int _process_ui_potentialpts(int nIterNo, long nUIPerTime, HAPBEGBase *pPIO, HAPBEGBase* pIO, HAPBEGBase* pAddPIO);

protected:
	CGDALFileManager* mpUrbanReClassiImg;	//重分类图像
//	CGDALFileManager* mpSuitIdxImg;			//土地发展适宜性影像
	CGDALFileManager* mpProbabilityAll;		//全局发展概率文件;
	CGDALFileManager* mpAddProb;			//外部概率文件

private:
	//参数
	int mnMaxIterNum;			//最大迭代次数
	long mnForeNextDiff;			//两次迭代差值
	double mdMinDevelopProb;	//最小发展概率
	bool mbIsSimulatePA;		//是否对限制发展区域进行模拟
	char msOutFilesName[1024];	//输出文件名
	int mnIsNeighborCalUseProbImg; //是否通过邻域概率计算概率 0-传统 1-影响因子 2-tan曲线
	bool mbIsUseForeProbability;	//是否使用上一次图像计算出的概率图
	bool mbIsUseAddProb;			//是否使用增加概率

private:
	long mnUrbanIncreaseNum;	//城市扩张数目
	int mnFastMethod;			//是否进行快速选取
	int mnFindRndPtsRestart;	//重新启动次数
	QList<UPoint> mlIncreasePts;	//增长的城市像素
	QList<UPoint> mlPotentialPts;	//大于所需要概率的像素



private:
	inline void _ilog(char* sMessage, char* sInstance= "info" );
	inline void _ilog( QString sMessage, char* sInstance = "info" );


// 	inline bool Src2DstCoordTrans(double src_x, double src_y, double* dst_x, double* dst_y, \
// 		CGDALFileManager* pSrcImg, CGDALFileManager* pDstImg);


};

#endif