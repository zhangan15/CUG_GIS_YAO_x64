#include "StreetViewRatingApp.h"
#include <QtGui>
#include <QtCore>
#include <QFileDialog>
#include <QMessageBox>

bool IMAGE_SCORE::operator==(IMAGE_SCORE s)
{
	if (s.sfilename.trimmed().isEmpty())
		return false;
	QFileInfo fi(sfilename);
	QFileInfo fi2(s.sfilename);
	QString s1 = fi.completeBaseName().trimmed().toLower();
	QString s2 = fi2.completeBaseName().trimmed().toLower();
	if (s1==s2) //fi.completeBaseName().trimmed().toLower() == fi2.completeBaseName().trimmed().toLower())
		return true;
	else
		return false;
}

StreetViewRatingApp::StreetViewRatingApp(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	srand(QTime::currentTime().msec());

	connect(ui.valueSlider, &QSlider::valueChanged, ui.valueBox, &QSpinBox::setValue);
	connect(ui.valueBox, SIGNAL(valueChanged(int)), ui.valueSlider, SLOT(setValue(int)));
	connect(ui.loadDirBtn, &QPushButton::clicked, this, &StreetViewRatingApp::openDir);
	connect(ui.nextBtn, &QPushButton::clicked, this, &StreetViewRatingApp::rating);
	connect(ui.skipBtn, &QPushButton::clicked, this, &StreetViewRatingApp::showImg);
	connect(ui.saveBtn, &QPushButton::clicked, this, &StreetViewRatingApp::saveData);

	ui.imgLbl->setScaledContents(true);
	mnCurID = 0;

	// request open a dir when start program
	//openDir();

	//random forest
	mnRfStatus = -1;
	mnAddDataCount = 0;
	mdUserError = 0.0f;
	mnFeatureDimension = 0;
	mdRatio = 100.0f;

	//need set params
	mnStartImageNum = 50;
	mnGapImageNum = 5;	
	mdUserErrorThreshold = 30;

}

void StreetViewRatingApp::openDir()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Image Directory"),	QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isEmpty())
	{
		ui.statusLbl->setText(QString("Unknown Dir."));
		return;
	}

	msCurDir = dir;
	msLogFile = msCurDir + "/rating_record.rfa";

	QStringList suffixList;
	suffixList << "*.bmp" << "*.png" << "*.jpg" << "*.tif";
	QFileInfoList filist = QDir(dir).entryInfoList(suffixList);

	

	//save msg to the file list
	mvImgScores.clear();
	mvScoredImg.clear();
	foreach(QFileInfo fi, filist)
	{
		IMAGE_SCORE imgSr;
		imgSr.sfilename = fi.absoluteFilePath();
		imgSr.nScore = -1;
		mvImgScores.append(imgSr);
	}

	//load features.csv
	QFile featureFile(dir+"./features.csv");
	if (!featureFile.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, "Missing feature file", "load feature.csv file error.");
		mvImgScores.clear();
	}
	else 
	{
		QTextStream _out(&featureFile);
		QString s = _out.readLine(); //remove title
		QStringList slist = s.split(",");
		mnFeatureDimension = slist.size() - 1;

		while (!_out.atEnd())
		{
			s = _out.readLine();
			slist = s.split(",");
			if (slist.size() != mnFeatureDimension + 1)
				continue;

			int nIdx = mvImgScores.indexOf(IMAGE_SCORE(slist[0]));
			if (nIdx < 0)	continue;


			mvImgScores[nIdx].vFeatures.clear();
			for (int i = 1; i < slist.size(); i++)
				mvImgScores[nIdx].vFeatures.append(slist[i].trimmed().toDouble());

		}

		featureFile.close();

	}

	while (1)
	{
		int i = 0;
		for (i = 0; i < mvImgScores.size(); i++)
		{
			if (mvImgScores[i].vFeatures.size() == 0)
			{
				mvImgScores.removeAt(i);
				break;
			}
		}

		if (i == mvImgScores.size())
			break;
	}

	//load rated images
	if (QFile::exists(msCurDir + "./auto_save_scores.csv"))
	{
		QFile _f(msCurDir + "./auto_save_scores.csv");
		if (_f.open(QIODevice::ReadOnly))
		{
			QTextStream _in(&_f);
		
			while (!_in.atEnd())
			{
				QString s = _in.readLine();
				QStringList slist = s.split(",");

				int nIdx = mvImgScores.indexOf(IMAGE_SCORE(slist[0]));
				if (nIdx < 0)	continue;

				mvImgScores[nIdx].nScore = slist[1].trimmed().toDouble();
				mvScoredImg.append(mvImgScores[nIdx]);
			}

			_f.close();
			trainRfPredictor();
		}
	}
	
	mnAddDataCount = 0;
	ui.statusLbl->setText(QString("Load %1 images with %2-D features. %3 images have been rated.").arg(mvImgScores.size()).arg(mnFeatureDimension).arg(mvScoredImg.size()));
	showImg();
}

