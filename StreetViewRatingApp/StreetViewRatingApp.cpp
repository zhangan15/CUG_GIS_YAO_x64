#include "StreetViewRatingApp.h"
#include <QtGui>
#include <QtCore>
#include <QFileDialog>
#include <QMessageBox>

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
	openDir();
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

	QStringList suffixList;
	suffixList << "*.bmp" << "*.png" << "*.jpg" << "*.tif";
	QFileInfoList filist = QDir(dir).entryInfoList(suffixList);

	ui.statusLbl->setText(QString("Load %1 images.").arg(filist.size()));

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

	showImg();
}

void StreetViewRatingApp::showImg()
{
	if (mvImgScores.size() == 0)
		return;

	while (1)
	{
		int nId = rand() % mvImgScores.size();
		if (mvImgScores[nId].nScore >= 0 )
			continue;

		QImage img;
		if (!img.load(mvImgScores[nId].sfilename))
			continue;
		
		ui.imgLbl->setPixmap(QPixmap::fromImage(img));
		ui.filepathLbl->setText(QString("Image ID: %1, %2").arg(nId).arg(mvImgScores[nId].sfilename));
		mnCurID = nId;
		break;		
	}
}

void StreetViewRatingApp::rating()
{
	if (mvImgScores.size() == 0)
		return;
	
	showImg();
	mvImgScores[mnCurID].nScore = ui.valueBox->value();
	mvScoredImg.append(mvImgScores[mnCurID]);

	ui.statusLbl->setText(QString("You have rated %1 images (total count = %2).").arg(mvScoredImg.size()).arg(mvImgScores.size()));
	ui.nextBtn->setFocus();
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
}
