#pragma once
class CClientDlg;
// CServiceSocket command target

class CServiceSocket : public CSocket
{
public:
        CServiceSocket(CClientDlg *pClientDlg);
	CServiceSocket();
	virtual ~CServiceSocket();

public:

  //  virtual void OnConnect(int nErrorCode);
    virtual void OnReceive(int nErrorCode);

    CClientDlg* m_pClientDlg;
};


