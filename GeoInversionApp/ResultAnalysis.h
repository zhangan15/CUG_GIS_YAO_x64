#ifndef CA_RESULT_ANALYSIS
#define CA_RESULT_ANALYSIS

#include <vector>
using namespace std;

class CGDALFileManager;
class CGDALVectorLayer;
class OGRCoordinateTransformation;

//������ʴ���÷�����=0�ɹ�, <0ʧ��
//-1: xml����ʧ��
//-2: ��������codeʧ��
//-3: ��������ļ�ʧ��
//-4: ��������ļ�IO����ʧ��
int FarmlandErosionAnalysis();

//���������ļ������=0�ɹ�, <0ʧ��
//д��������󣬲����㾫�ȣ�д��txt�ļ�
//-1: xml����ʧ��
//-2: ������֤�ļ�ʧ��
int AccuracyEvaluation();

//�������ۣ����س�ͳ�ƣ�=0�ɹ�, <0ʧ��
int AccuracyEvaluation_Region();
/************************************************************************
* pImg: ���������ط���Ӱ��
* pLayer: �سǷֲ�ͼ
* pResult: ��������ÿ���س�FID��Ӧ�ĳ��аٷֱ�
* nImgid: ��N-1�ε�����
************************************************************************/
bool StatisSingleMap(CGDALFileManager* pImg, CGDALVectorLayer* pLayer, double* &pResult, int nImgId);

//�������������
bool _makeAccuracyFile(double _cm[3][3], const char* _filename);



//�ж��Ƿ�����Vector
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