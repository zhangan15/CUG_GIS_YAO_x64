#include "UrbanChangeProbabilityAnalysis_V1.h"

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


void UrbanChangeProbabilityAnalysis_V1::_ilog( char* sMessage, char* sInstance/*= "info" */ )
{
	log4cpp::Category& log = log4cpp::Category::getInstance(sInstance);
	log.info(sMessage);
	cout<<sMessage<<endl;
}

void UrbanChangeProbabilityAnalysis_V1::_ilog( QString sMessage, char* sInstance /*= "info" */ )
{
	log4cpp::Category& log = log4cpp::Category::getInstance(sInstance);
	log.info(sMessage.toStdString().c_str());
	//_ilog(sMessage.toStdString().c_str(), sInstance);
	cout<<sMessage.toStdString().c_str()<<endl;
}

UrbanChangeProbabilityAnalysis_V1::UrbanChangeProbabilityAnalysis_V1(void)
{
	_ilog("Starting Calculating Urban Change Probability...");
	mpUrbanChange = NULL;
	mpAux_DistCapitanCity = NULL;
	mpAux_DistPrefectureCity = NULL;
	mpAux_DistCountyTown = NULL;
	mpAux_DistMainRoad = NULL;
	mpAux_DistMainRailway = NULL;
	mpProbabilityAll = NULL;

	mpIO_UrbanChange = NULL;
	mpIn_DistCapitanCity = NULL;
	mpIn_DistPrefectureCity = NULL;
	mpIn_DistCountyTown = NULL;
	mpIn_DistMainRoad = NULL;
	mpIn_DistMainRailway = NULL;

	mpProbFor9 = NULL;

	m_ndfInfo = -1;
	mbIsOpProbFor9 = false;
	mbIsLRTraining = false;
	mbIsNNTraining = false;
	m_lpts.clear();

	mnClassifiMode = 0;
}

UrbanChangeProbabilityAnalysis_V1::~UrbanChangeProbabilityAnalysis_V1(void)
{
	close();
}

void UrbanChangeProbabilityAnalysis_V1::close()
{
	if (mpIO_UrbanChange != NULL)
	{
		delete mpIO_UrbanChange;
		mpIO_UrbanChange = NULL;
	}

	if (mpIn_DistCapitanCity != NULL)
	{
		delete mpIn_DistCapitanCity;
		mpIn_DistCapitanCity = NULL;
	}

	if (mpIn_DistPrefectureCity != NULL)
	{
		delete mpIn_DistPrefectureCity;
		mpIn_DistPrefectureCity = NULL;
	}

	if (mpIn_DistCountyTown != NULL)
	{
		delete mpIn_DistCountyTown;
		mpIn_DistCountyTown = NULL;
	}


	if (mpIn_DistMainRoad != NULL)
	{
		delete mpIn_DistMainRoad;
		mpIn_DistMainRoad = NULL;
	}

	if (mpIn_DistMainRailway != NULL)
	{
		delete mpIn_DistMainRailway;
		mpIn_DistMainRailway = NULL;
	}

	if (mpProbFor9 != NULL)
	{
		mpProbFor9->Close();
		//delete mpProbFor9;
		mpProbFor9 = NULL;
	}

	if (mpUrbanChange != NULL)
	{
		mpUrbanChange->Close();
		//delete mpUrbanChange;
		mpUrbanChange = NULL;
	}

	if (mpAux_DistCapitanCity != NULL)
	{
		mpAux_DistCapitanCity->Close();
		//delete mpAux_DistCapitanCity;
		mpAux_DistCapitanCity = NULL;
	}

	if (mpAux_DistPrefectureCity != NULL)
	{
		mpAux_DistPrefectureCity->Close();
		//delete mpAux_DistPrefectureCity;
		mpAux_DistPrefectureCity = NULL;
	}

	if (mpAux_DistCountyTown != NULL)
	{
		mpAux_DistCountyTown->Close();
		//delete mpAux_DistCountyTown;
		mpAux_DistCountyTown = NULL;
	}

	if (mpAux_DistMainRoad != NULL)
	{
		mpAux_DistMainRoad->Close();
		//delete mpAux_DistMainRoad;
		mpAux_DistMainRoad = NULL;
	}

	if (mpAux_DistMainRailway != NULL)
	{
		mpAux_DistMainRailway->Close();
		//delete mpAux_DistMainRailway;
		mpAux_DistMainRailway = NULL;
	}

	if (mpProbabilityAll != NULL)
	{
		mpProbabilityAll->Close();
		//delete mpProbabilityAll;
		mpProbabilityAll = NULL;
	}

	
	
	m_lpts.clear();
	mbIsOpProbFor9 = false;
	mbIsLRTraining = false;
	mbIsNNTraining = false;
	mnClassifiMode = 0;

	_ilog("Finished Calculating Urban Change Probability.");
}

