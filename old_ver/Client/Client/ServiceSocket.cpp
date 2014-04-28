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

//이 소켓형에 연결된 *변수는 리시브할때를 감지한다.
//고로 이걸할때 바로 CClientDlg매인에 넣어줌 되겠다.
void CServiceSocket::OnReceive(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class
    m_pClientDlg->ProcessReceive(this);
    CSocket::OnReceive(nErrorCode);
}
