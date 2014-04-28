#ifndef QMSGUPDATETHREAD_H
#define QMSGUPDATETHREAD_H

#include <QThread>
#include <QReadWriteLock>
#include <QMap>
#include <QProcess>
#include "networkinterface.h"
class LauncherClient;
class ClientInfoPacket;
class CommandInfoPacket;
class MacAddressPacket;
class QMsgUpdateThread : public QThread
{
        Q_OBJECT

public:
    QMsgUpdateThread(QObject *parent = NULL);
    ~QMsgUpdateThread();

    QMap<QString, LauncherClient*>& getClientList() { return m_mapClient; }
    LauncherClient* findClient(QString ip);

    void writeLog(QString msg);
    void updateProgramState(QString strPath, unsigned char ucState);
    void stop();
    void init();

    void sendMagicP(QString ip);
private:
    void analizePacket();
    void storeMacAddressFromIP(QString reqIP);
    //void sendMacAddress(QString serverIP);
    void addClient(ClientInfoPacket* pPacket);
    void addClient(QString ip, QString mac, QString name);
    void removeClient(ClientInfoPacket* pPacket);
    void commandProcess(CommandInfoPacket* pPacket);
    //void macProcess(MacAddressPacket* pPacket);

    void addConnectionItem(QString ip, QString name);

    int createMagicP(QString macAddr, unsigned char* magicp);
    unsigned int HexStrToInt(QString hexStr);

    void loadClientInfo();
    void writeClientInfo();
    private slots:
        void processFinishSlot(int exitCode, QProcess::ExitStatus exitStatus);
        void processErrorSlot(QProcess::ProcessError error);

protected:
    void run();
private:
    volatile bool stopped;
    QMap<QString, LauncherClient*> m_mapClient;
    QMap<QString, QProcess*> m_mapProcess;
    QReadWriteLock lock;
    NetworkInterface netInterface;
};

#endif // QMSGUPDATETHREAD_H
