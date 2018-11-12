#include "UrbanChangeCAIteration.h"
//#include "UrbanChangeProbabilityAnalysis.h"

#include "global_include.h"

#include "haplib.h"
#include "hapbegbase.h"
#include "HAPBEGThread.h"
#include "hapbegfactory.h"
#include <QtCore/QString>

#include <stdio.h>
#include <stdlib.h>

#include <QtCore/QStringList>
#include <QtCore/QPoint>
using namespace std;


int nDirections[8][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1}};

void UrbanChangeCAIteration::_ilog( char* sMessage, char* sInstance/*= "info" */ )
{
	log4cpp::Category& log = log4cpp::Category::getInstance(sInstance);
	log.info(sMessage);
	cout<<sMessage<<endl;
}

void UrbanChangeCAIteration::_ilog( QString sMessage, char* sInstance /*= "info" */ )
{
	log4cpp::Category& log = log4cpp::Category::getInstance(sInstance);
	log.info(sMessage.toStdString().c_str());
	//_ilog(sMessage.toStdString().c_str(), sInstance);
	cout<<sMessage.toStdString().c_str()<<endl;
}

// bool UrbanChangeCAIteration::Src2DstCoordTrans( double src_x, double src_y, double* dst_x, double* dst_y, CGDALFileManager* pSrcImg, CGDALFileManager* pDstImg )
// {
// 	double lat, lon;	
// 	_Pixel2World(&lat, &lon, src_x, src_y, pSrcImg->m_header.m_MapInfo->m_adfGeotransform, pSrcImg->GetpoDataset()->GetProjectionRef());
// 	_World2Pixel(lat, lon, dst_x, dst_y, pDstImg->m_header.m_MapInfo->m_adfGeotransform, pDstImg->GetpoDataset()->GetProjectionRef());
// 
// 	if (dst_x<0 || dst_y<0 || *dst_x>=pDstImg->m_header.m_nSamples || *dst_y>=pDstImg->m_header.m_nLines)
// 		return false;
// 	else
// 		return true;
// }

UrbanChangeCAIteration::UrbanChangeCAIteration(void)
{
	_ilog("Starting Urban Development CA Iteration...");
	mpUrbanReClassiImg = NULL;
//	mpSuitIdxImg = NULL;
	mpProbabilityAll = NULL;
	mpAddProb = NULL;
	mnMaxIterNum = 1;
	mnForeNextDiff = 100;
	mdMinDevelopProb = 0.5f;
	mbIsSimulatePA = false;
	mnIsNeighborCalUseProbImg= 0;
	mbIsUseForeProbability = false;
	mnUrbanIncreaseNum = 0;
	strcpy(msOutFilesName, "");
	mlIncreasePts.clear();
	mlPotentialPts.clear();
	mnFastMethod = 1;
	mnFindRndPtsRestart = 5000;
	mbIsUseAddProb = false;
}

UrbanChangeCAIteration::~UrbanChangeCAIteration(void)
{
	close();
}

bool UrbanChangeCAIteration::run()
{
	int bResult = 0;

	bResult = loadCAParameters();
	if (bResult<0)
	{
		_ilog(QString("UrbanChangeCAIteration::loadCAParameters error! Code: %1").arg(bResult));
		return false;
	}

	bResult = loadAnalysisData();
	if (bResult<0)
	{
		_ilog(QString("UrbanChangeCAIteration::loadAnalysisData error! Code: %1").arg(bResult));
		return false;
	}


	//��������

	if (mnUrbanIncreaseNum > mnMaxIterNum && mnUrbanIncreaseNum > 0)
	{
		bResult = startCA_ui();
	}
	else
	{
		long nIterCount = 0, nDiffCount = 0;
		//startCA(1, nDiffCount);
		while (nIterCount < mnMaxIterNum)
		{
			nDiffCount = 0;
			bResult = startCA(nIterCount, nDiffCount);
			if (bResult<0)
			{
				_ilog(QString("UrbanChangeCAIteration::startCA! Code: %1").arg(bResult));
				return false;
			}

			_ilog(QString("CA Interations Information: Difference Between No. %1 And No. %2 = %3").arg(nIterCount+1).arg(nIterCount).arg(nDiffCount));

			//������ڴ���Ŀ
			if (nDiffCount <= mnForeNextDiff)
				break;

			nIterCount++;
		}

		_ilog(QString("Finished CA Iterations, NUM = %1").arg(nIterCount+1));
	}
	
	


	return true;
}

void UrbanChangeCAIteration::close()
{
	if (mpUrbanReClassiImg!=NULL)
	{
		mpUrbanReClassiImg->Close();
		//delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
	}

// 	if (mpSuitIdxImg!=NULL)
// 	{
// 		mpSuitIdxImg->Close();
// 		delete mpSuitIdxImg;
// 		mpSuitIdxImg = NULL;
// 	}

	if (mpProbabilityAll!=NULL)
	{
		mpProbabilityAll->Close();
	   //delete mpProbabilityAll;
		mpProbabilityAll = NULL;
	}

	if (mpAddProb!=NULL)
	{
		mpAddProb->Close();
		mpAddProb = NULL;
	}

	mnUrbanIncreaseNum = 0;

	_ilog("Urban Development CA Iteration Finished.");
}

