// ServiceSocket.cpp : 구현 파일입니다.
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


// CServiceSocket 멤버 함수

void CServiceSocket::OnReceive(int nErrorCode)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    m_pPublicServerDlg->ProcessReceive(this);
    CSocket::OnReceive(nErrorCode);
}

void CServiceSocket::OnClose(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class
    m_pPublicServerDlg->ProcessClose(this);
    CSocket::OnClose(nErrorCode);
}