#include "UrbanChangeMaker.h"
#include "global_include.h"

#include "haplib.h"
#include "hapbegbase.h"
#include "HAPBEGThread.h"
#include "hapbegfactory.h"

#include <QtCore/QStringList>
using namespace std;


void UrbanChangeMaker::_ilog( char* sMessage, char* sInstance/*= "info" */ )
{
	log4cpp::Category& log = log4cpp::Category::getInstance(sInstance);
	log.info(sMessage);
	cout<<sMessage<<endl;
}

void UrbanChangeMaker::_ilog( QString sMessage, char* sInstance /*= "info" */ )
{
	log4cpp::Category& log = log4cpp::Category::getInstance(sInstance);
	log.info(sMessage.toStdString().c_str());
	//_ilog(sMessage.toStdString().c_str(), sInstance);
	cout<<sMessage.toStdString().c_str()<<endl;
}

UrbanChangeMaker::UrbanChangeMaker(void)
{
	_ilog("Starting Urban Change Analysising...");
	mpDstImg = mpSrcImg = NULL;
	mpUrbanChangeImg = NULL;
	mpSuitIdxImg = NULL;
	mpUrbanReClassiImg = NULL;
	mpUrbanReClassiImg_Validation = NULL;
	mlCoverCode.clear();

	//判断xml文件是否存在
	QFileInfo info(XML_CONFIG);
	if (info.exists())
		_ilog(QString("XML_CONFIG: %1 is exist!").arg(info.filePath()));
	else
		_ilog(QString("XML_CONFIG: %1 is exist!").arg(info.filePath()), "info");

// 	if (info.exists())
// 	{
// 		//运行
// 		run();
// 	}

}

UrbanChangeMaker::~UrbanChangeMaker(void)
{
	close();	
}

void UrbanChangeMaker::close()
{	
	if (mpSrcImg!=NULL)
	{
		mpSrcImg->Close();
		//delete mpSrcImg;
		mpSrcImg = NULL;
	}

	if (mpDstImg!=NULL)
	{
		mpDstImg->Close();
		//delete mpDstImg;
		mpDstImg = NULL;
	}

	if (mpUrbanChangeImg != NULL)
	{
		mpUrbanChangeImg->Close();
		//delete mpUrbanChangeImg;
		mpUrbanChangeImg = NULL;
	}

	if (mpSuitIdxImg != NULL)
	{
		mpSuitIdxImg->Close();
		//delete mpSuitIdxImg;
		mpSuitIdxImg = NULL;
	}

	if (mpUrbanReClassiImg != NULL)
	{
		mpUrbanReClassiImg->Close();
		//delete mpUrbanReClassiImg;
		mpUrbanChangeImg = NULL;
	}

	if (mpUrbanReClassiImg_Validation!=NULL)
	{
		mpUrbanReClassiImg_Validation->Close();
		//delete mpUrbanReClassiImg_Validation;
		mpUrbanReClassiImg_Validation = NULL;
	}

	_ilog("Finished Urban Change Analysis.");
}

bool UrbanChangeMaker::findCoverCode( long nCode, LandCoverCode& cCode )
{
	int i=0;
	foreach ( LandCoverCode _lcc, mlCoverCode )
	{
		if (_lcc.nCode == nCode)
		{
			cCode = mlCoverCode[i];
			return true;
		}
		i++;
	}

	return false;
}

bool UrbanChangeMaker::run()
{
	int bResult = 0;

	bResult = loadLandCoverCode();
	if (bResult<0)
	{
		_ilog(QString("UrbanChangeMaker::loadLandCoverCode error! Code: %1").arg(bResult));
		return false;
	}

	bResult = loadLandCoverData();
	if (bResult<0)
	{
		_ilog(QString("UrbanChangeMaker::loadLandCoverData error! Code: %1").arg(bResult));
		return false;
	}

	bResult = ProcessUrbanChangeImage();
	if (bResult<0)
	{
		_ilog(QString("UrbanChangeMaker::ProcessUrbanChangeImage error! Code: %1").arg(bResult));
		return false;
	}

	return true;
}