int UrbanChangeCAIteration::loadCAParameters()
{
	_ilog("Loading CA Parameters...");

	CXml2Document doc(XML_CONFIG);

	//������������
	mnUrbanIncreaseNum = 0;
	if( 0 != doc.GetValue(CA_PARAMETER_URBAN_INCREASE_NUM, &mnUrbanIncreaseNum))
	{
		_ilog("Load CA_PARAMETER_URBAN_INCREASE_NUM error!");
		return -1;
	}
	_ilog(QString("CA Urban Increase Num = %1.").arg(mnUrbanIncreaseNum));

	//������������
	mnFindRndPtsRestart = 0;
	if( 0 != doc.GetValue(CA_PARAMETER_FIND_RNDPTS_RESTART_TIMES, &mnFindRndPtsRestart))
	{
		_ilog("Load CA_PARAMETER_FIND_RNDPTS_RESTART_TIMES error!");
		return -1;
	}
	_ilog(QString("CA Find Random Points Restart Times = %1.").arg(mnFindRndPtsRestart));

	//�Ƿ���п�������ͳ��
	mnFastMethod = 1;
	if( 0 != doc.GetValue(CA_PARAMETER_URBAN_INCREASE_FAST_METHOD, &mnFastMethod))
	{
		_ilog("Load CA_PARAMETER_URBAN_INCREASE_FAST_METHOD error, program will not adpot fast method!");
		mnFastMethod = 0;
	}
	_ilog(QString("CA Urban Increase Fast Method = %1.").arg(mnFastMethod));

	//����������
	if( 0 != doc.GetValue(CA_PARAMTER_MAX_ITER_NUM, &mnMaxIterNum))
	{
		_ilog("Load CA_PARAMTER_MAX_ITER_NUM error!");
		return -1;
	}
	_ilog(QString("CA Max Iterations Num = %1.").arg(mnMaxIterNum));

	//���ε�����ֵ
	if( 0 != doc.GetValue(CA_PARAMETER_FORE_NEXT_DIFFERENCE_NUM, &mnForeNextDiff))
	{
		_ilog("Load CA_PARAMETER_FORE_NEXT_DIFFERENCE_NUM error!");
		return -2;
	}
	_ilog(QString("CA Fore And Next Iterations Difference = %1.").arg(mnForeNextDiff));

	//��С��չ����
	if( 0 != doc.GetValue(CA_PARAMETER_DEVELOP_MIN_PROBABILITY, &mdMinDevelopProb))
	{
		_ilog("Load CA_PARAMETER_DEVELOP_MIN_PROBABILITY error!");
		return -3;
	}
	_ilog(QString("CA Urban Development Minimum Probabilty = %1.").arg(mdMinDevelopProb, 0, 'f', 6));

	//�Ƿ����������չ
	int _nval = 0;
	if( 0 != doc.GetValue(CA_PARAMETER_IS_SIMULATE_PROTAREA, &_nval))
	{
		_ilog("Load CA_PARAMETER_IS_SIMULATE_PROTAREA error!");
		return -4;
	}
	if (_nval<=0)
	{
		mbIsSimulatePA = false;
		_ilog(QString("CA Is Simulating Protective Area = No."));
	}
	else
	{
		mbIsSimulatePA = true;
		_ilog(QString("CA Is Simulating Protective Area = Yes."));
	}

	//�Ƿ�ʹ�������Ӹ���
	_nval = 0;
	if( 0 != doc.GetValue(NODE_BOOL_ADD_PROB, &_nval))
	{
		_ilog("Load NODE_BOOL_ADD_PROB error!");
		return -4;
	}
	if (_nval<=0)
	{
		mbIsUseAddProb = false;
		_ilog(QString("CA Is Simulating with Additional Probability Image = No."));
	}
	else
	{
		mbIsUseAddProb = true;
		_ilog(QString("CA Is Simulating with Additional Probability Image = Yes."));
	}

	//�Ƿ����������չ
	int _nval1 = 1;
	if( 0 != doc.GetValue(CA_PARAMETER_IS_NEIGHBORCAL_USEPROB, &_nval1))
	{
		_ilog("Load CA_PARAMETER_IS_NEIGHBORCAL_USEPROB error!");
		return -6;
	}
	if (_nval1<=0)
	{
		mnIsNeighborCalUseProbImg = 0;
		_ilog(QString("CA Calculating Neighbor Prob. Use ProbImg = Tranditional."));
	}
	else if(_nval1 == 1)
	{
		mnIsNeighborCalUseProbImg = 1;
		_ilog(QString("CA Calculating Neighbor Prob. Use ProbImg = Urban Development Index."));
	}
	else
	{
		mnIsNeighborCalUseProbImg = 2;
		_ilog(QString("CA Calculating Neighbor Prob. Use ProbImg = Tan Curve."));
	}

	//�Ƿ�ʹ��ǰһ�θ���ͼ
	_nval1 = 1;
	if( 0 != doc.GetValue(CA_PARAMETER_IS_USE_FORE_PROBIMG, &_nval1))
	{
		_ilog("Load CA_PARAMETER_IS_USE_FORE_PROBIMG error!");
		return -7;
	}
	if (_nval1<=0)
	{
		mbIsUseForeProbability = false;
		_ilog(QString("CA Next Iteration Use Fore Probability Result = No."));
	}
	else
	{
		mbIsUseForeProbability = true;
		_ilog(QString("CA Next Iteration Use Fore Probability Result = Yes."));
	} 

	//����ļ���
	if( 0 != doc.GetValue(NODE_OUTPUT_URBAN_DEVELOPMENT_CA_RESULT, msOutFilesName))
	{
		_ilog("Load NODE_OUTPUT_URBAN_DEVELOPMENT_CA_RESULT error!");
		return -5;
	}
	str_connect_begin(SOPFILEPATH, msOutFilesName);
	_ilog(QString("CA Output File Name = %1.").arg(msOutFilesName));

	_ilog("Load CA Parameters Finished.");
	return 0;

}

int UrbanChangeCAIteration::loadAnalysisData()
{
	_ilog("Loading Analysis Datasets...");
	
	CXml2Document doc(XML_CONFIG);
	
	//����xml
	char sReclassifiName[2048];
	if( 0 != doc.GetValue(NODE_OUTOUT_URBAN_CHANGE_RECLASSIFI_FILEPATH, sReclassifiName))
	{
		_ilog("Load NODE_OUTOUT_URBAN_CHANGE_RECLASSIFI_FILEPATH error!");
		return -101;
	}
	str_connect_begin(SOPFILEPATH, sReclassifiName);

// 	char sSuitIdxName[2048];
// 	if( 0 != doc.GetValue(NODE_OUTPUT_LAND_DEVELOP_SUITABLE_INDEX_FILENAME, sSuitIdxName))
// 	{
// 		_ilog("Load NODE_OUTPUT_LAND_DEVELOP_SUITABLE_INDEX_FILENAME error!");
// 		return -102;
// 	}

	char sProbImgName[2048];
	if( 0 != doc.GetValue(NODE_OUTPUT_PROBABILTY_DEVELOPED_FILE_PATH, sProbImgName))
	{
		_ilog("Load NODE_OUTPUT_PROBABILTY_DEVELOPED_FILE_PATH error!");
		return -103;
	}
	str_connect_begin(SOPFILEPATH, sProbImgName);

	char sAddProbImage[2048];
	if (mbIsUseAddProb == true)
	{
		if( 0 != doc.GetValue(NODE_ADD_PRON_FILE, sAddProbImage))
		{
			_ilog("Load NODE_ADD_PRON_FILE error!");
			return -104;
		}
		str_connect_begin(SAUXFILEPATH, sAddProbImage);
	}

	//�����ط���ͼ��
	if (mpUrbanReClassiImg!=NULL)
		delete mpUrbanReClassiImg;
	mpUrbanReClassiImg = new CGDALFileManager;

	if (!mpUrbanReClassiImg->LoadFrom(sReclassifiName))
	{
		_ilog(QString("Load Urban Reclassi Image File Failed! Filename: %1").arg(sReclassifiName));
		return -201;
	}
	_ilog(QString("Load Urban Reclassi Image File Successed. Filename: %1").arg(sReclassifiName));

	//�������ط�չ������ͼ��
// 	if (mpSuitIdxImg!=NULL)
// 		delete mpSuitIdxImg;
// 	mpSuitIdxImg = new CGDALFileManager;
// 
// 	if (!mpSuitIdxImg->LoadFrom(sSuitIdxName))
// 	{
// 		_ilog(QString("Load Urban Reclassi Image File Failed! Filename: %1").arg(sSuitIdxName));
// 		return -202;
// 	}
// 	_ilog(QString("Load Urban Reclassi Image File Successed. Filename: %1").arg(sSuitIdxName));

	//����ȫ�ַ�չ�����ļ�
	if (mpProbabilityAll!=NULL)
		delete mpProbabilityAll;
	mpProbabilityAll = new CGDALFileManager;

	if (!mpProbabilityAll->LoadFrom(sProbImgName))
	{
		_ilog(QString("Load Urban Probability Image File Failed! Filename: %1").arg(sProbImgName));
		return -203;
	}
	_ilog(QString("Load Urban Probability Image File Successed. Filename: %1").arg(sProbImgName));


	//�������Ӹ����ļ�
	if (mbIsUseAddProb)
	{
		if (mpAddProb!=NULL)
			delete mpAddProb;
		mpAddProb = new CGDALFileManager;

		if (!mpAddProb->LoadFrom(sAddProbImage))
		{
			_ilog(QString("Load Additional Probability Image File Failed! Filename: %1").arg(sAddProbImage));
			return -204;
		}
		_ilog(QString("Load Additional Probability Image File Successed. Filename: %1").arg(sAddProbImage));

	}
	

	_ilog("Load Analysis Datasets Successed.");
	return 0;

}

