// ServiceSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Server.h"
#include "ServiceSocket.h"

#include "ServerDlg.h"
// CServiceSocket

CServiceSocket::CServiceSocket(CServerDlg* pServerDlg)
{
    m_pPublicServerDlg = pServerDlg;
}

CServiceSocket::~CServiceSocket()
{
}


// CServiceSocket ��� �Լ�

void CServiceSocket::OnReceive(int nErrorCode)
{
    // TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
    m_pPublicServerDlg->ProcessReceive(this);
    CSocket::OnReceive(nErrorCode);
}

void CServiceSocket::OnClose(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class
    m_pPublicServerDlg->ProcessClose(this);
    CSocket::OnClose(nErrorCode);
}