#include "UrbanChangeProbabilityAnalysis_V2.h"
//#include "UrbanChangeProbabilityAnalysis.h"

#include "global_include.h"

#include "haplib.h"
#include "hapbegbase.h"
#include "HAPBEGThread.h"
#include "hapbegfactory.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <QtCore/QStringList>
using namespace std;
using namespace alglib;

void UrbanChangeProbabilityAnalysis_V2::_ilog( char* sMessage, char* sInstance/*= "info" */ )
{
	log4cpp::Category& log = log4cpp::Category::getInstance(sInstance);
	log.info(sMessage);
	cout<<sMessage<<endl;
}

void UrbanChangeProbabilityAnalysis_V2::_ilog( QString sMessage, char* sInstance /*= "info" */ )
{
	log4cpp::Category& log = log4cpp::Category::getInstance(sInstance);
	log.info(sMessage.toStdString().c_str());
	//_ilog(sMessage.toStdString().c_str(), sInstance);
	cout<<sMessage.toStdString().c_str()<<endl;
}

UrbanChangeProbabilityAnalysis_V2::UrbanChangeProbabilityAnalysis_V2(void)
{
	_ilog("Starting Calculating Urban Change Probability...");
	mpUrbanChange = NULL;
	mpProbabilityAll = NULL;
	mpIO_UrbanChange = NULL;
	mpProbFor9 = NULL;

	m_ndfInfo = -1;
	mbIsOpProbFor9 = false;
	mbIsLRTraining = false;
	mbIsNNTraining = false;
	m_lpts.clear();

	mnDataCount = 0;
	mvpMgr.clear();
	msbData.clear();
	mvpIO.clear();

	m_Fais = NULL;
	m_agNumJ = m_agNumber = m_numAg = m_numAb = 0;
	m_fExchangePg = m_fExchangePg = 0.0f;
	m_nClassNum = 0;
	m_nAgIteration = 0;
	mnTotalDataCount = 0;

// 	mbIsUseAddProb = false;
// 	mpAddProb = NULL;
// 	mpIO_AddProb = NULL;

	mnClassifiMode = 0;
}


UrbanChangeProbabilityAnalysis_V2::~UrbanChangeProbabilityAnalysis_V2(void)
{
	close();
}

void UrbanChangeProbabilityAnalysis_V2::close()
{
	if (mpIO_UrbanChange!=NULL)
	{
		//mpIO_UrbanChange->Close_MgrIn();
		delete mpIO_UrbanChange;
		mpIO_UrbanChange = NULL;
	}

	if (mpUrbanChange!=NULL)
	{
		mpUrbanChange->Close();
		mpUrbanChange = NULL;
	}

	if (mpProbabilityAll!=NULL)
	{
		mpProbabilityAll->Close();
		mpProbabilityAll = NULL;
	}

	if (mpProbFor9!=NULL)
	{
		mpProbFor9->Close();
		mpProbFor9 = NULL;
	}

// 	if (mpAddProb!=NULL)
// 	{
// 		mpAddProb->Close();
// 		mpAddProb = NULL;
// 	}

	foreach(HAPBEGBase* pIO, mvpIO)
	{
		if (pIO!=NULL)
		{
			//pIO->Close_MgrIn();
			delete pIO;
		}
		pIO = NULL;
	}
	mvpIO.clear();

	foreach(CGDALFileManager* pmgr, mvpMgr)
	{
		if (pmgr!=NULL)
		{
			pmgr->Close();
			pmgr = NULL;
		}
	}
	mvpMgr.clear();
	msbData.clear();

	m_lpts.clear();
	mbIsOpProbFor9 = false;
	mbIsLRTraining = false;
	mbIsNNTraining = false;
	mnClassifiMode = 0;
	mnDataCount = 0;

	//�����㷨���
	if (m_Fais!=NULL)
	{
		delete []m_Fais;
		m_Fais = NULL;
	}

	m_agNumJ = m_agNumber = m_numAg = m_numAb = 0;
	m_fExchangePg = m_fExchangePg = 0.0f;
	m_nClassNum = 0;
	m_nAgIteration = 0;

	_ilog("Finished Calculating Urban Change Probability.");
}


bool UrbanChangeProbabilityAnalysis_V2::run()
{
	m_ndfInfo = -1;
	mnDataCount = 0;
	int bResult = 0;

	//�жϷ���������
	CXml2Document doc(XML_CONFIG);
	if (0 != doc.GetValue(CLASSIFIER_MODE, &mnClassifiMode))
	{
		_ilog("Load CLASSIFIER_MODE Failed!");
		return false;
	}
	_ilog(QString("Classifier Mode = %1").arg(mnClassifiMode));
	
	//������б任�ļ�
	bResult = loadUrbanChangeData();
	if (bResult!=0)
	{
		_ilog(QString("Load Urban Change File Error. CODE = %1").arg(bResult));
		return false;
	}

	//���븨�������ļ�
	bResult = loadAuxiliaryData();
	if (bResult!=0)
	{
		_ilog(QString("Load Auxiliary Data Files Error. CODE = %1").arg(bResult));
		return false;
	}

	
	//������������ļ�
	bResult = makeRandomPointFiles();
	if (bResult!=0)
	{
		_ilog(QString("Make Random Points File Error. CODE = %1").arg(bResult));
		return false;
	}
	

	if (mnClassifiMode == 0)
	{
		//���ɭ��ѵ��
		bResult = trainRandomForestClassifier();
		if (bResult!=0)
		{
			_ilog(QString("Train Random Forest Classifier Error. CODE = %1").arg(bResult));
			return false;
		}

	}
	else if (mnClassifiMode == 1)
	{
		//�߼��ع�ģ��ѵ��
		bResult = trainLRClassifier();
		if (bResult!=0)
		{
			_ilog(QString("Train Logic Regression Classifier Error. CODE = %1").arg(bResult));
			return false;
		}

	}
	else if (mnClassifiMode == 2)
	{
		//������ģ��ѵ��
		bResult = trainNNClassifer();
		if (bResult!=0)
		{
			_ilog(QString("Train Neural Network Classifier Error. CODE = %1").arg(bResult));
			return false;
		}
	}
	else if (mnClassifiMode == 3)
	{
		//�����㷨ģ��ѵ��
		bResult = trainAGClassifier();
		if (bResult!=0)
		{
			_ilog(QString("Train Immune Algorithm Classifier Error. CODE = %1").arg(bResult));
			return false;
		}
	}
	else
	{
		_ilog(QString("Unknown Classifier Training Method = %1.").arg(mnClassifiMode));
		return false;
	}

	//����ȫͼ����
	bResult = CalWholeImageDevelpedProbability();
	if (bResult!=0)
	{
		_ilog(QString("Calculate Whole Image Developed Probability Error. CODE = %1").arg(bResult));
		return false;
	}

	return true;
}

int UrbanChangeProbabilityAnalysis_V2::loadUrbanChangeData()
{
	CXml2Document doc(XML_CONFIG);
	char sfilename[2048];
	if (0 != doc.GetValue(NODE_OUTPUT_URBAN_CHANGE_FILENAME, sfilename))
	{
		_ilog("Load NODE_OUTPUT_URBAN_CHANGE_FILENAME Failed!");
		return -1;
	}
	str_connect_begin(SOPFILEPATH, sfilename);

	if (mpUrbanChange!=NULL)
		delete mpUrbanChange;

	mpUrbanChange = new CGDALFileManager;
	if (!mpUrbanChange->LoadFrom(sfilename))
	{
		_ilog("Load Urban Change Data Error!");
		return -2;
	}

	_ilog("Load Urban Change Data Successed.");

	//��������UrbanChange�ļ�
	DIMS _dims;
	mpUrbanChange->m_header.InitDIMS(&_dims);
	mpIO_UrbanChange = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
	if (!mpIO_UrbanChange->Set_MgrIn(mpUrbanChange, _dims))
	{
		_ilog("Set Urban Change IO Error!");
		return -3;
	}
	_ilog("Set Urban Change IO Successed!");

	return 0;
}

