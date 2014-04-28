#ifndef NETMESSAGEPARSER_H
#define NETMESSAGEPARSER_H

#include <QObject>
#include <QThread>
#include <QMap>

#include "CircularQueue.h"
#include "message.h"

#include <list>
#include <string>

using std::list;
using std::string;

class Socket;

typedef int (*TGetMessageSizeFunc)(unsigned char);
typedef NetMessage* (*TCreateMessageFunc)(unsigned char);
typedef void (*TDeleteMessageFunc)(NetMessage*);

//-------------------------------------------------------------------------------------
class NetMessageParser : public QThread
{
Q_OBJECT
public:
    NetMessageParser();
    virtual ~NetMessageParser();

    void SetSocket(Socket* pSocket);

    void SetName(const char* pName);

    // For TCP
    virtual list<NetMessage*>& GetMessages(int iCount = -1);
    virtual void DeleteMessages();

    void Quit();

    void run();

    virtual void UpdateEvent();

    void SetGetMessageSizeFunc(TGetMessageSizeFunc Func);
    void SetCreateMessageFunc(TCreateMessageFunc Func);
    void SetDeleteMessageFunc(TDeleteMessageFunc Func);

protected:
    CQueue                  m_Queue;
    Socket*                 m_pSocket;
    bool                    m_bRun;

    TGetMessageSizeFunc     m_GetMessageSizeFunc;
    TCreateMessageFunc      m_CreateMessageFunc;
    TDeleteMessageFunc      m_DeleteMessageFunc;

    string                  m_Name;

    CRITICAL_SECTION        m_Locker;

    HANDLE                  m_ReceiveEvent;
    list<NetMessage*>	    m_TempMessages;
};
//-------------------------------------------------------------------------------------
class NetTcpMessageParser : public NetMessageParser
{
Q_OBJECT
public:
    NetTcpMessageParser();
    ~NetTcpMessageParser();

    void run();

    void UpdateEvent();

    list<NetMessage*>& GetMessages(int iCount = -1);
    void DeleteMessages();

private:
    list<NetMessage*>       m_MessageList;
    list<NetMessage*>       m_DeleteMessageList;
    
    CRITICAL_SECTION        m_DelLocker;
};
//-------------------------------------------------------------------------------------
#endif // NETMESSAGEPARSER_H