int UrbanChangeCAIteration::startCA_ui()
{
	//����ÿ��������
	mlIncreasePts.clear();
	mlPotentialPts.clear();
	int nUIPerTime = (double)mnUrbanIncreaseNum/(double)mnMaxIterNum + 0.5;

	int nIterNo = 0;
	for (nIterNo = 0; nIterNo<mnMaxIterNum; nIterNo++)
	{
		_ilog(QString("Starting No. %1 CA Analysis...").arg(nIterNo+1));

		if (mpUrbanReClassiImg == NULL /*|| mpSuitIdxImg == NULL*/ || mpProbabilityAll == NULL)
		{
			_ilog(QString("Some Image Point Is Null In No. %1 CA Analysis.").arg(nIterNo));
			return -1;
		}

		//��������ļ���
		QString outputFilename = QString("%1_IterNo_%2.tif").arg(msOutFilesName).arg(nIterNo);

		//������������ļ�
		QString opProbFilename = QString("%1_Probability_IterNo_%2.tif").arg(msOutFilesName).arg(nIterNo);

		//�������ļ�����һ�ŵĽ��
		if (nIterNo >= 1)
		{
			//����������һ���ط���ͼ��
			QString forefilename = QString("%1_IterNo_%2.tif").arg(msOutFilesName).arg(nIterNo-1);
			//��ʱ������mpUrbanClassiImg����һ�Ž��
			if (mpUrbanReClassiImg != NULL)
				delete mpUrbanReClassiImg;
			mpUrbanReClassiImg = new CGDALFileManager;
			if (!mpUrbanReClassiImg->LoadFrom(forefilename.toStdString().c_str()))
			{
				_ilog(QString("Load Fore Result Image Error. Filename = %1").arg(forefilename));
				return -2;
			}
			_ilog(QString("Load Fore Result Image Successed. Filename = %1").arg(forefilename));

		}	

		DIMS dims;
		mpUrbanReClassiImg->m_header.InitDIMS(&dims);

		//��������ļ�
		CGDALFileManager* pOut = new CGDALFileManager;
		pOut->m_header = mpUrbanReClassiImg->m_header;
		pOut->m_header.m_nSamples = dims.GetWidth(); 
		pOut->m_header.m_nLines = dims.GetHeight();
		pOut->m_header.m_nBands = dims.GetBands();
		pOut->m_header.m_nDataType = CAResultDTCode;

		//����������IO
		HAPBEGBase* pIO = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
		if (!pIO->Set_MgrIn(mpUrbanReClassiImg, dims, 4))
		{
			_ilog("IO Set Urban ReClassification Image In Error!");
			delete pIO;
			pIO = NULL;
			delete pOut;
			pOut = NULL;
			return -301;
		}
		_ilog("IO Set Urban ReClassification Image In Successed.");

		if (!pIO->Set_MgrOut(pOut, outputFilename.toStdString().c_str()))
		{
			_ilog("IO Set Urban Development CA Image Out Error!");
			delete pIO;
			pIO = NULL;
			delete pOut;
			pOut = NULL;
			return -302;
		}
		_ilog("IO Set Urban Development CA Image Out Successed.");

		//����ȫ�ַ�չ����IO
		HAPBEGBase* pProbIO = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
		if (!pProbIO->Set_MgrIn(mpProbabilityAll, dims, 4))
		{
			_ilog("IO Set Probability Image In Error!");
			delete pProbIO;
			pProbIO = NULL;
			delete pIO;
			pIO = NULL;
			delete pOut;
			pOut = NULL;
			return -304;
		}
		_ilog("IO Set Probability Image In Successed.");

		//��������
		HAPBEGBase* pAddProb = NULL;
		if (mbIsUseAddProb)
		{
			pAddProb = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
			DIMS dims1;
			mpAddProb->m_header.InitDIMS(&dims1);

			if (!pAddProb->Set_MgrIn(mpAddProb, dims1))
			{
				_ilog("IO Set Additional Probability Image In Error!");
				delete pAddProb;
				pAddProb = NULL;
				delete pProbIO;
				pProbIO = NULL;
				delete pIO;
				pIO = NULL;
				delete pOut;
				pOut = NULL;
				return -305;
			}
			_ilog("IO Set Additional Probability Image In Successed.");
		}
		
		int _nCode = 0;

		//ֻͳ��һ��
// 		if (mnFastMethod > 0 && nIterNo == 0)
// 		{
// 			staticPotentialPts(pProbIO);
// 		}

		if (mnFastMethod > 0)
		{
			//���ٷ�����ֱ��ͳ�ƿ��������ĳ�����Ŀ	
			staticPotentialPts(pProbIO, pIO, pAddProb);
			_nCode = _process_ui_potentialpts(nIterNo, nUIPerTime, pProbIO, pIO, pAddProb);
		}
		else
		{
			//��ͨ�������յ���������ģ�����������������ģ��
			_nCode = _process_ui(nIterNo, nUIPerTime, pProbIO, pIO, pAddProb);
		}

		//�ر�IO
		pIO->Close_MgrIn();
		pIO->Close_MgrOut();

		pProbIO->Close_MgrIn();

		if (mbIsUseAddProb==true && pAddProb!=NULL)
		{
			pAddProb->Close_MgrIn();
			delete pAddProb;
			pAddProb = NULL;
		}

		//ɾ��IO
		delete pIO;
		pIO = NULL;
		delete pProbIO;
		pProbIO = NULL;

		//�ͷ��ڴ�
		pOut->Close();
		pOut = NULL;

		//����ֵ
		if (_nCode != 0)
		{
			_ilog(QString("No. %1 CA Analysis Failed.").arg(nIterNo+1));
			return _nCode;
		}
		else
		{
			_ilog(QString("No. %1 CA Analysis Finished.").arg(nIterNo+1));
			//return 0;
		}

	}

	mlIncreasePts.clear();
	mlPotentialPts.clear();

	return 0;
}

