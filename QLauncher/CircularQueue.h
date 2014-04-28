#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#include <windows.h>

//메세지가 오면 저장시에 사이즈 + 위치를 알필요가다.
struct MsgInfo
{
    MsgInfo() {m_iSize = m_iPos = 0;}
    int m_iSize;
    int m_iPos;
};

#define QUEUE_BUFF_SIZE 10000  //ex msg  3 3 3 3 
class CQueue
{
public:
    CQueue() 
    {
        InitializeCriticalSection(&m_locker);
    }

    virtual ~CQueue() 
    {
        DeleteCriticalSection(&m_locker); 
    }

    bool Init_queue();

    int CPush(char* pBuff, int iSize);
    //void CPop(char* pBuff, int iSOF, int EOT);
    int CPop(char* pBuff);
    int Read(char* pBuff, int iReadSize);
    int ReadAndPop(char* pBuff, int iReadSize);

    int GetFront()
    {
        return m_iFront;
    }

    int GetRear()
    {
        return m_iRear;
    }

	//-Util Func
    bool IsFull(int iSize); //Size만큼 들어갈 수 있는가?
    bool IsEmpty();
    bool Size()
    {
        return m_iCount;
    }

private:    
    unsigned char m_cBuff[QUEUE_BUFF_SIZE];

    int m_iFront; //front : 큐의 처음 위치 : 데이터 항목의 삭제는 front에서 일어난다.
    int m_iRear; //rear : g큐의 마지막 값 위치 : 새로운 데이터의 항목의 삽입은 rear에서 일어난다.
    int m_iCount; //count : 큐의 원소 갯수

    CRITICAL_SECTION m_locker;
};

#endif // CIRCULARQUEUE_H