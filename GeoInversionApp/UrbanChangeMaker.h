#ifndef CLASS_URBANCHANGEMARKER
#define CLASS_URBANCHANGEMARKER

#include <QtCore/QList>
#include <QtCore/QString>

#include "gdalfilemanager.h"
#include "cxml2config.h"
#include "global_define.h"

class HAPBEGBase;



//土地分类编号
struct LandCoverCode 
{
	long nID;					//大类编号
	long nCode;					//小类编号
	long nReClassiID;			//重分类编号，1-城市，0-非城市, -1或2-限制发展区域
	QString sCatalog1st;		//1级类别
	QString sCatalog2nd;		//2级类别
	QString sComment;			//备注
};

class UrbanChangeMaker
{
public:
	UrbanChangeMaker(void);
	~UrbanChangeMaker(void);

public:
	bool run();
	void close();

protected:
	//载入分类图的编号说明，<0为失败，=0成功
	//-1: 载入xml文件错误; -2: 解析分类code文件错误
	int loadLandCoverCode();

	//载入分类图，<0为失败，=0成功
	//-1: 分类图数目太少; -2: 起始或结束年份文件缺失
	//-301: 开始图载入失败; -302: 结束图载入失败
	int loadLandCoverData();

	//开始比对图像，<0为失败，=0成功
	//-1: 输入图像为空
	//-2: 文件不含MAPINFO
	//-301: 原始输入图像IO-In设置错误
	//-302: 目标输入图像IO-In设置错误
	//-303: 原始输入图像IO-Out设置错误
	//-304: 土地适宜性指数文件输出设置出错
	//-305: 土地重分类文件输出设置错误
	//-4: 输出文件名读取错误
	//-5: 输入输出IO为空
	int ProcessUrbanChangeImage();

	template<class TT> int Process(HAPBEGBase* pSrcIO , HAPBEGBase* pDstIO, HAPBEGBase* pReClassIO, HAPBEGBase* pValiIO);

protected:
	//返回分类数据
	inline bool findCoverCode(long nCode, LandCoverCode& cCode);
	inline bool findReclassiIDfor2(long nCode1, long nCode2, long& nID1, long& nID2);
// 	inline bool Src2DstCoordTrans(double src_x, double src_y, double& dst_x, double& dst_y, \
// 					double* srcGeotransform, double* dstGeotransform, \
// 					const char* srcSpatialReferrence, const char* dstSpatialReferrence);

protected:
	CGDALFileManager* mpSrcImg;	//第一幅影像
	CGDALFileManager* mpDstImg;	//第二幅影像

	CGDALFileManager* mpUrbanReClassiImg;	//输出原始图像土地重分类图像,0-非城市,1城市,2限制性发展用地
	CGDALFileManager* mpUrbanReClassiImg_Validation;	//输出目标图像土地重分类图像, 验证
	
	CGDALFileManager* mpSuitIdxImg;	//土地适宜性影像
	CGDALFileManager* mpUrbanChangeImg; //输出土地变化影像
	QList<LandCoverCode> mlCoverCode;	//数据说明


private:
	inline void _ilog(char* sMessage, char* sInstance= "info" );
	inline void _ilog( QString sMessage, char* sInstance = "info" );


	
};


#endif


