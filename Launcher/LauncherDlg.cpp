// LauncherDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "Launcher.h"
#include "LauncherDlg.h"
#include "ReactionSock.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CLIENT_RUNNING_PROGRAM 1588


// CLauncherDlg ��ȭ ����

CLauncherDlg::CLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLauncherDlg::IDD, pParent)
{
        m_pMySocket = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_SELECT, m_cbMyType);
    DDX_Control(pDX, IDC_LIST_SERVER_INFO, m_ctrListServer);
    DDX_Control(pDX, IDC_LIST_CLIENT_FILE_LST, m_ctrFileListClient);
    DDX_Control(pDX, IDC_LIST_SERVER_VIEW_CLIENT_STATE, m_ctrListViewClientState);   
}

BEGIN_MESSAGE_MAP(CLauncherDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
        ON_BN_CLICKED(IDC_CONNECT, &CLauncherDlg::OnBnClickedConnect)
        ON_BN_CLICKED(IDC_SELECT_MYTYPE, &CLauncherDlg::OnBnClickedSelectMytype)
        ON_BN_CLICKED(IDC_TYPE_CANCEL, &CLauncherDlg::OnBnClickedTypeCancel)
        ON_NOTIFY(NM_DBLCLK, IDC_LIST_SERVER_INFO, &CLauncherDlg::OnNMDblclkListServerInfo)
        ON_BN_CLICKED(IDC_DISCONNECT, &CLauncherDlg::OnBnClickedDisconnect)
        ON_BN_CLICKED(IDC_SEND_FILELIST_, &CLauncherDlg::OnBnClickedSendFilelist)
        ON_BN_CLICKED(IDC_ADD_FILE_LIST, &CLauncherDlg::OnBnClickedAddFileList)
        ON_BN_CLICKED(IDC_DEL_LIST, &CLauncherDlg::OnBnClickedDelList)
        ON_BN_CLICKED(IDC_LETS_RUN_CLIENT, &CLauncherDlg::OnBnClickedLetsRunClient)
        ON_WM_TIMER()
        ON_BN_CLICKED(IDC_LETS_OFF_CLIENT, &CLauncherDlg::OnBnClickedLetsOffClient)
        ON_BN_CLICKED(IDC_LETS_EXIT_CLIENT, &CLauncherDlg::OnBnClickedLetsExitClient)
END_MESSAGE_MAP()


// CLauncherDlg �޽��� ó����

BOOL CLauncherDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
        m_cbMyType.SetCurSel(0);
        m_ctrListServer.SetColumnHeader("");
        m_ctrListViewClientState.SetColumnHeader("");
        m_ctrFileListClient.SetColumnHeader("");


        //��Ʈ�ѷ� ���߱�
        GetDlgItem(IDC_IPADDRESS)->ShowWindow(false);
        GetDlgItem(IDC_CONNECT)->ShowWindow(false);
        GetDlgItem(IDC_DISCONNECT)->ShowWindow(false);
        GetDlgItem(IDC_LIST_SERVER_INFO)->ShowWindow(false);
        GetDlgItem(IDC_LIST_CLIENT_FILE_LST)->ShowWindow(false);
        GetDlgItem(IDC_LIST_SERVER_VIEW_CLIENT_STATE)->ShowWindow(false);
        GetDlgItem(IDC_ADD_FILE_LIST)->ShowWindow(false);
        GetDlgItem(IDC_DEL_LIST)->ShowWindow(false);
        GetDlgItem(IDC_LETS_RUN_CLIENT)->ShowWindow(false);
        GetDlgItem(IDC_SEND_FILELIST_)->ShowWindow(false);
        GetDlgItem(IDC_LETS_OFF_CLIENT)->ShowWindow(false);
        GetDlgItem(IDC_LETS_EXIT_CLIENT)->ShowWindow(false);
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CLauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLauncherDlg::ProcessAccept()
{
    CReactionSock* pServiceSocket = NULL;
    pServiceSocket = new CReactionSock(this);


    if (!m_pMySocket->Accept(*pServiceSocket))
    {
        return;
    }
    else
    {
        //pServiceSocket�� ������ ���Ϳ� ��Ƽ� ������ �̳����� ������ �Ѵ�.
        m_pVecServiceSocket.push_back(pServiceSocket);  
    }

    m_ctrListServer.DeleteAllItems();

    //Accept�� �ϸ� ����Ʈ �� �����ְ� �ٽ� ����������.
    vector<CReactionSock*>::iterator pos;

    for (pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
    {
        CString PeerAddress;
        UINT PeerPort;
        (*pos)->GetPeerName(PeerAddress, PeerPort);
        int nIdx = m_ctrListServer.InsertItem(INT_MAX, "");
        m_ctrListServer.SetItemText(nIdx, 0, PeerAddress);
        m_ctrListServer.SetItemText(nIdx, 1, PeerPort);
        m_ctrListServer.SetItemText(nIdx, 2, "-");
        m_ctrListServer.SetItemText(nIdx, 3, "-");
        m_ctrListServer.SetItemText(nIdx, 4, "-");
        //�߰����� - ���⼭ �ʿ� ����� ����/pack�� �������ؼ� �������� ���� ������Ѵ�.
        // 2,3,4 Ŭ���̾�Ʈ state ���� ���ϰ�α���
        map<CReactionSock*, pack>::iterator mapPos;
        for (mapPos = m_MmapManager.begin(); mapPos != m_MmapManager.end(); ++mapPos)
        {
            CString PeerAddress_mapInfo;
            UINT PeerPort_mapInfo;

            mapPos->first->GetPeerName(PeerAddress_mapInfo, PeerPort_mapInfo);

            //��Ʈ, ������ ��ġ�ϸ� ��ġ�Ǵ� �������� ���ֵ�
            if (PeerAddress == PeerAddress_mapInfo && PeerPort == PeerPort_mapInfo)
            {//CHECK_OK_CLIENT
                //������ġ�� Ȯ���߰� pack�� ���ؼ� ������ ������ ���÷���
                if (mapPos->second.ServerToClientOrder == CHECK_PLZ_SERVER)
                {
                    m_ctrListServer.SetItemText(nIdx, 2, "�޼����� �����߽��ϴ�.");
                    m_ctrListServer.SetItemTextColor(nIdx,2,RGB(255,0,0)); 
                }

                //üũ�Ϸ� ���÷��̿� ���� ó��
                //������ġ�� Ȯ���߰� pack�� ���ؼ� ������ ������ ���÷���
                if (mapPos->second.ServerToClientOrder == CHECK_PLZ_SERVER)
                {
                    m_ctrListServer.SetItemText(nIdx, 2, "üũ�Ϸ�");
                    m_ctrListServer.SetItemTextColor(nIdx,2,RGB(0,255,0));
                }
            }
        }

    }

}


void CLauncherDlg::ProcessReceive(CReactionSock* pSocket)
{
    pack RevPack;
    ZeroMemory(&RevPack, sizeof(RevPack));
    pSocket->Receive(&RevPack, sizeof(RevPack));

    //�������� ���ú� ó��
    if (m_cbMyType.GetCurSel() == 0)
    {
        //�ش� ������ ��������ȸ�� ����
        //���Ϳ� ����� ������ �����ͼ�
        //������ ��ġ�Ǹ� map������ <����, ��Ŷ>�� �����Ѵ�.

        vector<CReactionSock*>::iterator pos;

        for (pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
        {
            //������ ���ϵ��� ��ȸ�ؼ� ������ �����ϰ�
            CString PeerAddress_Pos;
            UINT PeerPort_Pos;
            (*pos)->GetPeerName(PeerAddress_Pos, PeerPort_Pos);

            //�ش� pSocket�� ������ �����Ͽ� ���� ��ġ�Ǹ�
            //�� �ش� ����, ��Ŷ���� �����·� ����
            CString PeerAddress_Temp;
            UINT PeerPort_Temp;
            pSocket->GetPeerName(PeerAddress_Temp, PeerPort_Temp);

            if ( PeerPort_Pos == PeerPort_Temp && PeerAddress_Pos == PeerAddress_Temp)
            {
                //key �ش����, value �ش� RevPack�� �ְ� 
                m_MmapManager[pSocket] = RevPack;

                //���������� �˸���.m_ctrListServer��...

                for (int Idx =0; Idx<m_ctrListServer.GetItemCount(); Idx++)
                {
                    //������ ����
                    CString IpStr = m_ctrListServer.GetItemText(Idx,0);
                    //��Ʈ��ȣ ����
                    CString PortStr = m_ctrListServer.GetItemText(Idx,1);

                    //��ġ�� �ȴٸ�....
                    if (PeerAddress_Temp == IpStr && PeerPort_Temp == _ttoi(PortStr))
                    {
                        //int nIdx = m_ctrListServer.InsertItem(INT_MAX, "");
                        m_ctrListServer.SetItemText(Idx, 2, "�޼����� �����߽��ϴ�.");
                        m_ctrListServer.SetItemTextColor(Idx,2,RGB(255,0,0)); 

                        if ( RevPack.ProgramOn )
                        {
                            m_ctrListServer.SetItemText(Idx, 2, "���α׷� ������");
                            m_ctrListServer.SetItemTextColor(Idx,2,RGB(255,0,0)); 
                            m_ctrListServer.SetItemText(Idx, 4, RevPack.ServerSendFile);
                            m_ctrListServer.SetItemTextColor(Idx,2,RGB(255,0,0)); 
                        }
                    }
                }
            }
        }
    }
    //Ŭ���̾�Ʈ���� ���ú� ó��
    if (m_cbMyType.GetCurSel() == 1)
    {
        //����� �ʰ������� ��Ŷ������ ã�ڴ�.
        //��Ŷ ����->Ŭ���ɾ� PROGRAM_GO
        if (RevPack.ServerToClientOrder == PROGRAM_GO)
        {
            //������ ������ ����� ������ Ȯ�� �Ŀ� 
            ZeroMemory(&m_si, sizeof(SHELLEXECUTEINFO));
            m_si.cbSize = sizeof(m_si);
            m_si.lpVerb = "open";

            m_si.lpFile = (LPSTR)(LPCTSTR)RevPack.ServerSendFile;
            m_si.nShow = SW_SHOWDEFAULT;
            m_si.fMask = SEE_MASK_FLAG_DDEWAIT | SEE_MASK_NOCLOSEPROCESS;

            if (ShellExecuteEx(&m_si))
            {
                //m_onlyClientTempPack�� �ӽú�����
                ZeroMemory(&m_onlyClientTempPack, sizeof(m_onlyClientTempPack));
                m_onlyClientTempPack = RevPack;

                //�� Ÿ�̸Ӵ� �ش� ����/RevPack �ʿ���
                //�׻� ���鼭 program_on�� ����ָ� ������ �������ش�.
                //���Ҿ� ��ɾ�� ���α׷� �������̶�� �޽����� �����ְ�
                SetTimer(CLIENT_RUNNING_PROGRAM, 100, NULL);
            }
        }
        //��Ŷ ����->Ŭ���ɾ� ���α׷�������.
        else if (RevPack.ServerToClientOrder == PROGRAM_EXIT)
        {
            if (TerminateProcess(m_si.hProcess,0))
            {
                m_onlyClientTempPack.ProgramOn = false;
                m_onlyClientTempPack.ServerToClientOrder = CHECK_PLZ_SERVER;
                m_pMySocket->Send((char*)&m_onlyClientTempPack, sizeof(m_onlyClientTempPack));
                KillTimer(CLIENT_RUNNING_PROGRAM);
            }
        }
        //��Ŷ ����->Ŭ���ɾ� ����������ɾ�
        else if (RevPack.ServerToClientOrder == SOCKET_OUT)
        {
            this->OnClose(0);
            exit(0);
        }
    }
}


void CLauncherDlg::ProcessClose(CReactionSock* pSocket)
{
    vector<CReactionSock*>::iterator pos;

    //pSocket�� ����Ǿ��ִ� ���ϵ��� ��ġ�Ǵ³��� ������ �װ�ã�Ƽ� close��Ų��.
    for (pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
    {
        CString PeerAddress;
        UINT PeerPort;
        (*pos)->GetPeerName(PeerAddress, PeerPort);

        CString PeerAddressTemp;
        UINT PeerPortTemp;
        pSocket->GetPeerName(PeerAddressTemp, PeerPortTemp);

        if (PeerAddress == PeerAddressTemp &&  PeerPort == PeerPortTemp)
        {
            //������ �ݺ��� ��ġ�� �����ä�� ���´�.
            m_pVecServiceSocket.erase(pos);
            break;
        }
    }

    //�ٽ� ���͸� �����鼭 ����
    m_ctrListServer.DeleteAllItems();
    for (pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
    {
        CString PeerAddress;
        UINT PeerPort;
        (*pos)->GetPeerName(PeerAddress, PeerPort);
        int nIdx = m_ctrListServer.InsertItem(INT_MAX, "");
        m_ctrListServer.SetItemText(nIdx, 0, PeerAddress);
        m_ctrListServer.SetItemText(nIdx, 1, PeerPort);
        m_ctrListServer.SetItemText(nIdx, 2, "-");
        m_ctrListServer.SetItemText(nIdx, 3, "-");
    }
}


void CLauncherDlg::OnConnect(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class

    if (m_pMySocket) 
    {
        delete m_pMySocket;
        m_pMySocket = NULL;
    }

    m_pMySocket = new CReactionSock(this);
    if(!m_pMySocket->Create())
    {
        MessageBox("Create() - false");
    }

    m_pMySocket->SetMyType(CString("client"));
    SetWindowText("client");

    CIPAddressCtrl *pAddr = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS);

    if (!pAddr->IsBlank())
    {
        DWORD IpAddress;
        pAddr->GetAddress(IpAddress);

        BYTE a, b, c, d;

        a = HIBYTE(HIWORD(IpAddress));   
        b = LOBYTE(HIWORD(IpAddress));   
        c = HIBYTE(LOWORD(IpAddress));   
        d = LOBYTE(LOWORD(IpAddress));   

        CString strIpAddress;
        strIpAddress.Format("%d.%d.%d.%d", a,b,c,d);

        if (!m_pMySocket->Connect(strIpAddress, 9000))
        {
            MessageBox("Connect() - error");
        }
    }
    __super::OnConnect(nErrorCode);
}

BOOL CLauncherDlg::DestroyWindow()
{
    // TODO: Add your specialized code here and/or call the base class
    if (m_pMySocket) delete m_pMySocket;

    return __super::DestroyWindow();
}

void CLauncherDlg::OnBnClickedConnect()
{
    // TODO: Add your control notification handler code here
    OnConnect(0);
    GetDlgItem(IDC_IPADDRESS)->EnableWindow(false);
    GetDlgItem(IDC_CONNECT)->EnableWindow(false);
    GetDlgItem(IDC_DISCONNECT)->ShowWindow(true);
}

void CLauncherDlg::OnBnClickedSelectMytype()
{
    // TODO: Add your control notification handler code here
    int CurSel = m_cbMyType.GetCurSel();

    if (CurSel == 0) //����
    {
        InitServer(); //������ ���õ� init �� ���÷��� ����
    }
    else if (CurSel == 1) //Ŭ��
    {
        InitClient(); //Ŭ�� ���õ� init �� ���÷��� ����
    }
    else
    {
        return;
    }

    m_cbMyType.EnableWindow(false);
    GetDlgItem(IDC_SELECT_MYTYPE)->EnableWindow(false);
}

void CLauncherDlg::OnBnClickedTypeCancel()
{
    // TODO: Add your control notification handler code here
    if (m_pMySocket)
    {
        delete m_pMySocket;
        m_pMySocket = NULL;
    }
    m_cbMyType.EnableWindow(true);
    GetDlgItem(IDC_SELECT_MYTYPE)->EnableWindow(true);
    SetWindowText("Launcher");
    m_ctrListServer.SetColumnHeader("");


    //��Ʈ�ѷ� ���߱�
    GetDlgItem(IDC_IPADDRESS)->ShowWindow(false);
    GetDlgItem(IDC_CONNECT)->ShowWindow(false);
    GetDlgItem(IDC_DISCONNECT)->ShowWindow(false);
    GetDlgItem(IDC_LIST_SERVER_INFO)->ShowWindow(false);
    GetDlgItem(IDC_LIST_CLIENT_FILE_LST)->ShowWindow(false);
    GetDlgItem(IDC_LIST_SERVER_VIEW_CLIENT_STATE)->ShowWindow(false);
    GetDlgItem(IDC_ADD_FILE_LIST)->ShowWindow(false);
    GetDlgItem(IDC_DEL_LIST)->ShowWindow(false);
    GetDlgItem(IDC_LETS_RUN_CLIENT)->ShowWindow(false);
    GetDlgItem(IDC_SEND_FILELIST_)->ShowWindow(false);
    GetDlgItem(IDC_LETS_OFF_CLIENT)->ShowWindow(false);
    GetDlgItem(IDC_LETS_EXIT_CLIENT)->ShowWindow(false);
}


void CLauncherDlg::InitServer()
{
    if (m_pMySocket) 
    {
        delete m_pMySocket;
        m_pMySocket = NULL;
    }

    if (!AfxSocketInit())
    {
        AfxMessageBox("���� ��������");
        return;
    }

    m_pMySocket = new CReactionSock(this);

    if (!m_pMySocket->Create(9000))
    {
        AfxMessageBox("���� ��������");
        return;
    }
    if (!m_pMySocket->Listen())
    {
        AfxMessageBox("��������");
        return;
    }

    m_pMySocket->SetMyType(CString("server"));
    SetWindowText("server");

    m_ctrListServer.DeleteAllItems();

    m_ctrListServer.SetGridLines(TRUE);
    m_ctrListServer.SetEditable(FALSE);
    m_ctrListServer.SetSortable(FALSE);
    m_ctrListServer.SetColumnHeader("Ip Adress,90;Port,50;Client State,150;���ϸ�,90;���ϰ��,250");

    m_ctrListViewClientState.DeleteAllItems();
    m_ctrListViewClientState.SetGridLines(TRUE);
    m_ctrListViewClientState.SetEditable(FALSE);
    m_ctrListViewClientState.SetSortable(FALSE);
    m_ctrListViewClientState.SetColumnHeader("���డ�� ����,90;���� ���,200;");




    //��Ʈ�ѷ� ���̱�
    GetDlgItem(IDC_IPADDRESS)->ShowWindow(false);
    GetDlgItem(IDC_CONNECT)->ShowWindow(false);
    GetDlgItem(IDC_DISCONNECT)->ShowWindow(false);
    GetDlgItem(IDC_LIST_SERVER_INFO)->ShowWindow(true);
    GetDlgItem(IDC_LIST_CLIENT_FILE_LST)->ShowWindow(false);
    GetDlgItem(IDC_LIST_SERVER_VIEW_CLIENT_STATE)->ShowWindow(true);
    GetDlgItem(IDC_ADD_FILE_LIST)->ShowWindow(false);
    GetDlgItem(IDC_DEL_LIST)->ShowWindow(false);
    GetDlgItem(IDC_LETS_RUN_CLIENT)->ShowWindow(true);
    GetDlgItem(IDC_SEND_FILELIST_)->ShowWindow(false);
    GetDlgItem(IDC_LETS_OFF_CLIENT)->ShowWindow(true);
    GetDlgItem(IDC_LETS_EXIT_CLIENT)->ShowWindow(true);

}

void CLauncherDlg::InitClient()
{
    if (m_pMySocket) 
    {
        delete m_pMySocket;
        m_pMySocket = NULL;
    }

    if (!AfxSocketInit())
    {
        AfxMessageBox("���� ��������");
        return;
    }
    //m_pMySocket = new CReactionSock(this);
    //m_pMySocket->Create(9000);

    //m_pMySocket->SetMyType(CString("client"));
    SetWindowText("client");

    m_ctrFileListClient.DeleteAllItems();
    m_ctrFileListClient.SetGridLines(TRUE);
    m_ctrFileListClient.SetEditable(FALSE);
    m_ctrFileListClient.SetSortable(FALSE);
    m_ctrFileListClient.SetColumnHeader("���ϸ�,90;���,200;");


    //��Ʈ�ѷ� ���̱�
    GetDlgItem(IDC_IPADDRESS)->ShowWindow(true);
    GetDlgItem(IDC_CONNECT)->ShowWindow(true);
    GetDlgItem(IDC_DISCONNECT)->ShowWindow(true);
    GetDlgItem(IDC_LIST_SERVER_INFO)->ShowWindow(false);
    GetDlgItem(IDC_LIST_CLIENT_FILE_LST)->ShowWindow(true);
    GetDlgItem(IDC_LIST_SERVER_VIEW_CLIENT_STATE)->ShowWindow(false);
    GetDlgItem(IDC_ADD_FILE_LIST)->ShowWindow(true);
    GetDlgItem(IDC_DEL_LIST)->ShowWindow(true);
    GetDlgItem(IDC_LETS_RUN_CLIENT)->ShowWindow(false);
    GetDlgItem(IDC_SEND_FILELIST_)->ShowWindow(true);
}
void CLauncherDlg::OnNMDblclkListServerInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    *pResult = 0;
    int Idx = m_ctrListServer.GetSelectionMark();
    CString str = m_ctrListServer.GetItemText(Idx,1);

    UINT SelectPort = _ttoi(str);

    m_ctrListViewClientState.DeleteAllItems();
    m_ctrListViewClientState.SetGridLines(TRUE);
    m_ctrListViewClientState.SetEditable(FALSE);
    m_ctrListViewClientState.SetSortable(FALSE);
    m_ctrListViewClientState.SetColumnHeader("���డ�� ����,90;���� ���,200;ip,90;port,90;");
    
    map<CReactionSock*, pack>::iterator pos;
    for (pos = m_MmapManager.begin(); pos != m_MmapManager.end(); ++pos)
    {
        CString PeerAddress;
        UINT PeerPort;
        pos->first->GetPeerName(PeerAddress, PeerPort);

        if (PeerPort == SelectPort)
        {
            int Count = pos->second.ListCount;
            //0 ���ϸ�, 1 ��θ�, 2������, 3��Ʈ��ȣ
            pos->second.ServerToClientOrder = CHECK_OK_CLIENT;
            for (int Idx=0; Idx<Count; Idx++)
            {
                int nIdx = m_ctrListViewClientState.InsertItem(INT_MAX, "");
                m_ctrListViewClientState.SetItemText(nIdx, 0, _T(pos->second.File[nIdx]));
                m_ctrListViewClientState.SetItemText(nIdx, 1, _T(pos->second.FilePath[nIdx]));
                m_ctrListViewClientState.SetItemText(nIdx, 2, PeerAddress);
                m_ctrListViewClientState.SetItemText(nIdx, 3, PeerPort);
            }
        }

        //pack�� �� �޽������¸� ����Ŭ���� �ٲ�����ϴµ�
        //Ű���� ���ϰ� ������ ������ ��ġ�ϰ� �Ǹ� �װ��� �ٲ���� �޽���
        //ó���� ��Ȱ�� �ۿ��� �ҵ��ϴ�.
        vector<CReactionSock*>::iterator vecPos;
        for (vecPos = m_pVecServiceSocket.begin(); vecPos != m_pVecServiceSocket.end(); ++vecPos)
        {
            CString PeerAddress_;
            UINT PeerPort_;
            (*vecPos)->GetPeerName(PeerAddress_, PeerPort_);

            if (PeerAddress == PeerAddress_ && PeerPort == PeerPort_)
            {
                if (pos->second.ServerToClientOrder == CHECK_PLZ_SERVER)
                {
                    pos->second.ServerToClientOrder = CHECK_OK_CLIENT;
                }
                else if (pos->second.ServerToClientOrder == CHECK_OK_CLIENT)
                {
                    pos->second.ServerToClientOrder = CHECK_PLZ_SERVER;
                }
            }
        }



    }

    //Ŭ���̾�Ʈ���°� "�޼����� �����߽��ϴ�."�϶� ����Ʈ�� �ߴٸ�...
    str = m_ctrListServer.GetItemText(Idx,2);

    if (str == "�޼����� �����߽��ϴ�.")
    {
        m_ctrListServer.SetItemText(Idx, 2, "üũ�Ϸ�");
        m_ctrListServer.SetItemTextColor(Idx,2,RGB(0,255,0));
    }
}

void CLauncherDlg::OnClose(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class
    if (m_pMySocket)
    {
        m_pMySocket->Close();
        delete m_pMySocket;
    }

    m_pMySocket = NULL;

    __super::OnClose(nErrorCode);
}

void CLauncherDlg::OnBnClickedDisconnect()
{
    // TODO: Add your control notification handler code here
    OnClose(NULL);
    GetDlgItem(IDC_CONNECT)->EnableWindow(true);
    GetDlgItem(IDC_DISCONNECT)->ShowWindow(true);
    GetDlgItem(IDC_IPADDRESS)->EnableWindow(true);
    m_ctrListViewClientState.DeleteAllItems();
}

//Ŭ���̾�Ʈ -> ������ ���� ����� ��� ������.
void CLauncherDlg::OnBnClickedSendFilelist()
{
    // TODO: Add your control notification handler code here
    pack pack;
    ZeroMemory(&pack, sizeof(pack));

    int Count = m_ctrFileListClient.GetItemCount();

    for (int Idx = 0; Idx < Count; Idx++)
    {
        strcpy(pack.File[Idx], m_ctrFileListClient.GetItemText(Idx,0));
        strcpy(pack.FilePath[Idx], m_ctrFileListClient.GetItemText(Idx,1));
    }


    strcpy(pack.ServerSendFile, "test");
    pack.ListCount = Count;
    pack.ServerToClientOrder = CHECK_PLZ_SERVER;

    m_pMySocket->Send((char*)&pack, sizeof(pack));

    for (int Idx = 0; Idx < Count; Idx++)
    m_ctrFileListClient.SetItemBkColor(Idx,0,RGB(0,255,0));
}

void CLauncherDlg::OnBnClickedAddFileList()
{
    // TODO: Add your control notification handler code here

    char folderpath[256];
    GetCurrentDirectory(245, folderpath);

    CFileDialog dlg(true, NULL, NULL, OFN_HIDEREADONLY);
    dlg.m_ofn.lpstrInitialDir=_T(folderpath);

    if (dlg.DoModal() == IDOK)
    {
        CString strTemp;

        int nIdx = m_ctrFileListClient.InsertItem(INT_MAX, "");
        //0 ���ϸ�, 1 ���ϰ��
        m_ctrFileListClient.SetItemText(nIdx, 0, dlg.GetFileName());
        m_ctrFileListClient.SetItemText(nIdx, 1, dlg.GetPathName());
    }
}

void CLauncherDlg::OnBnClickedDelList()
{
    // TODO: Add your control notification handler code here

    int Idx = m_ctrFileListClient.GetSelectionMark();

    if (Idx > -1)
    m_ctrFileListClient.DeleteItem(Idx);
}

void CLauncherDlg::OnBnClickedLetsRunClient()
{
    // TODO: Add your control notification handler code here
    //�������� ���? � Ŭ���̾�Ʈ����???
    //x�� 2��, 3���� �����ǿ� ��Ʈ��ȣ�� ���ؼ� ������ �� �� �ִ�.
    int Idx = m_ctrListViewClientState.GetSelectionMark();
    CString IpStr = m_ctrListViewClientState.GetItemText(Idx, 2);
    CString PortStr = m_ctrListViewClientState.GetItemText(Idx, 3);

    map<CReactionSock*, pack>::iterator pos;

    for (pos = m_MmapManager.begin(); pos != m_MmapManager.end(); ++pos)
    {
        CString PeerAddress;
        UINT PeerPort;
        pos->first->GetPeerName(PeerAddress, PeerPort);

        if (PeerAddress == IpStr && PeerPort == _ttoi(PortStr))
        {
            
            strcpy(pos->second.ServerSendFile, m_ctrListViewClientState.GetItemText(Idx,1));
            pos->second.ServerToClientOrder = PROGRAM_GO;
            pos->first->Send((char*)&pos->second, sizeof(pack));
            break;
        }
    }
    //vector<CReactionSock*>::iterator pos;
    //for (pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
    //{
    //    CString PeerAddress;
    //    UINT PeerPort;
    //    (*pos)->GetPeerName(PeerAddress, PeerPort);

    //    if (PeerAddress == IpStr &&  PeerPort == _ttoi(PortStr))
    //    {
    //        (*pos)->Send((char*)&pack, sizeof(pack));
    //        break;
    //    }
    //}
}

void CLauncherDlg::OnBnClickedLetsOffClient()
{
    // TODO: Add your control notification handler code here
    int Idx = m_ctrListViewClientState.GetSelectionMark();
    CString IpStr = m_ctrListViewClientState.GetItemText(Idx, 2);
    CString PortStr = m_ctrListViewClientState.GetItemText(Idx, 3);

    map<CReactionSock*, pack>::iterator pos;

    for (pos = m_MmapManager.begin(); pos != m_MmapManager.end(); ++pos)
    {
        CString PeerAddress;
        UINT PeerPort;
        pos->first->GetPeerName(PeerAddress, PeerPort);

        if (PeerAddress == IpStr && PeerPort == _ttoi(PortStr))
        {
            
            strcpy(pos->second.ServerSendFile, m_ctrListViewClientState.GetItemText(Idx,1));
            pos->second.ServerToClientOrder = PROGRAM_EXIT;
            pos->first->Send((char*)&pos->second, sizeof(pack));
            break;
        }
    }
}

void CLauncherDlg::OnBnClickedLetsExitClient()
{
    // TODO: Add your control notification handler code here
    int Idx = m_ctrListViewClientState.GetSelectionMark();
    CString IpStr = m_ctrListViewClientState.GetItemText(Idx, 2);
    CString PortStr = m_ctrListViewClientState.GetItemText(Idx, 3);

    map<CReactionSock*, pack>::iterator pos;

    for (pos = m_MmapManager.begin(); pos != m_MmapManager.end(); ++pos)
    {
        CString PeerAddress;
        UINT PeerPort;
        pos->first->GetPeerName(PeerAddress, PeerPort);

        if (PeerAddress == IpStr && PeerPort == _ttoi(PortStr))
        {
            
            strcpy(pos->second.ServerSendFile, m_ctrListViewClientState.GetItemText(Idx,1));
            pos->second.ServerToClientOrder = SOCKET_OUT;
            pos->first->Send((char*)&pos->second, sizeof(pack));
            break;
        }
    }
}

void CLauncherDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default

    if (nIDEvent == CLIENT_RUNNING_PROGRAM)
    {
        DWORD ec;
        GetExitCodeProcess(m_si.hProcess, &ec);

        if (ec == STILL_ACTIVE)
        {
            m_onlyClientTempPack.ProgramOn = true;
            m_pMySocket->Send((char*)&m_onlyClientTempPack, sizeof(m_onlyClientTempPack));
        }
        else
        {
            m_onlyClientTempPack.ProgramOn = false;
            m_onlyClientTempPack.ServerToClientOrder = CHECK_PLZ_SERVER;
            m_pMySocket->Send((char*)&m_onlyClientTempPack, sizeof(m_onlyClientTempPack));
            KillTimer(CLIENT_RUNNING_PROGRAM);
        }
    }
    __super::OnTimer(nIDEvent);
}

