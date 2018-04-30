#pragma once

#include <QtWidgets/QDialog>
#include "ui_StreetViewRatingApp.h"

#include <QString>
#include <QList>

struct IMAGE_SCORE 
{
	QString sfilename;
	float nScore;
};

class StreetViewRatingApp : public QDialog
{
	Q_OBJECT

public:
	StreetViewRatingApp(QWidget *parent = Q_NULLPTR);

protected slots:
	void openDir();

protected:
	void showImg();
	void rating();
	void saveData();

	void closeEvent(QCloseEvent *e);


private:
	Ui::StreetViewRatingAppClass ui;
	QList<IMAGE_SCORE> mvImgScores;
	QList<IMAGE_SCORE> mvScoredImg;
	int mnCurID;
	QString msCurDir;
};
