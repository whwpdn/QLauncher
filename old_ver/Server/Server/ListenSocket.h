#pragma once

// CListenSocket ��� ����Դϴ�.
class CServerDlg;

class CListenSocket : public CSocket
{
public:
	CListenSocket(CServerDlg* pServerDlg);
	virtual ~CListenSocket();

        CServerDlg* m_pPublicServerDlg;
public:
    virtual void OnAccept(int nErrorCode);
    virtual void OnSend(int nErrorCode);
};


