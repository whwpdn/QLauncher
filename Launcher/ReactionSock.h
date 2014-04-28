#pragma once

// CReactionSock command target
class CLauncherDlg;
class CReactionSock : public CSocket
{
public:
	CReactionSock(CLauncherDlg* pLauncherDlg);
	virtual ~CReactionSock();

//함수부
public:
    virtual void OnClose(int nErrorCode);
    virtual void OnAccept(int nErrorCode);
    virtual void OnReceive(int nErrorCode);

    void SetMyType(CString& str) { m_strMyType = str;}

//변수부
private:
    CLauncherDlg* m_pLauncherDlg;
    CString       m_strMyType; //"server" , "client"
};


