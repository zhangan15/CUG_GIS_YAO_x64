#ifndef CLASS_URBANCHANGEMARKER
#define CLASS_URBANCHANGEMARKER

#include <QtCore/QList>
#include <QtCore/QString>

#include "gdalfilemanager.h"
#include "cxml2config.h"
#include "global_define.h"

class HAPBEGBase;



//���ط�����
struct LandCoverCode 
{
	long nID;					//������
	long nCode;					//С����
	long nReClassiID;			//�ط����ţ�1-���У�0-�ǳ���, -1��2-���Ʒ�չ����
	QString sCatalog1st;		//1�����
	QString sCatalog2nd;		//2�����
	QString sComment;			//��ע
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
	//�������ͼ�ı��˵����<0Ϊʧ�ܣ�=0�ɹ�
	//-1: ����xml�ļ�����; -2: ��������code�ļ�����
	int loadLandCoverCode();

	//�������ͼ��<0Ϊʧ�ܣ�=0�ɹ�
	//-1: ����ͼ��Ŀ̫��; -2: ��ʼ���������ļ�ȱʧ
	//-301: ��ʼͼ����ʧ��; -302: ����ͼ����ʧ��
	int loadLandCoverData();

	//��ʼ�ȶ�ͼ��<0Ϊʧ�ܣ�=0�ɹ�
	//-1: ����ͼ��Ϊ��
	//-2: �ļ�����MAPINFO
	//-301: ԭʼ����ͼ��IO-In���ô���
	//-302: Ŀ������ͼ��IO-In���ô���
	//-303: ԭʼ����ͼ��IO-Out���ô���
	//-304: ����������ָ���ļ�������ó���
	//-305: �����ط����ļ�������ô���
	//-4: ����ļ�����ȡ����
	//-5: �������IOΪ��
	int ProcessUrbanChangeImage();

	template<class TT> int Process(HAPBEGBase* pSrcIO , HAPBEGBase* pDstIO, HAPBEGBase* pReClassIO, HAPBEGBase* pValiIO);

protected:
	//���ط�������
	inline bool findCoverCode(long nCode, LandCoverCode& cCode);
	inline bool findReclassiIDfor2(long nCode1, long nCode2, long& nID1, long& nID2);
// 	inline bool Src2DstCoordTrans(double src_x, double src_y, double& dst_x, double& dst_y, \
// 					double* srcGeotransform, double* dstGeotransform, \
// 					const char* srcSpatialReferrence, const char* dstSpatialReferrence);

protected:
	CGDALFileManager* mpSrcImg;	//��һ��Ӱ��
	CGDALFileManager* mpDstImg;	//�ڶ���Ӱ��

	CGDALFileManager* mpUrbanReClassiImg;	//���ԭʼͼ�������ط���ͼ��,0-�ǳ���,1����,2�����Է�չ�õ�
	CGDALFileManager* mpUrbanReClassiImg_Validation;	//���Ŀ��ͼ�������ط���ͼ��, ��֤
	
	CGDALFileManager* mpSuitIdxImg;	//����������Ӱ��
	CGDALFileManager* mpUrbanChangeImg; //������ر仯Ӱ��
	QList<LandCoverCode> mlCoverCode;	//����˵��


private:
	inline void _ilog(char* sMessage, char* sInstance= "info" );
	inline void _ilog( QString sMessage, char* sInstance = "info" );


	
};


#endif