int UrbanChangeMaker::loadLandCoverCode()
{
	_ilog("Loading Land Cover Code File!");

	//载入xml文件，并将文件名写入路径中
	CXml2Document doc(XML_CONFIG);
	char _sfilename[1024];
	if( 0 != doc.GetValue(NODE_LANDCOVER_CODE, _sfilename))
	{
		_ilog("Load NODE_LANDCOVER_CODE error!");
		return -1;
	}
	_ilog("Load NODE_LANDCOVER_CODE Successed.");
	str_connect_begin(SLCFILEPATH, _sfilename);

	//开始解析文件
	QFile _file(_sfilename);
	if (!_file.open(QIODevice::ReadOnly))
	{
		_ilog(QString("Load %1 error!").arg(_sfilename));
		return -2;
	}

	QTextStream out(&_file);
	out.readLine();	//第一行省略
	while (!out.atEnd())
	{
		QString sline=out.readLine();
		QStringList slist = sline.split(',');
		if (slist.size()>=6)
		{
			LandCoverCode _lcc;
			_lcc.nCode = slist[0].toLong();
			_lcc.sCatalog2nd = slist[1];
			_lcc.nID = slist[2].toLong();
			_lcc.sCatalog1st = slist[3];
			_lcc.nReClassiID = slist[4].toLong();
			_lcc.sComment = slist[5];
			
			_ilog(sline);
			cout<<"ID = "<<_lcc.nID<<endl;
			cout<<"Code = "<<_lcc.nCode<<endl;
			cout<<"Reclassify ID = "<<_lcc.nReClassiID<<endl;
			cout<<"1st Catalog = "<<_lcc.sCatalog1st.toStdString().c_str()<<endl;
			cout<<"2nd Catalog = "<<_lcc.sCatalog2nd.toStdString().c_str()<<endl;
			cout<<"Comments = "<<_lcc.sComment.toStdString().c_str()<<endl;

			mlCoverCode.push_back(_lcc);			
		}
	}

	_file.close();

	if (mlCoverCode.size() != 0)
	{
		_ilog(QString("Load Land Cover Code File Successed. Number = %1").arg(mlCoverCode.size()));
	}

	return 0;
}

int UrbanChangeMaker::loadLandCoverData()
{
	_ilog("Loading Land Cover Data File!");

	CXml2Document doc(XML_CONFIG);
	//读取起始年和结束年
	int _startYear=0, _endYear=0;
	if( 0 != doc.GetValue(NODE_YEAR_OF_DATA_START, &_startYear))
	{
		_ilog("Load Start Year error!");
		return -1;
	}

	if( 0 != doc.GetValue(NODE_YEAR_OF_DATA_END, &_endYear))
	{
		_ilog("Load End Year error!");
		return -1;
	}

	//读取数据数目
	int _codeNum=0;
	doc.GetNode(NODE_LANDCOVER).GetAttribute("Filenum", &_codeNum);
	if (_codeNum <= 1)
	{
		_ilog("Land Cover Data FEWER than 2. Code = -1");
		return -1;
	}
	//读取数据文件名
	int _nYear = 0;
	char _sStartYearFilename[2048]="", _sEndYearFilename[2048]=""; 
	int _nflag = 0;
	for (int i=0; i<_codeNum; i++)
	{
		QString _nc = QString("%1/%2").arg(NODE_LANDCOVER).arg("File");
		CXml2Node _node = doc.GetNode(_nc.toStdString().c_str(), i);
		_node.GetValue("Year", &_nYear);
		
		//如果年份和要求相同
		if (_nYear == _startYear)
		{
			if (_node.GetValue("Filepath", _sStartYearFilename) == 0)
				_nflag+=1;
		}

		if (_nYear == _endYear)
		{
			if (_node.GetValue("Filepath", _sEndYearFilename) == 0)
				_nflag+=1;
		}
	}

	str_connect_begin(SLCFILEPATH, _sStartYearFilename);
	str_connect_begin(SLCFILEPATH, _sEndYearFilename);

	//判断是否读取文件成功
	_ilog(QString("Start Year LandCover File: %1").arg(QString(_sStartYearFilename)));
	_ilog(QString("End Year LandCover File: %1").arg(QString(_sEndYearFilename)));
	if (_nflag!=2)
	{
		_ilog("Miss Start or End Year File! . Code = -2");
		return -2;
	}

	//载入Src文件
	if (mpSrcImg!=NULL)
		delete mpSrcImg;
	
	mpSrcImg = new CGDALFileManager;
	if (!mpSrcImg->LoadFrom(_sStartYearFilename))
	{
		_ilog("Load Start File Error! . Code = -301");
		return -301;
	}
	_ilog("Load Start Year File Successed.");

	//载入Dst文件
	if (mpDstImg!=NULL)
		delete mpDstImg;

	mpDstImg = new CGDALFileManager;
	if (!mpDstImg->LoadFrom(_sEndYearFilename))
	{
		_ilog("Load End File Error! . Code = -302");
		return -302;
	}
	_ilog("Load End Year File Successed.");

	_ilog("Load Land Cover Data File Successed!");
	return 0;
}