int UrbanChangeCAIteration::startCA(int nIterNo, long& nChangeCount)
{
	_ilog(QString("Starting No. %1 CA Analysis...").arg(nIterNo+1));

	if (mpUrbanReClassiImg == NULL /*|| mpSuitIdxImg == NULL*/ || mpProbabilityAll == NULL)
	{
		_ilog(QString("Some Image Point Is Null In No. %1 CA Analysis.").arg(nIterNo));
		return -1;
	}

	//��������ļ���
	QString outputFilename = QString("%1_IterNo_%2.tif").arg(msOutFilesName).arg(nIterNo);
	
	//������������ļ�
	QString opProbFilename = QString("%1_Probability_IterNo_%2.tif").arg(msOutFilesName).arg(nIterNo);

	//�������ļ�����һ�ŵĽ��
	if (nIterNo >= 1)
	{
		//����������һ���ط���ͼ��
		QString forefilename = QString("%1_IterNo_%2.tif").arg(msOutFilesName).arg(nIterNo-1);
		//��ʱ������mpUrbanClassiImg����һ�Ž��
		if (mpUrbanReClassiImg != NULL)
			delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = new CGDALFileManager;
		if (!mpUrbanReClassiImg->LoadFrom(forefilename.toStdString().c_str()))
		{
			_ilog(QString("Load Fore Result Image Error. Filename = %1").arg(forefilename));
			return -2;
		}
		_ilog(QString("Load Fore Result Image Successed. Filename = %1").arg(forefilename));

		//����������һ�Ÿ����ļ�
		if (mbIsUseForeProbability)
		{
			QString foreopProbfilename = QString("%1_Probability_IterNo_%2.tif").arg(msOutFilesName).arg(nIterNo-1);
			if (mpProbabilityAll != NULL)
				delete mpProbabilityAll;
			mpProbabilityAll = new CGDALFileManager;
			if (!mpProbabilityAll->LoadFrom(foreopProbfilename.toStdString().c_str()))
			{
				_ilog(QString("Load Fore Probability Image Error. Filename = %1").arg(foreopProbfilename));
				return -2;
			}
			_ilog(QString("Load Fore Probability Image Successed. Filename = %1").arg(foreopProbfilename));
		}
		
	}	

	DIMS dims;
	mpUrbanReClassiImg->m_header.InitDIMS(&dims);

	//��������ļ�
	CGDALFileManager* pOut = new CGDALFileManager;
	pOut->m_header = mpUrbanReClassiImg->m_header;
	pOut->m_header.m_nSamples = dims.GetWidth(); 
	pOut->m_header.m_nLines = dims.GetHeight();
	pOut->m_header.m_nBands = dims.GetBands();
	pOut->m_header.m_nDataType = CAResultDTCode;

	

	

	//����������IO
	HAPBEGBase* pIO = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
	if (!pIO->Set_MgrIn(mpUrbanReClassiImg, dims, 4))
	{
		_ilog("IO Set Urban ReClassification Image In Error!");
		delete pIO;
		pIO = NULL;
		delete pOut;
		pOut = NULL;
		return -301;
	}
	_ilog("IO Set Urban ReClassification Image In Successed.");

	if (!pIO->Set_MgrOut(pOut, outputFilename.toStdString().c_str()))
	{
		_ilog("IO Set Urban Development CA Image Out Error!");
		delete pIO;
		pIO = NULL;
		delete pOut;
		pOut = NULL;
		return -302;
	}
	_ilog("IO Set Urban Development CA Image Out Successed.");

	//��������������ָ��IO
// 	HAPBEGBase* pSIdxIO = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
// 	if (!pSIdxIO->Set_MgrIn(mpSuitIdxImg, dims))
// 	{
// 		_ilog("IO Set Suitable Index Image In Error!");
// 		delete pSIdxIO;
// 		pSIdxIO = NULL;
// 		delete pIO;
// 		pIO = NULL;
// 		delete pOut;
// 		pOut = NULL;
// 		return -303;
// 	}
// 	_ilog("IO Set Suitable Index Image In Successed.");

	//������������ļ�
	CGDALFileManager* pProbOut = new CGDALFileManager;
	pProbOut->m_header = mpProbabilityAll->m_header;
	pProbOut->m_header.m_nSamples = dims.GetWidth(); 
	pProbOut->m_header.m_nLines = dims.GetHeight();
	pProbOut->m_header.m_nBands = dims.GetBands();
	pProbOut->m_header.m_nDataType = ProbabilityDTCode;

	//����ȫ�ַ�չ����IO
	HAPBEGBase* pProbIO = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
	if (!pProbIO->Set_MgrIn(mpProbabilityAll, dims, 4))
	{
		_ilog("IO Set Probability Image In Error!");
		delete pProbIO;
		pProbIO = NULL;
		delete pIO;
		pIO = NULL;
		delete pOut;
		pOut = NULL;
		delete pProbOut;
		pProbOut = NULL;
		return -304;
	}
	_ilog("IO Set Probability Image In Successed.");

	if (!pProbIO->Set_MgrOut(pProbOut, opProbFilename.toStdString().c_str()))
	{
		_ilog("IO Set Probability Image Out Error!");
		delete pProbIO;
		pProbIO = NULL;
		delete pIO;
		pIO = NULL;
		delete pOut;
		pOut = NULL;
		delete pProbOut;
		pProbOut = NULL;
		return -304;
	}
	_ilog("IO Set Probability Image Out Successed.");

	//��������
	HAPBEGBase* pAddProb = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
	if (mbIsUseAddProb)
	{		
		DIMS dims1;
		mpAddProb->m_header.InitDIMS(&dims1);
		if (!pAddProb->Set_MgrIn(mpAddProb, dims1))
		{
			_ilog("IO Set Additional Probability Image In Error!");
			delete pAddProb;
			pAddProb = NULL;
			delete pProbIO;
			pProbIO = NULL;
			delete pIO;
			pIO = NULL;
			delete pOut;
			pOut = NULL;
			return -305;
		}
		_ilog("IO Set Additional Probability Image In Successed.");
	}
	else
	{
		delete pAddProb;
		pAddProb = NULL;
	}

	//��ʼ����
	//ͳ�ƽ��и��ĵ�����
	nChangeCount = 0;


	//����
	//int _nCode = 0;
	int _nCode = _process(nIterNo, pIO, /*pSIdxIO, */pProbIO, pAddProb,nChangeCount);
	


	//�ر�IO
	pIO->Close_MgrIn();
	pIO->Close_MgrOut();
//	pSIdxIO->Close_MgrIn();
	pProbIO->Close_MgrIn();
	pProbIO->Close_MgrOut();

	if (mbIsUseAddProb==true && pAddProb!=NULL)
	{
		pAddProb->Close_MgrIn();
		delete pAddProb;
		pAddProb = NULL;
	}

	//ɾ��IO
	delete pIO;
	pIO = NULL;
// 	delete pSIdxIO;
// 	pSIdxIO = NULL;
	delete pProbIO;
	pProbIO = NULL;

	//�ͷ��ڴ�
	pOut->Close();
	//delete pOut;
	pOut = NULL;

	pProbOut->Close();
	//delete pProbOut;
	pProbOut = NULL;

	//����ֵ
	if (_nCode != 0)
	{
		_ilog(QString("No. %1 CA Analysis Finished.").arg(nIterNo+1));
		return _nCode;
	}
	else
	{
		_ilog(QString("No. %1 CA Analysis Finished.").arg(nIterNo+1));
		return 0;
	}
	

}

