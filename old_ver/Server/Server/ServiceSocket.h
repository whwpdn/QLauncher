#pragma once

// CServiceSocket 명령 대상입니다.
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


