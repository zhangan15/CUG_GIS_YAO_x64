#include "ResultAnalysis.h"
#include "global_include.h"

#include "haplib.h"
#include "hapbegbase.h"
#include "HAPBEGThread.h"
#include "hapbegfactory.h"

#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "GDALVectorManager.h"



int FarmlandErosionAnalysis()
{
	global_ilog("Starting Farmland Erosion Analysis...");
	//���ڸ��ص���ֵ
	vector<int> mvCode;
	mvCode.clear();

	CXml2Document doc(XML_CONFIG);

	//�ж��Ƿ�ִ��
	int _nval = 0;
	if (0 != doc.GetValue(FA_VALID_PROCESS, &_nval))
	{
		_nval = 1;
	}

	if (_nval >= 1)
	{
		global_ilog("Executing Farmland Erosion Process = 1.");		
	}
	else
	{
		global_ilog("Executing Farmland Erosion Process = 0, continue.");
		return 0;
	}

	//��ȡ�����ļ���
	char sClassifiFilename[2048];
	if (0 != doc.GetValue(FA_SRC_CLASSIMG_FILENAME, sClassifiFilename))
	{
		global_ilog("Load FA_SRC_CLASSIMG_FILENAME Failed!");
		return -1;
	}
	global_ilog(QString("Src Classification Image Filename = %1").arg(sClassifiFilename));

	//��ȡ��������
	char sFarmlandCode[2048];
	if (0 != doc.GetValue(FA_FRAMLAND_CLASSI_CODE, sFarmlandCode))
	{
		global_ilog("Load FA_FRAMLAND_CLASSI_CODE Failed!");
		return -1;
	}
	global_ilog(QString("Farmland Classification Code = %1").arg(sFarmlandCode));

	if (!_splitStringToVector(sFarmlandCode, mvCode))
	{
		global_ilog("Split String To Codes Error!");
		return -2;
	}
	global_ilog("Split String To Codes Successed.");

	//��ȡCA�����
	char sCAResult[2048];
	if (0 != doc.GetValue(FA_CA_RESULT_FILES, sCAResult))
	{
		global_ilog("Load FA_CA_RESULT_FILES Failed!");
		return -1;
	}
	global_ilog(QString("CA Result Filename = %1").arg(sCAResult));

	//��ȡ����ļ���
	char sOpResult[2048];
	if (0 != doc.GetValue(FA_OUTPUT_RESULT_FILES, sOpResult))
	{
		global_ilog("Load FA_OUTPUT_RESULT_FILES Failed!");
		return -1;
	}
	global_ilog(QString("Farmland Erosion Analysis Result Filename = %1").arg(sOpResult));

	//��������ļ�������IO
	//��ʼ����, ��������ļ�
	CGDALFileManager* pInClass = new CGDALFileManager;
	if (!pInClass->LoadFrom(sClassifiFilename))
	{
		global_ilog("Load Classification File Error!");
		delete pInClass;
		return -3;
	}
	DIMS in_dims;
	pInClass->m_header.InitDIMS(&in_dims);

	//��ʼѭ������
	int nCount = 0;
	while (1)
	{
		global_ilog(QString("Starting No. %1 Farmland Erosion Analysis...").arg(nCount+1));
		//�����ļ���
		QString sCAResultFile = QString("%1_IterNo_%2.tif").arg(sCAResult).arg(nCount);
		QString sOpResultFile = QString("%1_IterNo_%2.tif").arg(sOpResult).arg(nCount);

		//��������ļ�������
		if (!QFile::exists(sCAResultFile))
		{
			global_ilog(QString("%1 Is Not Exist! Exit.").arg(sCAResultFile));
			break;
		}

		//����CA Result�ļ�
		CGDALFileManager* pInCA = new CGDALFileManager;
		if (!pInCA->LoadFrom(sCAResultFile.toStdString().c_str()))
		{
			global_ilog(QString("Load CA Result File Error! Filename = %1").arg(sCAResultFile));
			delete pInCA;
			nCount++;
			continue;
		}
		global_ilog(QString("Load CA Result File Successed. Filename = %1").arg(sCAResultFile));

		DIMS cadims;
		pInCA->m_header.InitDIMS(&cadims);

		//��������ļ�
		CGDALFileManager* pOut = new CGDALFileManager;
		pOut->m_header = pInClass->m_header;
		pOut->m_header.m_nDataType = AnalysisDTCode;

		//�����������IO
		HAPBEGBase* pIO = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
		if (!pIO->Set_MgrIn(pInClass, in_dims))
		{
			global_ilog("Set Classification In Error! ");
			delete pIO;
			delete pInCA;
			delete pInClass;
			delete pOut;
			nCount++;
			continue;
		}
		global_ilog("Set IO - Classification Image In Successed.");
		
		if (!pIO->Set_MgrOut(pOut, sOpResultFile.toStdString().c_str()))
		{
			global_ilog("Set Farmland Erosion Image Out Error!");
			delete pIO;
			delete pInCA;
			delete pOut;
			nCount++;
			continue;
		}
		global_ilog("Set IO - Farmland Erosion Image Out Successed.");

		//����CA���In
		HAPBEGBase* pCAIO = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
		if (!pCAIO->Set_MgrIn(pInCA, cadims))
		{
			global_ilog("Set CA Result File In Error! ");
			delete pIO;
			delete pCAIO;
			delete pInCA;
			delete pOut;
			nCount++;
			continue;
		}
		global_ilog("Set IO - CA Result File In Successed.");

		//
		int iSamples = pInClass->m_header.m_nSamples;
		int iLines = pInClass->m_header.m_nLines;
		int n=0, m=0;

		AnalysisDT outval = 0;
		LandCoverDT class_val = 0;
		CAResultDT ca_val = 0;

		long nFarmLandCount = 0;
		long nFarmLandErosionCount = 0;

		for (n=0; n<iLines; n++)
		{
			//�����Ϣ�ļ�
			if ((n+1)%10 == 0)
				global_ilog(QString("Farmland Erosion Analysis: Iteration = %1, Rate: %2 / %3").arg(nCount+1).arg(n+1).arg(iLines));

			for (m=0; m<iSamples; m++)
			{
				class_val = *(LandCoverDT*)pIO->Read(m, n, 0);
				if (!_isBelongToVector((int)class_val, mvCode))
				{
					outval = FA_UNKNOWN;
					pIO->Write(&outval);
					continue;
				}

				nFarmLandCount++;
				double dst_x, dst_y;
				if (_Src2DstCoordTrans(m, n, dst_x, dst_y, pInClass, pInCA))
				{
					ca_val = *(CAResultDT*)pCAIO->Read(int(dst_x+0.5), int(dst_y+0.5), 0);

					//ca_val = *(CAResultDT*)pCAIO->Read(m, n, 0);
					if (ca_val == RECLASS_CITY)
					{
						outval = FA_FARMLAND_CHANGE;
						nFarmLandErosionCount++;
					}
					else
						outval = FA_FARMLAND_UNCHANGE;
				}
				else
				{
					outval = FA_UNKNOWN;
				}

				pIO->Write(&outval);				

			}

		}
		




		//�رպ�ɾ��IO
		pIO->Close_MgrIn();
		pIO->Close_MgrOut();
		delete pIO;

		pCAIO->Close_MgrIn();
		delete pCAIO;
		

		//�ͷ��ڴ�
		pInCA->Close();
		//delete pInCA;
		pInCA = NULL;

		pOut->Close();
		//delete pOut;
		pOut = NULL;

		double erosionrate = (double)nFarmLandErosionCount/(double)nFarmLandCount;
		global_ilog(QString("Farmland Erosion No. %1 Analysis Finished. Farmland Erosion Rate = %2").arg(nCount+1).arg(erosionrate, 0, 'f', 6));
		nCount++;
	}

	//�ͷ��ڴ�
	pInClass->Close();
	//delete pInClass;
	pInClass = NULL;


	global_ilog("Farmland Erosion Analysis Finished.");
	return 0;
}

