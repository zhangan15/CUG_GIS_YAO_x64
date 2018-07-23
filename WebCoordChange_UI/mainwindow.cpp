#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = 0;
    flags |= Qt::WindowMinimizeButtonHint;
    setWindowFlags(flags); // 设置禁止最大化
    setFixedSize(400,325); // 禁止改变窗口大小。
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    mbSkipHead = true;
    mnEncoding = 1;
    mnSeparation = 1;

    mnLngCol = 0;
    mnLatCol = 1;

    mnFromCoord = 1;
    mnToCoord = 1;

    QStringList vEncoding;
    vEncoding<<"UTF-8"; //<< "GB2312"
    ui->encoding_comboBox->addItems(vEncoding);

    ui->lngColBox->addItems(QStringList("0"));
    ui->latColBox->addItems(QStringList("0"));

    QStringList vCoords;
    vCoords<<"BD 09"<<"GCJ 02"<<"WGS 1984";
    ui->fromCoordBox->addItems(vCoords);
    ui->toCoordBox->addItems(vCoords);

	QObject::connect(ui->actionwhat_is_it, SIGNAL(triggered()), this, SLOT(aboutThis()));
	QObject::connect(ui->actionexit, SIGNAL(triggered()), this, SLOT(close()));

}


void MainWindow::aboutThis()
 {
    QString strAbout;
	strAbout = tr("          * Only for Text Files(eg. txt,csv, etc.)\n* * * * * Location Information is International Confidential!\n* * * * * DO NOT SPREAD!\n* * * * * DO NOT SPREAD!\n* * * * * DO NOT SPREAD!\nOnly for academic research and prohibited for commercial use\nAuthor: Penghua Liu(liuph3@mail2.sysu.edu.cn)");
    QMessageBox::information(this, tr("what is it?"), strAbout.toLocal8Bit());
 }

void MainWindow::on_loadFileBtn_clicked()
{
    QString searchPath = ".";
    QString qfn = QFileDialog::getOpenFileName(this,
            tr("Open File"),
            searchPath,
            tr("*.csv; *.txt")); //选择路径
    mstrInputFile = qfn;
    ui->fileLineEdit->setText(qfn);

    QFile infile(mstrInputFile);
	//空文件名;
	if (mstrInputFile == "")
		return;
    if (!infile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("Error"), "File can not open!");
    }
    QTextStream _in(&infile);
    QString smsg = _in.readLine();

    if (ui->skipHeadCheckBox->checkState()==Qt::Checked)
        mbSkipHead = true;
    else
        mbSkipHead = false;

    mvCols.clear();
    if (ui->commas_radBtn->isChecked())
    {
        mnSeparation = 1;
        if(mbSkipHead)
            mvCols= smsg.split(",");
        else
        {
            QStringList slist = smsg.split(",");
            int n = slist.length();
            for(int i = 0; i < n; i ++)
            {
                mvCols << QString("col%1").arg(i+1);
            }
        }
    }
    else if (ui->tab_radBtn->isChecked())
    {
        mnSeparation = 2;
        if(mbSkipHead)
            mvCols= smsg.split("\t");
        else
        {
            QStringList slist = smsg.split("\t");
            int n = slist.length();
            for(int i = 0; i < n; i ++)
            {
                mvCols << QString("col%1").arg(i+1);
            }
        }
    }
    else if (ui->space_radBtn->isChecked())
    {
        mnSeparation = 3;
        if(mbSkipHead)
            mvCols= smsg.split(" ");
        else
        {
            QStringList slist = smsg.split(" ");
            int n = slist.length();
            for(int i = 0; i < n; i ++)
            {
                mvCols << QString("col%1").arg(i+1);
            }
        }
    }
    else
    {
        mnSeparation = -1;
    }
    ui->lngColBox->clear();
    ui->latColBox->clear();
    ui->lngColBox->addItems(mvCols);
    ui->latColBox->addItems(mvCols);

    //ui->skipHeadCheckBox->setDisabled(true);
    //ui->commas_radBtn->setDisabled(true);

}

