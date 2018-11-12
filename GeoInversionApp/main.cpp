#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
#include <QtCore/qglobal.h>
#include <QtCore/QTime>
#include <QApplication>
#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>

#include "global_include.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"

#include "UrbanChangeMaker.h"
//#include "UrbanChangeProbabilityAnalysis_V1.h"
#include "UrbanChangeProbabilityAnalysis_V2.h"
#include "UrbanChangeCAIteration.h"
#include "ResultAnalysis.h"
#include "AuthorityRW.h"

char* XML_CONFIG = new char[2048];
int MAX_THREAD_NUM = 1;
char* SLCFILEPATH = new char[1024];
char* SAUXFILEPATH = new char[1024];
char* SOPFILEPATH = new char[1024];


bool isCoexit(char* str, int argc, char* argv[])
{
	for(int i=0; i<argc; i++)
	{
		if(strcmp(str, argv[i]) == 0)
		{
			return true;
		}
	}

	return false;
}


void logSettings( QString module )
{
	CXml2Document doc(XML_CONFIG);
	char logDirpath[2048];
	if (doc.GetValue(NODE_OUTPUT_LOG_DIR_PATH, logDirpath) != 0)
	{
		cout<<"LOG FILE LOAD ERROR!"<<endl;
		exit(0);
	}

	QString filePath = QString("%1/logs/").arg(logDirpath);
	QDir dir;
	// �жϵ�ǰ�ļ�·������Ч��
	if (!dir.exists(filePath))
	{
		dir.mkpath(filePath);
	}
	filePath = filePath.append(QString("%1_%2.log").arg(module).arg(QDate::currentDate().toString("yyyyMMdd")));
	// ��ʼ����־��ѯ
	// 1ʵ����һ��layout ����
	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	layout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S} %m%n");
	// 2. ��ʼ��һ��appender ����
	log4cpp::Appender* appender = new 
		log4cpp::FileAppender("FileAppender",
		filePath.toStdString());
	// 3. ��layout��������appender������
	appender->setLayout(layout);
	// 4. ʵ����һ��category����
	log4cpp::Category& log = log4cpp::Category::getInstance("info");
	// 5. ����additivityΪfalse���滻���е�appender
	log.setAdditivity(false);
	// 5. ��appender���󸽵�category��
	log.setAppender(appender);
	// 6. ����category�����ȼ������ڴ����ȼ�����־������¼
	log.setPriority(log4cpp::Priority::DEBUG);
}

void _ilog(char* sMessage, char* sInstance = "info" )
{
	log4cpp::Category& log = log4cpp::Category::getInstance(sInstance);
	log.info(sMessage);
	cout<<sMessage<<endl;
}

void _ilog( QString sMessage, char* sInstance = "info" )
{
	log4cpp::Category& log = log4cpp::Category::getInstance(sInstance);
	log.info(sMessage.toStdString().data());
	cout<<sMessage.toStdString().data()<<endl;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);


	//set code for locale, support chinese
