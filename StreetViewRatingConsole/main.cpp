#include <QtCore>
#include <iostream>
#include "alglib/statistics.h"
#include "alglib/dataanalysis.h"
#include "alglib/alglibmisc.h"
#include "RFWeights.h"
#include "WebCoordSystemTransform.h"
using namespace std;
using namespace alglib;

#define DIRECTION_COUNT 4
#define FEATURES_COUNT 151

class IMAGE_SCORE
{
public:
	IMAGE_SCORE() { sfilename = "", nScore = -1; vFeatures.clear(); }
	~IMAGE_SCORE() { sfilename = "", nScore = -1; vFeatures.clear(); };
	IMAGE_SCORE(QString sName) { sfilename = sName.trimmed(), nScore = -1; vFeatures.clear(); }

public:
	bool operator==(IMAGE_SCORE s)
	{
		if (s.sfilename.trimmed().isEmpty())
			return false;
		QString s1 = sfilename.trimmed().toLower();
		QString s2 = s.sfilename.trimmed().toLower();
		if (s1 == s2) //fi.completeBaseName().trimmed().toLower() == fi2.completeBaseName().trimmed().toLower())
			return true;
		else
			return false;
	}

public:
	QString sfilename;
	float nScore;
	QList<float> vFeatures;	//find features.csv
};


class POS_SCORE
{
public:
	POS_SCORE() {
		mdLat = 0, mdLon = 0;  
		mpDirection[0] = mpDirection[1] = mpDirection[2] = mpDirection[3] = -1; 
		memset(mpFeatures, 0, FEATURES_COUNT * sizeof(double));
	};
	POS_SCORE(double lat, double lon) { 
		mdLat = lat, mdLon = lon; 
		mpDirection[0] = mpDirection[1] = mpDirection[2] = mpDirection[3] = -1;
		memset(mpFeatures, 0, FEATURES_COUNT * sizeof(double));
	};
	~POS_SCORE() {
	};

public:
	bool operator== (POS_SCORE ps)
	{
		if (abs(mdLat - ps.mdLat) < 1e-6 && abs(mdLon - ps.mdLon) < 1e-6)
			return true;
		else
			return false;
	}

public:
	double mdLat, mdLon;
	double mpDirection[DIRECTION_COUNT];	//scores of 4 directions
	double mpFeatures[FEATURES_COUNT];
	double mdMax;
	double mdMin;
	double mdAve;
};

bool loadImageScoresOnly(char* sFeatureFilename, int& nFeatureCount, QList<IMAGE_SCORE>& vTotalImageScores)
{
	vTotalImageScores.clear();

	QFile featureFile(sFeatureFilename);
	if (!featureFile.open(QIODevice::ReadOnly))
	{
		cout << "Error: Load feature.csv file (ONLY) error." << endl;
		vTotalImageScores.clear();
	}
	else
	{
		QTextStream _out(&featureFile);
		QString s = _out.readLine(); //remove title
		QStringList slist = s.split(",");
		int nFeatureDimension = slist.size() - 1;
		nFeatureCount = nFeatureDimension;
		cout << "Feature dimension = " << nFeatureDimension << endl;

		int nCount = 0;
		while (!_out.atEnd())
		{
			nCount++;
			s = _out.readLine();
			slist = s.split(",");
			if (slist.size() != nFeatureDimension + 1)
				continue;

			IMAGE_SCORE imgScore;
			imgScore.sfilename = slist[0].trimmed();


			imgScore.vFeatures.clear();
			for (int i = 1; i < slist.size(); i++)
				imgScore.vFeatures.append(slist[i].trimmed().toDouble());

			vTotalImageScores.append(imgScore);

			if (nCount % 500 == 0)
				cout << ((QString("Loading features of No. %1 image (%2). Total image count = %3.")\
					.arg(nCount).arg(slist[0]).arg(vTotalImageScores.size()))).toStdString().data() << endl;

		}

		featureFile.close();

	}

	cout << QString("Load total image count = %1.")\
		.arg(vTotalImageScores.size()).toStdString().data() << endl;

	return true;
}