void StreetViewRatingApp::showImg()
{
	if (mvImgScores.size() == 0)
		return;

	while (1)
	{
		int nId = rand() % mvImgScores.size();

		//find unrated image
		if (mvImgScores[nId].nScore >= 0)
		{
			//if all images have been scored, then exit.
			if (mvScoredImg.size() == mvImgScores.size())
			{
				ui.statusLbl->setText(QString("All %1 images have been rated. Please save scores to file. Thank you.").arg(mvImgScores.size()));
				break;
			}
			continue;
		}

		QImage img;
		if (!img.load(mvImgScores[nId].sfilename))
			continue;
		
		ui.imgLbl->setPixmap(QPixmap::fromImage(img));
		ui.filepathLbl->setText(QString("Image ID: %1, %2").arg(nId).arg(mvImgScores[nId].sfilename));
		mnCurID = nId;

		if (mnRfStatus > 0)
		{
			alglib::real_1d_array inData, simScore;
			inData.setlength(mnFeatureDimension);
			for (int i = 0; i < mnFeatureDimension; i++)
				inData[i] = mvImgScores[nId].vFeatures[i];
			alglib::dfprocess(mdRfFitter, inData, simScore);
			double dval = simScore[0] * mdRatio;
			if (dval < 0) dval = 0;
			if (dval > 100) dval = 100;
			int simval = int(dval + 0.5);
			mdCurSimValue = dval;
			ui.valueBox->setValue(simval);
		}
		else
			ui.valueBox->setValue(50);

		break;		
	}

	
}

void StreetViewRatingApp::rating()
{
	if (mvImgScores.size() == 0)
		return;
	
	mvImgScores[mnCurID].nScore = ui.valueBox->value();
	mvScoredImg.append(mvImgScores[mnCurID]);

	ui.statusLbl->setText(QString("You have rated %1 images (total count = %2).").arg(mvScoredImg.size()).arg(mvImgScores.size()));
	ui.nextBtn->setFocus();
	
	if (mnRfStatus > 0)
		mdUserError += abs(mvImgScores[mnCurID].nScore - mdCurSimValue);
	
	if (mnAddDataCount >= mnGapImageNum)
	{
		mdUserError /= (double)mnGapImageNum;
		mdUserError *= mdRatio;
		ui.statusLbl->setText(QString("You have rated %1 images (total count = %2). Average current error = %3")\
			.arg(mvScoredImg.size()).arg(mvImgScores.size()).arg(mdUserError, 0, 'f', 3));
	}
		
	mnAddDataCount++;
	if (mnAddDataCount >= mnGapImageNum /*|| mdUserError >= mdUserErrorThreshold*/)
		trainRfPredictor();

	showImg();
}

void StreetViewRatingApp::saveData()
{
	QString savefilepath = QFileDialog::getSaveFileName(this, "Save rating file", msCurDir, "*.csv");
	if (savefilepath.isEmpty())
		return;

	QFile _f(savefilepath);
	if (!_f.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(this, "error", "save file create fail.");
		return;
	}
	QTextStream _in(&_f);
	_in << "filename, score" << "\r\n";

	foreach (IMAGE_SCORE imso, mvScoredImg)
	{
		QString str = QString("%1, %2").arg(QFileInfo(imso.sfilename).completeBaseName()).arg(imso.nScore);
		_in << str << "\r\n";
	}

	_in.flush();
	_f.flush();
	_f.close();
	
	ui.statusLbl->setText(QString("Save scores to %1 success. Thank you.").arg(savefilepath));
}

void StreetViewRatingApp::autoSaveData(QString sfilename)
{
	QFile _f(sfilename);
	if (!_f.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(this, "error", "save file create fail.");
		return;
	}
	QTextStream _in(&_f);
	_in << "filename, score" << "\r\n";

	foreach(IMAGE_SCORE imso, mvScoredImg)
	{
		QString str = QString("%1, %2").arg(QFileInfo(imso.sfilename).completeBaseName()).arg(imso.nScore);
		_in << str << "\r\n";
	}

	_in.flush();
	_f.flush();
	_f.close();
}

void StreetViewRatingApp::closeEvent(QCloseEvent *e)
{
	QMessageBox::StandardButton stdbtn = QMessageBox::warning(this, "warning", "You will exit the program. Do you need to save the score?", QMessageBox::Save | QMessageBox::Discard);
	if (stdbtn == QMessageBox::Save)
		saveData();

	e->accept();
}