int UrbanChangeProbabilityAnalysis_V2::loadAuxiliaryData()
{
	//��ȡ��ʱ�ļ�����Ŀ
	CXml2Document doc(XML_CONFIG);
	CXml2Node node0 = doc.GetNode(NODE_AUXLIARY_DATA);	
	if (0 != node0.GetAttribute("Filenum", &mnDataCount))
	{
		_ilog("Load NODE_AUXLIARY_DATA -Filenum Failed!");
		return -1;
	}

	if (0 != node0.GetAttribute("TotalFileNum", &mnTotalDataCount))
	{
		_ilog("Load NODE_AUXLIARY_DATA -TotalFileNum Failed!");
		mnTotalDataCount = mnDataCount;
	}

	if (mnDataCount <= 0 || mnTotalDataCount <= 0)
	{
		_ilog("Auxlixary Data Count <= 0!");
		return -1;
	}
	_ilog(QString("Auxiliary Data Count = %1").arg(mnDataCount));
	
	//����ռ�
	foreach(HAPBEGBase* pIO, mvpIO)
	{
		if (pIO!=NULL)
			delete pIO;
		pIO = NULL;
	}
	mvpIO.clear();

	foreach(CGDALFileManager* pmgr, mvpMgr)
	{
		if (pmgr!=NULL)
		{
			pmgr->Close();
			pmgr = NULL;
		}
	}
	mvpMgr.clear();
	msbData.clear();

	//
	int i = 0;
	for (i=0; i<mnDataCount; i++)
	{
		CGDALFileManager* pmgr = NULL;
		StaticBag_V2 sb;
		HAPBEGBase* pIO = NULL;

		//load
		int bResult = loadAuxiliaryData(i, pmgr, pIO, &sb);
		if (bResult != 0)
		{
			_ilog(QString("Load No. %1 Auxiliary Files Error!").arg(i+1));
			return bResult;
		}

		//
		mvpMgr.append(pmgr);
		mvpIO.append(pIO);
		msbData.append(sb);		
	}

	//���븨������
// 	mbIsUseAddProb = false;
// 	int _val = 0;
// 	if (0 != node0.GetAttribute(NODE_BOOL_ADD_PROB, &_val))
// 	{
// 		_val = 0;
// 	}
// 	if (_val <= 0)
// 	{
// 		_ilog("Problem will not use add prob image.");
// 		mbIsUseAddProb = false;
// 		return 0;
// 	}
// 	else
// 	{
// 		char sfilename[2048];
// 		if (0 != doc.GetValue(NODE_ADD_PRON_FILE, sfilename))
// 		{
// 			_ilog("Load NODE_ADD_PRON_FILE Failed! Problem will not use add prob image.");
// 			mbIsUseAddProb = false;
// 			return 0;
// 		}
// 		str_connect_begin(SOPFILEPATH, sfilename);
// 		_ilog(QString("Problem will use add prob image. filename = %1").arg(sfilename));
// 		mbIsUseAddProb = true;
// 
// 		///����ͼ��
// // 		if (mpAddProb!=NULL)
// // 			delete mpAddProb;
// // 		mpAddProb = new CGDALFileManager;
// // 
// // 		if (!mpAddProb->LoadFrom(sfilename))
// // 		{
// // 			_ilog("Load additional probablity image failed!");
// // 			return -2;
// // 		}
// // 
// // 		//����IO
// // 		DIMS _dims;
// // 		mpAddProb->m_header.InitDIMS(&_dims);
// // 		mpIO_AddProb = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
// // 		if (!mpIO_AddProb->Set_MgrIn(mpAddProb, _dims))
// // 		{
// // 			_ilog(QString("Set %1 IO Error!").arg("add prob image"));
// // 			return -4;
// // 		}
// // 		_ilog(QString("Set %1 IO Successed!").arg("add prob image"));			
// 
// 	}


	return 0;
}

int UrbanChangeProbabilityAnalysis_V2::loadAuxiliaryData( int nID, CGDALFileManager* &pmgr, HAPBEGBase* &pIO, StaticBag_V2* sbVal )
{
	CXml2Document doc(XML_CONFIG);
	CXml2Node node0 = doc.GetNode(NODE_AUXLIARY_DATA);	

	//��ȡ��ǰnode
	CXml2Node node_file;
	int i = 0;
	int _flag = 0;
	int _id = 0;
	for (i=0; i<mnTotalDataCount; i++)
	{
		node_file = node0.GetNode("File", i);
		if (0 != node_file.GetAttribute("id", &_id))
			continue;
		
		if (_id == nID)
		{
			_flag = 1;
			break;
		}
		else
			continue;

	}

	if (_flag == 0)
	{
		_ilog(QString("%d Auxiliary File Load Error!").arg(nID));
		return -1;
	}

	char sfilename[2048];
	char sComment[2048];
	//���ݵ�ǰnode��ȡ�ļ�
	QString nodename = "Filepath";
	//�����ļ�
	if (0 != node_file.GetValue(nodename.toStdString().c_str(), sfilename))
	{
		_ilog(QString("Load %1 Failed!").arg(nodename));
		return -1;
	}

	str_connect_begin(SAUXFILEPATH, sfilename);

	//sCommnet
	nodename = "Comment";
	//�����ļ�
	if (0 != node_file.GetValue(nodename.toStdString().c_str(), sComment))
	{
		_ilog(QString("Load %1 Failed!").arg(nodename));
		return -1;
	}

	//����ͳ���ļ�
	nodename = "MinimumValue";
	if (0 != node_file.GetValue(nodename.toStdString().c_str(), &(sbVal->minVal)))
	{
		_ilog("Load Statical Value Error!");
		return -301;
	}

	nodename = "MaximumValue";
	if (0 != node_file.GetValue(nodename.toStdString().c_str(), &(sbVal->maxVal)))
	{
		_ilog("Load Statical Value Error!");
		return -302;
	}

	nodename = "MeanValue";
	if (0 != node_file.GetValue(nodename.toStdString().c_str(), &(sbVal->meanVal)))
	{
		_ilog("Load Statical Value Error!");
		return -303;
	}

	int nVal = 1;
	nodename = "NearBetter";
	if (0 != node_file.GetValue(nodename.toStdString().c_str(), &nVal))
	{
		_ilog("Load Near Better Error!");
		return -303;
	}
	if (nVal <= 0)
		sbVal->isNearBetter = false;
	else 
		sbVal->isNearBetter = true;


	//����ͼ��
	if (pmgr!=NULL)
		delete pmgr;
	pmgr = new CGDALFileManager;

	if (!pmgr->LoadFrom(sfilename))
	{
		_ilog(QString("Load %1 Error!").arg(sComment));
		return -2;
	}

	_ilog(QString("Load %1 Successed.").arg(sComment));
	_ilog(QString("%1 Data Type = %2").arg(sComment).arg(pmgr->m_header.m_nDataType));
	_ilog(QString("%1 Data Statics: MinVal = %2, MaxVal = %3, MeanVal = %4, NearerBetter = %5.").arg(sComment).arg(sbVal->minVal).arg(sbVal->maxVal).arg(sbVal->meanVal).arg(nVal));

	//����IO
	DIMS _dims;
	pmgr->m_header.InitDIMS(&_dims);
	pIO = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
	if (!pIO->Set_MgrIn(pmgr, _dims))
	{
		_ilog(QString("Set %1 IO Error!").arg(sComment));
		return -3;
	}
	_ilog(QString("Set %1 IO Successed!").arg(sComment));

	return 0;

}

