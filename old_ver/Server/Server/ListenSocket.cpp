// ListenSocket.cpp : 구현 파일입니다.
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


// CListenSocket 멤버 함수

void CListenSocket::OnAccept(int nErrorCode)
{
    //MessageBox(NULL,"3","3",MB_OK);
    //매번클라가 접속할때 이경로를 지나간다.
    //즉 다이얼로그내에 이 리슨이나 서비스소켓의 값을 STL화시켜 해야겟다.
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    m_pPublicServerDlg->ProcessAccept();
    CSocket::OnAccept(nErrorCode);
}

//void CListenSocket::OnReceive(int nErrorCode)
//{
//    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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