int UrbanChangeCAIteration::_process_ui_potentialpts(int nIterNo, long nUIPerTime, HAPBEGBase *pPIO, HAPBEGBase* pIO, HAPBEGBase* pAddPIO)
{
	if (pIO == NULL || pPIO == NULL)
	{
		_ilog("Some IO May Be NULL.");
		return -4;
	}

	if (mlPotentialPts.size() == 0)
	{
		_ilog("Potential Points List is NULL.");
		return -5;
	}

	//���������Χ
	int iSamples = mpUrbanReClassiImg->m_header.m_nSamples;
	int iLines = mpUrbanReClassiImg->m_header.m_nLines;
	int n=0, m=0;

	//���������ֵ
	float _fDevelopProb = 0;
	float _fNeighborProb = 0;
	float _fRandomProb = 0;
	float _fProb = 0;

	//��ʱ����
	int tp_x = 0, tp_y = 0;
	UPoint tp_pt;
	UrbanReclassDT caVal;
	ProbabilityDT pbVal = 0, pAddProb = 1.0;
	//ProbabilityDT pbValOut = 0;
	UrbanReclassDT outVal = RECLASS_UNKNOWN;
	double dst_x, dst_y;

	/////////////////////////////////////////
	_ilog(QString("CA Simulation Starting No. %1 Iteration").arg(nIterNo+1));

	QList<UPoint> Iterpts;
	Iterpts.clear();

	int rdIdx = 0;
	int nSelPtsCount = 0;
	int nMaxRestartCount = mnFindRndPtsRestart;	//������ѭ��
	//��ʱ��Ϊ���Դ
	srand(QTime::currentTime().msec());
	while (Iterpts.size() < nUIPerTime)
	{
		nSelPtsCount++;
// 		if (nSelPtsCount > nMaxRestartCount)
// 			break;

		//���Ǳ�ڵ㲻���Ļ��������е���д���
		if (mlPotentialPts.size() <= nUIPerTime)
		{
			foreach (UPoint __pt, mlPotentialPts)
			{
				if (Iterpts.contains(__pt) || mlIncreasePts.contains(__pt))
					continue;

				Iterpts.append(__pt);
				mlIncreasePts.append(__pt);
			}
			break;
		}
		
		

		//����nUIPerTime������㣬�洢��perpts��	
// 		QList<UPoint> perpts;
// 		perpts.clear();

		if (nSelPtsCount%1000 == 0)
		{
			_ilog(QString("Iteration No. %1 Starting No. %2 Random Points Selection. IterPts = %3 / %4.")\
				.arg(nIterNo+1).arg(nSelPtsCount).arg(Iterpts.size()).arg(nUIPerTime));
		}
		

		rdIdx = (mlPotentialPts.size()-1)*(double)rand()/(double)RAND_MAX;
		tp_pt = mlPotentialPts[rdIdx];

		if (Iterpts.contains(tp_pt) || mlIncreasePts.contains(tp_pt))
			continue;

		Iterpts.append(tp_pt);
		mlIncreasePts.append(tp_pt);


// 			while (perpts.size()<nUIPerTime)	//Ϊ�����Ӹ��ʣ���������д����2��
// 			{		
// 

// 
// 				rdIdx = (mlPotentialPts.size()-1)*(double)rand()/(double)RAND_MAX;
// 				tp_pt = mlPotentialPts[rdIdx];
// 
// 				//�ж��Ƿ����
// 				if (perpts.contains(tp_pt) )
// 					continue;
// 
// 				perpts.append(tp_pt);
// 
// 				//��mlPotentialPts�������Ԫ
// 				//mlPotentialPts.removeAt(rdIdx);
// 
// 			}

// 			_ilog(">> ranking the points selected...");
// 			//��perpts����
// 			qSort(perpts.begin(), perpts.end());
// 
// 			_ilog(">> optimizing the points selected...");
// 			//�ж�perpts��ÿһ�������Ƿ�չΪ����		
// 			foreach (UPoint _pt, perpts)
// 			{
// 				m = _pt.x;
// 				n = _pt.y;
// 				//��ȡ��ǰ������������
// 				caVal = *(UrbanReclassDT*)pIO->Read(m, n, 0);
// 				//pbValOut = *(ProbabilityDT*)pPIO->Read(m, n, 0);
// 
// 				if (caVal == RECLASS_UNKNOWN || caVal == RECLASS_CITY)
// 				{
// 					continue;
// 				}
// 				else if (caVal == RELCASS_PROTECTIVE && mbIsSimulatePA == false)
// 				{
// 					continue;
// 				}
// 				else
// 				{
// 					//��ȡ��ǰ����
// 					if (_Src2DstCoordTrans(m, n, dst_x, dst_y, mpUrbanReClassiImg, mpProbabilityAll))
// 					{
// 						pbVal = *(ProbabilityDT*)pPIO->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
// 						_fDevelopProb = pbVal;
// 
// 						//�����������
// 						_fRandomProb = (double)rand()/(double)RAND_MAX;
// 
// 						//�����������
// 						if (mbIsNeighborCalUseProbImg)
// 							_fNeighborProb = _calNeighPrb2(m, n, pPIO, pIO);
// 						else
// 							_fNeighborProb = _calNeighPrb(m, n, pIO);
// 
// 						//���㷢չ����
// 						_fProb = _fDevelopProb*_fNeighborProb*_fRandomProb;
// 
// 						//_pt.uprob = _fProb;
// 
// 						if (_fProb >= mdMinDevelopProb)
// 						{
// 							Iterpts.append(_pt);	
// 							mlIncreasePts.append(_pt);
// 						}
// 						else
// 							continue;
// 					}
// 					else
// 					{
// 						continue;
// 					}			
// 				}
// 			}	

	}

	//����޳���ѡ�ĵ�
// 	if (Iterpts.size() > nUIPerTime)
// 	{
// 		int _nMaxNum = Iterpts.size() - nUIPerTime;
// 		int _ntpCount = 0;
// 		srand(QTime::currentTime().msec());
// 		for (_ntpCount = 0; _ntpCount < _nMaxNum; _ntpCount++)
// 		{
// 			int _i = (Iterpts.size()-1)*(double)rand()/(double)RAND_MAX;
// 			Iterpts.removeAt(_i);
// 		}
// 	}
			
	_ilog(QString("Iteration No. %1 Starting No. %2 Random Points Selection. IterPts = %3 / %4.")\
		.arg(nIterNo+1).arg(nSelPtsCount).arg(Iterpts.size()).arg(nUIPerTime));
	

	qSort(Iterpts.begin(), Iterpts.end());

		//�ȸ��ƺ����ͼ����Iterpts���³���
		for (n=0; n<iLines; n++)
		{
			//�����Ϣ�ļ�
			if ((n+1)%100 == 0)
				_ilog(QString("No. %3 Iteration: Processing Rate: %1 / %2").arg(n+1).arg(iLines).arg(nIterNo+1));

			for (m=0; m<iSamples; m++)
			{
				//��ȡ��ǰ������������
				caVal = *(UrbanReclassDT*)pIO->Read(m, n, 0);
				//pbValOut = *(ProbabilityDT*)pPIO->Read(m, n, 0);

				if (caVal == RECLASS_UNKNOWN)
				{
					outVal = RECLASS_UNKNOWN;
					pIO->Write(&outVal);
					continue;
				}

				else if (caVal == RECLASS_CITY)
				{
					outVal = RECLASS_CITY;
					pIO->Write(&outVal);

					continue;
				}

				else if (caVal == RELCASS_PROTECTIVE && mbIsSimulatePA == false)
				{
					outVal = RELCASS_PROTECTIVE;
					pIO->Write(&outVal);
					continue;
				}

				else
				{
					//��ȡ��ǰ����
					if (Iterpts.contains(UPoint(m, n)))
					{
						outVal = RECLASS_CITY;
						pIO->Write(&outVal);
						continue;
					}
					else
					{
						outVal = caVal;
						pIO->Write(&outVal);
						continue;
					}

				}		

			}
		}

		_ilog(QString("CA Simulation Finished No. %1 Iteration").arg(nIterNo+1));
	
	
	return 0;
}