int UrbanChangeProbabilityAnalysis_V2::makeRandomPointFiles()
{
	//��ȡ�����洢λ��
	CXml2Document doc(XML_CONFIG);
	char sfilename[2048];
	if (0 != doc.GetValue(NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH, sfilename))
	{
		_ilog("Load NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH Error!");
		return -1;
	}

	str_connect_begin(SOPFILEPATH, sfilename);

	//��ȡ�����������
	long nPointsCount;
	if (0 != doc.GetValue(NODE_RANDOM_POINTS_NUM, &nPointsCount))
	{
		_ilog("Load NODE_RANDOM_POINTS_NUM Error!");
		return -2;
	}


	//��ȡͼ����
	int iSamples = mpUrbanChange->m_header.m_nSamples;
	int iLines = mpUrbanChange->m_header.m_nLines;

	//��ȡÿ����Ҫȡ�ĵ����
	int iPointsPerLine = int(nPointsCount/iLines+1) * RANDOM_POINTS_PARAM;

	//�������������
	_ilog("Producting Random Points...");
	m_lpts.clear();
	int i=0, j=0, k=0;
	srand(QTime::currentTime().msec());

	int _nAvailPixelCount = 0;
	int _nToCityPixelCount = 0;
	QList<int> _vAvailIndex;
	QList<int> _vToCityIndex;
	LandCoverDT lcval;
	for (j=0; j<iLines; j++)
	{
		if ((j+1)%10==0)
			_ilog(QString("Selecting Random Points. Rate: %1 / %2").arg(j+1).arg(iLines));
		
		//ɨ��ȫͼͳ��ÿһ����Ч��ֵ����������jд������
		_nToCityPixelCount = 0;
		_nAvailPixelCount = 0;
		_vAvailIndex.clear();
		_vToCityIndex.clear();

		for (i=0; i<iSamples; i++)
		{
			lcval = *(LandCoverDT*)mpIO_UrbanChange->Read(i, j, 0);
			
			if (lcval == CODE_NOCITY_TO_CITY || lcval == CODE_PROTECTIVE_AREA_TO_CITY)
			{
				_nToCityPixelCount += 1;
				_vToCityIndex.append(i);
			}
			else if (lcval != CODE_UNKOWN_CHANGE)
			{
				_nAvailPixelCount += 1;
				_vAvailIndex.append(i);
			}
		}

		//�ж�ÿһ��ȡ����ֵ��ȡ�ǳ�����ֵ
		int nOtherCount = (iPointsPerLine/2>_nAvailPixelCount)?_nAvailPixelCount:iPointsPerLine/2;
		//�������ѡ
		for (k=0; k<nOtherCount; k++)
		{
			int _rnIdx = (double)(_nAvailPixelCount-1)*rand()/(double)RAND_MAX;
			FeaturePoint_V2 _pt;
			_pt.x = _vAvailIndex[_rnIdx];
			_pt.y = j;//_vAvailIndex[_rnIdx];
			m_lpts.append(_pt);
		}

		//ȡ������ֵ
		int nToCityCount = (iPointsPerLine/2>_nToCityPixelCount)?_nToCityPixelCount:iPointsPerLine/2;
		//�������ѡ
		for (k=0; k<nToCityCount; k++)
		{
			int _rnIdx = (double)(_nToCityPixelCount-1)*rand()/(double)RAND_MAX;
			FeaturePoint_V2 _pt;
			_pt.x = _vToCityIndex[_rnIdx]; //i;
			_pt.y = j; //_vToCityIndex[_rnIdx];
			m_lpts.append(_pt);
		}
	}

	/*
	for (i=0; i<nPointsCount*RANDOM_POINTS_PARAM; i++)
	{
		FeaturePoint _pt;
		_pt.x = (double)iSamples*rand()/(double)RAND_MAX;
		_pt.y = (double)iLines*rand()/(double)RAND_MAX;
		m_lpts.append(_pt);
	}
	*/
	_ilog(QString("Producted Random Points Successed. Count = %1.").arg(m_lpts.size()));
	
	//������������С��������
	qSort(m_lpts.begin(), m_lpts.end());

	//�������������
	for (i=0; i<m_lpts.size(); i++)
	{
		if ((i+1)%10 == 0)
		{
			_ilog(QString("Processing Points Rate: %1 / %2").arg(i+1).arg(m_lpts.size()));
		}

// 		_Pixel2World(&m_lpts[i].lat, &m_lpts[i].lon, m_lpts[i].x, m_lpts[i].y, mpUrbanChange->m_header.m_MapInfo->m_adfGeotransform,\
// 					mpUrbanChange->m_header.m_MapInfo->GetSpatialReferenceWKT());
		mpUrbanChange->pixel2World(&m_lpts[i].lat, &m_lpts[i].lon, m_lpts[i].x, m_lpts[i].y);
		m_lpts[i].class_code = *(LandCoverDT*)mpIO_UrbanChange->Read(m_lpts[i].x, m_lpts[i].y, 0);

		if (m_lpts[i].class_code == CODE_UNKOWN_CHANGE)
		{
			_ilog(QString("Point (%1, %2) Class Code Is NULL.").arg(m_lpts[i].lat, 0, 'f', 4).arg(m_lpts[i].lon, 0, 'f', 4));
			continue;
		}

		//�������
		m_lpts[i].probs.clear();

		for (j=0; j<mnDataCount; j++)
		{
			DistAuxiDT _val = 0.0;
			if(!calFeaturePixelValue(_val, m_lpts[i].lat, m_lpts[i].lon, j))
			{
				_ilog(QString("Point (%1, %2), Attribute No. %3 - Calculate Wrong Value.").arg(m_lpts[i].lat, 0, 'f', 4).arg(m_lpts[i].lon, 0, 'f', 4).arg(j+1));
				_val = 0.0;
			}

			m_lpts[i].addProbValue(_val);
		}

	}

	//������������
	QList<FeaturePoint_V2>::iterator _it;
	_it=m_lpts.begin();
	while(_it != m_lpts.end())
	{
		if (!(_it->isValid()))
		{
			m_lpts.erase(_it);
			_it=m_lpts.begin();
		}
		else
			_it++;
	}
	_ilog(QString("After Processing Available Random Points Count = %1.").arg(m_lpts.size()));


	//���������д���ļ�
	if (!writeRandomPointsToFile(sfilename))
		return -5;	

	//�������List���ͷ��ڴ�
	m_lpts.clear();

	_ilog("Write Random Points To File Successed.");
	return 0;
}

