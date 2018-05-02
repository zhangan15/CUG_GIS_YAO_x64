#pragma once

#include <QThread>

class StreetViewRatingApp;
class LoadFeatureThread : public QThread
{
	Q_OBJECT

public:
	LoadFeatureThread(QObject *parent);
	~LoadFeatureThread();

signals:
	void sendMsg(QString);
	void finished();


protected:
	void run();

private:
	StreetViewRatingApp* mpApp;
};
