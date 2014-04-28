// ReactionSock.cpp : implementation file
//

#include "stdafx.h"
#include "Launcher.h"
#include "ReactionSock.h"
#include "LauncherDlg.h"

// CReactionSock

CReactionSock::CReactionSock(CLauncherDlg* pLauncherDlg)
{
    m_strMyType = "nothing";
    m_pLauncherDlg = pLauncherDlg;
}

CReactionSock::~CReactionSock()
{
}


// CReactionSock member functions

void CReactionSock::OnClose(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class
    m_pLauncherDlg->ProcessClose(this);
    CSocket::OnClose(nErrorCode);
}

void CReactionSock::OnAccept(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class
 //   if (m_strMyType == "server")
        m_pLauncherDlg->ProcessAccept();
    CSocket::OnAccept(nErrorCode);
}

void CReactionSock::OnReceive(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class
    m_pLauncherDlg->ProcessReceive(this);
    CSocket::OnReceive(nErrorCode);
}