bool _isBelongToVector( int _val, vector<int>& _vec )
{
	foreach(int iv, _vec)
	{
		if (iv == _val)
			return true;
	}
	return false;
}

bool _splitStringToVector( char* sCode, vector<int>& _vec )
{
	QStringList _slist = QString(sCode).split(",", QString::SkipEmptyParts);
	if (_slist.size() == 0)
		return false;

	_vec.clear();
	foreach (QString str, _slist)
	{
		_vec.push_back(str.toInt());
	}

	return true;
}

// bool _src2DstCoordTrans( double src_x, double src_y, double* dst_x, double* dst_y, CGDALFileManager* pSrcImg, CGDALFileManager* pDstImg )
// {
// 	double lat, lon;	
// 	//_Pixel2World(&lat, &lon, src_x, src_y, pSrcImg->m_header.m_MapInfo->m_adfGeotransform, pSrcImg->GetpoDataset()->GetProjectionRef());
// 	//_World2Pixel(lat, lon, dst_x, dst_y, pDstImg->m_header.m_MapInfo->m_adfGeotransform, pDstImg->GetpoDataset()->GetProjectionRef());
// 
// 
// 
// 	if (dst_x<0 || dst_y<0 || *dst_x>=pDstImg->m_header.m_nSamples || *dst_y>=pDstImg->m_header.m_nLines)
// 		return false;
// 	else
// 		return true;
// }

