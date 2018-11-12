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

//����������
#ifndef FEATRUE_POINT_V2
#define FEATRUE_POINT_V2
class FeaturePoint_V2
{
public:
	FeaturePoint_V2()
	{
		x=0, y=0;		//��ͨ���꣬��ʱ��
		lon=0;		//����
		lat=0;		//γ��
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
		x=0, y=0;		//��ͨ���꣬��ʱ��
		lon=0;		//����
		lat=0;		//γ��
		probs.clear();
		class_code=0;	
	}

	void addProbValue(DistAuxiDT val)
	{
		probs.append(val);
	}

public:
	int x, y;		//��ͨ���꣬��ʱ��
	double lon;		//����
	double lat;		//γ��
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

	//���ع黯����
	/******************************************************************
	* x ����ֵ
	* x_min, x_max, x_mean ��Сֵ�����ֵ�;�ֵ
	* isNearerBetter �Ƿ�Խ��Խ��
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

	//mnCLassifiMode = 3
	//ѵ�������㷨������������0�ɹ���<0ʧ��
	//-1: ����xmlʧ��
	//-2: �������λ�ļ�ʧ��
	int trainAGClassifier();

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

	//���������ļ�
// 	bool mbIsUseAddProb;
// 	CGDALFileManager* mpAddProb;
// 	HAPBEGBase* mpIO_AddProb;

	QList<CGDALFileManager*> mvpMgr;
	QList<StaticBag_V2>	msbData;
	QList<HAPBEGBase*>	mvpIO;

	//������ص�
	QList<FeaturePoint_V2> m_lpts;

private:
	int mnClassifiMode;	//����ģʽ��0-���ɭ�֣�1-�߼��ع飬2-������, 3-�����㷨
	int mnDataCount;	//������Ŀ
	int mnTotalDataCount;	//��������Ŀ

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

	//��������㷨
	bool mbIsAgTraining;
	ag m_ag;	//��ԭ��Ĺ��죬�����ֱ�Ϊ��1.��ԭ��ǿ��ת���Ķ�άָ�롣2.��ԭ�ĸ�����3.��ԭ��ά��
	ais* m_Fais;	//���弯��, size = abnumber -
	int m_agNumJ;	//���������� -
	int m_agNumber; //��ԭ���������������+һ�������, �ļ��ó�
	int m_numAg;	//ѵ�����ݣ��ļ��ó�
	int m_numAb;	//������Ŀ -
	float m_fVariationPg;	//������� -
	float m_fExchangePg;	//�������� -
	int m_nClassNum;		//�����Ŀ, �ļ��ó�
	int m_nAgIteration;		//ѵ��ѭ������	-


private:
	inline void _ilog(char* sMessage, char* sInstance= "info" );
	inline void _ilog( QString sMessage, char* sInstance = "info" );

	//���븨����Ϣ
	//-1: ����xml�ļ���Ϣʧ��
	//-301, -302, -303: ����ͳ��ֵʧ��
	//-2: ����ͼ��ʧ��
	//-4: IOʧ��
	int loadAuxiliaryData(int nID, CGDALFileManager* &pmgr, HAPBEGBase* &pIO, StaticBag_V2* sbVal);

	//��ȡFeaturePoint������
	inline bool calFeaturePixelValue(DistAuxiDT& _prob, double _lat, double _lon, int nID);

	//�����Ƶ�д���ļ�
	bool writeRandomPointsToFile(char* sfilename);

private:
	inline DistAuxiDT bilinearGetData(HAPBEGBase* pIO, float dx, float dy, int bands);
	template<class TT> inline DistAuxiDT _getdata(HAPBEGBase* pIO, float dx, float dy, int bands);
};

#endif

