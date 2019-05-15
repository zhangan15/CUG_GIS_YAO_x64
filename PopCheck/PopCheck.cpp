#include "PopCheck.h"

PopCheck::PopCheck(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	msInNames.clear();
	//msOutNames.clear();
	msChNames.clear();
	msInNameList.clear();
	msOutNameList.clear();
	msChNameList.clear();

	connect(ui.actionCheck, &QAction::triggered, this, &PopCheck::display);
}

void PopCheck::makeInNameList()
{
	msInNames = ui.TextIn->toPlainText().trimmed().toLower();
	msInNameList = msInNames.split(QRegExp("\\d|\\W+"), QString::SkipEmptyParts);
}

void PopCheck::makeCheckNameList()
{
	msChNames = ui.TextCheck->toPlainText().trimmed().toLower();
	msChNameList = msChNames.split(QRegExp("\\d|\\W+"), QString::SkipEmptyParts);
}

void PopCheck::compOutNameList()
{
	if (msInNameList.isEmpty() || msChNameList.isEmpty())
		return;

	msOutNameList.clear();

	for (QString ch : msInNameList)
	{
		if (!msChNameList.contains(ch))
			msOutNameList.append(ch);
	}


}

void PopCheck::display()
{
	makeInNameList();
	makeCheckNameList();
	compOutNameList();

	ui.TextOut->clear();
	for (QString ch : msOutNameList)
	{
		ui.TextOut->appendPlainText(ch);
	}
}

