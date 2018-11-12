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
	//�������, =0�ɹ�, <0ʧ��
	//-1: ��������������ʧ��
	//-2: �������ε�����ֵʧ��
	//-3: ������С��չ����ʧ��
	//-4: �����Ƿ����������չ����ʧ��
	//-5: ��������ļ���ʧ��
	//-6: �����Ƿ����������ø���ͼʧ��
	//-7: �����Ƿ�ʹ��ǰһ�θ���ͼʧ��
	int loadCAParameters();
	

	//�����������, =0�ɹ�, <0ʧ��
	//-101: �����ط���ͼ��ʧ��
	//-102: �������ط�չ������ָ��ʧ��
	//-103: ���������ļ�ʧ��
	//-201: �����ط���ͼ��ʧ��
	//-202: �������ط�չ������ͼ��ʧ��
	//-203: ����ȫ�ַ�չ�����ļ�ʧ��
	int loadAnalysisData();

protected:
	//����һ��CA����, =0�ɹ�, <0ʧ��
	//-1: ����ͼ��Ϊ��
	//-2: ������һ��Ӱ��ʧ��
	//-301: ���������ļ��ط���ͼ��/��һ�����ͼ��ʧ��
	//-302: ��������ļ�ʧ��
	//-303: ���������ļ�����������ָ��ʧ��
	//-304: ���������ļ�ȫ�ַ�չ�����ļ�ʧ��
	//-4: IO����Ϊ��
	int startCA(int nIterNo, long& nChangeCount);	
	int _process(int nIterNo, HAPBEGBase* pIO, /*HAPBEGBase* pSIn,*/ HAPBEGBase* pPIO, HAPBEGBase* pAddPIO, long& nChangeCount);
	inline float _calNeighPrb(int x, int y, HAPBEGBase* pIO);						//�����������
	inline float _calNeighPrb_tan(int x, int y, HAPBEGBase* pIO);						//�����������
	inline float _calNeighPrb2(int x, int y, HAPBEGBase *pPIO, HAPBEGBase *pIO);	//ͨ��Ƶ�ʼ����������, ��Ϊ����Ȩ��1, �ǳ���Ȩ��Ϊ��չ����


protected:
	//����Ҫ�����������������ʼ����, =0�ɹ�, <0ʧ��
	//�����ÿ�εĸ���ͼ��д��
	//int _process_ui_wrong(int nIterNum, long nUrbanIncreaseNum, HAPBEGBase *pPIO, HAPBEGBase* pIO);
	int _process_ui(int nIterNo, long nUIPerTime, HAPBEGBase *pPIO, HAPBEGBase* pIO, HAPBEGBase* pAddPIO);
	int startCA_ui();

	//�����ݲ��Ǻܴ��ʱ������������ַ���
	//ͳ�Ʒ��ϸ��ʵ�����, ����mlPotentialPts, pPIO-����IO
	int staticPotentialPts(HAPBEGBase* pPIO, HAPBEGBase* pIO, HAPBEGBase* pAddPIO);
	int _process_ui_potentialpts(int nIterNo, long nUIPerTime, HAPBEGBase *pPIO, HAPBEGBase* pIO, HAPBEGBase* pAddPIO);

protected:
	CGDALFileManager* mpUrbanReClassiImg;	//�ط���ͼ��
//	CGDALFileManager* mpSuitIdxImg;			//���ط�չ������Ӱ��
	CGDALFileManager* mpProbabilityAll;		//ȫ�ַ�չ�����ļ�;
	CGDALFileManager* mpAddProb;			//�ⲿ�����ļ�

private:
	//����
	int mnMaxIterNum;			//����������
	long mnForeNextDiff;			//���ε�����ֵ
	double mdMinDevelopProb;	//��С��չ����
	bool mbIsSimulatePA;		//�Ƿ�����Ʒ�չ�������ģ��
	char msOutFilesName[1024];	//����ļ���
	int mnIsNeighborCalUseProbImg; //�Ƿ�ͨ��������ʼ������ 0-��ͳ 1-Ӱ������ 2-tan����
	bool mbIsUseForeProbability;	//�Ƿ�ʹ����һ��ͼ�������ĸ���ͼ
	bool mbIsUseAddProb;			//�Ƿ�ʹ�����Ӹ���

private:
	long mnUrbanIncreaseNum;	//����������Ŀ
	int mnFastMethod;			//�Ƿ���п���ѡȡ
	int mnFindRndPtsRestart;	//������������
	QList<UPoint> mlIncreasePts;	//�����ĳ�������
	QList<UPoint> mlPotentialPts;	//��������Ҫ���ʵ�����



private:
	inline void _ilog(char* sMessage, char* sInstance= "info" );
	inline void _ilog( QString sMessage, char* sInstance = "info" );


// 	inline bool Src2DstCoordTrans(double src_x, double src_y, double* dst_x, double* dst_y, \
// 		CGDALFileManager* pSrcImg, CGDALFileManager* pDstImg);


};

#endif