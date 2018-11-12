#ifndef CA_RESULT_ANALYSIS
#define CA_RESULT_ANALYSIS

#include <vector>
using namespace std;

class CGDALFileManager;
class CGDALVectorLayer;
class OGRCoordinateTransformation;

//耕地侵蚀利用分析，=0成功, <0失败
//-1: xml解析失败
//-2: 解析地类code失败
//-3: 载入分类文件失败
//-4: 输入分类文件IO设置失败
int FarmlandErosionAnalysis();

//精度评价文件输出，=0成功, <0失败
//写入混淆矩阵，并计算精度，写入txt文件
//-1: xml解析失败
//-2: 载入验证文件失败
int AccuracyEvaluation();

//精度评价，分县城统计，=0成功, <0失败
int AccuracyEvaluation_Region();
/************************************************************************
* pImg: 土地利用重分类影像
* pLayer: 县城分布图
* pResult: 输出结果，每个县城FID对应的城市百分比
* nImgid: 第N-1次迭代号
************************************************************************/
bool StatisSingleMap(CGDALFileManager* pImg, CGDALVectorLayer* pLayer, double* &pResult, int nImgId);

//输出混淆矩阵结果
bool _makeAccuracyFile(double _cm[3][3], const char* _filename);



//判断是否属于Vector
bool _isBelongToVector(int _val, vector<int>& _vec);
bool _splitStringToVector(char* sCode, vector<int>& _vec);
// 
// bool _src2DstCoordTrans(double src_x, double src_y, double* dst_x, double* dst_y, \
// 					   CGDALFileManager* pSrcImg, CGDALFileManager* pDstImg);


//
bool Src2Dst_Geo(double& dst_X, double& dst_Y, double src_X, double src_Y, OGRCoordinateTransformation* poTransform);



//
int getCAIterationNum();

#endif