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
�����¸�������ʱ��
1 ����ͼ��CGDALFileManager�����ݣ�, StaticBag��ͳ�ƣ�, HAPBEGBase��IO��;
2 ���¹��캯��������������close()��, ����FeaturePoint��;
3 ����loadAuxiliaryData();
4 ����makeRandomPointFiles(), ÿ���������ӱ���;
5 ����writeRandomPointsToFile(), д����Ƶ��ļ�����;
6 ����CalWholeImageDevelpedProbability()
**/

//����������
#ifndef FEATRUE_POINT_V1
#define FEATRUE_POINT_V1
class FeaturePoint_V1
{
public:
	FeaturePoint_V1()
	{
		x=0, y=0;		//��ͨ���꣬��ʱ��
		lon=0;		//����
		lat=0;		//γ��
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
	int x, y;		//��ͨ���꣬��ʱ��
	double lon;		//����
	double lat;		//γ��
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

	//���ع黯����
	/******************************************************************
	* x ����ֵ
	* x_min, x_max, x_mean ��Сֵ�����ֵ�;�ֵ
	* isNearerBetter �Ƿ�Խ��Խ��
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
	//����ͼ���ļ�������0ִ�гɹ���<0 ʧ��
	//-1: ����xmlʧ��
	//-2: ����ͼ��ʧ��
	//-3: IOʧ��
	int loadUrbanChangeData();

	//���븨�����ݣ�����0ִ�гɹ���<0ʧ��
	//-1: ����xml�ļ���Ϣʧ��
	//-301, -302, -303: ����ͳ��ֵʧ��
	//-2: ����ͼ��ʧ��
	//-4: IOʧ��
	int loadAuxiliaryData();

	//��ͼ���ϻ�ȡ������غ������洢���ı��ļ�������0�ɹ���<0ʧ��
	//-1: ����xml����ļ�λ�ýڵ�ʧ��
	//-2: ����xml������Ŀ�ڵ�ʧ��
	//-5: ������������ļ�ʧ��
	int makeRandomPointFiles();

protected:
	//mnClassifiMode = 0
	//ѵ�����ɭ�ַ�����������0�ɹ���<0ʧ��
	//-1: ����xmlʧ��
	//-2: �������λ�ļ�ʧ��
	//-3: ѵ��ʧ��
	int trainRandomForestClassifier();	

	//mnClassifiMode = 1
	//ѵ���߼��ع������������0�ɹ���<0ʧ��
	//-1: ����xmlʧ��
	//-2: ��ز�����ȡʧ��
	//-3: �������λ�ļ�ʧ��
	//-4: ѵ��ʧ��
	//-5: model file����ʧ��
	//-6: ѵ������̫��
	int trainLRClassifier();

	//mnCLassifiMode = 2
	//ѵ�������������������0�ɹ���<0ʧ��
	//-1: ����xmlʧ��
	//-2: �������λ�ļ�ʧ��
	int trainNNClassifer();

protected:
	//����ȫ�ֿ������ʣ�������ļ�������0�ɹ���<0ʧ��
	//-1: û�з�����
	//-2: ��IOΪNULL
	//-3: �����������ʧ��
	int CalWholeImageDevelpedProbability();




private:
	//���9���θ����ļ�
	CGDALFileManager* mpProbFor9;
	bool mbIsOpProbFor9;

	//��������ļ�
	CGDALFileManager* mpProbabilityAll;	//ȫ�ָ����ļ�

	//�ط�����
	CGDALFileManager* mpUrbanChange;
	HAPBEGBase* mpIO_UrbanChange;	//�������

	//���и���ͼ������
	CGDALFileManager* mpAux_DistCapitanCity;			//��ʡ�����
	StaticBag_V1 msbAux_DistCapitanCity;
	HAPBEGBase* mpIn_DistCapitanCity;

	CGDALFileManager* mpAux_DistPrefectureCity;		//��ؼ��о���
	StaticBag_V1 msbAux_DistPrefectureCity;
	HAPBEGBase* mpIn_DistPrefectureCity;
	
	CGDALFileManager* mpAux_DistCountyTown;			//��������ľ���
	StaticBag_V1 msbAux_DistCountyTown;
	HAPBEGBase* mpIn_DistCountyTown;
	
	CGDALFileManager* mpAux_DistMainRoad;				//����Ҫ��·����
	StaticBag_V1 msbAux_DistMainRoad;
	HAPBEGBase* mpIn_DistMainRoad;
	
	CGDALFileManager* mpAux_DistMainRailway;			//����Ҫ��·����
	StaticBag_V1 msbAux_DistMainRailway;
	HAPBEGBase* mpIn_DistMainRailway;

	//������ص�
	QList<FeaturePoint_V1> m_lpts;

private:
	int mnClassifiMode;	//����ģʽ��0-���ɭ�֣�1-�߼��ع飬2-������

private:
	//���ɭ�ַ�����
	alglib::decisionforest m_dfClassifier;
	alglib::ae_int_t m_ndfInfo;		//�ж����Ƿ������� =1ʱ�����������ʹ��
	alglib::dfreport m_dfReport;	//����������

	//�߼��ع�
	bool mbIsLRTraining;	//�Ƿ�����߼�ѵ��
	LR m_LRClassifier;	//�߼��ع������
	char m_smodelfile;	//ģ���ļ���ַ

	//������NN
	bool mbIsNNTraining;			//�Ƿ�NNѵ��
	alglib::mlptrainer m_NNTrainer;	//NNѵ����
	alglib::multilayerperceptron m_NNNetwork;	//���������
	alglib::mlpreport m_NNReport;


private:
	inline void _ilog(char* sMessage, char* sInstance= "info" );
	inline void _ilog( QString sMessage, char* sInstance = "info" );

	//���븨����Ϣ
	//-1: ����xml�ļ���Ϣʧ��
	//-301, -302, -303: ����ͳ��ֵʧ��
	//-2: ����ͼ��ʧ��
	//-4: IOʧ��
	int loadAuxiliaryData(CGDALFileManager* &pmgr, HAPBEGBase* &pIO, char* node_name, char* sComment, StaticBag_V1* sbVal);

// 	inline bool Src2DstCoordTrans(double src_x, double src_y, double* dst_x, double* dst_y, \
// 		CGDALFileManager* pSrcImg, CGDALFileManager* pDstImg);


	//��ȡFeaturePoint������
	inline bool calFeaturePixelValue(DistAuxiDT& _prob, double _lat, double _lon, CGDALFileManager* pMgr, HAPBEGBase* pIO, StaticBag_V1* pSb);

	//�����Ƶ�д���ļ�
	bool writeRandomPointsToFile(char* sfilename);

private:
	/****************************************************************
	* ��γ�Ⱥ����������ת��
	****************************************************************/
	//int in_World2Pixel(double lat, double lon, double *x, double *y, double* geoTransform, const char* lpszSpatialReferrence);
	//int in_Pixel2World(double *lat, double *lon, double x, double y, double* geoTransform, const char* lpszSpatialReferrence);

	
};



#endif

