#pragma once

// CServiceSocket ��� ����Դϴ�.
class CServerDlg;

class CServiceSocket : public CSocket
{
public:
	CServiceSocket(CServerDlg* pServerDlg);
	virtual ~CServiceSocket();

public:

    virtual void OnReceive(int nErrorCode);
    virtual void OnClose(int nErrorCode);

    CServerDlg* m_pPublicServerDlg;
};