int UrbanChangeCAIteration::_process_ui(int nIterNo, long nUIPerTime, HAPBEGBase *pPIO, HAPBEGBase* pIO, HAPBEGBase* pAddPIO)
{
	if (pIO == NULL || pPIO == NULL)
	{
		_ilog("Some IO May Be NULL.");
		return -4;
	}

	//���������Χ
	int iSamples = mpUrbanReClassiImg->m_header.m_nSamples;
	int iLines = mpUrbanReClassiImg->m_header.m_nLines;
	int n=0, m=0;

	//���������ֵ
	float _fDevelopProb = 0;
	float _fNeighborProb = 0;
	float _fRandomProb = 0;
	float _fProb = 0;

	//��ʱ����
	int tp_x = 0, tp_y = 0;
	UPoint tp_pt;
	UrbanReclassDT caVal;
	ProbabilityDT pbVal = 0, pAddProb = 1;
	//ProbabilityDT pbValOut = 0;
	UrbanReclassDT outVal = RECLASS_UNKNOWN;
	double dst_x, dst_y;

	/////////////////////////////////////////
	_ilog(QString("CA Simulation Starting No. %1 Iteration").arg(nIterNo+1));

	QList<UPoint> Iterpts;
	Iterpts.clear();

	int nSelPtsCount = 0;
	int nMaxRestartCount = mnFindRndPtsRestart;	//������ѭ��
	while (Iterpts.size() < nUIPerTime)
	{
		nSelPtsCount++;
		if (nSelPtsCount > nMaxRestartCount)
			break;

		_ilog(QString("Iteration No. %1 Starting No. %2 Random Points Selection. IterPts = %3 / %4.")\
			.arg(nIterNo+1).arg(nSelPtsCount).arg(Iterpts.size()).arg(nUIPerTime));
		
		//��ʱ��Ϊ���Դ
		srand(QTime::currentTime().msec());

		//����nUIPerTime������㣬�洢��perpts��	
		QList<UPoint> perpts;
		perpts.clear();
		while (perpts.size() < 2*nUIPerTime)	//Ϊ�����Ӹ��ʣ���������д����2��
		{				
			tp_x = (iSamples-1)*(double)rand()/(double)RAND_MAX;
			tp_y = (iLines-1)*(double)rand()/(double)RAND_MAX;
			tp_pt = UPoint(tp_x, tp_y);

			//�ж��Ƿ����
			if (perpts.contains(tp_pt) || mlIncreasePts.contains(tp_pt))
				continue;

			perpts.append(tp_pt);
		}

		//��perpts����
		qSort(perpts.begin(), perpts.end());

		//�ж�perpts��ÿһ�������Ƿ�չΪ����		
		foreach (UPoint _pt, perpts)
		{
			m = _pt.x;
			n = _pt.y;
			//��ȡ��ǰ������������
			caVal = *(UrbanReclassDT*)pIO->Read(m, n, 0);
			//pbValOut = *(ProbabilityDT*)pPIO->Read(m, n, 0);

			if (caVal == RECLASS_UNKNOWN || caVal == RECLASS_CITY)
			{
				continue;
			}
			else if (caVal == RELCASS_PROTECTIVE && mbIsSimulatePA == false)
			{
				continue;
			}
			else
			{
				pAddProb = 1.0f;
				if (mbIsUseAddProb && _Src2DstCoordTrans(m, n, dst_x, dst_y, mpUrbanReClassiImg, mpAddProb))
				{
					if (dst_x>=0 && dst_y>=0 && dst_x<mpAddProb->m_header.m_nSamples && dst_y<mpAddProb->m_header.m_nLines)
						pAddProb = *(ProbabilityDT*)pAddPIO->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
				}

				//��ȡ��ǰ����
				if (_Src2DstCoordTrans(m, n, dst_x, dst_y, mpUrbanReClassiImg, mpProbabilityAll))
				{
					pbVal = *(ProbabilityDT*)pPIO->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
					_fDevelopProb = pbVal;

					//�����������
					_fRandomProb = (double)rand()/(double)RAND_MAX;

					//�����������
					if (mnIsNeighborCalUseProbImg == 1)
						_fNeighborProb = _calNeighPrb2(m, n, pPIO, pIO);
					else if(mnIsNeighborCalUseProbImg == 0)
						_fNeighborProb = _calNeighPrb(m, n, pIO);
					else
						_fNeighborProb = _calNeighPrb_tan(m, n, pIO);


					//���㷢չ����
					_fProb = _fDevelopProb*_fNeighborProb*_fRandomProb*pAddProb;

					//_pt.uprob = _fProb;

					if (_fProb >= mdMinDevelopProb)
					{
						Iterpts.append(_pt);	
						mlIncreasePts.append(_pt);
					}
					else
						continue;
				}
				else
				{
					continue;
				}			
			}
		}		
	}

	if (Iterpts.size() > nUIPerTime)
	{
		int _nMaxNum = nUIPerTime - Iterpts.size();
		int _ntpCount = 0;
		srand(QTime::currentTime().msec());
		for (_ntpCount = 0; _ntpCount < _nMaxNum; _ntpCount++)
		{
			int _i = (Iterpts.size()-1)*(double)rand()/(double)RAND_MAX;
			Iterpts.removeAt(_i);
		}
	}

	_ilog(QString("Iteration No. %1 Starting No. %2 Random Points Selection. IterPts = %3 / %4.")\
		.arg(nIterNo+1).arg(nSelPtsCount).arg(Iterpts.size()).arg(nUIPerTime));

		//�ȸ��ƺ����ͼ����Iterpts���³���
		for (n=0; n<iLines; n++)
		{
			//�����Ϣ�ļ�
			if ((n+1)%10 == 0)
				_ilog(QString("No. %3 Iteration: Processing Rate: %1 / %2").arg(n+1).arg(iLines).arg(nIterNo+1));

			for (m=0; m<iSamples; m++)
			{
				//��ȡ��ǰ������������
				caVal = *(UrbanReclassDT*)pIO->Read(m, n, 0);
				//pbValOut = *(ProbabilityDT*)pPIO->Read(m, n, 0);

				if (caVal == RECLASS_UNKNOWN)
				{
					outVal = RECLASS_UNKNOWN;
					pIO->Write(&outVal);
					continue;
				}

				else if (caVal == RECLASS_CITY)
				{
					outVal = RECLASS_CITY;
					pIO->Write(&outVal);

					continue;
				}

				else if (caVal == RELCASS_PROTECTIVE && mbIsSimulatePA == false)
				{
					outVal = RELCASS_PROTECTIVE;
					pIO->Write(&outVal);
					continue;
				}

				else
				{
					//��ȡ��ǰ����
					if (Iterpts.contains(UPoint(m, n)))
					{
						outVal = RECLASS_CITY;
						pIO->Write(&outVal);
						continue;
					}
					else
					{
						outVal = caVal;
						pIO->Write(&outVal);
						continue;
					}

				}		

			}
		

		_ilog(QString("CA Simulation Finished No. %1 Iteration").arg(nIterNo+1));

	}
	return 0;
}