int UrbanChangeProbabilityAnalysis_V2::trainRandomForestClassifier()
{
	_ilog("Training Random Forest Classifier...");
	m_ndfInfo = -1;

	//��ȡ����������ļ���
	CXml2Document doc(XML_CONFIG);
	char sPointsfilename[2048];
	if (0 != doc.GetValue(NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH, sPointsfilename))
	{
		_ilog("Load NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH Failed!");
		return -1;
	}
	str_connect_begin(SOPFILEPATH, sPointsfilename);
	_ilog(QString("Random Points File Name = %1").arg(sPointsfilename));

	

	//��ȡ���ɭ����Ŀ
	int _nDecisionForestTrees = 0;
	if (0 != doc.GetValue(DF_PARAMETER_TREES_NUM, &_nDecisionForestTrees))
	{
		_ilog("Load DF_PARAMETER_TREES_NUM Failed!");
		return -1;
	}
	_ilog(QString("Random Decision Forest Trees Num = %2").arg(_nDecisionForestTrees));

	//��ȡpercent of a training set used to build individual trees.
	double _nDecisionForestSetPersent = 0;
	if (0 != doc.GetValue(DF_PARAMETER_SET_PERSENT, &_nDecisionForestSetPersent))
	{
		_ilog("Load DF_PARAMETER_SET_PERSENT Failed!");
		return -1;
	}
	_ilog(QString("Percent Of A Training Set Used To Build Individual Trees = %1").arg(_nDecisionForestSetPersent, 0, 'f', 6));

	//��ȡnumber of variables used when choosing best split
	int _dDecisionForestNRndVars = 0;
	if (0 != doc.GetValue(DF_PARAMETER_BEST_SPLIT, &_dDecisionForestNRndVars))
	{
		_ilog("Load DF_PARAMETER_BEST_SPLIT Failed!");
		return -1;
	}
	_ilog(QString("Number Of Variables Used When Choosing Best Split = %1").arg(_dDecisionForestNRndVars));

	//��ȡDecision Forest Output Result Data Filepath
	char sResultFilename[2048];
	if (0 != doc.GetValue(DF_OUTPUT_RESULT_DATA, sResultFilename))
	{
		_ilog("Load DF_OUTPUT_RESULT_DATA Failed!");
		return -1;
	}	
	str_connect_begin(SOPFILEPATH, sResultFilename);
	_ilog(QString("RDF Accuracy Output Filename = %1").arg(sResultFilename));

	QFile _file(sPointsfilename);
	if (!_file.open(QFile::ReadOnly))
	{
		_ilog(QString("Open Points File %1 Error!").arg(sPointsfilename));
		return -2;
	}

	QTextStream out(&_file);
	//�����ļ�����ȡ��ĸ��������������ͷ������
	QString _smsg = out.readLine();	//��һ��Ϊ�����Ͳ�������
	QStringList _slist = _smsg.split(",", QString::SkipEmptyParts);
	int nPointCount = _slist[0].toInt();
	int nParaCount = _slist[1].toInt();
	int nClassCount = _slist[2].toInt();
	_ilog(QString("Random Points: Points Number = %1, Params Number = %2, Classes Count = %3").arg(nPointCount).arg(nParaCount).arg(nClassCount));

	out.readLine();	//�ڶ���������
	//��������
	real_2d_array points_array;
	points_array.setlength(nPointCount, nParaCount+1);

	int i, j;
	for (i=0; i<points_array.rows(); i++)
	{
		//��ȡ������ÿһ��
		_smsg = out.readLine();	//��һ��Ϊ�����Ͳ�������
		_slist = _smsg.split(",", QString::SkipEmptyParts);

		for (j=0; j<points_array.cols()-1; j++)
		{
			points_array[i][j]=_slist[2+j].toDouble();	//ǰ2���Ǿ�γ��
		}

		//ÿһ�����һ����Ŀ�������ĸ�������һ�������
		points_array[i][points_array.cols()-1]=_slist[2+nParaCount].toInt();	
	}

	_file.close();


	clock_t _start = clock();
	//��ʼѵ��
	_ilog("Training Random Forest Classifier...");
	dfbuildrandomdecisionforest(points_array, nPointCount, nParaCount, nClassCount, _nDecisionForestTrees,\
		_nDecisionForestSetPersent, m_ndfInfo, m_dfClassifier, m_dfReport);
	_ilog(QString("Decision Forest Traning Result Code = %1").arg(m_ndfInfo));

	clock_t _end = clock();
	double _seconds = double(_end-_start)/(double)CLOCKS_PER_SEC;
	_ilog(QString("Random Training Time = %1 seconds.").arg(_seconds, 0, 'f', 6));

	//��ѵ�����д���ļ�
	if (m_ndfInfo == 1)
	{
		QFile _rltfile(sResultFilename);
		if (!_rltfile.open(QFile::WriteOnly))
		{
			_ilog("Train Result File Write Failed!");
			_ilog("Train Random Forest Classifier Finished.");
			return 0;
		}
		QTextStream _in(&_rltfile);

		_in<<QDateTime::currentDateTime().toString();
		_in<<endl;

		_smsg = QString("rel cls error = %1").arg(m_dfReport.relclserror, 0, 'f', 6);
		_in<<_smsg; _in<<endl;_ilog(_smsg);
		_smsg = QString("avgce = %1").arg(m_dfReport.avgce, 0, 'f', 6);
		_in<<_smsg; _in<<endl;_ilog(_smsg);
		_smsg = QString("rms error = %1").arg(m_dfReport.rmserror, 0, 'f', 6);
		_in<<_smsg; _in<<endl;_ilog(_smsg);
		_smsg = QString("avg error = %1").arg(m_dfReport.avgerror, 0, 'f', 6);
		_in<<_smsg; _in<<endl;_ilog(_smsg);
		_smsg = QString("avg rel error = %1").arg(m_dfReport.avgrelerror, 0, 'f', 6);
		_in<<_smsg; _in<<endl;_ilog(_smsg);
		_smsg = QString("oob rel cls error = %1").arg(m_dfReport.oobrelclserror, 0, 'f', 6);
		_in<<_smsg; _in<<endl;_ilog(_smsg);
		_smsg = QString("oob avgce = %1").arg(m_dfReport.oobavgce, 0, 'f', 6);
		_in<<_smsg; _in<<endl;_ilog(_smsg);
		_smsg = QString("oob rms error = %1").arg(m_dfReport.oobrmserror, 0, 'f', 6);
		_in<<_smsg; _in<<endl;_ilog(_smsg);
		_smsg = QString("oob avg error = %1").arg(m_dfReport.oobavgerror, 0, 'f', 6);
		_in<<_smsg; _in<<endl;_ilog(_smsg);
		_smsg = QString("oob avg rel error = %1").arg(m_dfReport.oobavgrelerror, 0, 'f', 6);
		_in<<_smsg; _in<<endl;_ilog(_smsg);

		_rltfile.close();

	}
	else
	{
		_ilog("Decision Forest Traning Failed.");
		return -3;
	}


	_ilog("Train Result File Writed Successed.");
	_ilog("Train Random Forest Classifier Finished.");
	return 0;
}

int UrbanChangeProbabilityAnalysis_V2::trainLRClassifier()
{
	_ilog("Training Logic Regression Classifier...");
	mbIsLRTraining = false;

	CXml2Document doc(XML_CONFIG);

	//��ȡ����������ļ���
	char sPointsfilename[2048];
	if (0 != doc.GetValue(NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH, sPointsfilename))
	{
		_ilog("Load NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH Failed!");
		return -3;
	}
	str_connect_begin(SOPFILEPATH, sPointsfilename);
	_ilog(QString("Random Points File Name = %1").arg(sPointsfilename));

	//����ģ���ļ�
	char model_file[2048];
	if (0 != doc.GetValue(LR_MODEL_FILEPATH, model_file))
	{
		_ilog("Load LR_MODEL_FILEPATH Failed!");
		return -2;
	}
	str_connect_begin(SOPFILEPATH, model_file);
	_ilog(QString("Saving LR Model File Path = %1").arg(model_file));


	//��ȡ����������
	int max_loop = 100;
	if (0 != doc.GetValue(LR_PARAMETER_MAX_LOOP, &max_loop))
	{
		_ilog("Load LR_PARAMETER_MAX_LOOP Failed!");
		return -2;
	}
	_ilog(QString("Maximal Iteration Loop Num = %1").arg(max_loop));

	//��ȡ��С������
	double loss_thrd = 1e-3;
	if (0 != doc.GetValue(LR_PARAMETER_LOSS_THREAD, &loss_thrd))
	{
		_ilog("Load LR_PARAMETER_LOSS_THREAD Failed!");
		return -2;
	}
	_ilog(QString("Loss Thread = %1").arg(loss_thrd, 0, 'f', 6));

	//��ȡѧϰ����
	float learn_rate = 1.0;
	if (0 != doc.GetValue(LR_PARAMETER_LEARN_RATE, &learn_rate))
	{
		_ilog("Load LR_PARAMETER_LOSS_THREAD Failed!");
		return -2;
	}
	_ilog(QString("Learn Rate = %1").arg(learn_rate, 0, 'f', 6));

	//��ȡ������ֵ
	float lambda = 0.0;
	if (0 != doc.GetValue(LR_PARAMETER_LAMBDA, &lambda))
	{
		_ilog("Load LR_PARAMETER_LAMBDA Failed!");
		return -2;
	}
	_ilog(QString("Lambda Of Gaussian Prior Regularization = %1").arg(lambda, 0, 'f', 6));

	//ƽ��Ȩ��
	int avgweight= 0;
	if (0 != doc.GetValue(LR_PARAMETER_AVG_WEIGHT, &avgweight))
	{
		_ilog("Load LR_PARAMETER_AVG_WEIGHT Failed!");
		return -2;
	}
	_ilog(QString("Average Loop Weight = %1").arg(avgweight));

	//�Ƿ������ļ�
	int _update = 0;
	if (0 != doc.GetValue(LR_PARAMETER_UPDATE_MODEL, &_update))
	{
		_ilog("Load LR_PARAMETER_UPDATE_MODEL Failed!");
		return -2;
	}
	_ilog(QString("Update LR Model File = %1").arg(_update));

	//��ȡͨ������
	float passAccu = 0.5;
	if (0 != doc.GetValue(LR_PARAMETER_PASS_ACCU, &passAccu))
	{
		_ilog("Load LR_PARAMETER_PASS_ACCU Failed!");
		return -2;
	}
	_ilog(QString("Pass Training Accuracy = %1").arg(passAccu, 0, 'f', 6));

	//��ʼѵ��
	if (!m_LRClassifier.load_ca_points_file(sPointsfilename))
	{
		_ilog("LR Load CA Random Points Error!");
		return -4;
	}

	clock_t _start = clock();

	//������һ��ѵ���ļ�
	if (_update) 
	{
		//���������һ�ν��ʧ�ܣ����½�һ��
		if (QFile::exists(model_file) || !m_LRClassifier.load_model(model_file))
		{
			_ilog(QString("LR Load Model File Error. Filepath = %1. Create New Model File Now.").arg(model_file));
			m_LRClassifier.init_omega();
		}
	}
	else 
	{
		m_LRClassifier.init_omega();	
	}

	float _lracc = m_LRClassifier.train_sgd(max_loop, loss_thrd, learn_rate, lambda, avgweight);		//����ѵ������
	m_LRClassifier.save_model(model_file);		//����ģ��

	//cout<<"Last Error = "<<1-_lracc<<endl;	
	_ilog(QString("LR Training Last Avg Error = %1").arg(1-_lracc, 0, 'f', 6));


	clock_t _end = clock();
	double _seconds = double(_end-_start)/(double)CLOCKS_PER_SEC;
	_ilog(QString("Logic Regression Time = %1 seconds.").arg(_seconds, 0, 'f', 6));

	//������ݾ���С��ͨ�������򷵻�0
	if (_lracc >= passAccu)
	{
		mbIsLRTraining = true;
		_ilog("Train Logic Regression Classifier Successed.");
		return 0;
	}
	else
	{
		mbIsLRTraining = false;
		_ilog("Train Logic Regression Classifier Failed, Training Accuracy Is Too Small.");
		return -6;
	}	
}