int UrbanChangeMaker::ProcessUrbanChangeImage()
{
	_ilog("Processing Output Urban Change Image Data File...");

	//判断文件有无
	if (mpSrcImg == NULL || mpDstImg == NULL)
	{
		_ilog("Src Img or End Img is NULL.");
		return -1;
	}

	//判断有无投影和坐标
	if (mpSrcImg->m_header.m_MapInfo == NULL || mpDstImg->m_header.m_MapInfo == NULL)
	{
		_ilog("Src Img or End Img have no MAPINFO.");
		return -2;
	}

	//读取输出文件名
	CXml2Document doc(XML_CONFIG);
	char _sOutfilename[1024];
	if( 0 != doc.GetValue(NODE_OUTPUT_URBAN_CHANGE_FILENAME, _sOutfilename))
	{
		_ilog("Load NODE_OUTPUT_URBAN_CHANGE_FILENAME error!");
		return -4;
	}
	_ilog("Load NODE_OUTPUT_URBAN_CHANGE_FILENAME Successed.");

	str_connect_begin(SOPFILEPATH, _sOutfilename);

	//读取土地适宜性指数输出文件名
	char _sLandDIOutfilename[1024];
	if( 0 != doc.GetValue(NODE_OUTPUT_LAND_DEVELOP_SUITABLE_INDEX_FILENAME, _sLandDIOutfilename))
	{
		_ilog("Load NODE_OUTPUT_LAND_DEVELOP_SUITABLE_INDEX_FILENAME error!");
		return -4;
	}
	_ilog("Load NODE_OUTPUT_LAND_DEVELOP_SUITABLE_INDEX_FILENAME Successed.");

	str_connect_begin(SOPFILEPATH, _sLandDIOutfilename);

	//读取土地重分类指数输出文件名
	char _sReclassiOutfilename[1024];
	if( 0 != doc.GetValue(NODE_OUTOUT_URBAN_CHANGE_RECLASSIFI_FILEPATH, _sReclassiOutfilename))
	{
		_ilog("Load NODE_OUTOUT_URBAN_CHANGE_RECLASSIFI_FILEPATH error!");
		return -4;
	}
	_ilog("Load NODE_OUTOUT_URBAN_CHANGE_RECLASSIFI_FILEPATH Successed.");

	str_connect_begin(SOPFILEPATH, _sReclassiOutfilename);

	//读取验证土地重分类指数输出文件名
	char _sValidReclassiOutfilename[1024];
	if( 0 != doc.GetValue(NODE_OUTOUT_URBAN_CHANGE_RECLASSIFI_VALIDATION_FILEPATH, _sValidReclassiOutfilename))
	{
		_ilog("Load NODE_OUTOUT_URBAN_CHANGE_RECLASSIFI_VALIDATION_FILEPATH error!");
		return -4;
	}
	_ilog("Load NODE_OUTOUT_URBAN_CHANGE_RECLASSIFI_FILEPATH Successed.");

	str_connect_begin(SOPFILEPATH, _sValidReclassiOutfilename);

	//开辟输出文件空间
	if (mpUrbanChangeImg!=NULL)
		delete mpUrbanChangeImg;
	mpUrbanChangeImg = new CGDALFileManager;

	if (mpSuitIdxImg!=NULL)
		delete mpSuitIdxImg;
	mpSuitIdxImg = new CGDALFileManager;

	if (mpUrbanReClassiImg!=NULL)
		delete mpUrbanReClassiImg;
	mpUrbanReClassiImg = new CGDALFileManager;

	if (mpUrbanReClassiImg_Validation!=NULL)
		delete mpUrbanReClassiImg_Validation;
	mpUrbanReClassiImg_Validation = new CGDALFileManager;

	//设置输入输出IO
	HAPBEGBase* srcImgIO = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
	HAPBEGBase* dstImgIO = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
	HAPBEGBase* reclassOut = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
	HAPBEGBase* reclassValiOut = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
	DIMS src_dims, dst_dims;
	mpSrcImg->m_header.InitDIMS(&src_dims);
	mpDstImg->m_header.InitDIMS(&dst_dims);

	if (!srcImgIO->Set_MgrIn(mpSrcImg, src_dims))
	{
		_ilog("Src Img Input Init Error.");
		delete srcImgIO;
		srcImgIO = NULL;
		delete dstImgIO;
		dstImgIO = NULL;
		delete reclassOut;
		reclassOut = NULL;
		delete reclassValiOut;
		reclassValiOut = NULL; 

		delete mpUrbanChangeImg;
		mpUrbanChangeImg = NULL;
		delete mpSuitIdxImg;
		mpSuitIdxImg = NULL;
		delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
		delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
		return -301;
	}
	_ilog("Src Img Input Init Successed.");

	if (!dstImgIO->Set_MgrIn(mpDstImg, dst_dims))
	{
		_ilog("Dst Img Input Init Error.");
		delete srcImgIO;
		srcImgIO = NULL;
		delete dstImgIO;
		dstImgIO = NULL;
		delete reclassOut;
		reclassOut = NULL;
		delete reclassValiOut;
		reclassValiOut = NULL;

		delete mpUrbanChangeImg;
		mpUrbanChangeImg = NULL;
		delete mpSuitIdxImg;
		mpSuitIdxImg = NULL;
		delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
		delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
		return -302;
	}
	_ilog("Dst Img Input Init Successed.");

	mpUrbanChangeImg->m_header = mpSrcImg->m_header;
	mpUrbanChangeImg->m_header.m_nSamples = src_dims.GetWidth(); 
	mpUrbanChangeImg->m_header.m_nLines = src_dims.GetHeight();
	mpUrbanChangeImg->m_header.m_nBands = src_dims.GetBands();
	mpUrbanChangeImg->m_header.m_nDataType = LandCoverDTCode;

	if(!srcImgIO->Set_MgrOut(mpUrbanChangeImg, _sOutfilename))
	{
		_ilog("Urban Change Img Output Init Error.");
		delete srcImgIO;
		srcImgIO = NULL;
		delete dstImgIO;
		dstImgIO = NULL;
		delete reclassOut;
		reclassOut = NULL;
		delete reclassValiOut;
		reclassValiOut = NULL;

		delete mpUrbanChangeImg;
		mpUrbanChangeImg = NULL;
		delete mpSuitIdxImg;
		mpSuitIdxImg = NULL;
		delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
		delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
		return -303;
	}
	_ilog(QString("Urban Change Output Init Success."));

	mpSuitIdxImg->m_header = mpSrcImg->m_header;
	mpSuitIdxImg->m_header.m_nSamples = src_dims.GetWidth(); 
	mpSuitIdxImg->m_header.m_nLines = src_dims.GetHeight();
	mpSuitIdxImg->m_header.m_nBands = src_dims.GetBands();
	mpSuitIdxImg->m_header.m_nDataType = LandSuitabilityDTCode;

	if(!dstImgIO->Set_MgrOut(mpSuitIdxImg, _sLandDIOutfilename))
	{
		_ilog("Land Develop Suitable Indexes Output Init Error.");
		delete srcImgIO;
		srcImgIO = NULL;
		delete dstImgIO;
		dstImgIO = NULL;
		delete reclassOut;
		reclassOut = NULL;
		delete reclassValiOut;
		reclassValiOut = NULL;

		delete mpUrbanChangeImg;
		mpUrbanChangeImg = NULL;
		delete mpSuitIdxImg;
		mpSuitIdxImg = NULL;
		delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
		delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
		return -304;
	}
	_ilog(QString("Land Develop Suitable Indexes Output Init Success."));


	mpUrbanReClassiImg->m_header = mpSrcImg->m_header;
	mpUrbanReClassiImg->m_header.m_nSamples = src_dims.GetWidth(); 
	mpUrbanReClassiImg->m_header.m_nLines = src_dims.GetHeight();
	mpUrbanReClassiImg->m_header.m_nBands = src_dims.GetBands();
	mpUrbanReClassiImg->m_header.m_nDataType = UrbanReclassDTCode;

	if(!reclassOut->Set_MgrOut(mpUrbanReClassiImg, _sReclassiOutfilename))
	{
		_ilog("Urban Reclassification File Output Init Error.");
		delete srcImgIO;
		srcImgIO = NULL;
		delete dstImgIO;
		dstImgIO = NULL;
		delete reclassOut;
		reclassOut = NULL;
		delete reclassValiOut;
		reclassValiOut = NULL;

		delete mpUrbanChangeImg;
		mpUrbanChangeImg = NULL;
		delete mpSuitIdxImg;
		mpSuitIdxImg = NULL;
		delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
		delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
		return -305;
	}
	_ilog(QString("Urban Reclassification File Output Init Success."));


	mpUrbanReClassiImg_Validation->m_header = mpSrcImg->m_header;
	mpUrbanReClassiImg_Validation->m_header.m_nSamples = src_dims.GetWidth(); 
	mpUrbanReClassiImg_Validation->m_header.m_nLines = src_dims.GetHeight();
	mpUrbanReClassiImg_Validation->m_header.m_nBands = src_dims.GetBands();
	mpUrbanReClassiImg_Validation->m_header.m_nDataType = UrbanReclassDTCode;

	if(!reclassValiOut->Set_MgrOut(mpUrbanReClassiImg_Validation, _sValidReclassiOutfilename))
	{
		_ilog("Urban Validation Reclassification File Output Init Error.");
		delete srcImgIO;
		srcImgIO = NULL;
		delete dstImgIO;
		dstImgIO = NULL;
		delete reclassOut;
		reclassOut = NULL;
		delete reclassValiOut;
		reclassValiOut = NULL;

		delete mpUrbanChangeImg;
		mpUrbanChangeImg = NULL;
		delete mpSuitIdxImg;
		mpSuitIdxImg = NULL;
		delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
		delete mpUrbanReClassiImg;
		mpUrbanReClassiImg = NULL;
		return -306;
	}
	_ilog(QString("Urban Validation Reclassification File Output Init Success."));

	_ilog("Outputing Urban Change Image And Land Develop Suitable Indexes Image Data...");

	//主运行函数
	int nResult = -5;
	//导入数据类型
	int iDataTypeIn = mpSrcImg->m_header.m_nDataType;
	//条件判断
	switch(iDataTypeIn)
	{
	case 1:
		nResult = Process<BYTE>(srcImgIO, dstImgIO, reclassOut, reclassValiOut);
		break;
	case 2:
		nResult = Process<short int>(srcImgIO, dstImgIO, reclassOut, reclassValiOut);
		break;
	case 3:
		nResult = Process<int>(srcImgIO, dstImgIO, reclassOut, reclassValiOut);
		break;
	case 4:
		nResult = Process<float>(srcImgIO, dstImgIO, reclassOut, reclassValiOut);
		break;
	case 5:
		nResult = Process<double>(srcImgIO, dstImgIO, reclassOut, reclassValiOut);
		break;
	case 12:
		nResult = Process<unsigned short>(srcImgIO, dstImgIO, reclassOut, reclassValiOut);
		break;
	default:
		break;
	}


	//最后释放空间
	srcImgIO->Close_MgrOut();
	srcImgIO->Close_MgrIn();
	dstImgIO->Close_MgrIn();
	dstImgIO->Close_MgrOut();
	reclassOut->Close_MgrOut();
	reclassValiOut->Close_MgrOut();
	
	_ilog("Output Urban Change Image And Land Develop Suitable Indexes Image Data Finished.");

	//清空内存
	delete srcImgIO;
	srcImgIO = NULL;
	delete dstImgIO;
	dstImgIO = NULL;
	delete reclassOut;
	reclassOut = NULL;
	delete reclassValiOut;
	reclassValiOut = NULL;
	_ilog("Processed Output Urban Change Image And Land Develop Suitable Indexes Image File Successed.");

	//返回值
	return nResult;
}

