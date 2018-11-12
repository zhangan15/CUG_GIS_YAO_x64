#include <QtCore>
#include <QtNetwork>
#include <iostream>
#include "AuthorityRW.h"
using namespace std;



QString getSourceCodeFromHttp(QString surl)
{
	QUrl url(surl); 

	QNetworkAccessManager manager; 
	QEventLoop loop; 

	//发出请求 
	QNetworkReply *reply = manager.get(QNetworkRequest(url)); 
	//请求结束并下载完成后，退出子事件循环 
	QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit())); 
	//开启子事件循环 
	loop.exec(); 

	//将读到的信息写入文件 
	QString code = QString::fromUtf8(reply->readAll()); 

	return code;
}


bool getAppAuthority(QString sID, QString appID, QString sUser)
{
	QString sCode = getSourceCodeFromHttp("http://www.geosimulation.cn/gis4win/program_authority.html").toLower();

	int nIndex = sCode.indexOf(sID.trimmed().toLower());
	if (nIndex < 0)
		return false;

	QString spStr = sCode.mid(nIndex, 100);
	spStr = spStr.split(";")[0];
	spStr = spStr.remove(sID.toLower().trimmed());
	spStr = spStr.remove(":");
	spStr = spStr.remove("[");
	spStr = spStr.remove("]");
	cout<<spStr.toStdString().data()<<endl;

	QStringList slist = spStr.split(",", QString::SkipEmptyParts);
	if (slist.size() < 4)
		return false;

	AuthorityStruct as;
	foreach(QString str, slist)
	{
		QStringList strlist = str.split("=", QString::SkipEmptyParts);
		if (strlist.size() < 2)
			return false;

		QString s1 = strlist[0].trimmed().toLower();
		QString s2 = strlist[1].trimmed().toLower();
		if (s1 == "appid")
			as.appID = s2;
		else if(s1 == "user")
			as.usrname = s2;
		else if(s1 == "date")
			as.lastDate = QDate::fromString(s2, "yyyyMMdd");
		else if(s1 == "enable")
			as.bEnable = s2.toInt();
	}

	//
	if (as.appID != appID.trimmed().toLower())
		return false;

	if (as.usrname != sUser.trimmed().toLower())
		return false;

	if (as.lastDate < QDate::currentDate())
		return false;

	if (as.bEnable <= 0)
		return false;

	return true;

}

bool getAppAuthority( QString sfilename )
{
	if (!QFile::exists(sfilename))
		return false;

	QString sID;
	QString appId;
	QString usrname;

	if (!readAuthoryFromFile(sfilename, sID, appId, usrname))
		return false;

	if (!getAppAuthority(sID, appId, usrname))
		return false;
	
	return true;
}

bool writeAuthory2File(QString sID, QString appId, QString usrname, QString sfilename)
{
	QFile _file(sfilename);
	if (!_file.open(QIODevice::WriteOnly))
		return false;

	QDataStream _in(&_file);
	_in<<RANDOM_VALUE;
	_in<<sID;
	_in<<appId;
	_in<<usrname;

	_file.flush();
	_file.close();
	return true;
}

bool readAuthoryFromFile(QString sfilename, QString& sID, QString& appId, QString& usrname)
{
	QFile _file(sfilename);
	if (!_file.open(QIODevice::ReadOnly))
		return false;

	QDataStream _out(&_file);

	int labelvalue;
	_out>>labelvalue;

	if (labelvalue != RANDOM_VALUE)
	{
		_file.close();
		return false;
	}

	_out>>sID;
	_out>>appId;
	_out>>usrname;

	_file.flush();
	_file.close();
	return true;
}

bool checkIDAvailable( QString sID )
{
	QString sCode = getSourceCodeFromHttp("http://www.geosimulation.cn/gis4win/program_authority.html").toLower();

	int nIndex = sCode.indexOf(sID.trimmed().toLower());
	if (nIndex < 0)
		return true;
	else
		return false;
}

QString getHTMLString( QString sID, QString appId, QString usrname, QDate _date, bool bEnable /*= true*/ )
{
	int nVal = bEnable?1:0;
	QString str = QString("%1:[APPID=%2,User=%3,Date=%4,Enable=%5];<br>").arg(sID.trimmed()).arg(appId.trimmed()).arg(usrname.trimmed()).arg(_date.toString("yyyyMMdd")).arg(nVal);
	return str;
}
