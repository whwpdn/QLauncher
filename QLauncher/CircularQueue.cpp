#include "CircularQueue.h"

int CQueue::CPush(char* pBuff, int iSize)
{   
    if (IsFull(iSize)) return -1;
    
    EnterCriticalSection(&m_locker);

    int iLast = m_iRear + iSize;
    if (iLast > QUEUE_BUFF_SIZE)
    {
        int iRemainder = iLast % QUEUE_BUFF_SIZE;
        int iFirstCopyCount = iSize - iRemainder;

        memcpy(&m_cBuff[m_iRear], pBuff, iFirstCopyCount);
        
        memcpy(m_cBuff, &pBuff[iFirstCopyCount], iRemainder);
    }
    else
        memcpy(&m_cBuff[m_iRear], pBuff, iSize);

    m_iRear = (m_iRear+iSize) % QUEUE_BUFF_SIZE;

    m_iCount += iSize;    
    
    LeaveCriticalSection(&m_locker);

    return iSize;
}

int CQueue::CPop(char* pBuff)
{
    if (IsEmpty()) return -1;

    EnterCriticalSection(&m_locker);

    int iLast = m_iFront + m_iCount;
    if (iLast > QUEUE_BUFF_SIZE)
    {
        int iRemainder = iLast % QUEUE_BUFF_SIZE;
        int iFirstCopyCount = m_iCount - iRemainder;
        memcpy(pBuff, &m_cBuff[m_iFront], iFirstCopyCount);

        memcpy(&pBuff[iFirstCopyCount], m_cBuff, iRemainder);
    }
    else
        memcpy(pBuff, &m_cBuff[m_iFront], m_iCount);

    m_iFront = (m_iFront+m_iCount) % QUEUE_BUFF_SIZE;
    int iReturn = m_iCount = 0;

    LeaveCriticalSection(&m_locker);

    return iReturn;
}
//---------------------------------------------------------------------------------------
int CQueue::Read(char* pBuff, int iReadSize)
{
    if (m_iCount < iReadSize) return -1;

    EnterCriticalSection(&m_locker);

    int iLast = m_iFront + iReadSize;
    if (iLast > QUEUE_BUFF_SIZE)
    {
        int iRemainder = iLast % QUEUE_BUFF_SIZE;
        int iFirstCopyCount = iReadSize - iRemainder;
        memcpy(pBuff, &m_cBuff[m_iFront], iFirstCopyCount);

        memcpy(&pBuff[iFirstCopyCount], m_cBuff, iRemainder);
    }
    else
        memcpy(pBuff, &m_cBuff[m_iFront], m_iCount);

    //m_iFront = (m_iFront+iReadSize) % QUEUE_BUFF_SIZE;
    //m_iCount -= iReadSize;

    LeaveCriticalSection(&m_locker);

    return iReadSize;
}
//---------------------------------------------------------------------------------------
int CQueue::ReadAndPop(char* pBuff, int iReadSize)
{
    if (m_iCount < iReadSize) return -1;

    EnterCriticalSection(&m_locker);
 
    int iLast = m_iFront + iReadSize;
    if (iLast > QUEUE_BUFF_SIZE)
    {
        int iRemainder = iLast % QUEUE_BUFF_SIZE;
        int iFirstCopyCount = iReadSize - iRemainder;
        memcpy(pBuff, &m_cBuff[m_iFront], iFirstCopyCount);

        memcpy(&pBuff[iFirstCopyCount], m_cBuff, iRemainder);
    }
    else
        memcpy(pBuff, &m_cBuff[m_iFront], m_iCount);

    m_iFront = (m_iFront+iReadSize) % QUEUE_BUFF_SIZE;
    m_iCount -= iReadSize;

    LeaveCriticalSection(&m_locker);

    return true;
}
//---------------------------------------------------------------------------------------
bool CQueue::Init_queue()
{
    EnterCriticalSection(&m_locker);

    memset(m_cBuff, 0, sizeof(char) * QUEUE_BUFF_SIZE);
    m_iCount = 0;
    m_iFront = m_iRear = 0;

    LeaveCriticalSection(&m_locker);
   
    return true;
}

bool CQueue::IsFull(int iSize)
{
    return (m_iCount+iSize)>=QUEUE_BUFF_SIZE?true:false;
}

bool CQueue::IsEmpty()
{
    return m_iCount<=0?true:false;
}