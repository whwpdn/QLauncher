// ServiceSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ServiceSocket.h"

#include "ClientDlg.h"
// CServiceSocket
CServiceSocket::CServiceSocket(CClientDlg *pClientDlg)
{
    m_pClientDlg = pClientDlg;
}

CServiceSocket::CServiceSocket()
{
}

CServiceSocket::~CServiceSocket()
{
}

// CServiceSocket member functions
//void CServiceSocket::OnConnect(int nErrorCode)
//{
//    // TODO: Add your specialized code here and/or call the base class
//    CSocket::OnConnect(nErrorCode);
//}

//�� �������� ����� *������ ���ú��Ҷ��� �����Ѵ�.
//��� �̰��Ҷ� �ٷ� CClientDlg���ο� �־��� �ǰڴ�.
void CServiceSocket::OnReceive(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class
    m_pClientDlg->ProcessReceive(this);
    CSocket::OnReceive(nErrorCode);
}
