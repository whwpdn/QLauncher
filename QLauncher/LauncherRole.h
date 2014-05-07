#ifndef LAUNCHERROLE_H
#define LAUNCHERROLE_H

#include <QObject>
#include "ui_qlauncher.h"
#include <QTimer>
#include <QTime>
#include "ManageProcessor.h"
//#include <stdio.h>
class NetMessage;
class Socket;
class NetMessageManager;
class ClientInfoPacket;
class SubProgramInfoPacket;

class CheckBoxUpdatePacket;
class CommandInfoPacket;
class ProgramInfoPacket;
class SubClientInfoPacket;
class QProcess;
class LauncherClient;
class LauncherProgramInfo;
class LauncherSubClientInfo;
class QStandardItem;
class ServerSocketInfo;

struct ServerSocketInfo
{
    Socket* m_pSocket;
    bool m_bState;
    QString m_strName;
	QString m_strHost;
	QString m_strGroupName;
	int m_iReceiveServerTime;
    ServerSocketInfo()
    {
        m_pSocket = 0;
        m_bState = false;
        m_strName = "";
		m_strHost = "";
		m_iReceiveServerTime =0;
		m_strGroupName = "";
    };
    ServerSocketInfo(Socket* pSocket, bool bState, QString strName,QString strHost,QString strGroupName)
    {
        m_pSocket = pSocket;
        m_bState = bState;
        m_strName = strName;
		m_strHost = strHost;
		m_strGroupName = strGroupName;
		m_iReceiveServerTime =0;
	};
};
class LauncherRole : public QObject
{
	Q_OBJECT

public:
	LauncherRole();
	virtual ~LauncherRole();

	virtual void initUI() = 0;
	virtual void initComm() = 0;
	virtual void Release() = 0;
	virtual void Update() = 0;
	//virtual void sendPacket(NetMessage* pMsg, QString logMsg = "") = 0;
	virtual void sendPacket(NetMessage* pMsg, QString logMsg = "") =0;
	void setUI(Ui::QLauncherClass* pUI);
	public slots:
		virtual void OnConnection() = 0;
signals:
	void updateProgramStateSignal(unsigned char, unsigned char, LauncherClient* pClient,QString strGroupName);
	void connectionTimerFin();
	void connectionTimerRestart();
	void writeLogSignal(QString log);
	
protected:
	Ui::QLauncherClass* m_pUI;
	Socket*     m_pTCPSocket;
	bool m_bModified;
	QString		m_LocalIP;
	QTime		m_Time;
};

class ServerRole : public LauncherRole
{
	Q_OBJECT

public:
	virtual void initUI();
	virtual void initComm();
	virtual void Release();
	virtual void Release(QString strHost);
	virtual void Update();
	void setLocalIP(QString strLocalIP);
	void ProcessMessage();
public:
    static ServerRole& GetInstance();
	void setServerIP(QString ip,QString port);

public: 
	void ProcessProgramInfoServer(ProgramInfoPacket* aPacket);
	void AddProgramServer(LauncherClient* pLauncherClient, LauncherProgramInfo* pInfo);
	void RemoveProgramServer(LauncherClient* pLauncherClient, LauncherProgramInfo* pLauncherProgramInfo);
	void checkConnection(NetMessage* pMsg);
	virtual void sendPacket(NetMessage* pMsg, QString logMsg = "");
	void updateProgramList(int row);
	void updateProgramList(QString strIP);
	LauncherClient* findClient(QString ip);
	QStandardItem* FindClientItem(LauncherClient* pLauncherClient);
	QStandardItem* FindProgramItem(LauncherClient* pLauncherClient, unsigned char id);

