#include "mainwindow.h"
#include <QApplication>
#include <QtCore/QSettings>
#include "login_dlg.hpp"

QString writelog(QString qtime)
{
    QString fn = "./config.ini";
    QFile outfile(fn);
    if (! outfile.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(NULL, "Quit", "First Registration failed!");
        return NULL;
    }
    QTextStream _out(&outfile);
    _out << "first start time: "<<qtime<<" ";
    outfile.close();
    return qtime;
}

QString searchTime()
{
    QFile infile("./config.ini");
    if (!infile.exists())
    {
        QDateTime t1=QDateTime::currentDateTime();
        return writelog(t1.toString("yyyyMMdd"));

    }
    else
    {
        if(!infile.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(NULL, "Quit", "Registration failed!");
            return NULL;
        }
        QTextStream _in(&infile);
        QString smsg = _in.readLine();
        QStringList slist = smsg.split(":");
        QString qtime = slist[1].trimmed();
        infile.close();
        cout<<qtime.toStdString().c_str()<<endl;
        return qtime;
    }

}




int main(int argc, char *argv[])
{
//	QApplication::addLibraryPath("F:/install/profession/Qt/5.9.1/5.9.1/msvc2015_64/plugins");
    QApplication a(argc, argv);
	
// 	login_dlg lg_alg;
// 	if (!lg_alg.exec())
// 	{
// 		QString strStartTime = searchTime();
// 		QDateTime originTime = QDateTime::fromString(strStartTime, "yyyyMMdd");
// 		originTime = originTime.addMonths(1);
// 		cout << originTime.toString("yyyyMMdd").toStdString().c_str();
// 		QDateTime currentTime = QDateTime::currentDateTime();
// 
// 		if (currentTime > originTime || originTime < QDateTime::fromString("20171023", "yyyyMMdd"))
// 		{
// 			QMessageBox::information(NULL, "Quit", "Due to the overdue of the software, it cannot function properly.");
// 			return -1;
// 		}
// 	}

    MainWindow w;
    w.show();
    
    return a.exec();
}
