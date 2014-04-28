#ifndef LAUNCHERCLIENT_H
#define LAUNCHERCLIENT_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QMetaType>

class NetMessageManager;

struct LauncherProgramInfo
{
    unsigned char m_ucID;
    unsigned char m_ucState;
    QString m_strName;
    QString m_strPath;
    LauncherProgramInfo()
    {
        m_ucID = 0;
        m_ucState = 0;
        m_strName = "";
        m_strPath = "";
    };
    LauncherProgramInfo(unsigned char id, unsigned char state, QString name, QString path)
    {
        m_ucID = id;
        m_ucState = state;
        m_strName = name;
        m_strPath = path;
    };
};
struct LauncherSubClientInfo
{
	unsigned char m_ucID;
	QString m_strName;
	QString m_ip;
	QMap<unsigned char, LauncherProgramInfo*> ProgramMap;
	LauncherSubClientInfo()
	{
		m_ucID = 0;
		m_strName = "";

		m_ip = "";
	};
	LauncherSubClientInfo(unsigned char id, QString name,QString ip)
	{
		m_ucID = id;
		m_strName = name;
		m_ip = ip;
	};
};

class LauncherClient : public QObject
{
    Q_OBJECT
public:
    LauncherClient(QObject *parent = NULL);
    ~LauncherClient();

    void SetID(int id) { ID = id; }
    int GetID() { return ID; }
    QString getClientIP() { return m_strClientIP; }
    QString getClientName() { return m_strClientName; }
    QString getMacAddress() { return m_strMacAddr; }

	QString getClientType()	{ return m_strClientType; } // 클라이언트 타입 server인지 client인지

    void setClientIP(QString ip) { m_strClientIP = ip; }
    void setClientName(QString name) { m_strClientName = name; }
    void setMacAddress(QString mac) { m_strMacAddr = mac; }

	void setClientType(QString type){ m_strClientType = type; }

	void setClientMap();

    LauncherProgramInfo* addProgramInfo(unsigned char id, unsigned char state, QString name, QString path);

	LauncherSubClientInfo* addSubClientInfo(unsigned char id,QString name, QString ip);
	LauncherProgramInfo* addProgramInfo(QString strIP,unsigned char id, unsigned char state, QString name, QString path);

    void removeProgramInfo(char id);
	void removeProgramInfo(QString strIP,char id);
	void removeSubClientInfo(QString strIP);

    QMap<unsigned char, LauncherProgramInfo*>& getProgramMap() { return m_ProgramMap; }
	QMap<unsigned char, LauncherProgramInfo*>& getProgramMap(QString ip) { return m_ClientMap[ip]->ProgramMap; }
	QMap<QString, LauncherSubClientInfo*>& getSubClientMap() { return m_ClientMap; }

    LauncherProgramInfo* getProgramInfo(unsigned char id);
	LauncherProgramInfo* getProgramInfo(QString strIP,unsigned char id);
	LauncherSubClientInfo* getSubClientInfo(QString strIP);
	

    void SetMessageManager(NetMessageManager* pMessageManager)
    {
        m_pNetMessageManager = pMessageManager;
    }

    NetMessageManager* GetMessageManager()
    {
        return m_pNetMessageManager;
    }

private:
    int ID;
    QString m_strClientIP;
    QString m_strClientName;
    QString m_strMacAddr;
	QString m_strClientType;

    QMap<unsigned char, LauncherProgramInfo*> m_ProgramMap;
	QMap<QString, LauncherSubClientInfo*> m_ClientMap;

    NetMessageManager*          m_pNetMessageManager;
};

Q_DECLARE_METATYPE(LauncherProgramInfo*);
Q_DECLARE_METATYPE(LauncherClient*);

#endif // LAUNCHERCLIENT_H