bool loadImageScoresFromFile(char* sFeatureFilename, char* sScoreFilename, int& nFeatureCount, QList<IMAGE_SCORE>& vTotalImageScores, QList<IMAGE_SCORE>& vRatedImageScores)
{
	vTotalImageScores.clear();
	vRatedImageScores.clear();

	QFile featureFile(sFeatureFilename);
	if (!featureFile.open(QIODevice::ReadOnly))
	{
		cout << "Error: Load feature.csv file error." << endl;
		vTotalImageScores.clear();
		vRatedImageScores.clear();
	}
	else
	{
		QTextStream _out(&featureFile);
		QString s = _out.readLine(); //remove title
		QStringList slist = s.split(",");
		int nFeatureDimension = slist.size() - 1;
		nFeatureCount = nFeatureDimension;
		cout << "Feature dimension = " << nFeatureDimension << endl;

		int nCount = 0;
		while (!_out.atEnd())
		{
			nCount++;
			s = _out.readLine();
			slist = s.split(",");
			if (slist.size() != nFeatureDimension + 1)
				continue;

			IMAGE_SCORE imgScore;
			imgScore.sfilename = slist[0].trimmed();


			imgScore.vFeatures.clear();
			for (int i = 1; i < slist.size(); i++)
				imgScore.vFeatures.append(slist[i].trimmed().toDouble());

			vTotalImageScores.append(imgScore);

			if (nCount % 500 == 0)
				cout<<((QString("Loading features of No. %1 image (%2). Total image count = %3.")\
					.arg(nCount).arg(slist[0]).arg(vTotalImageScores.size()))).toStdString().data()<<endl;

		}

		featureFile.close();

	}

	//load rated images
	QFile _f(sScoreFilename);
	if (_f.open(QIODevice::ReadOnly))
	{
		QTextStream _in(&_f);

		int nCount = 0;
		while (!_in.atEnd())
		{
			nCount++;
			QString s = _in.readLine();
			QStringList slist = s.split(",");

			int nIdx = vTotalImageScores.indexOf(IMAGE_SCORE(slist[0]));
			if (nIdx < 0)	continue;

			vTotalImageScores[nIdx].nScore = slist[1].trimmed().toDouble();
			vRatedImageScores.append(vTotalImageScores[nIdx]);

			if (nCount % 50 == 0)
				cout << QString("Loading rated scores of No. %1 image (%2).").arg(nCount).arg(slist[0]).toStdString().data() << endl;
		}

		_f.close();
	}
	
	cout << QString("Load total image count = %1, scored image count = %2")\
		.arg(vTotalImageScores.size()).arg(vRatedImageScores.size()).toStdString().data() << endl;

	return true;
	
}