/*
int UrbanChangeCAIteration::_process_ui_wrong(int nIterNum, long nUrbanIncreaseNum, HAPBEGBase *pPIO, HAPBEGBase* pIO)
{
	if (pIO == NULL)
	{
		_ilog("Some IO May Be NULL.");
		return -4;
	}

	//�����ĳ�����Ԫ
	QList<UPoint> lIncreasePts;
	lIncreasePts.clear();

	//����ÿ�ε���������
	long nUIPerTime = (double)nUrbanIncreaseNum/(double)nIterNum + 1;

	//���������Χ
	int iSamples = mpUrbanReClassiImg->m_header.m_nSamples;
	int iLines = mpUrbanReClassiImg->m_header.m_nLines;
	int n=0, m=0;

	//���������ֵ
	float _fDevelopProb = 0;
	float _fNeighborProb = 0;
	float _fRandomProb = 0;
	float _fProb = 0;

	//��ʱ����
	int tp_x = 0, tp_y = 0;
	UPoint tp_pt;
	UrbanReclassDT caVal;
	ProbabilityDT pbVal = 0;
	//ProbabilityDT pbValOut = 0;
	UrbanReclassDT outVal = RECLASS_UNKNOWN;
	double dst_x, dst_y;

	//��ʼѭ��
	int nIterNo = 0;
	for (nIterNo=0; nIterNo<nIterNum; nIterNo++)
	{
		_ilog(QString("CA Simulation Starting No. %1 Iteration / %2").arg(nIterNo+1).arg(nIterNum));
		//��ʱ��Ϊ���Դ
		srand(QTime::currentTime().msec());

		//int nPerTimeCount = 0;

		QList<UPoint> Iterpts;
		Iterpts.clear();

		while (Iterpts.size() < nUIPerTime)
		{
			//����nUIPerTime������㣬�洢��perpts��	
			QList<UPoint> perpts;
			perpts.clear();
			while (perpts.size() < nUIPerTime)
			{				
				tp_x = iSamples*(double)rand()/(double)RAND_MAX;
				tp_y = iLines*(double)rand()/(double)RAND_MAX;
				tp_pt = UPoint(tp_x, tp_y);

				//�ж��Ƿ����
				if (perpts.contains(tp_pt) || lIncreasePts.contains(tp_pt))
					continue;

				perpts.append(tp_pt);
			}

			//��perpts����
			qSort(perpts.begin(), perpts.end());

			//�ж�perpts��ÿһ�������Ƿ�չΪ����		
			foreach (UPoint _pt, perpts)
			{
				m = _pt.x;
				n = _pt.y;
				//��ȡ��ǰ������������
				caVal = *(UrbanReclassDT*)pIO->Read(m, n, 0);
				//pbValOut = *(ProbabilityDT*)pPIO->Read(m, n, 0);

				if (caVal == RECLASS_UNKNOWN || caVal == RECLASS_CITY)
				{
					continue;
				}
				else if (caVal == RELCASS_PROTECTIVE && mbIsSimulatePA == false)
				{
					continue;
				}
				else
				{
					//��ȡ��ǰ����
					if (_Src2DstCoordTrans(m, n, dst_x, dst_y, mpUrbanReClassiImg, mpProbabilityAll))
					{
						pbVal = *(ProbabilityDT*)pPIO->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
						_fDevelopProb = pbVal;

						//�����������
						_fRandomProb = (double)rand()/(double)RAND_MAX;

						//�����������
						if (mbIsNeighborCalUseProbImg)
							_fNeighborProb = _calNeighPrb2(m, n, pPIO, pIO);
						else
							_fNeighborProb = _calNeighPrb(m, n, pIO);

						//���㷢չ����
						_fProb = _fDevelopProb*_fNeighborProb*_fRandomProb;

						_pt.uprob = _fProb;

						if (_fProb > mdMinDevelopProb)
						{
							Iterpts.append(_pt);	
							lIncreasePts.append(_pt);
						}
						else
							continue;
					}
					else
					{
						continue;
					}			
				}
			}		
		}

		//�ȸ��ƺ����ͼ����Iterpts���³���
		for (n=0; n<iLines; n++)
		{
			//�����Ϣ�ļ�
			if ((n+1)%10 == 0)
				_ilog(QString("No. %3 Iteration: Processing Rate: %1 / %2").arg(n+1).arg(iLines).arg(nIterNo+1));

			for (m=0; m<iSamples; m++)
			{
				//��ȡ��ǰ������������
				caVal = *(UrbanReclassDT*)pIO->Read(m, n, 0);
				//pbValOut = *(ProbabilityDT*)pPIO->Read(m, n, 0);

				if (caVal == RECLASS_UNKNOWN)
				{
					outVal = RECLASS_UNKNOWN;
					pIO->Write(&outVal);

// 					pbValOut = 0;
// 					pPIO->Write(&pbValOut);
					continue;
				}

				else if (caVal == RECLASS_CITY)
				{
					outVal = RECLASS_CITY;
					pIO->Write(&outVal);

// 					pbValOut = 1;
// 					pPIO->Write(&pbValOut);
					continue;
				}

				else if (caVal == RELCASS_PROTECTIVE && mbIsSimulatePA == false)
				{
					outVal = RELCASS_PROTECTIVE;
					pIO->Write(&outVal);

// 					pbValOut = 0;
// 					pPIO->Write(&pbValOut);
					continue;
				}

				else
				{
					//��ȡ��ǰ����
					if (Iterpts.contains(UPoint(m, n)))
					{
// 						pbVal = *(ProbabilityDT*)pPIO->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
// 						
// 						_fDevelopProb = pbVal;
// 						//�����������
// 						_fRandomProb = (double)rand()/(double)RAND_MAX;
// 						//�����������
// 						if (mbIsNeighborCalUseProbImg)
// 							_fNeighborProb = _calNeighPrb2(m, n, pPIO, pIO);
// 						else
// 							_fNeighborProb = _calNeighPrb(m, n, pIO);
// 
// 						//���㷢չ����
// 						_fProb = _fDevelopProb*_fNeighborProb*_fRandomProb;
// 
// 						pbValOut = _fProb;
// 						pPIO->Write(&pbValOut);


						outVal = RECLASS_CITY;
						pIO->Write(&outVal);
						continue;
					}
					else
					{
						outVal = caVal;
						pIO->Write(&outVal);

// 						pbValOut = 0;
// 						pPIO->Write(&pbValOut);
						continue;
					}

				}		

			}
		}


	}	

	return 0;
}
*/