bool UrbanChangeProbabilityAnalysis_V1::run()
{
	m_ndfInfo = -1;
	int bResult = 0;

	//判断分类器类型
	CXml2Document doc(XML_CONFIG);
	if (0 != doc.GetValue(CLASSIFIER_MODE, &mnClassifiMode))
	{
		_ilog("Load CLASSIFIER_MODE Failed!");
		return false;
	}
	_ilog(QString("Classifier Mode = %1").arg(mnClassifiMode));

	//载入城市变换文件
	bResult = loadUrbanChangeData();
	if (bResult!=0)
	{
		_ilog(QString("Load Urban Change File Error. CODE = %1").arg(bResult));
		return false;
	}

	//载入辅助数据文件
	bResult = loadAuxiliaryData();
	if (bResult!=0)
	{
		_ilog(QString("Load Auxiliary Data Files Error. CODE = %1").arg(bResult));
		return false;
	}

	//制作随机数据文件
	bResult = makeRandomPointFiles();
	if (bResult!=0)
	{
		_ilog(QString("Make Random Points File Error. CODE = %1").arg(bResult));
		return false;
	}

	if (mnClassifiMode == 0)
	{
		//随机森林训练
		bResult = trainRandomForestClassifier();
		if (bResult!=0)
		{
			_ilog(QString("Train Random Forest Classifier Error. CODE = %1").arg(bResult));
			return false;
		}

	}
	else if (mnClassifiMode == 1)
	{
		//逻辑回归模型训练
		bResult = trainLRClassifier();
		if (bResult!=0)
		{
			_ilog(QString("Train Logic Regression Classifier Error. CODE = %1").arg(bResult));
			return false;
		}

	}
	else if (mnClassifiMode == 2)
	{
		//逻辑回归模型训练
		bResult = trainNNClassifer();
		if (bResult!=0)
		{
			_ilog(QString("Train Neural Network Classifier Error. CODE = %1").arg(bResult));
			return false;
		}
	}
	else
	{
		_ilog(QString("Unknown Classifier Training Method = %1.").arg(mnClassifiMode));
		return false;
	}

	//计算全图概率
	bResult = CalWholeImageDevelpedProbability();
	if (bResult!=0)
	{
		_ilog(QString("Calculate Whole Image Developed Probability Error. CODE = %1").arg(bResult));
		return false;
	}

	


	return true;
}

int UrbanChangeProbabilityAnalysis_V1::loadUrbanChangeData()
{
	CXml2Document doc(XML_CONFIG);
	char sfilename[2048];
	if (0 != doc.GetValue(NODE_OUTPUT_URBAN_CHANGE_FILENAME, sfilename))
	{
		_ilog("Load NODE_OUTPUT_URBAN_CHANGE_FILENAME Failed!");
		return -1;
	}

	str_connect_begin(SOPFILEPATH, sfilename);

// 	char soutputfilename[2048];
// 	if (0 != doc.GetValue(NODE_OUTPUT_PROBABILTY_DEVELOPED_FILE_PATH, soutputfilename))
// 	{
// 		_ilog("Load NODE_OUTPUT_PROBABILTY_DEVELOPED_FILE_PATH Failed!");
// 		return -1;
// 	}

	if (mpUrbanChange!=NULL)
		delete mpUrbanChange;

	mpUrbanChange = new CGDALFileManager;
	if (!mpUrbanChange->LoadFrom(sfilename))
	{
		_ilog("Load Urban Change Data Error!");
		return -2;
	}

	_ilog("Load Urban Change Data Successed.");

	//设置输入UrbanChange文件
	DIMS _dims;
	mpUrbanChange->m_header.InitDIMS(&_dims);
	mpIO_UrbanChange = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
	if (!mpIO_UrbanChange->Set_MgrIn(mpUrbanChange, _dims))
	{
		_ilog("Set Urban Change IO Error!");
		return -3;
	}
	_ilog("Set Urban Change IO Successed!");

	//设置输出文件
// 	mpProbabilityAll = new CGDALFileManager;
// 	mpProbabilityAll->m_header = mpUrbanChange->m_header;
// 	mpProbabilityAll->m_header.m_nBands = 1;
// 	mpProbabilityAll->m_header.m_nDataType = DistAuxiDTCode;		//float类型
// 
// 	if (!mpIO_UrbanChange->Set_MgrOut(mpProbabilityAll, soutputfilename))
// 	{
// 		_ilog("Set Probability Data File IO Error!");
// 		return -3;
// 	}
// 	_ilog("Set Probability Data File IO Successed!");
	

	return 0;
}

int UrbanChangeProbabilityAnalysis_V1::loadAuxiliaryData()
{
	int bResult;

	bResult = loadAuxiliaryData(mpAux_DistCapitanCity, mpIn_DistCapitanCity, NODE_DISTANCE_CAPITAN_CITY, "Capitan City Distance", &msbAux_DistCapitanCity);
	if (bResult != 0)
		return bResult;

	bResult = loadAuxiliaryData(mpAux_DistPrefectureCity, mpIn_DistPrefectureCity, NODE_DISTANCE_PREFECTURE_CITY, "Prefecture City Distance", &msbAux_DistPrefectureCity);
	if (bResult != 0)
		return bResult;

	bResult = loadAuxiliaryData(mpAux_DistCountyTown, mpIn_DistCountyTown, NODE_DISTANCE_COUNTY_TOWN, "County Town Distance", &msbAux_DistCountyTown);
	if (bResult != 0)
		return bResult;

	bResult = loadAuxiliaryData(mpAux_DistMainRoad, mpIn_DistMainRoad, NODE_DISTANCE_MAIN_ROAD, "Main Road Distance", &msbAux_DistMainRoad);
	if (bResult != 0)
		return bResult;

	bResult = loadAuxiliaryData(mpAux_DistMainRailway, mpIn_DistMainRailway, NODE_DISTANCE_MAIN_RAILWAY, "Main Railway Distance", &msbAux_DistMainRailway);
	if (bResult != 0)
		return bResult;

	return bResult;
}