int CalculateScoresBySelf()
{
	char* sFeatureFilename = "H:\\WuhanStreetview\\object_features.csv";
	char* sRatedFilename = "H:\\WuhanStreetview\\scores\\auto_save_scores_wealthy.csv";
	char* slogFilename = "H:\\WuhanStreetview\\object_total_scores\\training_wealthy.rfa";
	char* sOutputFilename = "H:\\WuhanStreetview\\object_total_scores\\total_sim_scores_wealthy.csv";

	QList<IMAGE_SCORE> vTotalImageScores;
	QList<IMAGE_SCORE> vRatedImageScores;
	int nFeatureDimension = 0;
	loadImageScoresFromFile(sFeatureFilename, sRatedFilename, nFeatureDimension, vTotalImageScores, vRatedImageScores);

	//training
	//构建数据集
	alglib::decisionforest RfFitter;
	alglib::ae_int_t nRfStatus;
	alglib::dfreport RfReport;
	alglib::real_2d_array arr;
	arr.setlength(vRatedImageScores.size(), nFeatureDimension + 1);
	int i, j;
	int nClass = 0;
	for (i = 0; i < vRatedImageScores.size(); i++)
	{
		for (j = 0; j < nFeatureDimension; j++)
			arr[i][j] = vRatedImageScores[i].vFeatures[j];
		arr[i][nFeatureDimension] = double(vRatedImageScores[i].nScore);
	}

	//训练
	dfbuildrandomdecisionforest(arr, vRatedImageScores.size(), nFeatureDimension, 1, 50, 0.66, nRfStatus, RfFitter, RfReport);

	//训练结果写入文件
	QFile _f(slogFilename);
	if (!_f.open(QIODevice::Append))
		return -1;
	QTextStream _in(&_f);
	_in << "******" << "\r\n";
	_in << QString("DATE AND TIME: %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")) << "\r\n";
	_in << QString("IUPUT DATA COUNT = %1").arg(vRatedImageScores.size()) << "\r\n";

	if (nRfStatus < 0)
	{
		cout << "The predictor trained fail." << endl;
		_in << QString("STATUS CODE = %1").arg(nRfStatus) << "\r\n";
	}
	else
	{
		cout << (QString("The predictor has been trained success. Rated data count = %2. The average error = %1. ")\
			.arg(RfReport.oobavgerror, 0, 'f', 6).arg(vRatedImageScores.size())).toStdString().data() << endl;;
		_in << QString("STATUS CODE = %1").arg(nRfStatus) << "\r\n";
		_in << QString("RFA AVGCE = %1").arg(RfReport.avgce, 0, 'f', 6) << "\r\n";
		_in << QString("RFA AVG ERROR = %1").arg(RfReport.avgerror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA AVG REL ERROR = %1").arg(RfReport.avgrelerror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA RMSE ERROR = %1").arg(RfReport.rmserror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA OOB AVGCE = %1").arg(RfReport.oobavgce, 0, 'f', 6) << "\r\n";
		_in << QString("RFA OOB AVG ERROR = %1").arg(RfReport.oobavgerror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA OOB AVG REL CLS ERROR = %1").arg(RfReport.oobrelclserror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA OOB RMSE ERROR = %1").arg(RfReport.oobrmserror, 0, 'f', 6) << "\r\n";

		_in << "******" << "\r\n";

		//计算训练权重
		cout << "calculating weights of spatial variables..." << endl;
		alglib::real_1d_array RFWeights;
		RFWeights.setlength(nFeatureDimension);
		CRFWeights::calRFWeights(arr, &RfFitter, RFWeights, RF_INTER);
		cout << "calculated weights done." << endl;

		//
		_in << "Variable Weights:" << "\r\n";
		for (int i = 0; i < nFeatureDimension; i++) {
			QString _s = QString("%1, %2").arg(i).arg(RFWeights[i], 0, 'f', 9);
			_in << _s << "\r\n";
		}


		_in.flush();

		_in << "******" << "\r\n";
		std::string sRFStructure;
		alglib::dfserialize(RfFitter, sRFStructure);
		_in << "Current RF Structure: " << "\r\n";
		_in << sRFStructure.data() << "\r\n";

	}

	_in << "******" << "\r\n\r\n\r\n";
	_in.flush();
	_f.flush();
	_f.close();

	cout << "The predictor has been trained success." << endl;

	//TODO: 读入其他特征集，重设vTotalImageScores


	//预测分数，写入
	QList<POS_SCORE> pos_scores;
	real_1d_array test_arr, y;
	test_arr.setlength(nFeatureDimension);
	int nCount = 0;
	foreach(IMAGE_SCORE isor, vTotalImageScores)
	{
		nCount++;
		for (j = 0; j < nFeatureDimension; j++)
			test_arr[j] = isor.vFeatures[j];

		POS_SCORE ps;
		QStringList _slist = isor.sfilename.split("_");
		if (_slist.size() < 3)
			continue;

		ps.mdLon = _slist[0].trimmed().toDouble();
		ps.mdLat = _slist[1].trimmed().toDouble();

		dfprocess(RfFitter, test_arr, y);
		double val = y[0];


		int ndirection = _slist[2].trimmed().toInt();
		switch (ndirection)
		{
		case 0:
			ps.mpDirection[0] = val;
			break;
		case 90:
			ps.mpDirection[1] = val;
			break;
		case 180:
			ps.mpDirection[2] = val;
			break;
		case 270:
			ps.mpDirection[3] = val;
			break;
		}

		int nIdx = pos_scores.indexOf(ps);
		if (nIdx < 0)
			pos_scores.append(ps);
		else
		{
			int ndirection = _slist[2].trimmed().toInt();
			switch (ndirection)
			{
			case 0:
				pos_scores[nIdx].mpDirection[0] = val;
				break;
			case 90:
				pos_scores[nIdx].mpDirection[1] = val;
				break;
			case 180:
				pos_scores[nIdx].mpDirection[2] = val;
				break;
			case 270:
				pos_scores[nIdx].mpDirection[3] = val;
				break;
			}
		}

		if (nCount % 500 == 0)
			cout << ((QString("Loading features of No. %1 image. Total image count = %3.")\
				.arg(nCount).arg(vTotalImageScores.size()))).toStdString().data() << endl;
	}

	//寻找最大值和平均值
	cout << "processing scores ..." << endl;
	//foreach(POS_SCORE ps, pos_scores)
	for (int k = 0; k < pos_scores.size(); k++)
	{
		double dMax = pos_scores[k].mpDirection[0];
		double dMin = pos_scores[k].mpDirection[0];
		double dAve = 0;

		int nValidIdxCount = 0;
		for (int i = 0; i < 4; i++)
		{
			dMax = (dMax < pos_scores[k].mpDirection[i] && pos_scores[k].mpDirection[i] >= 0) ? pos_scores[k].mpDirection[i] : dMax;
			dMin = (dMin > pos_scores[k].mpDirection[i] && pos_scores[k].mpDirection[i] >= 0) ? pos_scores[k].mpDirection[i] : dMin;

			if (pos_scores[k].mpDirection[i] < 0)
				continue;

			nValidIdxCount++;
			dAve += pos_scores[k].mpDirection[i];
		}

		if (nValidIdxCount != 0)
			dAve /= (double)nValidIdxCount;
		else
			dAve = 50.0;

		pos_scores[k].mdMax = dMax;
		pos_scores[k].mdMin = dMin;
		pos_scores[k].mdAve = dAve;
	}

	//写入文件
	cout << "writing file into file ..." << endl;
	QFile ofile(sOutputFilename);
	if (!ofile.open(QIODevice::WriteOnly))
	{
		cout << "create output file fail." << endl;
		return -3;
	}
	QTextStream _in0(&ofile);
	_in0 << "id, gcj_lon, gcj_lat, wgs_lon, wgs_lat, 0, 90, 180, 270, MAX_SCORES, MIN_SCORES, AVG_SCORES" << "\r\n";



	nCount = 0;
	foreach(POS_SCORE ps, pos_scores)
	{
		double wgs_lat, wgs_lon;
		WEBCSTRANSFORM::gcj2wgs(ps.mdLat, ps.mdLon, wgs_lat, wgs_lon);

		QString s = QString("%1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12")\
			.arg(nCount).arg(ps.mdLon, 0, 'f', 9).arg(ps.mdLat, 0, 'f', 9)\
			.arg(wgs_lon, 0, 'f', 9).arg(wgs_lat, 0, 'f', 9).arg(ps.mpDirection[0], 0, 'f', 6)\
			.arg(ps.mpDirection[1], 0, 'f', 6).arg(ps.mpDirection[2], 0, 'f', 6).arg(ps.mpDirection[3], 0, 'f', 6)\
			.arg(ps.mdMax, 0, 'f', 6).arg(ps.mdMin, 0, 'f', 6).arg(ps.mdAve, 0, 'f', 6);

		_in0 << s << "\r\n";

		nCount++;
		if (nCount % 1000 == 0)
			cout << QString("writing %1 / %2 data ...").arg(nCount).arg(pos_scores.size()).toStdString().data() << endl;
	}


	_in0.flush();
	ofile.flush();
	ofile.close();

	return 0;
}