void MainWindow::on_saveFileBtn_clicked()
{
    QString searchPath = ".";
    QString qfn = QFileDialog::getSaveFileName(this,
            tr("Save to File"),
            searchPath,
            tr("*.csv; *.txt")); //选择路径
    mstrOutputFile = qfn;

    ui->fileLineEdit_2->setText(qfn);
}

void MainWindow::on_fileLineEdit_textEdited(const QString &arg1)
{
    mstrInputFile = ui->fileLineEdit->text();
}

void MainWindow::on_fileLineEdit_2_textEdited(const QString &arg1)
{
    mstrOutputFile = ui->fileLineEdit_2->text();
}

void MainWindow::on_runBtn_clicked()
{
    QString qfrom = ui->fromCoordBox->currentText();
    QString qto = ui->toCoordBox->currentText();
    //cout <<qfrom.toStdString().c_str()<<" to "<< qto.toStdString().c_str()<<endl;

    QString qlngName = ui->lngColBox->currentText();
    QString qlatName = ui->latColBox->currentText();
    int lng_ind = mvCols.indexOf(qlngName);
    int lat_ind = mvCols.indexOf(qlatName);


    QFile infile(mstrInputFile);
    if (!infile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this, tr("Error"), "Input File is empty or can not open!");
		return;
    }
    QTextStream _in(&infile);
    QString smsg;


    QFile outfile(mstrOutputFile);
    if(! outfile.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Error"), "Output File is empty or can not write!");
		return;
    }
    QTextStream _out(&outfile);

    if (ui->skipHeadCheckBox->checkState()==Qt::Checked)
    {
        mbSkipHead = true;
        smsg = _in.readLine();
        _out<<smsg<<"\r\n";
    }
    else
        mbSkipHead = false;


    QStringList slist;
    while (! _in.atEnd())
    {
        smsg = _in.readLine();

        if (ui->commas_radBtn->isChecked())
        {
            mnSeparation = 1;
            slist = smsg.split(",");

        }
        else if (ui->tab_radBtn->isChecked())
        {
            mnSeparation = 2;
            slist = smsg.split("\t");
        }
        else if (ui->space_radBtn->isChecked())
        {
            mnSeparation = 3;
            slist = smsg.split(" ");
        }
        else
        {
            mnSeparation = -1;
        }
        double dlng = slist[lng_ind].trimmed().toDouble();
        double dlat = slist[lat_ind].trimmed().toDouble();

        double target_lng, target_lat;

        if(qfrom == "BD 09" && qto == "GCJ 02")
            WEBCSTRANSFORM::bd092gcj(dlat, dlng, target_lat, target_lng);
        else if (qfrom == "BD 09" && qto == "WGS 1984")
            WEBCSTRANSFORM::bd092wgs(dlat, dlng, target_lat, target_lng);
        else if (qfrom == "BD 09" && qto == "BD 09")
            ;
        else if (qfrom == "GCJ 02" && qto == "BD 09")
            WEBCSTRANSFORM::gcj2bd09(dlat, dlng, target_lat, target_lng);
        else if (qfrom == "GCJ 02" && qto == "GCJ 02")
            ;
        else if (qfrom == "GCJ 02" && qto == "WGS 1984")
            WEBCSTRANSFORM::gcj2wgs(dlat, dlng, target_lat, target_lng);
        else if (qfrom == "WGS 1984" && qto == "BD 09")
            WEBCSTRANSFORM::wgs2bd09(dlat, dlng, target_lat, target_lng);
        else if (qfrom == "WGS 1984" && qto == "GCJ 02")
            WEBCSTRANSFORM::wgs2gcj(dlat, dlng, target_lat, target_lng);
        else if (qfrom == "GCJ 02" && qto == "WGS 1984")
            ;

        slist[lng_ind] = QString("%1").arg(target_lng, 0, 'f', 12);
        slist[lat_ind] = QString("%1").arg(target_lat, 0, 'f', 12);

        _out << slist[0];
        for(int i = 1; i < slist.length(); i ++)
        {
            _out << ","<< slist[i];
        }
        _out<<"\r\n";

    }

    infile.close();
    outfile.close();
    QMessageBox::information(this, tr("Congratulations"), "Coordinates Transformation Success!");
}