int UrbanChangeProbabilityAnalysis_V1::loadAuxiliaryData(CGDALFileManager* &pmgr, HAPBEGBase* &pIO, char* node_name, char* sComment, StaticBag_V1* sbVal )
{
	CXml2Document doc(XML_CONFIG);
	char sfilename[2048];

	QString nodename = QString(node_name)+"/Filepath";
	//载入省会距离文件
	if (0 != doc.GetValue(nodename.toStdString().c_str(), sfilename))
	{
		_ilog(QString("Load %1 Failed!").arg(node_name));
		return -1;
	}

	str_connect_begin(SAUXFILEPATH, sfilename);

	//载入统计文件
	nodename = QString(node_name)+"/MinimumValue";
	if (0 != doc.GetValue(nodename.toStdString().c_str(), &(sbVal->minVal)))
	{
		_ilog("Load Statical Value Error!");
		return -301;
	}

	nodename = QString(node_name)+"/MaximumValue";
	if (0 != doc.GetValue(nodename.toStdString().c_str(), &(sbVal->maxVal)))
	{
		_ilog("Load Statical Value Error!");
		return -302;
	}

	nodename = QString(node_name)+"/MeanValue";
	if (0 != doc.GetValue(nodename.toStdString().c_str(), &(sbVal->meanVal)))
	{
		_ilog("Load Statical Value Error!");
		return -303;
	}

	//载入图像
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
	_ilog(QString("%1 Data Statics: MinVal = %2, MaxVal = %3, MeanVal = %4.").arg(sComment).arg(sbVal->minVal).arg(sbVal->maxVal).arg(sbVal->meanVal));

	//设置IO
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

int UrbanChangeProbabilityAnalysis_V1::makeRandomPointFiles()
{
	//读取随机点存储位置
	CXml2Document doc(XML_CONFIG);
	char sfilename[2048];
	if (0 != doc.GetValue(NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH, sfilename))
	{
		_ilog("Load NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH Error!");
		return -1;
	}

	str_connect_begin(SOPFILEPATH, sfilename);

	//读取随机坐标数据
	long nPointsCount;
	if (0 != doc.GetValue(NODE_RANDOM_POINTS_NUM, &nPointsCount))
	{
		_ilog("Load NODE_RANDOM_POINTS_NUM Error!");
		return -2;
	}


	//获取图像宽高
	int iSamples = mpUrbanChange->m_header.m_nSamples;
	int iLines = mpUrbanChange->m_header.m_nLines;

	//获取每行需要取的点个数
	int iPointsPerLine = int(nPointsCount/iLines+1) * RANDOM_POINTS_PARAM;

	//产生随机点坐标
	_ilog("Producting Random Points...");
	m_lpts.clear();
	int i=0, j=0, k=0;
	srand(QTime::currentTime().msec());

	int _nAvailPixelCount = 0;
	int _nToCityPixelCount = 0;
	QList<int> _vAvailIndex;
	QList<int> _vToCityIndex;
	LandCoverDT lcval;
	for (i=0; i<iSamples; i++)
	{
		if ((i+1)%10==0)
			_ilog(QString("Selecting Random Points. Rate: %1 / %2").arg(i+1).arg(iSamples));
		
		//扫描全图统计每一行有效数值个数，并将j写入其中
		_nToCityPixelCount = 0;
		_nAvailPixelCount = 0;
		_vAvailIndex.clear();
		_vToCityIndex.clear();

		for (j=0; j<iLines; j++)
		{
			lcval = *(LandCoverDT*)mpIO_UrbanChange->Read(i, j, 0);
			
			if (lcval == CODE_NOCITY_TO_CITY || lcval == CODE_PROTECTIVE_AREA_TO_CITY)
			{
				_nToCityPixelCount += 1;
				_vToCityIndex.append(j);
			}
			else if (lcval != CODE_UNKOWN_CHANGE)
			{
				_nAvailPixelCount += 1;
				_vAvailIndex.append(j);
			}
		}

		//判断每一行取多少值，取非城市数值
		int nOtherCount = (iPointsPerLine/2>_nAvailPixelCount)?_nAvailPixelCount:iPointsPerLine/2;
		//从中随机选
		for (k=0; k<nOtherCount; k++)
		{
			int _rnIdx = (double)(_nAvailPixelCount-1)*rand()/(double)RAND_MAX;
			FeaturePoint_V1 _pt;
			_pt.x = i;
			_pt.y = _vAvailIndex[_rnIdx];
			m_lpts.append(_pt);
		}

		//取城市数值
		int nToCityCount = (iPointsPerLine/2>_nToCityPixelCount)?_nToCityPixelCount:iPointsPerLine/2;
		//从中随机选
		for (k=0; k<nToCityCount; k++)
		{
			int _rnIdx = (double)(_nToCityPixelCount-1)*rand()/(double)RAND_MAX;
			FeaturePoint_V1 _pt;
			_pt.x = i;
			_pt.y = _vToCityIndex[_rnIdx];
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
	
	//将随机点坐标从小到大排序
	qSort(m_lpts.begin(), m_lpts.end());

	//处理随机点坐标
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

		if(!calFeaturePixelValue(m_lpts[i].prob_DistCapitanCity, m_lpts[i].lat, m_lpts[i].lon, mpAux_DistCapitanCity, mpIn_DistCapitanCity, &msbAux_DistCapitanCity))
		{
			_ilog(QString("Point (%1, %2) Capitan City Distance Calculate Wrong Value.").arg(m_lpts[i].lat, 0, 'f', 4).arg(m_lpts[i].lon, 0, 'f', 4));
			continue;
		}

		if(!calFeaturePixelValue(m_lpts[i].prob_DistPrefectureCity, m_lpts[i].lat, m_lpts[i].lon, mpAux_DistPrefectureCity, mpIn_DistPrefectureCity, &msbAux_DistPrefectureCity))
		{
			_ilog(QString("Point (%1, %2) Prefecture City Distance Calculate Wrong Value.").arg(m_lpts[i].lat, 0, 'f', 4).arg(m_lpts[i].lon, 0, 'f', 4));
			continue;
		}

		if(!calFeaturePixelValue(m_lpts[i].prob_DistCountyTown, m_lpts[i].lat, m_lpts[i].lon, mpAux_DistCountyTown, mpIn_DistCountyTown, &msbAux_DistCountyTown))
		{
			_ilog(QString("Point (%1, %2) County Town Distance Calculate Wrong Value.").arg(m_lpts[i].lat, 0, 'f', 4).arg(m_lpts[i].lon, 0, 'f', 4));
			continue;
		}

		if(!calFeaturePixelValue(m_lpts[i].prob_DistMainRoad, m_lpts[i].lat, m_lpts[i].lon, mpAux_DistMainRoad, mpIn_DistMainRoad, &msbAux_DistMainRoad))
		{
			_ilog(QString("Point (%1, %2) Main Road Distance Calculate Wrong Value.").arg(m_lpts[i].lat, 0, 'f', 4).arg(m_lpts[i].lon, 0, 'f', 4));
			continue;
		}

		if(!calFeaturePixelValue(m_lpts[i].prob_DistMainRailway, m_lpts[i].lat, m_lpts[i].lon, mpAux_DistMainRailway, mpIn_DistMainRailway, &msbAux_DistMainRailway))
		{
			_ilog(QString("Point (%1, %2) Main Railway Distance Calculate Wrong Value.").arg(m_lpts[i].lat, 0, 'f', 4).arg(m_lpts[i].lon, 0, 'f', 4));
			continue;
		}

	}

	//清空无用坐标点
	QList<FeaturePoint_V1>::iterator _it;
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


	//将随机坐标写入文件
	if (!writeRandomPointsToFile(sfilename))
		return -5;	

	//清空像素List，释放内存
	m_lpts.clear();

	_ilog("Write Random Points To File Successed.");
	return 0;
}


bool UrbanChangeProbabilityAnalysis_V1::calFeaturePixelValue(DistAuxiDT& _prob, double _lat, double _lon, CGDALFileManager* pMgr, HAPBEGBase* pIO, StaticBag_V1* pSb )
{
	double _x, _y;
// 	_World2Pixel(_lat, _lon, &_x, &_y, pMgr->m_header.m_MapInfo->m_adfGeotransform,\
// 		pMgr->m_header.m_MapInfo->GetSpatialReferenceWKT());

	pMgr->world2Pixel(_lat, _lon, &_x, &_y);
	if (_x < 0 || _y<0 || _x>=pMgr->m_header.m_nSamples || _y>=pMgr->m_header.m_nLines)
	{
		_prob = 0;
		return false;
	}
	_prob = *(DistAuxiDT*)pIO->Read(int(_x+0.5), int(_y+0.5), 0);
	_prob = pSb->calXPower(_prob);

	return true;
}

// bool UrbanChangeProbabilityAnalysis::Src2DstCoordTrans( double src_x, double src_y, double* dst_x, double* dst_y, CGDALFileManager* pSrcImg, CGDALFileManager* pDstImg )
// {
//  	double lat, lon;	
//  	_Pixel2World(&lat, &lon, src_x, src_y, pSrcImg->m_header.m_MapInfo->m_adfGeotransform, pSrcImg->GetpoDataset()->GetProjectionRef()/*pSrcImg->m_header.m_MapInfo->GetSpatialReferenceWKT()*/);
//  	_World2Pixel(lat, lon, dst_x, dst_y, pDstImg->m_header.m_MapInfo->m_adfGeotransform, pDstImg->GetpoDataset()->GetProjectionRef());
// 
// 	if (dst_x<0 || dst_y<0 || *dst_x>=pDstImg->m_header.m_nSamples || *dst_y>=pDstImg->m_header.m_nLines)
// 		return false;
// 	else
// 		return true;
// }


bool UrbanChangeProbabilityAnalysis_V1::writeRandomPointsToFile( char* sfilename )
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

	int _nFeatureCount = NUM_AUXILIARY_DATA;	//特征数目
	int _nClassCount = NUM_CODE_URBAN_CHANGE;	//0-9
	int* _pFeatureStatic = new int[_nClassCount];
	memset(_pFeatureStatic, 0, _nClassCount*sizeof(int));
	//写入数据总行数和特征数 m_lpts.size() * 5, 如果添加了辅助数据此处要改
	_smsg = QString("%1, %2, %3").arg(m_lpts.size()).arg(_nFeatureCount).arg(_nClassCount);
	_in<<_smsg;
	_in<<endl;

	//写入头
	_smsg = "Logitude, Latitude, DistToCapitanCity, DistToPrefectureCity, DistToCountyTown, DistToMainRoad, DistToMainRailway, ClassiID";
	_in<<_smsg;
	_in<<endl;


	foreach(FeaturePoint_V1 _pt, m_lpts)
	{
		_smsg = QString("%1, %2, %3, %4, %5, %6, %7, %8")\
				//.arg(_pt.x/*, 0, 'f', 6*/).arg(_pt.y/*, 0, 'f', 6*/)
				.arg(_pt.lon, 0, 'f', 6).arg(_pt.lat, 0, 'f', 6)\
				.arg(_pt.prob_DistCapitanCity, 0, 'f', 6)\
				.arg(_pt.prob_DistPrefectureCity, 0, 'f', 6)\
				.arg(_pt.prob_DistCountyTown, 0, 'f', 6)\
				.arg(_pt.prob_DistMainRoad, 0, 'f', 6)\
				.arg(_pt.prob_DistMainRailway, 0, 'f', 6)\
				.arg(_pt.class_code);
		_in<<_smsg;	
		_in<<endl;

		//统计每类的类别数目
		_pFeatureStatic[_pt.class_code] += 1;
	}

	//输出类别统计，在最后
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

int UrbanChangeProbabilityAnalysis_V1::trainNNClassifer()
{
	_ilog("Training Neural Network Classifier...");
	mbIsNNTraining = false;

	//获取输入随机点文件名
	CXml2Document doc(XML_CONFIG);
	char sPointsfilename[2048];
	if (0 != doc.GetValue(NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH, sPointsfilename))
	{
		_ilog("Load NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH Failed!");
		return -1;
	}
	str_connect_begin(SOPFILEPATH, sPointsfilename);
	_ilog(QString("Random Points File Name = %1").arg(sPointsfilename));

	//获取隐藏层数
	int nHiddenLevel= 5;
	if (0 != doc.GetValue(NN_PARAMETER_HIDDEN_NUM, &nHiddenLevel))
	{
		_ilog("Load NN_PARAMETER_HIDDEN_NUM Failed!");
		return -1;
	}
	_ilog(QString("NN Hidden Level = %1").arg(nHiddenLevel));

	//获取训练次数
	int nRestartNum = 10;
	if (0 != doc.GetValue(NN_PARAMETER_RESTART_NUM, &nRestartNum))
	{
		_ilog("Load NN_PARAMETER_RESTART_NUM Failed!");
		return -1;
	}
	_ilog(QString("NN Restart Num = %1").arg(nRestartNum));

	//获取输出报告名
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
	str_connect_begin(SOPFILEPATH, sPointsfilename);

	QTextStream out(&_file);
	//解析文件，获取点的个数、参数个数和分类类别
	QString _smsg = out.readLine();	//第一行为点数和参数个数
	QStringList _slist = _smsg.split(",", QString::SkipEmptyParts);
	int nPointCount = _slist[0].toInt();
	int nParaCount = _slist[1].toInt();
	int nClassCount = _slist[2].toInt();
	_ilog(QString("Random Points: Points Number = %1, Params Number = %2, Classes Count = %3").arg(nPointCount).arg(nParaCount).arg(nClassCount));

	out.readLine();	//第二行是描述
	//构建数组
	real_2d_array points_array;
	points_array.setlength(nPointCount, nParaCount+1);

	int i, j;
	for (i=0; i<points_array.rows(); i++)
	{
		//读取接下来每一行
		_smsg = out.readLine();	//第一行为点数和参数个数
		_slist = _smsg.split(",", QString::SkipEmptyParts);

		for (j=0; j<points_array.cols()-1; j++)
		{
			points_array[i][j]=_slist[2+j].toDouble();	//前2列是经纬度
		}

		//每一行最后一个数目是属于哪个类别，最后一列是类别
		points_array[i][points_array.cols()-1]=_slist[2+nParaCount].toInt();	
	}

	_file.close();

	clock_t _start = clock();

	//训练分类器
	_ilog("Training Neural Network Classifier...");
	mlpcreatetrainercls(nParaCount, nClassCount, m_NNTrainer);
	mlpcreatec1(nParaCount, nHiddenLevel, nClassCount, m_NNNetwork);
	mlpsetdataset(m_NNTrainer, points_array, nPointCount);
	mlptrainnetwork(m_NNTrainer, m_NNNetwork, nRestartNum, m_NNReport);
	_ilog(QString("Neural Network Classifier Trained Average Error = %1").arg(m_NNReport.avgerror));

	clock_t _end = clock();
	double _seconds = double(_end-_start)/(double)CLOCKS_PER_SEC;
	_ilog(QString("Neural Network Training Time = %1 seconds.").arg(_seconds, 0, 'f', 6));

	//输出训练精度文件
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

int UrbanChangeProbabilityAnalysis_V1::trainRandomForestClassifier()
{
	_ilog("Training Random Forest Classifier...");
	m_ndfInfo = -1;
	
	//获取输入随机点文件名
	CXml2Document doc(XML_CONFIG);
	char sPointsfilename[2048];
	if (0 != doc.GetValue(NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH, sPointsfilename))
	{
		_ilog("Load NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH Failed!");
		return -1;
	}
	str_connect_begin(SOPFILEPATH, sPointsfilename);
	_ilog(QString("Random Points File Name = %1").arg(sPointsfilename));

	//获取随机森林数目
	int _nDecisionForestTrees = 0;
	if (0 != doc.GetValue(DF_PARAMETER_TREES_NUM, &_nDecisionForestTrees))
	{
		_ilog("Load DF_PARAMETER_TREES_NUM Failed!");
		return -1;
	}
	_ilog(QString("Random Decision Forest Trees Num = %2").arg(_nDecisionForestTrees));

	//获取percent of a training set used to build individual trees.
	double _nDecisionForestSetPersent = 0;
	if (0 != doc.GetValue(DF_PARAMETER_SET_PERSENT, &_nDecisionForestSetPersent))
	{
		_ilog("Load DF_PARAMETER_SET_PERSENT Failed!");
		return -1;
	}
	_ilog(QString("Percent Of A Training Set Used To Build Individual Trees = %1").arg(_nDecisionForestSetPersent, 0, 'f', 6));

	//获取number of variables used when choosing best split
	int _dDecisionForestNRndVars = 0;
	if (0 != doc.GetValue(DF_PARAMETER_BEST_SPLIT, &_dDecisionForestNRndVars))
	{
		_ilog("Load DF_PARAMETER_BEST_SPLIT Failed!");
		return -1;
	}
	_ilog(QString("Number Of Variables Used When Choosing Best Split = %1").arg(_dDecisionForestNRndVars));

	//获取Decision Forest Output Result Data Filepath
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
	//解析文件，获取点的个数、参数个数和分类类别
	QString _smsg = out.readLine();	//第一行为点数和参数个数
	QStringList _slist = _smsg.split(",", QString::SkipEmptyParts);
	int nPointCount = _slist[0].toInt();
	int nParaCount = _slist[1].toInt();
	int nClassCount = _slist[2].toInt();
	_ilog(QString("Random Points: Points Number = %1, Params Number = %2, Classes Count = %3").arg(nPointCount).arg(nParaCount).arg(nClassCount));

	out.readLine();	//第二行是描述
	//构建数组
	real_2d_array points_array;
	points_array.setlength(nPointCount, nParaCount+1);

	int i, j;
	for (i=0; i<points_array.rows(); i++)
	{
		//读取接下来每一行
		_smsg = out.readLine();	//第一行为点数和参数个数
		_slist = _smsg.split(",", QString::SkipEmptyParts);

		for (j=0; j<points_array.cols()-1; j++)
		{
			points_array[i][j]=_slist[2+j].toDouble();	//前2列是经纬度
		}

		//每一行最后一个数目是属于哪个类别，最后一列是类别
		points_array[i][points_array.cols()-1]=_slist[2+nParaCount].toInt();	
	}

	_file.close();


	clock_t _start = clock();
	//开始训练
	_ilog("Training Random Forest Classifier...");
	dfbuildrandomdecisionforest(points_array, nPointCount, nParaCount, nClassCount, _nDecisionForestTrees,\
					_nDecisionForestSetPersent, m_ndfInfo, m_dfClassifier, m_dfReport);
	_ilog(QString("Decision Forest Traning Result Code = %1").arg(m_ndfInfo));

	clock_t _end = clock();
	double _seconds = double(_end-_start)/(double)CLOCKS_PER_SEC;
	_ilog(QString("Random Training Time = %1 seconds.").arg(_seconds, 0, 'f', 6));

	//将训练结果写入文件
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

int UrbanChangeProbabilityAnalysis_V1::trainLRClassifier()
{
	_ilog("Training Logic Regression Classifier...");
	mbIsLRTraining = false;

	CXml2Document doc(XML_CONFIG);

	//获取输入随机点文件名
	char sPointsfilename[2048];
	if (0 != doc.GetValue(NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH, sPointsfilename))
	{
		_ilog("Load NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH Failed!");
		return -3;
	}
	str_connect_begin(SOPFILEPATH, sPointsfilename);
	_ilog(QString("Random Points File Name = %1").arg(sPointsfilename));

	//载入模型文件
	char model_file[2048];
	if (0 != doc.GetValue(LR_MODEL_FILEPATH, model_file))
	{
		_ilog("Load LR_MODEL_FILEPATH Failed!");
		return -2;
	}
	str_connect_begin(SOPFILEPATH, model_file);
	_ilog(QString("Saving LR Model File Path = %1").arg(model_file));


	//获取最大迭代次数
	int max_loop = 100;
	if (0 != doc.GetValue(LR_PARAMETER_MAX_LOOP, &max_loop))
	{
		_ilog("Load LR_PARAMETER_MAX_LOOP Failed!");
		return -2;
	}
	_ilog(QString("Maximal Iteration Loop Num = %1").arg(max_loop));

	//获取最小错误率
	double loss_thrd = 1e-3;
	if (0 != doc.GetValue(LR_PARAMETER_LOSS_THREAD, &loss_thrd))
	{
		_ilog("Load LR_PARAMETER_LOSS_THREAD Failed!");
		return -2;
	}
	_ilog(QString("Loss Thread = %1").arg(loss_thrd, 0, 'f', 6));

	//获取学习比率
	float learn_rate = 1.0;
	if (0 != doc.GetValue(LR_PARAMETER_LEARN_RATE, &learn_rate))
	{
		_ilog("Load LR_PARAMETER_LOSS_THREAD Failed!");
		return -2;
	}
	_ilog(QString("Learn Rate = %1").arg(learn_rate, 0, 'f', 6));

	//获取函数阈值
	float lambda = 0.0;
	if (0 != doc.GetValue(LR_PARAMETER_LAMBDA, &lambda))
	{
		_ilog("Load LR_PARAMETER_LAMBDA Failed!");
		return -2;
	}
	_ilog(QString("Lambda Of Gaussian Prior Regularization = %1").arg(lambda, 0, 'f', 6));

	//平均权重
	int avgweight= 0;
	if (0 != doc.GetValue(LR_PARAMETER_AVG_WEIGHT, &avgweight))
	{
		_ilog("Load LR_PARAMETER_AVG_WEIGHT Failed!");
		return -2;
	}
	_ilog(QString("Average Loop Weight = %1").arg(avgweight));

	//是否升级文件
	int _update = 0;
	if (0 != doc.GetValue(LR_PARAMETER_UPDATE_MODEL, &_update))
	{
		_ilog("Load LR_PARAMETER_UPDATE_MODEL Failed!");
		return -2;
	}
	_ilog(QString("Update LR Model File = %1").arg(_update));

	//获取通过精度
	float passAccu = 0.5;
	if (0 != doc.GetValue(LR_PARAMETER_PASS_ACCU, &passAccu))
	{
		_ilog("Load LR_PARAMETER_PASS_ACCU Failed!");
		return -2;
	}
	_ilog(QString("Pass Training Accuracy = %1").arg(passAccu, 0, 'f', 6));

	//开始训练
	if (!m_LRClassifier.load_ca_points_file(sPointsfilename))
	{
		_ilog("LR Load CA Random Points Error!");
		return -4;
	}

	clock_t _start = clock();

	//载入上一次训练文件
	if (_update) 
	{
		//如果载入上一次结果失败，则新建一个
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

	float _lracc = m_LRClassifier.train_sgd(max_loop, loss_thrd, learn_rate, lambda, avgweight);		//载入训练参数
	m_LRClassifier.save_model(model_file);		//保存模型

	//cout<<"Last Error = "<<1-_lracc<<endl;	
	_ilog(QString("LR Training Last Avg Error = %1").arg(1-_lracc, 0, 'f', 6));


	clock_t _end = clock();
	double _seconds = double(_end-_start)/(double)CLOCKS_PER_SEC;
	_ilog(QString("Logic Regression Time = %1 seconds.").arg(_seconds, 0, 'f', 6));

	//如果反演精度小于通过精度则返回0
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

int UrbanChangeProbabilityAnalysis_V1::CalWholeImageDevelpedProbability()
{
// 	if (m_ndfInfo != 1)
// 	{
// 		_ilog("Decision Forest Classifier Is Not Trained!");
// 		return -1;
// 	}

	//设置输出图像
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
	mpProbabilityAll->m_header.m_nDataType = DistAuxiDTCode;		//float类型

	if (!mpIO_UrbanChange->Set_MgrOut(mpProbabilityAll, soutputfilename))
	{
		_ilog("Set Probability Data File IO Error!");
		return -3;
	}
	_ilog("Set Probability Data File IO Successed!");

	//判断是否要生成全图概率图像
	int _nVal = 0;
	if (0 != doc.GetValue(NODE_IS_OUTPUT_9BANDS_PROB_IMG, &_nVal))
	{
		_ilog("Load NODE_IS_OUTPUT_9BANDS_PROB_IMG Failed!");
		_nVal = 0;
	}

	if (_nVal<=0)	mbIsOpProbFor9 = false;
	else			mbIsOpProbFor9 = true;
	
	int _nFlag = 0;	//为1时准备成功
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


	//获取生成的概率图类型
	int _nProbabiltyType = 1;
	if (0 != doc.GetValue(NODE_PARAM_PROBABILITY_PARAM, &_nProbabiltyType))
	{
		_ilog("Load NODE_PARAM_PROBABILITY_PARAM Failed!");
		_nProbabiltyType = 1;
	}
	_ilog(QString("Output Probability Image Type = %1").arg(_nProbabiltyType));


	//检查IO是否存在
	if (mpIO_UrbanChange==NULL || mpIn_DistCapitanCity==NULL || mpIn_DistPrefectureCity==NULL \
		|| mpIn_DistCountyTown==NULL || mpIn_DistMainRoad==NULL || mpIn_DistMainRailway==NULL)
	{
		_ilog("Main or Auxiliary IO Maybe Not Exist!");
		return -2;
	}

	_ilog("Calculating Whole Image Developed Probability...");
	int iSamples, iLines, bands;
	iSamples = mpIO_UrbanChange->Get_MgrIn()->m_header.m_nSamples;
	iLines = mpIO_UrbanChange->Get_MgrIn()->m_header.m_nLines;
	bands = mpIO_UrbanChange->Get_MgrIn()->m_header.m_nBands;

	LandCoverDT lcval;
	ProbabilityDT outVal;
	DistAuxiDT distVal;

	int m, n, kk;

	//随机森林使用待判定文件
	real_1d_array _classiArr;
	_classiArr.setlength(NUM_AUXILIARY_DATA);	
	//分类概率分布
	real_1d_array _classRlt;

	//逻辑回归使用待判定文件
	vector<float> _lrClassiArr;
	vector<float> _lrClassiRlt;


	//获取辅助数据
	double dst_x, dst_y;

	for (m=0; m<iLines; m++)
	{
		if ((m+1)%5==0)
			_ilog(QString("Calculating Probability: Processing Rate: %1 / %2").arg(m+1).arg(iLines));

		for (n=0; n<iSamples; n++)
		{
			/********************************************************************
			//error: 存在微小的内存泄露！！！
			*********************************************************************/

			if (mnClassifiMode == 1)
			{
				_lrClassiArr.clear();
				_lrClassiRlt.clear();
			}				

			dst_x=n, dst_y=m;
			lcval = *(LandCoverDT*)mpIO_UrbanChange->Read(n, m, 0);

			//如果为lcval为无效数值
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
			
			
			//省会中心距离
			if(_Src2DstCoordTrans(n, m, dst_x, dst_y, mpUrbanChange, mpAux_DistCapitanCity))
			{
				distVal = *(DistAuxiDT*)mpIn_DistCapitanCity->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
				if (mnClassifiMode == 0 || mnClassifiMode == 2)
					_classiArr[0] = msbAux_DistCapitanCity.calXPower(distVal);
				else
					_lrClassiArr.push_back(msbAux_DistCapitanCity.calXPower(distVal));
			}
			else
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

			
			//地级市距离
			if(_Src2DstCoordTrans(n, m, dst_x, dst_y, mpUrbanChange, mpAux_DistPrefectureCity))
			{
				distVal = *(DistAuxiDT*)mpIn_DistPrefectureCity->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
				if (mnClassifiMode == 0 || mnClassifiMode == 2)
					_classiArr[1] = msbAux_DistPrefectureCity.calXPower(distVal);
				else
					_lrClassiArr.push_back(msbAux_DistPrefectureCity.calXPower(distVal));

			}
			else
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

			//县城中心距离
			if(_Src2DstCoordTrans(n, m, dst_x, dst_y, mpUrbanChange, mpAux_DistCountyTown))
			{
				distVal = *(DistAuxiDT*)mpIn_DistCountyTown->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
				if (mnClassifiMode == 0 || mnClassifiMode == 2)
					_classiArr[2] = msbAux_DistCountyTown.calXPower(distVal);
				else
					_lrClassiArr.push_back(msbAux_DistCountyTown.calXPower(distVal));
			}
			else
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

			//到主要道路距离
			if(_Src2DstCoordTrans(n, m, dst_x, dst_y, mpUrbanChange, mpAux_DistMainRoad))
			{
				distVal = *(DistAuxiDT*)mpIn_DistMainRoad->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
				if (mnClassifiMode == 0 || mnClassifiMode == 2)
					_classiArr[3] = msbAux_DistMainRoad.calXPower(distVal);
				else
					_lrClassiArr.push_back(msbAux_DistMainRoad.calXPower(distVal));
			}
			else
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

			//主要铁路距离
			if(_Src2DstCoordTrans(n, m, dst_x, dst_y, mpUrbanChange, mpAux_DistMainRailway))
			{
				distVal = *(DistAuxiDT*)mpIn_DistMainRailway->Read(int(dst_x+0.5), int(dst_y+0.5), 0);
				if (mnClassifiMode == 0 || mnClassifiMode == 2)
					_classiArr[4] = msbAux_DistMainRailway.calXPower(distVal);
				else
					_lrClassiArr.push_back(msbAux_DistMainRailway.calXPower(distVal));
			}
			else
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
		
			//分类
			if (mnClassifiMode == 0 || mnClassifiMode == 2)
			{
				if (mnClassifiMode == 0)
					dfprocess(m_dfClassifier, _classiArr, _classRlt);	//随机森林
				else
					mlpprocess(m_NNNetwork, _classiArr, _classRlt);		//神经网络
				
				//输出结果
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
			else
			{
				//逻辑回归分类
				m_LRClassifier.classify_data(_lrClassiArr, _lrClassiRlt);
				//输出结果
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

	mpIn_DistCapitanCity->Close_MgrIn();
	if (_nFlag==1 && mbIsOpProbFor9)
	{
		mpIn_DistCapitanCity->Close_MgrOut();
	}

	mpIn_DistPrefectureCity->Close_MgrIn();
	mpIn_DistCountyTown->Close_MgrIn();
	mpIn_DistMainRoad->Close_MgrIn();
	mpIn_DistMainRailway->Close_MgrIn();


	mpIO_UrbanChange->Close_MgrIn();
	mpIO_UrbanChange->Close_MgrOut();

	_nFlag = 0;


	_ilog("Calculated Whole Image Developed Probability Successed.");
	return 0;
}