void StreetViewRatingApp::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_S)
		showImg();
	else if (e->key() == Qt::Key_R)
		rating();

	switch (e->key())
	{
	case Qt::Key_0:
		ui.valueBox->setValue(0);
		break;
	case Qt::Key_1:
		ui.valueBox->setValue(10);
		break;
	case Qt::Key_2:
		ui.valueBox->setValue(20);
		break;
	case Qt::Key_3:
		ui.valueBox->setValue(30);
		break;
	case Qt::Key_4:
		ui.valueBox->setValue(40);
		break;
	case Qt::Key_5:
		ui.valueBox->setValue(50);
		break;
	case Qt::Key_6:
		ui.valueBox->setValue(60);
		break;
	case Qt::Key_7:
		ui.valueBox->setValue(70);
		break;
	case Qt::Key_8:
		ui.valueBox->setValue(80);
		break;
	case Qt::Key_9:
		ui.valueBox->setValue(90);
		break;
	case Qt::Key_Equal:
		ui.valueBox->setValue(ui.valueBox->value() + 5);
		break;
	case Qt::Key_Minus:
		ui.valueBox->setValue(ui.valueBox->value() - 5);
		break;		
	}
}

void StreetViewRatingApp::trainRfPredictor()
{
	if (mvScoredImg.size() < mnStartImageNum)
	{
		//ui.statusLbl->setText("The total rated image is too small, the predictor will not train.");
		return;
	}

	ui.statusLbl->setText("The predictor is training, please waiting...");

	//构建数据集
	alglib::real_2d_array arr;
	arr.setlength(mvScoredImg.size(), mnFeatureDimension + 1);
	int i, j;
	for (i=0; i<mvScoredImg.size(); i++)
	{
		for (j=0; j<mnFeatureDimension; j++)
			arr[i][j] = mvScoredImg[i].vFeatures[j];
		arr[i][mnFeatureDimension] = double(mvImgScores[i].nScore)/mdRatio;
	}
	

	mnRfStatus = -1;	
	alglib::dfbuildrandomdecisionforest(arr, mvScoredImg.size(), mnFeatureDimension, 1, 50, 0.66, mnRfStatus, mdRfFitter, mRfReport);
	

	// write to log file
	QFile _f(msLogFile);
	if (!_f.open(QIODevice::Append))
		return;
	QTextStream _in(&_f);
	_in << "******" << "\r\n";
	_in << QString("DATE AND TIME: %1").arg(QDateTime::currentDateTime().toString("YY-MM-DD hh:mm:ss")) << "\r\n";
	_in << QString("IUPUT DATA COUNT = %1").arg(mvScoredImg.size()) << "\r\n";

	if (mnRfStatus < 0)
	{
		ui.statusLbl->setText("The predictor has been trained fail.");
		_in << QString("STATUS CODE = %1").arg(mnRfStatus) << "\r\n";
	}
	else
	{
		ui.statusLbl->setText(QString("The predictor has been trained success. Rated data count = %2. The average error = %1. ")\
			.arg(mRfReport.oobavgerror, 0, 'f', 6).arg(mvScoredImg.size()));
		_in << QString("STATUS CODE = %1").arg(mnRfStatus) << "\r\n";
		_in << QString("CUR USER ERROR = %1").arg(mdUserError, 0, 'f', 6) << "\r\n";
		_in << QString("RFA AVGCE = %1").arg(mRfReport.avgce, 0, 'f', 6) << "\r\n";
		_in << QString("RFA AVG ERROR = %1").arg(mRfReport.avgerror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA AVG REL ERROR = %1").arg(mRfReport.avgrelerror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA RMSE ERROR = %1").arg(mRfReport.rmserror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA OOB AVGCE = %1").arg(mRfReport.oobavgce, 0, 'f', 6) << "\r\n";
		_in << QString("RFA OOB AVG ERROR = %1").arg(mRfReport.oobavgerror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA OOB AVG REL CLS ERROR = %1").arg(mRfReport.oobrelclserror, 0, 'f', 6) << "\r\n";
		_in << QString("RFA OOB RMSE ERROR = %1").arg(mRfReport.oobrmserror, 0, 'f', 6) << "\r\n";		

		std::string sRFStructure;
		alglib::dfserialize(mdRfFitter, sRFStructure);
		_in << "Current RF Structure: " << "\r\n";
		_in << sRFStructure.data() << "\r\n";

	}

	_in << "******" << "\r\n\r\n\r\n";
	_in.flush();
	_f.flush();
	_f.close();

	autoSaveData(msCurDir + "./auto_save_scores.csv");

	mdUserError = 0;
	mnAddDataCount = 0;
}