int UrbanChangeProbabilityAnalysis_V2::trainAGClassifier()
{
	_ilog("Training Immune Algorithm Classifier...");
	mbIsAgTraining = false;

	//��ȡ����������ļ���
	CXml2Document doc(XML_CONFIG);
	char sPointsfilename[2048];
	if (0 != doc.GetValue(NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH, sPointsfilename))
	{
		_ilog("Load NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH Failed!");
		return -1;
	}
	str_connect_begin(SOPFILEPATH, sPointsfilename);
	_ilog(QString("Random Points File Name = %1").arg(sPointsfilename));

	//��ȡ����������
	if (0 != doc.GetValue(AG_PARAMETER_CLASSIFIER_ORDER, &m_agNumJ))
	{
		_ilog("Load AG_PARAMETER_CLASSIFIER_ORDER Failed!");
		return -1;
	}
	_ilog(QString("AG Classifier Order = %1").arg(m_agNumJ));

	//��ȡ������Ŀ
	if (0 != doc.GetValue(AG_PARAMETER_ANTIBODIES_NUM, &m_numAb))
	{
		_ilog("Load AG_PARAMETER_ANTIBODIES_NUM Failed!");
		return -1;
	}
	_ilog(QString("AG Antibodies Num = %1").arg(m_numAb));

	//��ʼ�����弯��
	if (m_Fais!=NULL)
		delete []m_Fais;
	m_Fais = new ais[m_numAb];

	//��ȡ�������
	if (0 != doc.GetValue(AG_PARAMETER_VARIATION_PROB, &m_fVariationPg))
	{
		_ilog("Load AG_PARAMETER_VARIATION_PROB Failed!");
		return -1;
	}
	_ilog(QString("AG Variation Probability = %1").arg(m_fVariationPg));

	//��ȡת������
	if (0 != doc.GetValue(AG_PARAMETER_EXCHANGE_PROB, &m_fExchangePg))
	{
		_ilog("Load AG_PARAMETER_EXCHANGE_PROB Failed!");
		return -1;
	}
	_ilog(QString("AG Exchange Probability = %1").arg(m_fExchangePg));

	//��ȡѵ��ѭ������
	if (0 != doc.GetValue(AG_PARAMETER_ITERATION_NUM, &m_nAgIteration))
	{
		_ilog("Load AG_PARAMETER_ITERATION_NUM Failed!");
		return -1;
	}
	_ilog(QString("AG Training Iteration Num = %1").arg(m_nAgIteration));

	//��ȡ���������
	char sResultFilename[2048];
	if (0 != doc.GetValue(AG_PARAMETER_ACCU_REPORT, sResultFilename))
	{
		_ilog("Load AG_PARAMETER_ACCU_REPORT Failed!");
		return -1;
	}	
	str_connect_begin(SOPFILEPATH, sResultFilename);
	_ilog(QString("AG Accuracy Output Filename = %1").arg(sResultFilename));

	//����ѵ�����ļ�
	QFile _file(sPointsfilename);
	if (!_file.open(QFile::ReadOnly))
	{
		_ilog(QString("Open Points File %1 Error!").arg(sPointsfilename));
		return -2;
	}

	QTextStream out(&_file);
	//�����ļ�����ȡ��ĸ��������������ͷ������
	QString _smsg = out.readLine();	//��һ��Ϊ�����Ͳ�������
	QStringList _slist = _smsg.split(",", QString::SkipEmptyParts);
	int nPointCount = _slist[0].toInt();
	int nParaCount = _slist[1].toInt();
	int nClassCount = _slist[2].toInt();
	_ilog(QString("Random Points: Points Number = %1, Params Number = %2, Classes Count = %3").arg(nPointCount).arg(nParaCount).arg(nClassCount));

	//��ȡ��ԭ����
	m_agNumber = nParaCount + 1;
	m_numAg = nPointCount;
	m_nClassNum = nClassCount;

	out.readLine();	//�ڶ���������

	int i =0, j=0;
	//����ѵ�����ݼ�
	double** pXY = new double*[m_numAg];
	for (i=0; i<m_numAg; i++)
	{
		pXY[i] = new double[m_agNumber];
	}

	for (i=0; i<m_numAg; i++)
	{
		//��ȡ������ÿһ��
		_smsg = out.readLine();	//��һ��Ϊ�����Ͳ�������
		_slist = _smsg.split(",", QString::SkipEmptyParts);

		for (j=0; j<m_agNumber-1; j++)
		{
			pXY[i][j]=_slist[2+j].toDouble();	//ǰ2���Ǿ�γ��
		}

		//ÿһ�����һ����Ŀ�������ĸ�������һ�������
		pXY[i][m_agNumber-1]=_slist[2+nParaCount].toInt();	
	}
	
	_file.close();

	clock_t _start = clock();

	//ѵ��������
	_ilog("Training Immune Algorithm Classifier...");
	//cout<<"loc 1"<<endl;
	m_ag.setAg(pXY, m_numAg, m_agNumber);
	//cout<<"loc 2"<<endl;
	GetClassifier(m_Fais, m_ag, m_numAb, m_agNumJ, m_numAg, m_agNumber,\
				m_fVariationPg, m_fExchangePg, m_nClassNum, m_nAgIteration);
	clock_t _end = clock();
	double _seconds = double(_end-_start)/(double)CLOCKS_PER_SEC;
	_ilog(QString("Immune Algorithm Training Time = %1 seconds.").arg(_seconds, 0, 'f', 6));

	//д�뾫���ļ�
	QFile _accufile(sResultFilename);
	if (!_accufile.open(QFile::WriteOnly))
	{
		_ilog(QString("Open Accuracy File %1 Error!").arg(sResultFilename));
		return -2;
	}

	QTextStream _in(&_accufile);

	_in<<QDateTime::currentDateTime().toString();
	_in<<endl;

	for (i=0; i<m_numAb; i++)
	{
		double _fval = m_Fais[i].Verification(m_ag.myAg2, m_numAg, m_agNumber, m_nClassNum);
		_smsg = QString("Training Accuracy of No. %1 Antibody = %2").arg(i+1).arg(_fval, 0, 'f', 6);
		_ilog(_smsg); _in<<_smsg; _in<<endl;
	}

	_accufile.flush();
	_accufile.close();

	//����ڴ�
	for (i=0; i<m_numAg; i++)
	{
		delete[] pXY[i];
	}
	delete []pXY;

	mbIsAgTraining = true;
	return 0;
}