template<class TT> int UrbanChangeMaker::Process(HAPBEGBase* pSrcIO , HAPBEGBase* pDstIO, HAPBEGBase* pReClassIO, HAPBEGBase* pValiIO)
{
	if (pSrcIO==NULL || pDstIO == NULL || pReClassIO == NULL)
		return -5;
	
	int iSamples, iLines, bands;
	iSamples = pSrcIO->Get_MgrIn()->m_header.m_nSamples;
	iLines = pSrcIO->Get_MgrIn()->m_header.m_nLines;
	bands = pSrcIO->Get_MgrIn()->m_header.m_nBands;

	int iCntAll = iLines; //设置总共的进度条记录次数
	int iCount =0;

	int m,n,k; //循环变量
	TT srcVal, dstVal;
	LandCoverDT outVal;
	LandSuitabilityDT outLDSIVal;
	UrbanReclassDT urOutVal, urValiOutVal;
	TT TTValue;

	//获取转换参数
	double* srcGeotransform = mpSrcImg->m_header.m_MapInfo->m_adfGeotransform;
	double* dstGeotransform = mpDstImg->m_header.m_MapInfo->m_adfGeotransform;
	const char* srcSpatialReferrence = mpSrcImg->m_header.m_MapInfo->GetSpatialReferenceWKT();
	const char* dstSpatialReferrence = mpDstImg->m_header.m_MapInfo->GetSpatialReferenceWKT();

	//查找coverCode的最小值和最大值
	long minCoverCode = mlCoverCode[0].nCode;
	long maxCoverCode = mlCoverCode[0].nCode;
	foreach (LandCoverCode _lcc, mlCoverCode)
	{
		if (_lcc.nCode<minCoverCode)
			minCoverCode = _lcc.nCode;

		if (_lcc.nCode>maxCoverCode)
			maxCoverCode = _lcc.nCode;
	}

	//初试类别和结束类别
	//LandCoverCode cSrcCode;
	//LandCoverCode cDstCode;
	long nReclassiID1, nReclassiID2;

	for (m=0;m<iLines;m++)
	{
		iCount++;
		if ((iCount+1)%5==0)
			_ilog(QString("Processing Rate: %1 / %2").arg(iCount+1).arg(iLines));
		
		for(n=0;n<iSamples;n++)
		{
			outVal = 0;
			outLDSIVal = 0;
			srcVal = *(TT*)pSrcIO->Read(n,m,0);
			double dst_x, dst_y;

			//如果小于最小值和最大值则输出为0
			if (srcVal<minCoverCode || srcVal>maxCoverCode)
			{
				outVal = CODE_UNKOWN_CHANGE;
				pSrcIO->Write(&outVal);
				outLDSIVal = INDEX_DEVELOP_UNABLE;
				pDstIO->Write(&outLDSIVal);
				urOutVal = RECLASS_UNKNOWN;
				pReClassIO->Write(&urOutVal);
				urValiOutVal = RECLASS_UNKNOWN;
				pValiIO->Write(&urValiOutVal);
				
				continue;
			}

			
			if (_Src2DstCoordTrans(n, m, dst_x, dst_y, mpSrcImg, mpDstImg))
			{
				//读取目标影像数据
				dstVal = *(TT*)pDstIO->Read(int(dst_x+0.5),int(dst_y+0.5),0);

				//查找目标code
				bool _bResult = findReclassiIDfor2(srcVal, dstVal, nReclassiID1, nReclassiID2);

				//如果两个有一个为空
				if (_bResult == false)
				{
					outVal = CODE_UNKOWN_CHANGE;	//无效值
					pSrcIO->Write(&outVal);
					outLDSIVal = INDEX_DEVELOP_UNABLE;
					pDstIO->Write(&outLDSIVal);
					urOutVal = RECLASS_UNKNOWN;
					pReClassIO->Write(&urOutVal);
					urValiOutVal = RECLASS_UNKNOWN;
					pValiIO->Write(&urValiOutVal);
					continue;
				}

				//判断城市变化
				if (nReclassiID1 == 0)
					urOutVal = RECLASS_UNCITY;
				else if (nReclassiID1 == 1)
					urOutVal = RECLASS_CITY;
				else if (nReclassiID1 == -1)
					urOutVal = RELCASS_PROTECTIVE;
				else
					urOutVal = RECLASS_UNKNOWN;

				//判断城市变化
				if (nReclassiID2 == 0)
					urValiOutVal = RECLASS_UNCITY;
				else if (nReclassiID2 == 1)
					urValiOutVal = RECLASS_CITY;
				else if (nReclassiID2 == -1)
					urValiOutVal = RELCASS_PROTECTIVE;
				else
					urValiOutVal = RECLASS_UNKNOWN;

				//判断发展指数
				if (nReclassiID1 == 0)
					outLDSIVal = INDEX_DEVELOP_ABLE;
				else if (nReclassiID1 == 1)
					outLDSIVal = INDEX_DEVELOP_UNABLE;
				else if (nReclassiID1 == -1)
					outLDSIVal = INDEX_PROTECTIVE;
				else
					outLDSIVal = INDEX_DEVELOP_UNABLE;

				//判断城市转换类型
				if (nReclassiID1 == -1 && nReclassiID2 == -1)
					outVal = CODE_PROTECTIVE_AREA_UNCHANGE;	
				else if (nReclassiID1 == -1 && nReclassiID2 == 0)
					outVal = CODE_PROTECTIVE_AREA_TO_NOCITY;
				else if (nReclassiID1 == -1 && nReclassiID2 == 1)
					outVal = CODE_PROTECTIVE_AREA_TO_CITY;

				else if (nReclassiID1 == 0 && nReclassiID2 == -1)
					outVal = CODE_NOCITY_TO_PROTECTIVE_AREA;
				else if (nReclassiID1 == 0 && nReclassiID2 == 0)
					outVal = CODE_NOCITY_TO_NOCITY;
				else if (nReclassiID1 == 0 && nReclassiID2 == 1)
					outVal = CODE_NOCITY_TO_CITY;

				else if (nReclassiID1 == 1 && nReclassiID2 == -1)
					outVal = CODE_CITY_TO_PROTECTIVE_AREA;
				else if (nReclassiID1 == 1 && nReclassiID2 == 0)
					outVal = CODE_CITY_TO_NOCITY;
				else if (nReclassiID1 == 1 && nReclassiID2 == 1)
					outVal = CODE_CITY_TO_CITY;

				else
					outVal = CODE_UNKOWN_CHANGE;	

			}
			else
			{
				urOutVal = RECLASS_UNKNOWN;
				outVal = CODE_UNKOWN_CHANGE;	//无效值
				outLDSIVal = INDEX_DEVELOP_UNABLE;
				urValiOutVal = RECLASS_UNKNOWN;
			}

			pSrcIO->Write(&outVal);
			pDstIO->Write(&outLDSIVal);
			pReClassIO->Write(&urOutVal);
			pValiIO->Write(&urValiOutVal);

		}
	}


	return 0;
}