int CalculateScoresByOtherAreaScores()
{
	//beautiful, boring, depressing, lively, safety, wealthy
	char* sTrainingFeatureFilename = "H:\\StreetviewFolder\\WuhanStreetview\\object_features.csv";
	char* sTrainingRatedFilename = "H:\\StreetviewFolder\\WuhanStreetview\\scores\\auto_save_scores_beautiful.csv";
	char* sPredictFeatureFilename = "H:\\StreetviewFolder\\hangzhouStreetview\\features\\object_features.csv";
	char* slogFilename = "H:\\StreetviewFolder\\hangzhouStreetview\\scores\\training_beautiful.rfa";
	char* sOutputFilename = "H:\\StreetviewFolder\\hangzhouStreetview\\scores\\total_sim_scores_beautiful.csv";
	

	QList<IMAGE_SCORE> vTotalImageScores;
	QList<IMAGE_SCORE> vRatedImageScores;
	int nFeatureDimension = 0;
	loadImageScoresFromFile(sTrainingFeatureFilename, sTrainingRatedFilename, nFeatureDimension, vTotalImageScores, vRatedImageScores);

	//training
	//构建数据集
	alglib::decisionforest RfFitter;
	alglib::ae_int_t nRfStatus;
	alglib::dfreport RfReport;
	alglib::real_2d_array arr;
	arr.setlength(vRatedImageScores.size(), nFeatureDimension + 1);
	int i, j;
	int nClass = 0;
	for (i = 0; i < vRatedImageScores.size(); i++)
	{
		for (j = 0; j < nFeatureDimension; j++)
			arr[i][j] = vRatedImageScores[i].vFeatures[j];
		arr[i][nFeatureDimension] = double(vRatedImageScores[i].nScore);
	}

	//训练
	dfbuildrandomdecisionforest(arr, vRatedImageScores.size(), nFeatureDimension, 1, 50, 0.66, nRfStatus, RfFitter, RfReport);

	//训练结果写入文件
	QFile _f(slogFilename);
	if (!_f.open(QIODevice::Append))
		return -1;
	QTextStream _in(&_f);
	_in << "******" << "\r\n";
	_in << QString("DATE AND TIME: %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")) << "\r\n";
	_in << QString("IUPUT DATA COUNT = %1").arg(vRatedImageScores.size()) << "\r\n";

	if (nRfStatus < 0)
	{
		cout << "The predictor trained fail." << endl;
		_in << QString("STATUS CODE = %1").arg(nRfStatus) << "\r\n";
	}
	else
	{
		cout << (QString("The predictor has been trained success. Rated data count = %2. The average error = %1. ")\
			.arg(RfReport.oobavgerror, 0, 'f', 6).arg(vRatedImageScores.size())).toStdString().data() << endl;;
		_in << QString("STATUS CODE = %1").arg(nRfStatus) << "\r\n";
		_in << QString("RFA AVGCE = %1").arg(RfReport.avgce, 0, 'f', 6) << "\r\n";
		_in << QString("RFA AVG ERROR = %1").arg(RfReport.avgerror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA AVG REL ERROR = %1").arg(RfReport.avgrelerror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA RMSE ERROR = %1").arg(RfReport.rmserror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA OOB AVGCE = %1").arg(RfReport.oobavgce, 0, 'f', 6) << "\r\n";
		_in << QString("RFA OOB AVG ERROR = %1").arg(RfReport.oobavgerror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA OOB AVG REL CLS ERROR = %1").arg(RfReport.oobrelclserror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA OOB RMSE ERROR = %1").arg(RfReport.oobrmserror, 0, 'f', 6) << "\r\n";

		_in << "******" << "\r\n";

		//计算训练权重
		cout << "calculating weights of spatial variables..." << endl;
		alglib::real_1d_array RFWeights;
		RFWeights.setlength(nFeatureDimension);
		CRFWeights::calRFWeights(arr, &RfFitter, RFWeights, RF_INTER);
		cout << "calculated weights done." << endl;

		//
		_in << "Variable Weights:" << "\r\n";
		for (int i = 0; i < nFeatureDimension; i++) {
			QString _s = QString("%1, %2").arg(i).arg(RFWeights[i], 0, 'f', 9);
			_in << _s << "\r\n";
		}


		_in.flush();

		_in << "******" << "\r\n";
		std::string sRFStructure;
		alglib::dfserialize(RfFitter, sRFStructure);
		_in << "Current RF Structure: " << "\r\n";
		_in << sRFStructure.data() << "\r\n";

	}

	_in << "******" << "\r\n\r\n\r\n";
	_in.flush();
	_f.flush();
	_f.close();

	cout << "The predictor has been trained success." << endl;

	//TODO: 读入其他特征集，重设vTotalImageScores
	cout << "Loading predict features ..." << endl;
	int nPredictFeatureDimension = 0;
	vTotalImageScores.clear();
	loadImageScoresOnly(sPredictFeatureFilename, nPredictFeatureDimension, vTotalImageScores);
	if (nPredictFeatureDimension != nFeatureDimension)
	{
		cout << "Error: The predict feature size is not same as input feature size." << endl;
		return -5;
	}
	cout << "Loaded predict features success." << endl;

	//预测分数，写入
	QList<POS_SCORE> pos_scores;
	real_1d_array test_arr, y;
	test_arr.setlength(nFeatureDimension);
	int nCount = 0;
	foreach(IMAGE_SCORE isor, vTotalImageScores)
	{
		nCount++;
		for (j = 0; j < nFeatureDimension; j++)
			test_arr[j] = isor.vFeatures[j];

		POS_SCORE ps;
		QStringList _slist = isor.sfilename.split("_");
		if (_slist.size() < 3)
			continue;

		ps.mdLon = _slist[0].trimmed().toDouble();
		ps.mdLat = _slist[1].trimmed().toDouble();

		dfprocess(RfFitter, test_arr, y);
		double val = y[0];


		int ndirection = _slist[2].trimmed().toInt();
		switch (ndirection)
		{
		case 0:
			ps.mpDirection[0] = val;
			break;
		case 90:
			ps.mpDirection[1] = val;
			break;
		case 180:
			ps.mpDirection[2] = val;
			break;
		case 270:
			ps.mpDirection[3] = val;
			break;
		}

		int nIdx = pos_scores.indexOf(ps);
		if (nIdx < 0)
			pos_scores.append(ps);
		else
		{
			int ndirection = _slist[2].trimmed().toInt();
			switch (ndirection)
			{
			case 0:
				pos_scores[nIdx].mpDirection[0] = val;
				break;
			case 90:
				pos_scores[nIdx].mpDirection[1] = val;
				break;
			case 180:
				pos_scores[nIdx].mpDirection[2] = val;
				break;
			case 270:
				pos_scores[nIdx].mpDirection[3] = val;
				break;
			}
		}

		if (nCount % 500 == 0)
			cout << ((QString("Loading features of No. %1 image. Total image count = %3.")\
				.arg(nCount).arg(vTotalImageScores.size()))).toStdString().data() << endl;
	}

	//寻找最大值和平均值
	cout << "processing scores ..." << endl;
	//foreach(POS_SCORE ps, pos_scores)
	for (int k = 0; k < pos_scores.size(); k++)
	{
		double dMax = pos_scores[k].mpDirection[0];
		double dMin = pos_scores[k].mpDirection[0];
		double dAve = 0;

		int nValidIdxCount = 0;
		for (int i = 0; i < 4; i++)
		{
			dMax = (dMax < pos_scores[k].mpDirection[i] && pos_scores[k].mpDirection[i] >= 0) ? pos_scores[k].mpDirection[i] : dMax;
			dMin = (dMin > pos_scores[k].mpDirection[i] && pos_scores[k].mpDirection[i] >= 0) ? pos_scores[k].mpDirection[i] : dMin;

			if (pos_scores[k].mpDirection[i] < 0)
				continue;

			nValidIdxCount++;
			dAve += pos_scores[k].mpDirection[i];
		}

		if (nValidIdxCount != 0)
			dAve /= (double)nValidIdxCount;
		else
			dAve = 50.0;

		pos_scores[k].mdMax = dMax;
		pos_scores[k].mdMin = dMin;
		pos_scores[k].mdAve = dAve;
	}

	//写入文件
	cout << "writing file into file ..." << endl;
	QFile ofile(sOutputFilename);
	if (!ofile.open(QIODevice::WriteOnly))
	{
		cout << "create output file fail." << endl;
		return -3;
	}
	QTextStream _in0(&ofile);
	_in0 << "id, gcj_lon, gcj_lat, wgs_lon, wgs_lat, 0, 90, 180, 270, MAX_SCORES, MIN_SCORES, AVG_SCORES" << "\r\n";



	nCount = 0;
	foreach(POS_SCORE ps, pos_scores)
	{
		double wgs_lat, wgs_lon;
		WEBCSTRANSFORM::gcj2wgs(ps.mdLat, ps.mdLon, wgs_lat, wgs_lon);

		QString s = QString("%1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12")\
			.arg(nCount).arg(ps.mdLon, 0, 'f', 9).arg(ps.mdLat, 0, 'f', 9)\
			.arg(wgs_lon, 0, 'f', 9).arg(wgs_lat, 0, 'f', 9).arg(ps.mpDirection[0], 0, 'f', 6)\
			.arg(ps.mpDirection[1], 0, 'f', 6).arg(ps.mpDirection[2], 0, 'f', 6).arg(ps.mpDirection[3], 0, 'f', 6)\
			.arg(ps.mdMax, 0, 'f', 6).arg(ps.mdMin, 0, 'f', 6).arg(ps.mdAve, 0, 'f', 6);

		_in0 << s << "\r\n";

		nCount++;
		if (nCount % 1000 == 0)
			cout << QString("writing %1 / %2 data ...").arg(nCount).arg(pos_scores.size()).toStdString().data() << endl;
	}


	_in0.flush();
	ofile.flush();
	ofile.close();

	return 0;
}