int AccuracyEvaluation()
{
	global_ilog("Starting CA Result Evaluation...");

	CXml2Document doc(XML_CONFIG);

	//�ж��Ƿ�ִ��
	int _nval = 0;
	if (0 != doc.GetValue(AE_VALID_PROCESS, &_nval))
	{
		_nval = 1;
	}

	if (_nval >= 1)
	{
		global_ilog("Executing Accuracy Evaluation Process = 1.");		
	}
	else
	{
		global_ilog("Executing Accuracy Evaluation Process = 0, continue.");
		return 0;
	}
	//����ԭʼ�����ļ�
	char sSrcFilename[1024];
	if (0 != doc.GetValue(AE_SRC_RECLASS_FILE, sSrcFilename))
	{
		global_ilog("Load AE_SRC_RECLASS_FILE Failed!");
		return -1;
	}
	global_ilog(QString("Source Reclassification Image Filename = %1").arg(sSrcFilename));

	//����Validation
	char sValiFilename[1024];
	if (0 != doc.GetValue(AE_DST_RECLASS_FILE, sValiFilename))
	{
		global_ilog("Load AE_DST_RECLASS_FILE Failed!");
		return -1;
	}
	global_ilog(QString("Validation Reclassification Image Filename = %1").arg(sValiFilename));


	//��ȡCA�����
	char sCAResult[2048];
	if (0 != doc.GetValue(AE_CA_RESULT_FILES, sCAResult))
	{
		global_ilog("Load AE_CA_RESULT_FILES Failed!");
		return -1;
	}
	global_ilog(QString("CA Result Filename = %1").arg(sCAResult));

	//��ȡ����ļ���
	char sOpResult[2048];
	if (0 != doc.GetValue(AE_OUTPUT_RESULT_FILES, sOpResult))
	{
		global_ilog("Load AE_OUTPUT_RESULT_FILES Failed!");
		return -1;
	}
	global_ilog(QString("Accuracy Evaluation Result Filename = %1").arg(sOpResult));

	//��������ļ�������IO
	//��ʼ����, ��������ļ�
	CGDALFileManager* pValiImg = new CGDALFileManager;
	if (!pValiImg->LoadFrom(sValiFilename))
	{
		global_ilog("Load Dst Classification File Error!");
		delete pValiImg;
		return -2;
	}
	DIMS in_dims;
	pValiImg->m_header.InitDIMS(&in_dims);

	CGDALFileManager* pSrcImg = new CGDALFileManager;
	if (!pSrcImg->LoadFrom(sValiFilename))
	{
		global_ilog("Load Src Classification File Error!");
		delete pSrcImg;
		return -2;
	}
	DIMS in_src_dims;
	pSrcImg->m_header.InitDIMS(&in_src_dims);

	int nCount = 0;
	while(1)
	{
		global_ilog(QString("Starting No. %1 Accuracy Evaluation...").arg(nCount+1));
		//�����ļ���
		QString sCAResultFile = QString("%1_IterNo_%2.tif").arg(sCAResult).arg(nCount);
		QString sOpResultFile = QString("%1_IterNo_%2.accu").arg(sOpResult).arg(nCount);	//����ļ�Ϊ�ı��ļ�

		//��������ļ������ڣ��˳�ѭ��
		if (!QFile::exists(sCAResultFile))
		{
			global_ilog(QString("%1 Is Not Exist! Exit.").arg(sCAResultFile));
			break;
		}

		//
		CGDALFileManager* pCAImg = new CGDALFileManager;
		if (!pCAImg->LoadFrom(sCAResultFile.toStdString().c_str()))
		{
			global_ilog(QString("Load CA Result File Error! Filename = %1").arg(sCAResultFile));
			delete pCAImg;
			nCount++;
			continue;
		}
		global_ilog(QString("Load CA Result File Successed. Filename = %1").arg(sCAResultFile));
		DIMS ca_dims;
		pCAImg->m_header.InitDIMS(&ca_dims);


		//��������
		HAPBEGBase* pSrcIn = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
		HAPBEGBase* pValiIn = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
		HAPBEGBase* pCAIn = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);

		if (!pSrcIn->Set_MgrIn(pSrcImg, in_src_dims))
		{
			global_ilog("Set Source Image In Error! ");
			delete pValiIn;
			delete pSrcIn;
			delete pCAIn;
			delete pCAImg;
			nCount++;
			continue;
		}
		global_ilog("Set IO - Source Image In Successed.");

		if (!pValiIn->Set_MgrIn(pValiImg, in_dims))
		{
			global_ilog("Set Validation Image In Error! ");
			delete pValiIn;
			delete pSrcIn;
			delete pCAIn;
			delete pCAImg;
			nCount++;
			continue;
		}
		global_ilog("Set IO - Validation Image In Successed.");

		if (!pCAIn->Set_MgrIn(pCAImg, ca_dims))
		{
			global_ilog("Set CA Result Image In Error! ");
			delete pValiIn;
			delete pSrcIn;
			delete pCAIn;
			delete pCAImg;
			nCount++;
			continue;
		}
		global_ilog("Set IO - CA Result Image In Successed.");

		int m, n;
		//��ȡ��������
		double _ConfusionMatrix[3][3];
		for (m=0; m<3; m++)
		{
			for (n=0; n<3; n++)
			{
				_ConfusionMatrix[m][n]=0.0f;
			}
		}

		//
		int iSamples = pValiImg->m_header.m_nSamples;
		int iLines = pValiImg->m_header.m_nLines;

		CAResultDT org_val, src_val, ca_val;

		for (n=0; n<iLines; n++)
		{
			//�����Ϣ�ļ�
			if ((n+1)%50 == 0)
				global_ilog(QString("Accuracy Analysis: Iteration = %1, Rate: %2 / %3").arg(nCount+1).arg(n+1).arg(iLines));

			for (m=0; m<iSamples; m++)
			{
				src_val = *(CAResultDT*)pValiIn->Read(m, n, 0);
				org_val = *(CAResultDT*)pSrcIn->Read(m, n, 0);

				
				if (src_val == RECLASS_UNKNOWN)
					continue;
				
// 				if (org_val == src_val)
// 					continue;

				double dst_x, dst_y;
				if (_Src2DstCoordTrans(m, n, dst_x, dst_y, pValiImg, pCAImg))
				{
					ca_val = *(CAResultDT*)pCAIn->Read(int(dst_x+0.5), int(dst_y+0.5), 0);					
// 
// 					if (src_val==ca_val && org_val == ca_val)
// 						continue;

					//ca_val = *(CAResultDT*)pCAIn->Read(m, n, 0);
					if (ca_val == RECLASS_UNKNOWN)
						continue;

					if (src_val<=0 || src_val>3 || ca_val<=0 || ca_val>3)
						continue;

					_ConfusionMatrix[src_val-1][ca_val-1] += 1.0;
					
				}			

			}

		}


		//�����������������ļ�
		if (!_makeAccuracyFile(_ConfusionMatrix, sOpResultFile.toStdString().c_str()))
			global_ilog("Make Accuracy File Error!");		
		else
			global_ilog("Make Accuracy File Successed.");
		

		//����ڴ�
		pSrcIn->Close_MgrIn();
		pValiIn->Close_MgrIn();
		pCAIn->Close_MgrIn();
		delete pSrcIn;
		delete pCAIn;
		delete pValiIn;

		pCAImg->Close();
		//delete pCAImg;

		global_ilog(QString("No. %1 Accuracy Evaluation Finished.").arg(nCount+1));
		nCount++;
	}


	pSrcImg->Close();
	pValiImg->Close();
	//delete pValiImg;
	pValiImg = NULL;

	global_ilog("CA Result Evaluation Finished.");
	return 0;

}

