#pragma once

#include <QtWidgets/QDialog>
#include "ui_StreetViewRatingApp.h"

#include <QString>
#include <QList>

#include "alglib/statistics.h"
#include "alglib/dataanalysis.h"
#include "alglib/alglibmisc.h"

class LoadFeatureThread;

class IMAGE_SCORE 
{
public:
	IMAGE_SCORE() { sfilename = "", nScore = -1; vFeatures.clear(); }
	~IMAGE_SCORE() { sfilename = "", nScore = -1; vFeatures.clear(); };
	IMAGE_SCORE(QString sName) { sfilename = sName.trimmed()+".addsuffix", nScore = -1; vFeatures.clear(); }

public:
	bool operator==(IMAGE_SCORE s);

public:
	QString sfilename;
	float nScore;
	QList<float> vFeatures;	//find features.csv
};

class StreetViewRatingApp : public QDialog
{
	Q_OBJECT

public:
	StreetViewRatingApp(QWidget *parent = Q_NULLPTR);
	~StreetViewRatingApp();

protected slots:
	void openDir();

protected:
	void showImg();
	void rating();
	void saveData();
	void autoSaveData(QString sfilename);

private:
	void closeEvent(QCloseEvent *e);
	void keyPressEvent(QKeyEvent *e);

public:
	void trainRfPredictor();


public:
	Ui::StreetViewRatingAppClass ui;
	QList<IMAGE_SCORE> mvImgScores;
	QList<IMAGE_SCORE> mvScoredImg;
	int mnCurID;
	QString msCurDir;

public:
	alglib::decisionforest mdRfFitter;
	alglib::ae_int_t mnRfStatus;
	alglib::dfreport mRfReport;
	int mnAddDataCount;
	double mdUserError;
	double mdUserErrorThreshold;	// average error per mnAddDataCount
	double mdCurSimValue;
	int mnFeatureDimension;
	int mnGapImageNum;
	int mnStartImageNum;
	QString msLogFile;
	double mdRatio;

	alglib::mlptrainer mNNtrn;
	alglib::multilayerperceptron mNetwork;
	alglib::mlpreport mNNRep;

	//LoadFeatureThread* mpFeatureThread;
	bool mbCanRating;


};
