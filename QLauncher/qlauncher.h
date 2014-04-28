#ifndef QLAUNCHER_H
#define QLAUNCHER_H

#include <QtGui/QDialog>
#include <QTimer>
#include "ui_qlauncher.h"
//#include "QMsgUpdateThread.h"

class LauncherRole;
class LauncherClient;
class QLauncher : public QDialog
{
	Q_OBJECT

public:
	enum {
		SERVER = 0,
		CLIENT
	};

public:
	QLauncher(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QLauncher();

	bool loadConfig();

	public slots:
		void writeLog(QString msg);
		void updateProgramStateSlot(unsigned char, unsigned char ucState, LauncherClient* pClient);
		void updateSlot();
		void connectionTimerFinSlot();
		void connectionTimerReStartSlot();

private:
	void initUI();
public:
	Ui::QLauncherClass ui;

private:
	QTimer      m_ConnectionTimer;
	QTimer      m_UpdateTimer;
	QString     m_ServerIP;
	QString		m_ServerPort;
	QString		m_LocalIP;
	LauncherRole* m_pCurrentRole;
};

QString getLocalIP();
#endif // QLAUNCHER_H