// bool UrbanChangeMaker::Src2DstCoordTrans( double src_x, double src_y, double& dst_x, double& dst_y,  double* srcGeotransform, double* dstGeotransform,  const char* srcSpatialReferrence, const char* dstSpatialReferrence )
// {
// 	double lat, lon;
// 	_Pixel2World(&lat, &lon, src_x, src_y, srcGeotransform, srcSpatialReferrence );
// 	_World2Pixel(lat, lon, &dst_x, &dst_y, dstGeotransform, dstSpatialReferrence);
// 	
// 	if (dst_x<0 || dst_y<0 || dst_x>=mpDstImg->m_header.m_nSamples || dst_y>=mpDstImg->m_header.m_nLines)
// 		return false;
// 	else
// 		return true;
// }



bool UrbanChangeMaker::findReclassiIDfor2( long nCode1, long nCode2, long& nID1, long& nID2 )
{
	int nflag = 0;
	nID1 = -1; nID2 = -1;

	foreach ( LandCoverCode _lcc, mlCoverCode )
	{
		if (_lcc.nCode == nCode1)
		{
			nID1 = _lcc.nReClassiID;
			nflag += 1;
		}

		if (_lcc.nCode == nCode2)
		{
			nID2 = _lcc.nReClassiID;
			nflag += 1;
		}

		if (nflag == 2)
		{
			return true;
		}

	}

	return false;	

}