int StatisticFeaturesbyAve()
{
	char* sFeatureFilename = "H:\\StreetviewFolder\\hangzhouStreetview\\features\\object_features.csv";
	char* sOutputFilename = "H:\\StreetviewFolder\\hangzhouStreetview\\scores\\total_features_statistic.csv";

	// 保存到ImageScores
	QList<IMAGE_SCORE> vTotalImageScores;
	int nFeatureDimension = 0;
	loadImageScoresOnly(sFeatureFilename, nFeatureDimension, vTotalImageScores);

	if (nFeatureDimension != FEATURES_COUNT)
	{
		cout << "unknown feature count. Actual Feature Dimension = " << nFeatureDimension << endl;
		return -1;
	}

	QList<POS_SCORE> pos_scores;
	int nCount = 0;

	foreach(IMAGE_SCORE isor, vTotalImageScores)
	{
		nCount++;

		if (nCount % 1000 == 0)
			cout << "calculating No. " << nCount << " / " << vTotalImageScores.size() << " ..." << endl;

		POS_SCORE ps;
		QStringList _slist = isor.sfilename.split("_");
		if (_slist.size() < 3)
			continue;

		ps.mdLon = _slist[0].trimmed().toDouble();
		ps.mdLat = _slist[1].trimmed().toDouble();		

		int nIdx = pos_scores.indexOf(ps);
		if (nIdx < 0)
		{
			for (int i = 0; i < nFeatureDimension; i++)
				ps.mpFeatures[i] += isor.vFeatures[i];
			pos_scores.append(ps);
		}
		else
		{
			for (int i = 0; i < nFeatureDimension; i++)
				pos_scores[nIdx].mpFeatures[i] += isor.vFeatures[i];
		}
	}

	//对每个除以4处理
	cout << "processing scores ..." << endl;
	//foreach(POS_SCORE ps, pos_scores)
	for (int k = 0; k < pos_scores.size(); k++)
	{
		for (int i = 0; i < nFeatureDimension; i++)
			pos_scores[k].mpFeatures[i] /= 4.0f;
	}


	//写入文件
	cout << "writing file into file ..." << endl;
	QFile ofile(sOutputFilename);
	if (!ofile.open(QIODevice::WriteOnly))
	{
		cout << "create output file fail." << endl;
		return -3;
	}
	QTextStream _in0(&ofile);
	QString stitle = "id, gcj_lon, gcj_lat, wgs_lon, wgs_lat, entropy";
	for (int i = 0; i < nFeatureDimension; i++)
		stitle += QString(", id_%1").arg(i);
	_in0 << stitle << "\r\n";



	nCount = 0;
	foreach(POS_SCORE ps, pos_scores)
	{
		double wgs_lat, wgs_lon;
		WEBCSTRANSFORM::gcj2wgs(ps.mdLat, ps.mdLon, wgs_lat, wgs_lon);

		//计算熵
		double dEntropy = 0.0f;
		for (int i = 0; i<nFeatureDimension; i++)
		{
			if (ps.mpFeatures[i] <= 10e-6)
				continue;
			dEntropy += ps.mpFeatures[i] * log(ps.mpFeatures[i]);
		}
		dEntropy = -dEntropy;

		//输出
		QString s = QString("%1, %2, %3, %4, %5, %6")\
			.arg(nCount).arg(ps.mdLon, 0, 'f', 9).arg(ps.mdLat, 0, 'f', 9)\
			.arg(wgs_lon, 0, 'f', 9).arg(wgs_lat, 0, 'f', 9).arg(dEntropy, 0, 'f', 9);

		for (int i = 0; i < nFeatureDimension; i++)
			s += QString(", %1").arg(ps.mpFeatures[i], 0, 'f', 6);

		_in0 << s << "\r\n";

		nCount++;
		if (nCount % 1000 == 0)
			cout << QString("writing %1 / %2 data ...").arg(nCount).arg(pos_scores.size()).toStdString().data() << endl;
	}


	_in0.flush();
	ofile.flush();
	ofile.close();


	return 1;
}

int main(int argc, char *argv[])
{	
	//统计各类地物占比
	StatisticFeaturesbyAve();
	//心理学感知
	CalculateScoresByOtherAreaScores();
	return 1;
}