int UrbanChangeCAIteration::_process( int nIterNo, HAPBEGBase *pIO, /*HAPBEGBase* pSIn,*/ HAPBEGBase *pPIO, HAPBEGBase* pAddPIO, long& nChangeCount )
{
	if (pIO == NULL || pPIO == NULL)
	{
		_ilog("Some IO May Be NULL.");
		return -4;
	}

	nChangeCount = 0;

	//���������Χ
	int iSamples = mpUrbanReClassiImg->m_header.m_nSamples;
	int iLines = mpUrbanReClassiImg->m_header.m_nLines;
	int n=0, m=0;

	//���������ֵ
	float _fDevelopProb = 0;
	float _fNeighborProb = 0;
	float _fRandomProb = 0;
	float _fProb = 0;

	UrbanReclassDT caVal = RECLASS_UNKNOWN;	//������������
	//LandSuitabilityDT lsVal = INDEX_DEVELOP_UNABLE;
	ProbabilityDT pbVal = 0, pAddProb = 1.0;
	ProbabilityDT pbValOut = 0;
	UrbanReclassDT outVal = RECLASS_UNKNOWN;

// 	char filename[2048];
// 	pAddPIO->Get_MgrIn()->GetFileName(filename);
// 	cout<<pAddPIO->Get_MgrIn()->m_header.m_nDataType<<endl;
// 	cout<<filename<<endl;
	

	double dst_x, dst_y;
	//���������
	srand(QTime::currentTime().msec());
	for (n=0; n<iLines; n++)
	{
		//�����Ϣ�ļ�
		if ((n+1)%10 == 0)
			_ilog(QString("No. %4 Iteration: Processing Rate: %1 / %2, Change To City Count = %3").arg(n+1).arg(iLines).arg(nChangeCount).arg(nIterNo+1));

		for (m=0; m<iSamples; m++)
		{
			//��ȡ��ǰ������������
			caVal = *(UrbanReclassDT*)pIO->Read(m, n, 0);
			//pbValOut = *(ProbabilityDT*)pPIO->Read(m, n, 0);

			if (caVal == RECLASS_UNKNOWN)
			{
				outVal = RECLASS_UNKNOWN;
				pIO->Write(&outVal);

				pbValOut = 0;
				pPIO->Write(&pbValOut);
				continue;
			}

			else if (caVal == RECLASS_CITY)
			{
				outVal = RECLASS_CITY;
				pIO->Write(&outVal);

				pbValOut = 1;
				pPIO->Write(&pbValOut);
				continue;
			}

			else if (caVal == RELCASS_PROTECTIVE && mbIsSimulatePA == false)
			{
				outVal = RELCASS_PROTECTIVE;
				pIO->Write(&outVal);

				pbValOut = 0;
				pPIO->Write(&pbValOut);
				continue;
			}

			else
			{
				pAddProb = 1.0f;
				if (mbIsUseAddProb && _Src2DstCoordTrans(m, n, dst_x, dst_y, mpUrbanReClassiImg, mpAddProb))
				{
					//cout<<dst_x<<", "<<dst_y<<endl;
					if (dst_x>=0 && dst_y>=0 && dst_x<mpAddProb->m_header.m_nSamples && dst_y<mpAddProb->m_header.m_nLines)
						pAddProb = *(ProbabilityDT*)pAddPIO->Read(int(dst_x+0.5), int(dst_y+0.5), 0);

					if (pAddProb<0)
					{
						pAddProb = 0;
					}
				}

				//��ȡ��ǰ����
				if (_Src2DstCoordTrans(m, n, dst_x, dst_y, mpUrbanReClassiImg, mpProbabilityAll))
				{
					pbVal = *(ProbabilityDT*)pPIO->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
					//pbVal = *(ProbabilityDT*)pPIO->Read(m, n, 0);
					_fDevelopProb = pbVal;

					//�����������
					_fRandomProb = (double)rand()/(double)RAND_MAX;

					//�����������
					if (mnIsNeighborCalUseProbImg == 1)
						_fNeighborProb = _calNeighPrb2(m, n, pPIO, pIO);
					else if (mnIsNeighborCalUseProbImg == 0)
						_fNeighborProb = _calNeighPrb(m, n, pIO);
					else
						_fNeighborProb = _calNeighPrb_tan(m, n, pIO);
					
					//���㷢չ����
					_fProb = _fDevelopProb*_fNeighborProb*_fRandomProb*pAddProb;

					pbValOut = _fProb;
					pPIO->Write(&pbValOut);

					if (_fProb >= mdMinDevelopProb)
					{
						nChangeCount++;
						outVal = RECLASS_CITY;
						pIO->Write(&outVal);
						continue;
					}
					else
					{
						pIO->Write(&caVal);
						continue;
					}


				}
				else
				{
					outVal = caVal;
					pIO->Write(&outVal);

					pbValOut = 0;
					pPIO->Write(&pbValOut);
					continue;
				}

			}		

		}
	}




	return 0;
}

float UrbanChangeCAIteration::_calNeighPrb( int x, int y, HAPBEGBase* pIO )
{
	int _nCityCount = 0;

	//ͳ������
	int x1, y1;
	for (int i=0; i<8; i++)
	{
			x1 = x+nDirections[i][0];
			y1 = y+nDirections[i][1];
			UrbanReclassDT _Val = *(UrbanReclassDT*)pIO->ReadL(x1, y1, 0);
			if (_Val == RECLASS_CITY)
				_nCityCount++;
	}
	
	return (double)_nCityCount/8.0f;
}

float UrbanChangeCAIteration::_calNeighPrb2( int x, int y, HAPBEGBase* pPIO, HAPBEGBase* pIO )
{
	float _nCityCount = 0;
	ProbabilityDT _Val;
	UrbanReclassDT _uVal;

	//ͳ������
	int x1, y1;
	for (int i=0; i<8; i++)
	{
		x1 = x+nDirections[i][0];
		y1 = y+nDirections[i][1];
		ProbabilityDT _Val = *(ProbabilityDT*)pPIO->ReadL(x1, y1, 0);
		UrbanReclassDT _uVal = *(UrbanReclassDT*)pIO->ReadL(x1, y1, 0);

		if (_uVal == RECLASS_CITY)
			_nCityCount += 1;
		else
			_nCityCount += _Val;
	}

	return (double)_nCityCount/8.0f;

}

int UrbanChangeCAIteration::staticPotentialPts(HAPBEGBase* pPIO, HAPBEGBase* pIO, HAPBEGBase* pAddPIO)
{
	if (pPIO == NULL)
	{
		_ilog("UrbanChangeCAIteration::staticPotentialPts : Some IO May Be NULL.");
		return -4;
	}

	_ilog(QString("Starting Statistic Potential Points..."));

	//�����ĳ�����Ԫ
	mlPotentialPts.clear();

	//���������Χ
	int iSamples = mpProbabilityAll->m_header.m_nSamples;
	int iLines = mpProbabilityAll->m_header.m_nLines;
	int n=0, m=0;
	double dst_x, dst_y;

	ProbabilityDT pbVal = 0, pAddProb = 1.0;
	UrbanReclassDT caVal = RECLASS_UNKNOWN;	//������������
	float _fDevelopProb = 0;
	float _fNeighborProb = 0;
	float _fRandomProb = 0;

	//��ʱ��Ϊ���Դ
	srand(QTime::currentTime().msec());

	for (n=0; n<iLines; n++)
	{
		//�����Ϣ�ļ�
		if ((n+1)%100 == 0)
			_ilog(QString("Statistic Potential Points: %1 / %2 ").arg(n+1).arg(iLines));

		for (m=0; m<iSamples; m++)
		{
			pAddProb = 1.0f;
			if (mbIsUseAddProb && _Src2DstCoordTrans(m, n, dst_x, dst_y, mpUrbanReClassiImg, mpAddProb))
			{
				if (dst_x>=0 && dst_y>=0 && dst_x<mpAddProb->m_header.m_nSamples && dst_y<mpAddProb->m_header.m_nLines)
					pAddProb = *(ProbabilityDT*)pAddPIO->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
			}

			caVal = *(UrbanReclassDT*)pIO->Read(m, n, 0);
			if (caVal == RECLASS_UNKNOWN || caVal == RECLASS_CITY)
				continue;
			else if (caVal == RELCASS_PROTECTIVE && mbIsSimulatePA == false)
				continue;

			//��ȡ��ǰ����
			pbVal = *(ProbabilityDT*)pPIO->Read(m, n, 0);

			if (mnIsNeighborCalUseProbImg == 1)
				_fNeighborProb = _calNeighPrb2(m, n, pPIO, pIO);
			else if (mnIsNeighborCalUseProbImg == 0)
				_fNeighborProb = _calNeighPrb(m, n, pIO);
			else
				_fNeighborProb = _calNeighPrb_tan(m, n, pIO);

			//�����������
			_fRandomProb = (double)rand()/(double)RAND_MAX;

			if (pbVal*_fNeighborProb*_fRandomProb*pAddProb >= mdMinDevelopProb)
			{
				mlPotentialPts.append(UPoint(m, n));
			}
		}
	}

	_ilog(QString("Finished Statistic Potential Points. Total Num = %1 ").arg(mlPotentialPts.size()));

	return 0;
}

float UrbanChangeCAIteration::_calNeighPrb_tan( int x, int y, HAPBEGBase* pIO )
{
	int _nCityCount = 0;

	//ͳ������
	int x1, y1;
	for (int i=0; i<8; i++)
	{
		x1 = x+nDirections[i][0];
		y1 = y+nDirections[i][1];
		UrbanReclassDT _Val = *(UrbanReclassDT*)pIO->ReadL(x1, y1, 0);
		if (_Val == RECLASS_CITY)
			_nCityCount++;
	}

	double _prob = (tan(0.25f*double(_nCityCount)-1.2f)+3.0f)/4.0f - 0.09f;
	return (double)_prob;
}
