// ListenSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Server.h"
#include "ListenSocket.h"

#include "ServerDlg.h"

// CListenSocket

CListenSocket::CListenSocket(CServerDlg* pServerDlg)
{
    m_pPublicServerDlg = pServerDlg;
}


CListenSocket::~CListenSocket()
{
}


// CListenSocket ��� �Լ�

void CListenSocket::OnAccept(int nErrorCode)
{
    //MessageBox(NULL,"3","3",MB_OK);
    //�Ź�Ŭ�� �����Ҷ� �̰�θ� ��������.
    //�� ���̾�α׳��� �� �����̳� ���񽺼����� ���� STLȭ���� �ؾ߰ٴ�.
    // TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
    m_pPublicServerDlg->ProcessAccept();
    CSocket::OnAccept(nErrorCode);
}

//void CListenSocket::OnReceive(int nErrorCode)
//{
//    // TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
//   // m_pPublicServerDlg->ProcessReceive(this);
//    CSocket::OnReceive(nErrorCode);
//}
//
//void CListenSocket::OnSend(int nErrorCode)
//{
//    // TODO: Add your specialized code here and/or call the base class
//
//    CSocket::OnSend(nErrorCode);
//}

void CListenSocket::OnSend(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class

    m_pPublicServerDlg->ProcessSend(this);
    CSocket::OnSend(nErrorCode);
}