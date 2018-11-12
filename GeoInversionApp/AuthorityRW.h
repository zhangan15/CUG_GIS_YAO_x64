#ifndef AUXRHORITY_READ_WRITE
#define AUXRHORITY_READ_WRITE

#include <QtCore/QString>
#include <QtCore/QDate>

#define RANDOM_VALUE 0xFF10EF

struct AuthorityStruct
{
	QString appID;
	QString usrname;
	QDate lastDate;
	int bEnable;
};

QString getSourceCodeFromHttp(QString surl);
bool getAppAuthority(QString sID, QString appID, QString sUser);
bool writeAuthory2File(QString sID, QString appId, QString usrname, QString sfilename);
bool readAuthoryFromFile(QString sfilename, QString& sID, QString& appId, QString& usrname);

//default sfilename = "./key.au"
bool getAppAuthority(QString sfilename = "./key.au");

bool checkIDAvailable(QString sID);
QString getHTMLString(QString sID, QString appId, QString usrname, QDate _date, bool bEnable = true);



#endif