#include "LoadFeatureThread.h"
#include "StreetViewRatingApp.h"
#include <QtGui>
#include <QtCore>


LoadFeatureThread::LoadFeatureThread(QObject *parent)
	: QThread(parent)
{
	mpApp = (StreetViewRatingApp*)parent;
}

LoadFeatureThread::~LoadFeatureThread()
{
}


void LoadFeatureThread::run()
{
	emit sendMsg("Loading image features...");
	QFile featureFile(mpApp->msCurDir + "./features.csv");
	if (!featureFile.open(QIODevice::ReadOnly))
	{
		emit sendMsg("Error: Load feature.csv file error.");
		mpApp->mvImgScores.clear();
	}
	else
	{
		QTextStream _out(&featureFile);
		QString s = _out.readLine(); //remove title
		QStringList slist = s.split(",");
		mpApp->mnFeatureDimension = slist.size() - 1;

		int nCount = 0;
		while (!_out.atEnd())
		{
			nCount++;
			s = _out.readLine();
			slist = s.split(",");
			if (slist.size() != mpApp->mnFeatureDimension + 1)
				continue;

			//int nIdx = mpApp->mvImgScores.indexOf(IMAGE_SCORE(slist[0]));
			//if (nIdx < 0)	continue;

			QString sCurName = slist[0].trimmed().toLower();//QFileInfo(slist[0]).completeBaseName().trimmed().toLower();

			int nIdx = -1;
			if (mpApp->mvImgNameHash.contains(sCurName))
				nIdx = mpApp->mvImgNameHash[sCurName];
			else
				continue;


			mpApp->mvImgScores[nIdx].vFeatures.clear();
			for (int i = 1; i < slist.size(); i++)
				mpApp->mvImgScores[nIdx].vFeatures.append(slist[i].trimmed().toDouble());

			if (nCount % 100 == 0)
				emit sendMsg(QString("Loading features of No. %1 image (%2). Total image count = %3.")\
					.arg(nCount).arg(slist[0]).arg(mpApp->mvImgScores.size()));

		}

		featureFile.close();

	}

	while (1)
	{
		int i = 0;
		for (i = 0; i < mpApp->mvImgScores.size(); i++)
		{
			if (mpApp->mvImgScores[i].vFeatures.size() == 0)
			{
				mpApp->mvImgScores.removeAt(i);
				break;
			}
		}

		if (i == mpApp->mvImgScores.size())
			break;
	}

	//
	mpApp->mvImgNameHash.clear();
	for (int i = 0; i < mpApp->mvImgScores.size(); i++)
		mpApp->mvImgNameHash.insert(QFileInfo(mpApp->mvImgScores[i].sfilename).completeBaseName().trimmed().toLower(), i);
	

	//load rated images
	if (QFile::exists(mpApp->msCurDir + "./auto_save_scores.csv"))
	{
		
		QFile _f(mpApp->msCurDir + "./auto_save_scores.csv");
		if (_f.open(QIODevice::ReadOnly))
		{
			QTextStream _in(&_f);

			int nCount = 0;
			while (!_in.atEnd())
			{
				nCount++;
				QString s = _in.readLine();
				QStringList slist = s.split(",");

				QString sCurName = slist[0].trimmed().toLower();
				int nIdx = -1;
				if (mpApp->mvImgNameHash.contains(sCurName))
					nIdx = mpApp->mvImgNameHash[sCurName];
				else
					continue;
				

// 				int nIdx = mpApp->mvImgScores.indexOf(IMAGE_SCORE(slist[0]));
// 				if (nIdx < 0)	continue;

				mpApp->mvImgScores[nIdx].nScore = slist[1].trimmed().toDouble();
				mpApp->mvScoredImg.append(mpApp->mvImgScores[nIdx]);

				if (nCount % 50 == 0)
					emit sendMsg(QString("Loading rated scores of No. %1 image (%2).").arg(nCount).arg(slist[0]));
			}

			_f.close();
			mpApp->trainRfPredictor();		

		}
	}

	emit finished();
}
