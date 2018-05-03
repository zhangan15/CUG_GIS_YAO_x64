#include "TrainingThread.h"
#include "StreetViewRatingApp.h"
#include <QtGui>
#include <QtCore>
#include "alglib/dataanalysis.h"

TrainingThread::TrainingThread(QObject *parent)
	: QThread(parent)
{
	mpApp = (StreetViewRatingApp*)parent;
}

TrainingThread::~TrainingThread()
{
}

void TrainingThread::run()
{
	//构建数据集
	alglib::real_2d_array arr;
	arr.setlength(mpApp->mvScoredImg.size(), mpApp->mnFeatureDimension + 1);
	int i, j;
	int nClass = 0;
	for (i = 0; i < mpApp->mvScoredImg.size(); i++)
	{
		for (j = 0; j < mpApp->mnFeatureDimension; j++)
			arr[i][j] = mpApp->mvScoredImg[i].vFeatures[j];
		arr[i][mpApp->mnFeatureDimension] = int(double(mpApp->mvScoredImg[i].nScore) / mpApp->mdRatio + 0.5);
		//nClass = nClass < mpApp->mvScoredImg[i].nScore ? mpApp->mvScoredImg[i].nScore : nClass;
	}


	mpApp->mnRfStatus = -1;
	alglib::dfbuildrandomdecisionforest(arr, mpApp->mvScoredImg.size(), mpApp->mnFeatureDimension, 1, 50, 0.66, mpApp->mnRfStatus, mpApp->mdRfFitter, mpApp->mRfReport);

	emit finished();
}