int UrbanChangeProbabilityAnalysis_V2::trainNNClassifer()
{
	_ilog("Training Neural Network Classifier...");
	mbIsNNTraining = false;

	//��ȡ����������ļ���
	CXml2Document doc(XML_CONFIG);
	char sPointsfilename[2048];
	if (0 != doc.GetValue(NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH, sPointsfilename))
	{
		_ilog("Load NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH Failed!");
		return -1;
	}
	str_connect_begin(SOPFILEPATH, sPointsfilename);
	_ilog(QString("Random Points File Name = %1").arg(sPointsfilename));

	//��ȡ���ز���
	int nHiddenLevel= 5;
	if (0 != doc.GetValue(NN_PARAMETER_HIDDEN_NUM, &nHiddenLevel))
	{
		_ilog("Load NN_PARAMETER_HIDDEN_NUM Failed!");
		return -1;
	}
	_ilog(QString("NN Hidden Level = %1").arg(nHiddenLevel));

	//��ȡѵ������
	int nRestartNum = 10;
	if (0 != doc.GetValue(NN_PARAMETER_RESTART_NUM, &nRestartNum))
	{
		_ilog("Load NN_PARAMETER_RESTART_NUM Failed!");
		return -1;
	}
	_ilog(QString("NN Restart Num = %1").arg(nRestartNum));

	//��ȡ���������
	char sResultFilename[2048];
	if (0 != doc.GetValue(NN_PARAMETER_ACCU_REPORT, sResultFilename))
	{
		_ilog("Load NN_PARAMETER_ACCU_REPORT Failed!");
		return -1;
	}	
	str_connect_begin(SOPFILEPATH, sResultFilename);
	_ilog(QString("NN Accuracy Output Filename = %1").arg(sResultFilename));

	QFile _file(sPointsfilename);
	if (!_file.open(QFile::ReadOnly))
	{
		_ilog(QString("Open Points File %1 Error!").arg(sPointsfilename));
		return -2;
	}

	QTextStream out(&_file);
	//�����ļ�����ȡ��ĸ��������������ͷ������
	QString _smsg = out.readLine();	//��һ��Ϊ�����Ͳ�������
	QStringList _slist = _smsg.split(",", QString::SkipEmptyParts);
	int nPointCount = _slist[0].toInt();
	int nParaCount = _slist[1].toInt();
	int nClassCount = _slist[2].toInt();
	_ilog(QString("Random Points: Points Number = %1, Params Number = %2, Classes Count = %3").arg(nPointCount).arg(nParaCount).arg(nClassCount));

	out.readLine();	//�ڶ���������
	//��������
	real_2d_array points_array;
	points_array.setlength(nPointCount, nParaCount+1);

	int i, j;
	for (i=0; i<points_array.rows(); i++)
	{
		//��ȡ������ÿһ��
		_smsg = out.readLine();	//��һ��Ϊ�����Ͳ�������
		_slist = _smsg.split(",", QString::SkipEmptyParts);

		for (j=0; j<points_array.cols()-1; j++)
		{
			points_array[i][j]=_slist[2+j].toDouble();	//ǰ2���Ǿ�γ��
		}

		//ÿһ�����һ����Ŀ�������ĸ�������һ�������
		points_array[i][points_array.cols()-1]=_slist[2+nParaCount].toInt();	
	}

	_file.close();

	clock_t _start = clock();

	//ѵ��������
	_ilog("Training Neural Network Classifier...");
	mlpcreatetrainercls(nParaCount, nClassCount, m_NNTrainer);
	mlpcreatec1(nParaCount, nHiddenLevel, nClassCount, m_NNNetwork);
	mlpsetdataset(m_NNTrainer, points_array, nPointCount);
	mlptrainnetwork(m_NNTrainer, m_NNNetwork, nRestartNum, m_NNReport);
	_ilog(QString("Neural Network Classifier Trained Average Error = %1").arg(m_NNReport.avgerror));

	clock_t _end = clock();
	double _seconds = double(_end-_start)/(double)CLOCKS_PER_SEC;
	_ilog(QString("Neural Network Training Time = %1 seconds.").arg(_seconds, 0, 'f', 6));

	//���ѵ�������ļ�
	QFile _rltfile(sResultFilename);
	if (!_rltfile.open(QFile::WriteOnly))
	{
		_ilog("Train Result File Write Failed!");
		_ilog("Train Neural Network Classifier Finished.");
		return 0;
	}
	QTextStream _in(&_rltfile);

	_in<<QDateTime::currentDateTime().toString();
	_in<<endl;

	_smsg = QString("rel cls error = %1").arg(m_NNReport.relclserror, 0, 'f', 6);
	_in<<_smsg; _in<<endl;_ilog(_smsg);
	_smsg = QString("avgce = %1").arg(m_NNReport.avgce, 0, 'f', 6);
	_in<<_smsg; _in<<endl;_ilog(_smsg);
	_smsg = QString("rms error = %1").arg(m_NNReport.rmserror, 0, 'f', 6);
	_in<<_smsg; _in<<endl;_ilog(_smsg);
	_smsg = QString("avg error = %1").arg(m_NNReport.avgerror, 0, 'f', 6);
	_in<<_smsg; _in<<endl;_ilog(_smsg);
	_smsg = QString("avg rel error = %1").arg(m_NNReport.avgrelerror, 0, 'f', 6);
	_in<<_smsg; _in<<endl;_ilog(_smsg);
	_smsg = QString("ngrad = %1").arg(m_NNReport.ngrad);
	_in<<_smsg; _in<<endl;_ilog(_smsg);
	_smsg = QString("nhess = %1").arg(m_NNReport.nhess);
	_in<<_smsg; _in<<endl;_ilog(_smsg);
	_smsg = QString("ncholesky = %1").arg(m_NNReport.ncholesky);
	_in<<_smsg; _in<<endl;_ilog(_smsg);


	_rltfile.flush();
	_rltfile.close();


	_ilog("Train Result File Writed Successed.");
	mbIsNNTraining = true;
	_ilog("Train Neural Network Classifier Finished.");
	return 0;
}

