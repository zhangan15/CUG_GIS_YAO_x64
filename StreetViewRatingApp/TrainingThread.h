#pragma once

#include <QThread>

class StreetViewRatingApp;
class TrainingThread : public QThread
{
	Q_OBJECT

public:
	TrainingThread(QObject *parent);
	~TrainingThread();

signals:
	//void sendMsg(QString);
	void finished();


protected:
	void run();

private:
	StreetViewRatingApp* mpApp;
};
