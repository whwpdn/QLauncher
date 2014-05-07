#include "NetMessageParser.h"

#include "Socket.h"

//------------------------------------------------------------------------------------------------
NetMessageParser::NetMessageParser() :
m_bRun(true),
m_pSocket(0),
m_GetMessageSizeFunc(0),
m_CreateMessageFunc(0),
m_DeleteMessageFunc(0)
{
    m_Queue.Init_queue();

    InitializeCriticalSection(&m_Locker);

    m_ReceiveEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}
//------------------------------------------------------------------------------------------------
NetMessageParser::~NetMessageParser()
{
    DeleteCriticalSection(&m_Locker); 
}
//------------------------------------------------------------------------------------------------
void NetMessageParser::SetSocket(Socket* pSocket)
{   
    m_pSocket = pSocket;
}
//------------------------------------------------------------------------------------------------
void NetMessageParser::SetName(const char* pName)
{
    if (! pName) return;
    m_Name = pName;
}
//------------------------------------------------------------------------------------------------
list<NetMessage*>& NetMessageParser::GetMessages(int iCount)
{
    return m_TempMessages;
}
//------------------------------------------------------------------------------------------------
void NetMessageParser::DeleteMessages()
{
}
//------------------------------------------------------------------------------------------------
void NetMessageParser::Quit()
{
    m_bRun = false;
}
//------------------------------------------------------------------------------------------------
void NetMessageParser::run()
{   
}
//------------------------------------------------------------------------------------------------
void NetMessageParser::UpdateEvent()
{
}
//------------------------------------------------------------------------------------------------
void NetMessageParser::SetGetMessageSizeFunc(TGetMessageSizeFunc Func)
{
    m_GetMessageSizeFunc = Func;
}
//------------------------------------------------------------------------------------------------
void NetMessageParser::SetCreateMessageFunc(TCreateMessageFunc Func)
{
    m_CreateMessageFunc = Func;
}
//------------------------------------------------------------------------------------------------
void NetMessageParser::SetDeleteMessageFunc(TDeleteMessageFunc Func)
{
    m_DeleteMessageFunc = Func;
}
//------------------------------------------------------------------------------------------------
NetTcpMessageParser::NetTcpMessageParser()
{
    InitializeCriticalSection(&m_DelLocker);   
}
//------------------------------------------------------------------------------------------------
NetTcpMessageParser::~NetTcpMessageParser()
{
    DeleteCriticalSection(&m_DelLocker); 

    list<NetMessage*>::iterator it, end;
    for (it = m_MessageList.begin(), end = m_MessageList.end(); it != end; ++it)
        delete (*it);

    for (it = m_DeleteMessageList.begin(), end = m_DeleteMessageList.end(); it != end; ++it)
        delete (*it);
}
//------------------------------------------------------------------------------------------------
void NetTcpMessageParser::UpdateEvent()
{
    if (m_pSocket->IsReadReady())
        SetEvent(m_ReceiveEvent);
}
//------------------------------------------------------------------------------------------------
void NetTcpMessageParser::run()
{   
    if (! m_pSocket) return;
    if (! m_GetMessageSizeFunc) return;
    if (! m_CreateMessageFunc) return;

    NetMessage   sTmpHeadMsg;
    char         sBuff[10000];
    bool         sbNeedParsing;
    int          siMessageSize;
    int          siReadyByte;

    list<NetMessage*>::iterator it, end;

    while (m_bRun)
    {   
        //if (m_pSocket->IsReadReady())
        //{
        siReadyByte = m_pSocket->Receive(sBuff, 10000);
        if (siReadyByte > 0)
            m_Queue.CPush(sBuff, siReadyByte);
		else if(siReadyByte==0)
		{
	
		}
		else
		{
			WaitForSingleObject(m_ReceiveEvent, 100);

			ResetEvent(m_ReceiveEvent);
		}
        //}
        //else
        //    Sleep(0);

        sbNeedParsing = true;

        while (sbNeedParsing)
        {
            if (m_Queue.Read(sBuff, HEADER_SIZE) < 0)
            {
                sbNeedParsing = false;
                Sleep(0);
                continue;        
            }

            sTmpHeadMsg.FillData(sBuff, HEADER_SIZE);

            siMessageSize = (*m_GetMessageSizeFunc)(sTmpHeadMsg.m_ucID);

            if (! siMessageSize)
            {
                printf("Not Exist Message : %x from %s\n", sTmpHeadMsg.m_ucID, m_Name.c_str());
                m_Queue.Init_queue();
                continue;
            }

            if (m_Queue.ReadAndPop(sBuff, siMessageSize) > 0)
            {
                NetMessage* pMessage = (*m_CreateMessageFunc)(sTmpHeadMsg.m_ucID);
                if (! pMessage)
                {
                    printf("%s Not exist message : %x from %s\n", m_Name.c_str(), sTmpHeadMsg.m_ucID, m_Name.c_str());
                    break;
                }

                pMessage->FillData(sBuff, siMessageSize);

                EnterCriticalSection(&m_Locker);

                m_MessageList.push_back(pMessage);

                LeaveCriticalSection(&m_Locker);
            }
            else
            {
                sbNeedParsing = false;
                Sleep(0);
            }
        }

        EnterCriticalSection(&m_DelLocker);

        for (it = m_DeleteMessageList.begin(), end = m_DeleteMessageList.end();
            it != end; ++it)
            (*m_DeleteMessageFunc)(*it);
        m_DeleteMessageList.clear();

        LeaveCriticalSection(&m_DelLocker);
    }
}
//------------------------------------------------------------------------------------------------
list<NetMessage*>& NetTcpMessageParser::GetMessages(int iCount)
{
    EnterCriticalSection(&m_Locker);

    if (iCount < 0)
    {
        m_TempMessages = m_MessageList;

        m_MessageList.clear();
    }
    else
    {
        if (m_MessageList.size() < iCount)
            iCount = m_MessageList.size();

        list<NetMessage*>::iterator begin, end;
        begin = end = m_MessageList.begin();
        advance(end, iCount);

        m_TempMessages.insert(m_TempMessages.end(), begin, end);

        m_MessageList.erase(begin, end);
    }

    LeaveCriticalSection(&m_Locker);

	return m_TempMessages;
}
//------------------------------------------------------------------------------------------------
void NetTcpMessageParser::DeleteMessages()
{
    EnterCriticalSection(&m_DelLocker);

    m_DeleteMessageList.insert(m_DeleteMessageList.end(), m_TempMessages.begin(), m_TempMessages.end());
    m_TempMessages.clear();
 
    LeaveCriticalSection(&m_DelLocker);
}
//------------------------------------------------------------------------------------------------