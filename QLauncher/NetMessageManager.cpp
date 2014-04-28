#include "NetMessageManager.h"

//--------------------------------------------------------------------------------------------
NetMessageManager::NetMessageManager(int iType) :
m_pSendSocket(0)
, m_iType(iType)
{
	m_Name = "";
    if (iType == TCP)
        m_pParser = new NetTcpMessageParser;
}
//--------------------------------------------------------------------------------------------
NetMessageManager::~NetMessageManager()
{
    m_pParser->Quit();

    m_pParser->wait();    

    delete m_pParser;
}
//--------------------------------------------------------------------------------------------
NetMessageParser* NetMessageManager::GetParser()
{
    return m_pParser;
}
//--------------------------------------------------------------------------------------------
void NetMessageManager::SetSocket(Socket* pSocket)
{
    m_pParser->SetSocket(pSocket);

    m_pSendSocket = pSocket;
}
//--------------------------------------------------------------------------------------------
void NetMessageManager::SetName(const char* pName)
{
    m_Name = pName;

    if (m_pParser)
        m_pParser->SetName(pName);
}
string NetMessageManager::GetName()
{
    return m_Name;
}
//--------------------------------------------------------------------------------------------
void NetMessageManager::StartReceiving()
{
    m_pParser->start();
}
//--------------------------------------------------------------------------------------------
int NetMessageManager::Send(const char* data, int iDataSize)
{
    if (! m_pSendSocket) return -1;

    return m_pSendSocket->Send(data, iDataSize);
}
//--------------------------------------------------------------------------------------------
int NetMessageManager::SendDatagram(const char* data, int iDataSize, const InternetAddress& aAddr, int iPort)
{
    if (! m_pSendSocket) return -1;

    return m_pSendSocket->SendTo(data, iDataSize, aAddr);
}
//--------------------------------------------------------------------------------------------
