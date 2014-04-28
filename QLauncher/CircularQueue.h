#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#include <windows.h>

//�޼����� ���� ����ÿ� ������ + ��ġ�� ���ʿ䰡��.
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
    bool IsFull(int iSize); //Size��ŭ �� �� �ִ°�?
    bool IsEmpty();
    bool Size()
    {
        return m_iCount;
    }

private:    
    unsigned char m_cBuff[QUEUE_BUFF_SIZE];

    int m_iFront; //front : ť�� ó�� ��ġ : ������ �׸��� ������ front���� �Ͼ��.
    int m_iRear; //rear : gť�� ������ �� ��ġ : ���ο� �������� �׸��� ������ rear���� �Ͼ��.
    int m_iCount; //count : ť�� ���� ����

    CRITICAL_SECTION m_locker;
};

#endif // CIRCULARQUEUE_H