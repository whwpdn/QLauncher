#ifndef NETMESSAGEMANAGER_H
#define NETMESSAGEMANAGER_H

#include <QObject>
#include <QThread>
//#include <QHostAddress>
#include "internet.h"
#include "NetMessageParser.h"

class NetMessageManager : public QObject
{
    Q_OBJECT
public:
enum 
{
    TCP = 0,
};

public:
    NetMessageManager(int Type = TCP);
    ~NetMessageManager();

    NetMessageParser* GetParser();

    void    SetSocket(Socket* pSocket);

    void    SetName(const char* pName);
    //void    SetSocketDescriptor(int iSocketDescriptor);
	string GetName();

    void    StartReceiving();

    int     Send(const char* data, int iDataSize);
    int     SendDatagram(const char* data, int iDataSize, const InternetAddress& aAddr, int iPort);

private:
    NetMessageParser*       m_pParser;
    //QAbstractSocket*        m_pSendSocket;
    Socket*                 m_pSendSocket;
    int                     m_iSocketDescriptor;
    int                     m_iType;
    string                  m_Name;
};

#endif // NETMESSAGEMANAGER_H