bool _makeAccuracyFile( double _cm[3][3], const char* _filename )
{
	//����������������kappaϵ��
	double _overAllAccu = 0;			//����������
	double _commission[3] = {0, 0, 0};	//������
	double _omission[3] = {0, 0, 0};	//©�����
	double _prodAccu[3] = {0, 0, 0};	//��ͼ����
	double _userAccu[3] = {0, 0, 0};	//�û�����
	double _kappa = 0;					//kappaϵ��

	//��ʼ����
	int i, j;	//i-row, j-col
	double sum_all = 0;
	double sum_row[3] = {0, 0, 0};
	double sum_col[3] = {0, 0, 0};
	double sum_duijiaoxian = 0;	//�Խ��ߺ�
	for (i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
		{
			if (i==j)
				sum_duijiaoxian += _cm[i][j];

			sum_all += _cm[i][j];
			sum_row[i] += _cm[i][j];	
			sum_col[j] += _cm[i][j];

		}
	}

	_overAllAccu = sum_duijiaoxian/sum_all;	//���徫��
	for (i=0; i<3; i++)
	{
		_commission[i] = (sum_row[i]-_cm[i][i])/sum_row[i];
		_omission[i] = (sum_col[i]-_cm[i][i])/sum_col[i];
		_prodAccu[i] = _cm[i][i]/sum_col[i];
		_userAccu[i] = _cm[i][i]/sum_row[i];
	}

	//����kappaϵ��
	double _val = 0;
	for (i=0; i<3; i++)
	{
		_val += sum_row[i]*sum_col[i];
	}

	_kappa = (sum_all*sum_duijiaoxian-_val)/(sum_all*sum_all-_val);

	//д���ļ�
	QFile _file(_filename);
	if (!_file.open(QFile::WriteOnly))
	{
		global_ilog(QString("Open File To Write Error! Filename = %1").arg(_filename));
		return false;
	}
	QTextStream _in(&_file);

	QString _msg = QFileInfo(_filename).fileName();
	_in<<_msg; _in<<endl; global_ilog(_msg);

	_msg = QDateTime::currentDateTime().toString();
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_in<<endl;
	_in<<endl;

	//д���������
	_msg = "Confusion Matrix: ";
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = "\tCity\tNon-City\tLimited";
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("City\t%1\t%2\t%3").arg(_cm[0][0], 0, 'f', 4).arg(_cm[0][1], 0, 'f', 4).arg(_cm[0][2], 0, 'f', 4);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("Non-City\t%1\t%2\t%3").arg(_cm[1][0], 0, 'f', 4).arg(_cm[1][1], 0, 'f', 4).arg(_cm[1][2], 0, 'f', 4);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("Limited\t%1\t%2\t%3").arg(_cm[2][0], 0, 'f', 4).arg(_cm[2][1], 0, 'f', 4).arg(_cm[2][2], 0, 'f', 4);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_in<<endl;

	//д�����徫��
	_msg = QString("Overall Accuracy (���徫��) = %1").arg(_overAllAccu, 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);

	//д��Kappaϵ��
	_msg = QString("Kappa = %1").arg(_kappa, 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_in<<endl;

	//д����ྫ��
	_msg = "City Accuracy:";
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("Commission Error (������) = %1").arg(_commission[0], 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("Ommission Error (©�����) = %1").arg(_omission[0], 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("Mapping Accuracy (��ͼ����) = %1").arg(_prodAccu[0], 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("User Accuracy (�û�����) = %1").arg(_userAccu[0], 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_in<<endl;

	_msg = "Non-City Accuracy:";
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("Commission Error (������) = %1").arg(_commission[1], 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("Ommission Error (©�����) = %1").arg(_omission[1], 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("Mapping Accuracy (��ͼ����) = %1").arg(_prodAccu[1], 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("User Accuracy (�û�����) = %1").arg(_userAccu[1], 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_in<<endl;

	_msg = "Limited Area Accuracy:";
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("Commission Error (������) = %1").arg(_commission[2], 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("Ommission Error (©�����) = %1").arg(_omission[2], 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("Mapping Accuracy (��ͼ����) = %1").arg(_prodAccu[2], 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_msg = QString("User Accuracy (�û�����) = %1").arg(_userAccu[2], 0, 'f', 6);
	_in<<_msg; _in<<endl; global_ilog(_msg);
	_in<<endl;


	_in.flush();
	_file.close();
	return true;
}

bool Src2Dst_Geo( double& dst_X, double& dst_Y, double src_X, double src_Y, OGRCoordinateTransformation* poTransform )
{
	if( poTransform == NULL )  
	{
		return false;
	}
	else
	{
		dst_X = src_X;
		dst_Y = src_Y;
		poTransform->Transform(1, &dst_X, &dst_Y);

		return true;
	}
}

int AccuracyEvaluation_Region()
{
	global_ilog("Starting CA Result Evaluation Using Vector File...");

	CXml2Document doc(XML_CONFIG);

	//�ж��Ƿ�ִ��
	int _nval = 0;
	if (0 != doc.GetValue(AE_VALID_PROCESS, &_nval))
	{
		_nval = 1;
	}

	if (_nval >= 1)
	{
		global_ilog("Executing Accuracy Evaluation Process = 1.");
	}
	else
	{
		global_ilog("Executing Accuracy Evaluation Process = 0, continue.");
		return 0;
	}


	//����ʸ��������
	char sVecFile[1024];
	if (0 != doc.GetValue(AE_MAP_VECTOR_FILE, sVecFile))
	{
		global_ilog("Load AE_MAP_VECTOR_FILE Failed!");
		return -1;
	}
	global_ilog(QString("Vector Image Filename = %1").arg(sVecFile));

	//����ԭʼ�ط���Ӱ��
	char sSrcFilename[1024];
	if (0 != doc.GetValue(AE_SRC_RECLASS_FILE, sSrcFilename))
	{
		global_ilog("Load AE_SRC_RECLASS_FILE Failed!");
		return -1;
	}
	global_ilog(QString("Original Reclassification Image Filename = %1").arg(sSrcFilename));

	//����Validation
	char sValiFilename[1024];
	if (0 != doc.GetValue(AE_DST_RECLASS_FILE, sValiFilename))
	{
		global_ilog("Load AE_DST_RECLASS_FILE Failed!");
		return -1;
	}
	global_ilog(QString("Target Reclassification Image Filename = %1").arg(sValiFilename));


	//��ȡCA�����
	char sCAResult[2048];
	if (0 != doc.GetValue(AE_CA_RESULT_FILES, sCAResult))
	{
		global_ilog("Load AE_CA_RESULT_FILES Failed!");
		return -1;
	}
	global_ilog(QString("CA Result Filename = %1").arg(sCAResult));

	//��ȡ����ļ���
	char sOpResult[2048];
	if (0 != doc.GetValue(AE_OUTPUT_RESULT_FILES, sOpResult))
	{
		global_ilog("Load AE_OUTPUT_RESULT_FILES Failed!");
		return -1;
	}
	global_ilog(QString("Accuracy Evaluation Result Filename = %1").arg(sOpResult));

	//����ʸ������
	CGDALVectorManager* pVec = new CGDALVectorManager;
	if (!pVec->LoadFrom(sVecFile))
	{
		global_ilog("Load Vector File Error!");
		delete pVec;
		return -2;
	}
	global_ilog("Load Vector File Successed.");

	//��������ļ�������IO

	//�����ط����ļ�
	CGDALFileManager* pSrcImg = new CGDALFileManager;
	if (!pSrcImg->LoadFrom(sSrcFilename))
	{
		global_ilog("Load Classification File Error!");
		delete pVec;
		delete pSrcImg;
		return -2;
	}
	global_ilog("Load Classification File Successed.");

	//����vali�����ļ�
	CGDALFileManager* pValiImg = new CGDALFileManager;
	if (!pValiImg->LoadFrom(sValiFilename))
	{
		global_ilog("Load Validation Classification File Error!");
		delete pVec;
		delete pSrcImg;
		delete pValiImg;
		return -2;
	}
	global_ilog("Load Validation Classification File Successed.");

	int nCount = 0;
	//��ȡCAִ�д���
	nCount = getCAIterationNum();


	//ͳ�Ʒ���ͼ��
	double* pSrcStatic = NULL;
	double* pValiStatic = NULL;
	int _sucflag = 0;
	
	//�������̣߳���������ֻ��Ҫ2���߳̾͹���
#pragma omp parallel sections num_threads(2<MAX_THREAD_NUM ? 2:MAX_THREAD_NUM)
	{
#pragma omp section
		{
			global_ilog(QString("No. %1 CPU Core Starting Original Data Statistics...").arg(omp_get_thread_num()));
			CGDALVectorManager* pVec1 = new CGDALVectorManager;
			if (!pVec1->LoadFrom(sVecFile))
			{
				global_ilog("Original Data Statistics Load Vector File Error!");
				delete pVec1;
				_sucflag = -3;
			}
			global_ilog("Original Data Statistics Load Vector File Successed.");

			if (!StatisSingleMap(pSrcImg, pVec1->GetpLayer(0), pSrcStatic, -2))
			{
				global_ilog("Statistic Original Classification Error!");
				_sucflag = -1;
			}
			global_ilog("Statistic Original Classification Success.");

			delete pVec1;
		}
		

#pragma omp section
		{
			global_ilog(QString("No. %1 CPU Core Starting Validation Data Statistics...").arg(omp_get_thread_num()));
			CGDALVectorManager* pVec1 = new CGDALVectorManager;
			if (!pVec1->LoadFrom(sVecFile))
			{
				global_ilog("Validation Data Statistics Load Vector File Error!");
				delete pVec1;
				_sucflag = -3;
			}
			global_ilog("Validation Data Statistics Load Vector File Successed.");

			if (!StatisSingleMap(pValiImg, pVec1->GetpLayer(0), pValiStatic, -1))
			{
				global_ilog("Statistic Validation Classification Error!");
				_sucflag = -1;
			}
			global_ilog("Statistic Validation Classification Success.");

			delete pVec1;
		}
			
	}
	
	if (_sucflag < 0)
	{
		global_ilog("Statistic Original / Validation Classification Error!");
		return _sucflag;
	}
	
	//ͳ��CA
	double** vpCAStatics = new double*[nCount];
	for (int kk=0; kk<nCount; kk++)
		vpCAStatics[kk] = NULL;

	//�������̣߳�������������٣���ʹ�õ����������߳�
	//������������Vector���ݶ�д��֧�ֶ��̣߳�����ÿ���̱߳��뿪һ��
	int iterCount = 0;
	
#pragma omp parallel for num_threads(MAX_THREAD_NUM>nCount ? nCount:MAX_THREAD_NUM) schedule(dynamic)
	for (iterCount = 0; iterCount < nCount; iterCount++)
	{
		global_ilog(QString("No. %2 CPU Core Starting No. %1 Accuracy Evaluation...").arg(iterCount+1).arg(omp_get_thread_num()));
		//�����ļ���
		QString sCAResultFile = QString("%1_IterNo_%2.tif").arg(sCAResult).arg(iterCount);

		
		//��������ļ������ڣ��˳�ѭ��
		if (!QFile::exists(sCAResultFile))
		{
			global_ilog(QString("%1 Is Not Exist! Exit.").arg(sCAResultFile));
			continue;
		}

		CGDALVectorManager* pVec_tp = new CGDALVectorManager;
		if (!pVec_tp->LoadFrom(sVecFile))
		{
			//vFailedID.push_back(iterCount);
			global_ilog(QString("No. %1 Load Vector File Error!").arg(iterCount+1));
			delete pVec_tp;
			continue;
		}
		global_ilog(QString("No. %1 Load Vector File Successed.").arg(iterCount+1));

		//
		CGDALFileManager* pCAImg = new CGDALFileManager;
		if (!pCAImg->LoadFrom(sCAResultFile.toStdString().c_str()))
		{
			//vFailedID.push_back(iterCount);
			global_ilog(QString("Load CA Result File Error! Filename = %1").arg(sCAResultFile));
			delete pCAImg;
			delete pVec_tp;
			continue;
		}
		global_ilog(QString("No. %1 Load CA Iteration File Successed.").arg(iterCount+1));


		//��ʼ����
		double* pCAStatic = NULL;
		if (!StatisSingleMap(pCAImg, pVec_tp->GetpLayer(0), pCAStatic, iterCount))
		{
			if (pCAStatic!=NULL)
			{
				delete pCAStatic;
				pCAStatic = NULL;
			}
			//vFailedID.push_back(iterCount);
			global_ilog(QString("Statistic No. %1 CA Result Failed on No. %2 CPU Core.").arg(iterCount+1).arg(omp_get_thread_num()));
			//vpCAStatics.push_back(pCAStatic);
			vpCAStatics[iterCount] = pCAStatic;
			continue;
		}
		global_ilog(QString("Statistic No. %1 CA Result Success on No. %2 CPU Core.").arg(iterCount+1).arg(omp_get_thread_num()));
		//vpCAStatics.push_back(pCAStatic);
		vpCAStatics[iterCount] = pCAStatic;

		//����ڴ�
		pCAImg->Close();
		//delete pCAImg;
		delete pVec_tp;

		global_ilog(QString("No. %1 Accuracy Evaluation Finished on No. %2 CPU Core.").arg(iterCount+1).arg(omp_get_thread_num()));
	}

	//ʧ�ܵ���������(���߳�)
	int nRestartCount = 0;
	nCount = getCAIterationNum();
	while (1)
	{
		QList<int> vFailedID;
		vFailedID.clear();
		int nFailedCount = 0;
		for (iterCount = 0; iterCount<nCount; iterCount++)
		{
			if (vpCAStatics[iterCount] == NULL)
			{
				nFailedCount++;
				vFailedID.push_back(iterCount);

			}
		}

		//�޴������˳�
		if (nFailedCount == 0)
		{
			break;
		}

		nRestartCount++;
		global_ilog(QString("%1 Statistics have been failed, No. %2 Restart Precessing...").arg(nFailedCount).arg(nRestartCount));

		if (nFailedCount > 0)
		{
			int nFailedIter = 0;			
//#pragma omp parallel for num_threads(MAX_THREAD_NUM/2>nFailedCount ? nFailedCount:MAX_THREAD_NUM/2) private(nFailedIter) schedule(dynamic)
			for (nFailedIter = 0; nFailedIter < nFailedCount; nFailedIter++)
			{
				//ʧ�ܵ�ID
				iterCount = vFailedID[nFailedIter];

				global_ilog(QString("No. %2 CPU Core Starting No. %1 Accuracy Evaluation...").arg(iterCount+1).arg(omp_get_thread_num()));
				//�����ļ���
				QString sCAResultFile = QString("%1_IterNo_%2.tif").arg(sCAResult).arg(iterCount);


				//��������ļ������ڣ��˳�ѭ��
				if (!QFile::exists(sCAResultFile))
				{
					global_ilog(QString("%1 Is Not Exist! Exit.").arg(sCAResultFile));
					//break;
					continue;
				}

				CGDALVectorManager* pVec_tp = new CGDALVectorManager;
				if (!pVec_tp->LoadFrom(sVecFile))
				{
					global_ilog(QString("No. %1 Load Vector File Error!").arg(iterCount+1));
					delete pVec_tp;
					//break;
					continue;
				}
				global_ilog(QString("No. %1 Load Vector File Successed.").arg(iterCount+1));

				//
				CGDALFileManager* pCAImg = new CGDALFileManager;
				if (!pCAImg->LoadFrom(sCAResultFile.toStdString().c_str()))
				{
					global_ilog(QString("Load CA Result File Error! Filename = %1").arg(sCAResultFile));
					delete pCAImg;
					delete pVec_tp;
					//break;
					continue;
				}
				global_ilog(QString("No. %1 Load CA Iteration File Successed.").arg(iterCount+1));


				//��ʼ����
				double* pCAStatic = NULL;
				if (!StatisSingleMap(pCAImg, pVec_tp->GetpLayer(0), pCAStatic, iterCount))
				{
					if (pCAStatic!=NULL)
					{
						delete pCAStatic;
						pCAStatic = NULL;
					}

					global_ilog(QString("Statistic No. %1 CA Result Failed on No. %2 CPU Core.").arg(iterCount+1).arg(omp_get_thread_num()));
					//vpCAStatics.push_back(pCAStatic);
					vpCAStatics[iterCount] = pCAStatic;
					continue;
				}
				global_ilog(QString("Statistic No. %1 CA Result Success on No. %2 CPU Core.").arg(iterCount+1).arg(omp_get_thread_num()));
				//vpCAStatics.push_back(pCAStatic);
				vpCAStatics[iterCount] = pCAStatic;

				//����ڴ�
				pCAImg->Close();
				//delete pCAImg;
				delete pVec_tp;

				global_ilog(QString("No. %1 Accuracy Evaluation Finished on No. %2 CPU Core.").arg(iterCount+1).arg(omp_get_thread_num()));
			}
		}
	}

	

	//statistics
	int i = 0, j = 0;
	int cityCount = pVec->GetpLayer(0)->GetFeaturesCount();

	//����ÿһ�ε�������
	double* pError = new double[nCount];
	memset(pError, 0, (nCount)*sizeof(double));

	int _falseflag = 0;
	for (j=0; j<nCount; j++)
	{
		global_ilog(QString("Statistical No. %1 CA Result...").arg(j+1));
		double* pdata = vpCAStatics[j];
		if (pdata != NULL)
		{
			for (i=0; i<cityCount; i++)
			{
				//Alex Hangen-Zanker, Pin Martens. Map Comparison for Comprehensive Assessment of Geosimulation Methods, 2008.
				pError[j] += ((pdata[i]-pSrcStatic[i])-(pValiStatic[i]-pSrcStatic[i]))\
						*((pdata[i]-pSrcStatic[i])-(pValiStatic[i]-pSrcStatic[i]));
			}
		}
		else
		{
			pError[j] += 0;
			_falseflag = -1;
			break;
		}
			
	}

	for (j=0; j<nCount; j++)
	{
		if (_falseflag == -1)
			pError[j] = -1;
		else
			pError[j] = sqrt(pError[j])/(double)nCount;
	}	

	//д���ļ�
	///////////////
	global_ilog("Writing Into Statical File...");
	QString sOpResultFile = QString("%1.statics").arg(sOpResult);	//����ļ�Ϊ�ı��ļ�
	QFile file(sOpResultFile);
	if (!file.open(QFile::WriteOnly))
	{
		global_ilog(QString("Open Statics File To Write Error! Filename: %1").arg(sOpResultFile));
		return -5;
	}

	
	QTextStream in(&file);

	//
	QString smsg = QString("Provinces Num = %1").arg(pVec->GetpLayer(0)->GetFeaturesCount());
	in<<smsg; in<<endl;
	smsg = QString("CA Iterations Num = %1").arg(nCount);
	in<<smsg; in<<endl;

	smsg = "nFID, Nation, Province, Name, E-Name, Original, Validation, ";
	for (i=0; i<nCount; i++)
	{
		if (i!=nCount-1)
			smsg += QString("CA-%1 ,").arg(i+1);
		else
			smsg += QString("CA-%1").arg(i+1);
	}
	in<<smsg; in<<endl;

	
	for (i=0; i<cityCount; i++)
	{
		global_ilog(QString("Write into No. %1 data.").arg(i+1));
		smsg = QString("%1, %2, %3, %4, %5, %6, %7, ").arg(i).arg("China")\
			.arg(pVec->GetpLayer(0)->GetFieldAttributeAsInteger(i, 2))\
			.arg(pVec->GetpLayer(0)->GetFieldAttributeAsString(i, 3))\
			.arg(pVec->GetpLayer(0)->GetFieldAttributeAsString(i, 4))\
			.arg(pSrcStatic[i], 0, 'f', 6).arg(pValiStatic[i], 0, 'f', 6);

		

		for (j=0; j<nCount; j++)
		{
			double* pdata = vpCAStatics[j];
			if (pdata!=NULL)
			{
				if (j!=nCount-1)
					smsg += QString("%1, ").arg(vpCAStatics[j][i], 0, 'f' , 6);
				else
					smsg += QString("%1").arg(vpCAStatics[j][i], 0, 'f' , 6);
			}
			else
			{
				if (j!=nCount-1)
					smsg += QString("%1, ").arg(-1.0, 0, 'f' , 6);
				else
					smsg += QString("%1").arg(-1.0, 0, 'f' , 6);
			}
			
		}

		in<<smsg; in<<endl;
	}

	in.flush();
	
	//���һ��д�����
	smsg = QString("%1, %2, %3, %4, %5, %6, %7, ").arg("Sum_Error").arg("China")\
		.arg("NULL")\
		.arg("NULL")\
		.arg("NULL")\
		.arg(0.0f, 0, 'f', 6).arg(0.0f, 0, 'f', 6);

	for (j=0; j<nCount; j++)
	{
		if (j!=nCount-1)
			smsg += QString("%1, ").arg(pError[j], 0, 'f' , 6);
		else
			smsg += QString("%1").arg(pError[j], 0, 'f' , 6);
	}

	global_ilog(smsg);
	in<<smsg; in<<endl;

	//�ر��ļ�
	in.flush();
	file.close();

	global_ilog("Write Into Statical File Success.");


	///////release memeory////////
	pSrcImg->Close();
	pSrcImg = NULL;

	pValiImg->Close();
	pValiImg = NULL;

	delete pVec;
	pVec = NULL;
	
	//release memory
	if (pSrcStatic!=NULL)
	{
		delete []pSrcStatic;
		pSrcStatic = NULL;
	}

	if (pValiStatic!=NULL)
	{
		delete []pValiStatic;
		pValiStatic = NULL;
	}

// 	for (int i=0; i<vpCAStatics.size(); i++)
// 	{
// 		if (vpCAStatics[i] != NULL)
// 		{
// 			delete []vpCAStatics[i];
// 			vpCAStatics[i] = NULL;
// 		}
// 	}
// 	vpCAStatics.clear();

	for (i=0; i<nCount; i++)
	{
		if (vpCAStatics[i] != NULL)
		{
			delete []vpCAStatics[i];
			vpCAStatics[i] = NULL;
		}
	}
	delete []vpCAStatics;

	delete []pError;

	//finished.
	global_ilog("CA Result Evaluation Using Vector File Finished.");
	return 0;

}

bool StatisSingleMap( CGDALFileManager* pImg, CGDALVectorLayer* pLayer, double* &pResult, int nImgId )
{
	if (pImg == NULL || pLayer == NULL)
	{
		global_ilog("Image or Layer is NULL!");
		return false;
	}

	char filename[512];
	pImg->GetFileName(filename);

	//�����ļ�IO, �����
	HAPBEGBase* pIn = HAPBEGFactory::CreateHapBegInstance(RowPriority, false);
	DIMS dims;
	pImg->m_header.InitDIMS(&dims);

	if (!pIn->Set_MgrIn(pImg, dims))
	{
		global_ilog(QString("Set File IO Error! Filename: %1").arg(filename));
		delete pIn;
		return false;
	}

	//��ȡ����
	int nCityCount = pLayer->GetFeaturesCount();
	if (pResult!=NULL)
		delete []pResult;
	pResult = new double[nCityCount];
	memset(pResult, 0, nCityCount*sizeof(double));

	//ͼ���ʸ��������ת��
	OGRSpatialReference* imgSR = (OGRSpatialReference*)OSRNewSpatialReference(pImg->GetpoDataset()->GetProjectionRef());
	OGRCoordinateTransformation* pTrans = OGRCreateCoordinateTransformation(imgSR, pLayer->GetpoSpatialReference());

	//��ʼ����
	int nCityInter = 0;
	OGRPolygon* pCityPoly = NULL;
	for (nCityInter = 0; nCityInter < nCityCount; nCityInter++)
	{
		global_ilog(QString("Processing No. %3 Urban Land Statistics: %1 / %2").arg(nCityInter+1).arg(nCityCount).arg(nImgId+1));
		
		//��ȡ��״
		pCityPoly = (OGRPolygon*)(pLayer->GetpoFeature(nCityInter)->GetGeometryRef());

		//��ȡ����
		OGREnvelope env;
		pCityPoly->getEnvelope(&env);

		//��ʸ��ͼ���л�ȡ��Ե��γ��
		double minLon, minLat, maxLon, maxLat;
		pLayer->ground2World(&minLat, &minLon, env.MinX, env.MinY);
		pLayer->ground2World(&maxLat, &maxLon, env.MaxX, env.MaxY);

		//����γ��ת��Ϊͼ�ε���������
		double minX, minY, maxX, maxY;
		pImg->world2Pixel(minLat, minLon, &minX, &minY);
		pImg->world2Pixel(maxLat, maxLon, &maxX, &maxY);

		if (minX > maxX)
		{
			double _tp = minX;
			minX = maxX;
			maxX = _tp;			
		}

		if (minY > maxY)
		{
			double _tp = minY;
			minY = maxY;
			maxY = _tp;			
		}

		//ȡֵ
		int i, j;
		double GeoX, GeoY;
		OGRPoint pt;
		UrbanReclassDT tVal;
		long nSumCount = 0;
		long nCity = 0;
		for (j=int(minY+0.5); j<=int(maxY+0.5); j++)
		{
			for (i=int(minX+0.5); i<=int(maxX+0.5); i++)
			{
				if (i<0 || j<0 || i>=pImg->m_header.m_nSamples || j>=pImg->m_header.m_nLines)
					continue;

				//����������ת��Ϊͼ��������				
				pImg->pixel2Ground(i, j, &GeoX, &GeoY);
				//ͼ��������ת��Ϊʸ���������
				pTrans->Transform(1, &GeoX, &GeoY);	//ת��Ϊʸ��ͼ���ϵ�����

				//������ͼ��
				pt.setX(GeoX);
				pt.setY(GeoY);

				//�ж��Ƿ�����˵�
				if (pCityPoly->Contains(&pt) == 0)
					continue;
				
				nSumCount++;
				//��ȡͼ���ϵ�����
				tVal = *(UrbanReclassDT*)pIn->Read(i, j, 0);

				//�ж��Ƿ�Ϊ����
				if ( tVal == RECLASS_CITY )
					nCity++;
			}
		}

		//�õ��ٷֱ�
		pResult[nCityInter] = (double)nCity/(double)nSumCount;

		global_ilog(QString("No. %1 Urban Area Percent: %2%").arg(nCityInter+1).arg(pResult[nCityInter]*100, 0, 'f', 4));
		

	}

	pIn->Close_MgrIn();
	delete pIn;

	OCTDestroyCoordinateTransformation(pTrans);
	OSRDestroySpatialReference(imgSR);

	return true;
}

int getCAIterationNum()
{
	CXml2Document doc(XML_CONFIG);
	//��ȡCA�����
	char sCAResult[2048];
	if (0 != doc.GetValue(AE_CA_RESULT_FILES, sCAResult))
	{
		return 0;
	}
	
	int nCount = 0;

	while (1)
	{
		QString sCAResultFile = QString("%1_IterNo_%2.tif").arg(sCAResult).arg(nCount);

		//��������ļ������ڣ��˳�ѭ��
		if (!QFile::exists(sCAResultFile))
		{
			//global_ilog(QString("%1 Is Not Exist! Exit.").arg(sCAResultFile));
			break;
		}

		nCount++;
	}

	global_ilog(QString("CA Iteration Num = %1").arg(nCount));
	return nCount;
}