// 	QTextCodec* codec =QTextCodec::codecForLocale();
// 	QTextCodec::setCodecForCStrings(codec);
// 	QTextCodec::setCodecForTr(codec);

	//
	if (argc<2)
	{
		cout<<"parameters error, plz input xml configure filename!"<<endl;
		return -1;
	}
	
	strcpy(XML_CONFIG, argv[1]);
	
	//register
	GDALAllRegister();
	OGRRegisterAll();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	
	/************************************************************************/
	/* Log Settings                                                         */
	/************************************************************************/
	logSettings(QString("%1_%2").arg(LOG_MODULE).arg(QTime::currentTime().toString("H_m_s_a")).toStdString().c_str());

	//////////////////////max_thread_num
	CXml2Document doc(XML_CONFIG);
	if (0 != doc.GetValue(NODE_MAX_THREADS_NUM, &MAX_THREAD_NUM))
	{
		MAX_THREAD_NUM = omp_get_max_threads();
	}

	if (MAX_THREAD_NUM <= 0 || MAX_THREAD_NUM > omp_get_max_threads())
		MAX_THREAD_NUM = omp_get_max_threads();
	
	_ilog(QString("Used Thread Number = %1").arg(MAX_THREAD_NUM));


	////////////////////////��ȡ�ļ�·��
	
	if (0 != doc.GetValue(NODE_FILEPATH_LANDCOVER, SLCFILEPATH))
	{
		global_ilog("Load NODE_FILEPATH_LANDCOVER Failed!");
		return -1;
	}
	global_ilog(QString("land cover data filepath = %1").arg(SLCFILEPATH));

	if (0 != doc.GetValue(NODE_FILEPATH_AUXILIARY, SAUXFILEPATH))
	{
		global_ilog("Load NODE_FILEPATH_AUXILIARY Failed!");
		return -1;
	}
	global_ilog(QString("auxiliary data filepath = %1").arg(SAUXFILEPATH));

	if (0 != doc.GetValue(NODE_FILEPATH_OUTPUT, SOPFILEPATH))
	{
		global_ilog("Load NODE_FILEPATH_OUTPUT Failed!");
		return -1;
	}
	global_ilog(QString("output data filepath = %1").arg(SOPFILEPATH));

	//////////////////////////////////////////////////////////////////////
	//authority
// 	if (!getAppAuthority("./key.au"))
// 	{
// 		_ilog("app key file of this program is out-of-date. exit.");
// 		return -101;
// 	}
// 	_ilog("app key is authorized.");

	///////////////////////////////////////////////////////////////////////
	_ilog("Starting Programme.");

	//��������
	bool bresult = true;
	
	//Module No.1 ׼�����ݣ��������б仯ͼ
	_ilog("Programme Starting Urban Change Analysis...");
	UrbanChangeMaker maker;
	bresult = maker.run();
	maker.close();	

	//Module No.2 �������ݷ�����������н�������ͼ
	if (bresult == false)
	{
		_ilog("Programme Can Not Starting Urban Change Probability Analysis. Exit.");
		return -1;
	}
	_ilog("Programme Starting Urban Change Probability Analysis...");
	UrbanChangeProbabilityAnalysis_V2 analyser;		//version 2.0
	//UrbanChangeProbabilityAnalysis_V1 analyser;	//version 1.0
	bresult = analyser.run();
	analyser.close();		

	//Module No.3 CA�������������ͼ��
	if (bresult == false)
	{
		_ilog("Programme Can Not Starting Urban Development CA Iterations. Exit.");
		return -2;
	}
	_ilog("Programme Starting Urban Development CA Iterations...");
	UrbanChangeCAIteration ca;
	bresult = ca.run();
	ca.close();	
	
	//Module No.4 ��������
	if (bresult == false)
	{
		_ilog("Programme Can Not Starting Accuracy Evaluation. Exit.");
		return -1;
	}
	_ilog("Programme Starting Accuracy Evaluation...");
	//int _rlt = AccuracyEvaluation();
	int _rlt = AccuracyEvaluation_Region();
	if (_rlt < 0)
	{
		bresult = false;
		_ilog(QString("Accuracy Evaluation Failed. Code = ").arg(_rlt));
	}
	else
		bresult = true;


	//Module No.5 ũ����ʴ����
// 	if (bresult == false)
// 	{
// 		_ilog("Programme Can Not Starting Farmland Erosion Analysis. Exit.");
// 		return -1;
// 	}
// 	_ilog("Programme Starting Farmland Erosion Analysis...");
// 	int _rlt = FarmlandErosionAnalysis();
// 	if (_rlt < 0)
// 	{
// 		bresult = false;
// 		_ilog(QString("Farmland Erosion Analysis Failed. Code = ").arg(_rlt));
// 	}
// 	else
// 		bresult = true;
	

	//����
	if (bresult == false)
	{
		_ilog("Programme Execute Failed.");
	}
	_ilog("Programme Execute Successed.");

	return 0;
}