	public slots:
		void onButtonSlot(bool clicked);
		void offButtonSlot(bool clicked);
		void OnAllButtonSlot(bool clicked);
		void OffAllButtonSlot(bool clicked);
		void treeSelectSlot(const QModelIndex& index);
		//void ConnectionCheckSlot();
		void powerButtonSlot(bool clicked);
		void powerOffButtonSlot(bool clicked);
		virtual void OnConnection();
		void rebootSlot(bool clicked);
		//void delButtonSlot(bool clicked);
		void treeItemChangedSlot(QStandardItem* item);
		void saveIpConfig(bool clicked);
		void changeCheckServerListSlot(int state);
		void changeCheckMacListSlot(int state);
		void btnServerConnSlot(bool clicked);
private:
	ServerRole();
	virtual ~ServerRole();

	void LoadWOLData();	// wol 관련
	void WriteWOLData();
	void storeMacAddressFromIP(QString reqIP);
	void SetWOLDataToList();

	// client 관련
	void addClient(ClientInfoPacket* pPacket, NetMessageManager* pNetMessageManager = 0);
	void addConnectionItem(LauncherClient* pLauncherClient);
	void removeClient(QString strIP);
	void removeClient(ClientInfoPacket* pPacket);
	//void sendState();

	///
	bool addServerInfo();
	void SetServerListToTable();
	void SetWolTable();
	void ResetServerListTable();
	void checkServerState();
	QStandardItem* FindGroupItem(QString strGroupName);
	void UpdateServerListTable();

private:
    static ServerRole    sm_Instance;
private:
	NetMessageManager*              m_pNetMessageManager;
	QMap<QString, LauncherClient*>  m_mapClient;
	QMap<QString, NetMessageManager*>     m_NetMessageManagerMap;
	LauncherClient*                 m_pActiveLauncherClient;
	QModelIndex                     m_ConnectionListSel;
	QMap<QString, QString>			m_MacAddresses;
	QTimer		m_ConnectionCheckTimer;
    int			m_iOldTime;
	QString     m_ServerIP;
	int			m_ServerPort;
	// 테스트
	//QVector<QString>    m_strTargetIp;
	QMap<QString, ServerSocketInfo*>     m_SocketMap;
};

class ClientRole : public LauncherRole
{
	Q_OBJECT

public:
	virtual void initUI();
	virtual void initComm();
	virtual void Release();
	virtual void Update();
	void ProcessMessage();
	void setLocalIP(QString strLocalIP);
	
public:
    static ClientRole& GetInstance();
	void setServerIP(QString ip,QString port);

public:
	void AddProgramClient(const QString& aPath);
	void AddProgram2UIClient(const QString& aPath);

	void SendAddProgramPacket(const QString& path, unsigned char id);
	void SendAddProgramPacketOne(const QString& path, unsigned char id,QString strHost);
	void sendPacketOne(NetMessage* pMsg, QString logMsg,QString strHost);
	virtual void sendPacket(NetMessage* pMsg, QString logMsg = "");
	bool isAllowedProgram(unsigned char id);

	void loadProgramList();
	void saveProgramList();

	public slots:
		void addButtonSlot(bool clicked);
		void delButtonSlot(bool clicked);
		//void ConnectionSendSlot();
		virtual void OnConnection();
		void saveIpConfig(bool clicked);

private:
	ClientRole();
	virtual ~ClientRole();
	void commandProcess(CommandInfoPacket* pPacket);
	void ProcessProgramInfo(ProgramInfoPacket* pPacket);
	//void checkServerState();
	void sendState();

private:
    static ClientRole    sm_Instance;

private:
	//클라이언트용
	QMap<QString, NetMessageManager*>     m_NetMessageManagerMap;
	NetMessageManager*              m_pNetMessageManager;
	QMap<unsigned char, QProcess*>  m_mapProcesses;
	QMap<unsigned char, QString>    m_mapPrograms;
	//QVector<QString>    m_strAddProgramPath;
	QString     m_ServerIP;
	int			m_ServerPort;
	//int			m_iReceiveServerTime;
	bool		m_bReseted;
	QTimer		m_ConnectionCheckTimer;
	unsigned char m_ucProgramId;
	QTime		m_Time;
	int			m_iOldTime;
	//QString     m_LocalIP;
	
};
#endif // LAUNCHERROLE_H