int UrbanChangeProbabilityAnalysis_V2::CalWholeImageDevelpedProbability()
{
	//�������ͼ��
	CXml2Document doc(XML_CONFIG);
	char soutputfilename[2048];
	if (0 != doc.GetValue(NODE_OUTPUT_PROBABILTY_DEVELOPED_FILE_PATH, soutputfilename))
	{
		_ilog("Load NODE_OUTPUT_PROBABILTY_DEVELOPED_FILE_PATH Failed!");
		return -1;
	}
	str_connect_begin(SOPFILEPATH, soutputfilename);

	if (mpProbabilityAll!=NULL)
		delete mpProbabilityAll;

	mpProbabilityAll = new CGDALFileManager;
	mpProbabilityAll->m_header = mpUrbanChange->m_header;
	mpProbabilityAll->m_header.m_nBands = 1;
	mpProbabilityAll->m_header.m_nDataType = DistAuxiDTCode;		//float����

	if (!mpIO_UrbanChange->Set_MgrOut(mpProbabilityAll, soutputfilename))
	{
		_ilog("Set Probability Data File IO Error!");
		return -3;
	}
	_ilog("Set Probability Data File IO Successed!");
	
	HAPBEGBase* mpIn_DistCapitanCity = mvpIO[0];
	//�ж��Ƿ�Ҫ����ȫͼ����ͼ��
	int _nVal = 0;
	if (0 != doc.GetValue(NODE_IS_OUTPUT_9BANDS_PROB_IMG, &_nVal))
	{
		_ilog("Load NODE_IS_OUTPUT_9BANDS_PROB_IMG Failed!");
		_nVal = 0;
	}

	if (_nVal<=0)	mbIsOpProbFor9 = false;
	else			mbIsOpProbFor9 = true;
	
	int _nFlag = 0;	//Ϊ1ʱ׼���ɹ�
	if (mbIsOpProbFor9)
	{
		char s9BandsName[2048];
		if (0 == doc.GetValue(NODE_OUTPUT_9BANDS_PROBIMG_FILEPATH, s9BandsName))
		{
			_ilog("Load NODE_OUTPUT_9BANDS_PROBIMG_FILEPATH Successed.");
			str_connect_begin(SOPFILEPATH, s9BandsName);

			if (mpProbFor9!=NULL)
				delete mpProbFor9;
			mpProbFor9 = new CGDALFileManager;

			mpProbFor9 = new CGDALFileManager;
			mpProbFor9->m_header = mpUrbanChange->m_header;
			mpProbFor9->m_header.m_nBands = 9;
			mpProbFor9->m_header.m_nDataType = ProbabilityDTCode;

			if (!mpIn_DistCapitanCity->Set_MgrOut(mpProbFor9, s9BandsName))
			{
				_ilog("Set 9 Bands Probability Error!");	
				return -2014;
			}
			_nFlag = 1;
		}
		
	}


	//��ȡ���ɵĸ���ͼ����
	int _nProbabiltyType = 1;
	if (0 != doc.GetValue(NODE_PARAM_PROBABILITY_PARAM, &_nProbabiltyType))
	{
		_ilog("Load NODE_PARAM_PROBABILITY_PARAM Failed!");
		_nProbabiltyType = 1;
	}
	_ilog(QString("Output Probability Image Type = %1").arg(_nProbabiltyType));


	//���IO�Ƿ����
	if (mpIO_UrbanChange==NULL || mpIn_DistCapitanCity==NULL)
	{
		_ilog("Main or Auxiliary IO Maybe Not Exist!");
		return -2;
	}

	foreach (HAPBEGBase* pIO, mvpIO)
	{
		if (pIO == NULL)
		{
			_ilog("Main or Auxiliary IO Maybe Not Exist!");
			return -2;
		}
	}

	//
	_ilog("Calculating Whole Image Developed Probability...");
	int iSamples, iLines;
	iSamples = mpUrbanChange->m_header.m_nSamples;
	iLines = mpUrbanChange->m_header.m_nLines;

	LandCoverDT lcval;
	ProbabilityDT outVal;
	DistAuxiDT distVal;

	int m, n, kk;

	//���ɭ��ʹ�ô��ж��ļ�
	real_1d_array _classiArr;
	_classiArr.setlength(mnDataCount);	
	//������ʷֲ�
	real_1d_array _classRlt;

	//�߼��ع�ʹ�ô��ж��ļ�
	vector<float> _lrClassiArr;
	vector<float> _lrClassiRlt;

	//�����㷨ʹ�õĴ��ж��ļ�
	double* _agClassiData;
	double* _agResult;
	if (mnClassifiMode == 3)
	{
		double* _agClassiData = new double[m_agNumber-1];
		double* _agResult = new double[m_nClassNum];
		memset(_agClassiData, 0, (m_agNumber-1)*sizeof(double));
		memset(_agResult, 0, m_nClassNum*sizeof(double));
	}
	

	//��ȡ��������
	double dst_x, dst_y;
	int _cID = 0;
	int nThreadnum = mnDataCount<MAX_THREAD_NUM ? mnDataCount:MAX_THREAD_NUM;

	for (m=0; m<iLines; m++)
	{
		if ((m+1)%5==0)
			_ilog(QString("Calculating Probability: Processing Rate: %1 / %2").arg(m+1).arg(iLines));

		for (n=0; n<iSamples; n++)
		{

			/********************************************************************
			//error: ����΢С���ڴ�й¶������
			*********************************************************************/
			if (mnClassifiMode == 1)
			{
				_lrClassiArr.clear();
				_lrClassiRlt.clear();
			}
			else if (mnClassifiMode == 3)
			{
				memset(_agClassiData, 0, (m_agNumber-1)*sizeof(double));
				memset(_agResult, 0, m_nClassNum*sizeof(double));
			}

			dst_x=n, dst_y=m;
			lcval = *(LandCoverDT*)mpIO_UrbanChange->Read(n, m, 0);

			//���ΪlcvalΪ��Ч��ֵ
			if (lcval == CODE_UNKOWN_CHANGE)
			{
				outVal = 0;
				mpIO_UrbanChange->Write(&outVal);

				if (_nFlag==1 && mbIsOpProbFor9)
				{
					for (kk=1; kk<=9; kk++)
					{
						mpIn_DistCapitanCity->Write(&outVal);
					}

				}

				continue;
			}
			
			int _nonflag = 0;

//#pragma omp parallel for num_threads(nThreadnum) reduction(+:_nonflag) schedule(dynamic) 
			for (_cID = 0; _cID < mnDataCount; _cID++)
			{
				if(_Src2DstCoordTrans(n, m, dst_x, dst_y, mpUrbanChange, mvpMgr[_cID]))
				{
					distVal = bilinearGetData(mvpIO[_cID], dst_x, dst_y, 0);//*(DistAuxiDT*)mvpIO[_cID]->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
					if (mnClassifiMode == 0 || mnClassifiMode == 2)
						_classiArr[_cID] = msbData[_cID].calXPower(distVal);
					else if (mnClassifiMode == 3)
						_agClassiData[_cID] = msbData[_cID].calXPower(distVal);
					else
						_lrClassiArr.push_back(msbData[_cID].calXPower(distVal));
				}
				else
				{
					_nonflag = 1;
					break;
				}
			}

			//���������һ������Чֵ
			if (_nonflag == 1)
			{
				outVal = 0;
				mpIO_UrbanChange->Write(&outVal);

				if (_nFlag==1 && mbIsOpProbFor9)
				{
					for (kk=1; kk<=9; kk++)
					{
						mpIn_DistCapitanCity->Write(&outVal);
					}

				}

				continue;	
			}


			//����
			if (mnClassifiMode == 0 || mnClassifiMode == 2)
			{
				if (mnClassifiMode == 0)
					dfprocess(m_dfClassifier, _classiArr, _classRlt);	//���ɭ��
				else
					mlpprocess(m_NNNetwork, _classiArr, _classRlt);		//������
				
				//������
				switch (_nProbabiltyType)
				{
				case 1:
					outVal = _classRlt[CODE_NOCITY_TO_CITY];
					break;
				case 2:
					outVal = _classRlt[CODE_CITY_TO_CITY];
					break;
				case 3:
					outVal = _classRlt[CODE_PROTECTIVE_AREA_TO_CITY];
					break;
				case 4:
					outVal = _classRlt[CODE_NOCITY_TO_CITY]+_classRlt[CODE_PROTECTIVE_AREA_TO_CITY];
					break;
				case 5:
					outVal = _classRlt[CODE_NOCITY_TO_CITY]+_classRlt[CODE_CITY_TO_CITY]+_classRlt[CODE_PROTECTIVE_AREA_TO_CITY];
					break;
				case 6:
					outVal = _classRlt[CODE_PROTECTIVE_AREA_TO_CITY];
					break;
				case 7:
					outVal = _classRlt[CODE_CITY_TO_NOCITY];
					break;
				default:
					outVal = _classRlt[CODE_NOCITY_TO_CITY];
				}	
			}
			else if (mnClassifiMode == 3)
			{
				ClassifySoft(m_Fais, _agClassiData, m_agNumber-1, m_nClassNum, _agResult, m_numAb);
				//������
				switch (_nProbabiltyType)
				{
				case 1:
					outVal = _agResult[CODE_NOCITY_TO_CITY];
					break;
				case 2:
					outVal = _agResult[CODE_CITY_TO_CITY];
					break;
				case 3:
					outVal = _agResult[CODE_PROTECTIVE_AREA_TO_CITY];
					break;
				case 4:
					outVal = _agResult[CODE_NOCITY_TO_CITY]+_agResult[CODE_PROTECTIVE_AREA_TO_CITY];
					break;
				case 5:
					outVal = _agResult[CODE_NOCITY_TO_CITY]+_agResult[CODE_CITY_TO_CITY]+_agResult[CODE_PROTECTIVE_AREA_TO_CITY];
					break;
				case 6:
					outVal = _agResult[CODE_PROTECTIVE_AREA_TO_CITY];
					break;
				case 7:
					outVal = _agResult[CODE_CITY_TO_NOCITY];
					break;
				default:
					outVal = _agResult[CODE_NOCITY_TO_CITY];
				}	
			}
			else
			{
				//�߼��ع����
				m_LRClassifier.classify_data(_lrClassiArr, _lrClassiRlt);
				//������
				switch (_nProbabiltyType)
				{
				case 1:
					outVal = _lrClassiRlt[CODE_NOCITY_TO_CITY];
					break;
				case 2:
					outVal = _lrClassiRlt[CODE_CITY_TO_CITY];
					break;
				case 3:
					outVal = _lrClassiRlt[CODE_PROTECTIVE_AREA_TO_CITY];
					break;
				case 4:
					outVal = _lrClassiRlt[CODE_NOCITY_TO_CITY]+_lrClassiRlt[CODE_PROTECTIVE_AREA_TO_CITY];
					break;
				case 5:
					outVal = _lrClassiRlt[CODE_NOCITY_TO_CITY]+_lrClassiRlt[CODE_CITY_TO_CITY]+_lrClassiRlt[CODE_PROTECTIVE_AREA_TO_CITY];
					break;
				case 6:
					outVal = _lrClassiRlt[CODE_PROTECTIVE_AREA_TO_CITY];
					break;
				case 7:
					outVal = _lrClassiRlt[CODE_CITY_TO_NOCITY];
					break;
				default:
					outVal = _lrClassiRlt[CODE_NOCITY_TO_CITY];
				}	
			}			
			

			mpIO_UrbanChange->Write(&outVal);

			if (_nFlag==1 && mbIsOpProbFor9)
			{
				if (mnClassifiMode == 0 || mnClassifiMode == 2)
				{
					for (kk=1; kk<=9; kk++)
					{
						outVal = _classRlt[kk];
						mpIn_DistCapitanCity->Write(&outVal);
					}
				}
				else if (mnClassifiMode == 3)
				{
					for (kk=1; kk<=9; kk++)
					{
						outVal = _agResult[kk];
						mpIn_DistCapitanCity->Write(&outVal);
					}
				}
				else
				{
					for (kk=1; kk<=9; kk++)
					{
						outVal = _lrClassiRlt[kk];
						mpIn_DistCapitanCity->Write(&outVal);
					}
				}
				
				
			}

			//_classRlt.release();
			
			
		}
	}

	if (mnClassifiMode == 3)
	{
		delete []_agClassiData;
		delete []_agResult;
	}
	

	mpIn_DistCapitanCity->Close_MgrIn();
	if (_nFlag==1 && mbIsOpProbFor9)
	{
		mpIn_DistCapitanCity->Close_MgrOut();
	}
	//delete mpIn_DistCapitanCity;

	for (_cID = 1; _cID < mnDataCount; _cID++)
	{
		mvpIO[_cID]->Close_MgrIn();
	}
	
	mpIO_UrbanChange->Close_MgrIn();
	mpIO_UrbanChange->Close_MgrOut();

	_nFlag = 0;

	_ilog("Calculated Whole Image Developed Probability Successed.");
	return 0;
}

