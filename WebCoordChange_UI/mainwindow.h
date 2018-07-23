#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QMenuBar>
#include <QDir>
#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QLabel>
#include <QtCore/QtCore>
#include <iostream>
using namespace std;
#include "WebCoordSystemTransform.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();

private slots:
    void aboutThis();

    void on_loadFileBtn_clicked();

    void on_saveFileBtn_clicked();

    void on_fileLineEdit_textEdited(const QString &arg1);

    void on_fileLineEdit_2_textEdited(const QString &arg1);

    void on_runBtn_clicked();

private:
    Ui::MainWindow *ui;

private:
    QString mstrInputFile;
    QString mstrOutputFile;

    QStringList mvCols;
    bool mbSkipHead;
    int mnSeparation;
    int mnEncoding;

    int mnLngCol;
    int mnLatCol;

    int mnFromCoord;
    int mnToCoord;


};

#endif // MAINWINDOW_H