bool UrbanChangeProbabilityAnalysis_V2::calFeaturePixelValue( DistAuxiDT& _prob, double _lat, double _lon, int nID )
{
	double _x, _y;

	CGDALFileManager* pMgr = mvpMgr[nID];
	HAPBEGBase* pIO = mvpIO[nID];
	StaticBag_V2* pSb = &(msbData[nID]);

	pMgr->world2Pixel(_lat, _lon, &_x, &_y);
	if (_x < 0 || _y<0 || _x>=pMgr->m_header.m_nSamples || _y>=pMgr->m_header.m_nLines)
	{
		_prob = 0;
		return false;
	}
	//_prob = *(DistAuxiDT*)pIO->Read(int(_x+0.5), int(_y+0.5), 0);
	_prob = bilinearGetData(pIO, _x, _y, 0);//*(DistAuxiDT*)pIO->Read(int(_x+0.5), int(_y+0.5), 0);
	_prob = pSb->calXPower(_prob);

	return true;

}

bool UrbanChangeProbabilityAnalysis_V2::writeRandomPointsToFile( char* sfilename )
{
	QFile _file(sfilename);
	if (!_file.open(QIODevice::WriteOnly))
	{
		_ilog(QString("Open %1 for Writing error!").arg(sfilename));
		return false;
	}

	_ilog(QString("Opening %1 Writing...").arg(sfilename));
	//
	QTextStream _in(&_file);
	QString _smsg;

	int _nFeatureCount = mnDataCount;	//������Ŀ
	int _nClassCount = NUM_CODE_URBAN_CHANGE;	//0-9
	int* _pFeatureStatic = new int[_nClassCount];
	memset(_pFeatureStatic, 0, _nClassCount*sizeof(int));
	//д�������������������� m_lpts.size() * 5, �������˸������ݴ˴�Ҫ��
	_smsg = QString("%1, %2, %3").arg(m_lpts.size()).arg(_nFeatureCount).arg(_nClassCount);
	_in<<_smsg;
	_in<<endl;

	//д��ͷ
	int i;
	QString _tpstr = "";
	for (i =0; i<_nFeatureCount; i++)
	{
		_tpstr += QString("Feature-%1, ").arg(i+1);
	}
	_smsg = QString("Logitude, Latitude, %1ClassiID").arg(_tpstr); //;"Logitude, Latitude, DistToCapitanCity, DistToPrefectureCity, DistToCountyTown, DistToMainRoad, DistToMainRailway, ClassiID";
	_in<<_smsg;
	_in<<endl;

	
	foreach(FeaturePoint_V2 _pt, m_lpts)
	{
		_tpstr.clear();
		for (i=0; i<_nFeatureCount; i++)
		{
			_tpstr += QString("%1, ").arg(_pt.probs[i], 0, 'f', 6);
		}
		_smsg = QString("%1, %2, %3%4")\
			//.arg(_pt.x/*, 0, 'f', 6*/).arg(_pt.y/*, 0, 'f', 6*/)
			.arg(_pt.lon, 0, 'f', 6).arg(_pt.lat, 0, 'f', 6)\
			.arg(_tpstr)\
			.arg(_pt.class_code);
		_in<<_smsg;	
		_in<<endl;

		//ͳ��ÿ��������Ŀ
		_pFeatureStatic[_pt.class_code] += 1;
	}

	//������ͳ�ƣ������
	_in<<endl;
	_smsg = "Classes Counts Statistics:";
	_in<<_smsg<<endl;
	for (int i=0; i<_nClassCount; i++)
	{
		_smsg = QString("Class %1 = Count %2").arg(i).arg(_pFeatureStatic[i]);
		_ilog(_smsg);
		_in<<_smsg;	
		_in<<endl;
	}

	_in.flush();
	_file.close();

	delete []_pFeatureStatic;

	_ilog(QString("Open %1 Writed Successed.").arg(sfilename));
	return true;
}

DistAuxiDT UrbanChangeProbabilityAnalysis_V2::bilinearGetData( HAPBEGBase* pIO, float dx, float dy, int bands )
{
	float _val = 0;

	switch(pIO->Get_MgrIn()->m_header.m_nDataType)
	{
	case 1:
		_val = _getdata<unsigned char>(pIO, dx, dy, bands);
		break;
	case 2:
		_val = _getdata<short int>(pIO, dx, dy, bands);
		break;
	case 3:
		_val = _getdata<int>(pIO, dx, dy, bands);
		break;
	case 4:
		_val = _getdata<float>(pIO, dx, dy, bands);
		break;
	case 5:
		_val = _getdata<double>(pIO, dx, dy, bands);
		break;
	case 12:
		_val = _getdata<unsigned short>(pIO, dx, dy, bands);
		break;
	default:
		_val = 0;
		break;
	}


	return _val;
}

template<class TT> DistAuxiDT UrbanChangeProbabilityAnalysis_V2::_getdata(HAPBEGBase* pIO, float dx, float dy, int bands)
{
	TT val[4];
	double t1, t2, t, tx1, ty1;

	//get the image width and height
	int widthIn = pIO->m_DimsIn.GetWidth();//m_MultiIO->Get_MgrIn()->m_header.m_nSamples;
	int heightIn = pIO->m_DimsIn.GetHeight();//m_MultiIO->Get_MgrIn()->m_header.m_nLines;

	//calculate the excursion
	float xpos = dx - int(dx);
	float ypos = dy - int(dy);

	//get the pixel value of 4-neighbour
	tx1 = dx+1.0; ty1 = dy+1.0;
// 	if (tx>=widthIn) tx = widthIn - 1;	//width, height
// 	if (ty>=heightIn) ty = heightIn - 1;
// 	if (tx1>=widthIn) tx1 = widthIn - 1;
// 	if (ty1>=heightIn) ty1 = heightIn - 1;

	// 	val[0] =*(TT*)m_MultiIO->Read(int(tx), int(ty), bands);
	// 	return *(TT*)m_MultiIO->Read(int(tx), int(ty), bands);

	val[0] =*(TT*)pIO->ReadL(int(dx), int(dy), bands);	//band
	val[1] =*(TT*)pIO->ReadL(int(tx1), int(dy), bands);
	val[2] =*(TT*)pIO->ReadL(int(dx), int(ty1), bands);
	val[3] =*(TT*)pIO->ReadL(int(tx1), int(ty1), bands);

	//y-direction interpolation
	t1 = (1-ypos)*(double)val[0]+ypos*(double)val[2];
	t2 = (1-ypos)*(double)val[1]+ypos*(double)val[3];

	//x-direction interpolation
	t = (1-xpos)*t1 + xpos*t2;

	return (TT)t;
}