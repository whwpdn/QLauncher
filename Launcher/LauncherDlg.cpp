// LauncherDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Launcher.h"
#include "LauncherDlg.h"
#include "ReactionSock.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CLIENT_RUNNING_PROGRAM 1588


// CLauncherDlg 대화 상자

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


// CLauncherDlg 메시지 처리기

BOOL CLauncherDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
        m_cbMyType.SetCurSel(0);
        m_ctrListServer.SetColumnHeader("");
        m_ctrListViewClientState.SetColumnHeader("");
        m_ctrFileListClient.SetColumnHeader("");


        //컨트롤러 감추기
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
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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
        //pServiceSocket를 별도로 벡터에 담아서 앞으로 이놈으로 관리를 한다.
        m_pVecServiceSocket.push_back(pServiceSocket);  
    }

    m_ctrListServer.DeleteAllItems();

    //Accept를 하면 리스트 다 지워주고 다시 갱신해주자.
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
        //추가수정 - 여기서 맵에 저장된 소켓/pack을 비교점검해서 원상태의 값은 살려야한다.
        // 2,3,4 클라이언트 state 부터 파일경로까지
        map<CReactionSock*, pack>::iterator mapPos;
        for (mapPos = m_MmapManager.begin(); mapPos != m_MmapManager.end(); ++mapPos)
        {
            CString PeerAddress_mapInfo;
            UINT PeerPort_mapInfo;

            mapPos->first->GetPeerName(PeerAddress_mapInfo, PeerPort_mapInfo);

            //포트, 아이피 일치하면 일치되는 소켓으로 간주됨
            if (PeerAddress == PeerAddress_mapInfo && PeerPort == PeerPort_mapInfo)
            {//CHECK_OK_CLIENT
                //소켓일치를 확인했고 pack을 통해서 정보를 끄집어 디스플레이
                if (mapPos->second.ServerToClientOrder == CHECK_PLZ_SERVER)
                {
                    m_ctrListServer.SetItemText(nIdx, 2, "메세지가 도착했습니다.");
                    m_ctrListServer.SetItemTextColor(nIdx,2,RGB(255,0,0)); 
                }

                //체크완료 디스플레이에 대한 처리
                //소켓일치를 확인했고 pack을 통해서 정보를 끄집어 디스플레이
                if (mapPos->second.ServerToClientOrder == CHECK_PLZ_SERVER)
                {
                    m_ctrListServer.SetItemText(nIdx, 2, "체크완료");
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

    //서버에서 리시브 처리
    if (m_cbMyType.GetCurSel() == 0)
    {
        //해당 소켓의 아이피조회를 통해
        //벡터에 저장된 소켓을 가져와서
        //소켓이 일치되면 map형태의 <소켓, 패킷>을 저장한다.

        vector<CReactionSock*>::iterator pos;

        for (pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
        {
            //벡터의 소켓들을 순회해서 정보를 추출하고
            CString PeerAddress_Pos;
            UINT PeerPort_Pos;
            (*pos)->GetPeerName(PeerAddress_Pos, PeerPort_Pos);

            //해당 pSocket의 정보를 추출하여 비교후 일치되면
            //그 해당 소켓, 패킷으로 맵형태로 저장
            CString PeerAddress_Temp;
            UINT PeerPort_Temp;
            pSocket->GetPeerName(PeerAddress_Temp, PeerPort_Temp);

            if ( PeerPort_Pos == PeerPort_Temp && PeerAddress_Pos == PeerAddress_Temp)
            {
                //key 해당소켓, value 해당 RevPack을 넣고 
                m_MmapManager[pSocket] = RevPack;

                //도착했음을 알린다.m_ctrListServer에...

                for (int Idx =0; Idx<m_ctrListServer.GetItemCount(); Idx++)
                {
                    //아이피 추출
                    CString IpStr = m_ctrListServer.GetItemText(Idx,0);
                    //포트번호 추출
                    CString PortStr = m_ctrListServer.GetItemText(Idx,1);

                    //일치가 된다면....
                    if (PeerAddress_Temp == IpStr && PeerPort_Temp == _ttoi(PortStr))
                    {
                        //int nIdx = m_ctrListServer.InsertItem(INT_MAX, "");
                        m_ctrListServer.SetItemText(Idx, 2, "메세지가 도착했습니다.");
                        m_ctrListServer.SetItemTextColor(Idx,2,RGB(255,0,0)); 

                        if ( RevPack.ProgramOn )
                        {
                            m_ctrListServer.SetItemText(Idx, 2, "프로그램 동작중");
                            m_ctrListServer.SetItemTextColor(Idx,2,RGB(255,0,0)); 
                            m_ctrListServer.SetItemText(Idx, 4, RevPack.ServerSendFile);
                            m_ctrListServer.SetItemTextColor(Idx,2,RGB(255,0,0)); 
                        }
                    }
                }
            }
        }
    }
    //클라이언트에서 리시브 처리
    if (m_cbMyType.GetCurSel() == 1)
    {
        //저장된 맵공간에서 패킷정보를 찾겠다.
        //패킷 서버->클라명령어 PROGRAM_GO
        if (RevPack.ServerToClientOrder == PROGRAM_GO)
        {
            //서버가 지정한 경로의 파일을 확인 후에 
            ZeroMemory(&m_si, sizeof(SHELLEXECUTEINFO));
            m_si.cbSize = sizeof(m_si);
            m_si.lpVerb = "open";

            m_si.lpFile = (LPSTR)(LPCTSTR)RevPack.ServerSendFile;
            m_si.nShow = SW_SHOWDEFAULT;
            m_si.fMask = SEE_MASK_FLAG_DDEWAIT | SEE_MASK_NOCLOSEPROCESS;

            if (ShellExecuteEx(&m_si))
            {
                //m_onlyClientTempPack는 임시보관소
                ZeroMemory(&m_onlyClientTempPack, sizeof(m_onlyClientTempPack));
                m_onlyClientTempPack = RevPack;

                //이 타이머는 해당 소켓/RevPack 맵에서
                //항상 돌면서 program_on을 찍어주며 서버로 샌드해준다.
                //더불어 명령어는 프로그램 런닝중이라는 메시지도 보내주고
                SetTimer(CLIENT_RUNNING_PROGRAM, 100, NULL);
            }
        }
        //패킷 서버->클라명령어 프로그램끄세요.
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
        //패킷 서버->클라명령어 소켓해제명령어
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

    //pSocket와 저장되어있는 소켓들중 일치되는놈이 있으면 그걸찾아서 close시킨다.
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
            //현재의 반복자 위치를 기억한채로 나온다.
            m_pVecServiceSocket.erase(pos);
            break;
        }
    }

    //다시 벡터를 읽으면서 갱신
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

    if (CurSel == 0) //서버
    {
        InitServer(); //서버에 관련된 init 및 디스플레이 셋팅
    }
    else if (CurSel == 1) //클라
    {
        InitClient(); //클라에 관련된 init 및 디스플레이 셋팅
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


    //컨트롤러 감추기
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
        AfxMessageBox("소켓 생성오류");
        return;
    }

    m_pMySocket = new CReactionSock(this);

    if (!m_pMySocket->Create(9000))
    {
        AfxMessageBox("소켓 생성오류");
        return;
    }
    if (!m_pMySocket->Listen())
    {
        AfxMessageBox("리슨오류");
        return;
    }

    m_pMySocket->SetMyType(CString("server"));
    SetWindowText("server");

    m_ctrListServer.DeleteAllItems();

    m_ctrListServer.SetGridLines(TRUE);
    m_ctrListServer.SetEditable(FALSE);
    m_ctrListServer.SetSortable(FALSE);
    m_ctrListServer.SetColumnHeader("Ip Adress,90;Port,50;Client State,150;파일명,90;파일경로,250");

    m_ctrListViewClientState.DeleteAllItems();
    m_ctrListViewClientState.SetGridLines(TRUE);
    m_ctrListViewClientState.SetEditable(FALSE);
    m_ctrListViewClientState.SetSortable(FALSE);
    m_ctrListViewClientState.SetColumnHeader("실행가능 파일,90;파일 경로,200;");




    //컨트롤러 보이기
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
        AfxMessageBox("소켓 생성오류");
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
    m_ctrFileListClient.SetColumnHeader("파일명,90;경로,200;");


    //컨트롤러 보이기
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
    m_ctrListViewClientState.SetColumnHeader("실행가능 파일,90;파일 경로,200;ip,90;port,90;");
    
    map<CReactionSock*, pack>::iterator pos;
    for (pos = m_MmapManager.begin(); pos != m_MmapManager.end(); ++pos)
    {
        CString PeerAddress;
        UINT PeerPort;
        pos->first->GetPeerName(PeerAddress, PeerPort);

        if (PeerPort == SelectPort)
        {
            int Count = pos->second.ListCount;
            //0 파일명, 1 경로명, 2아이피, 3포트번호
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

        //pack의 현 메시지상태를 더블클릭에 바꿔줘야하는데
        //키값이 소켓과 벡터의 소켓이 일치하게 되면 그것을 바꿔줘야 메시지
        //처리에 원활한 작용을 할듯하다.
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

    //클라이언트상태가 "메세지가 도착했습니다."일때 셀렉트를 했다면...
    str = m_ctrListServer.GetItemText(Idx,2);

    if (str == "메세지가 도착했습니다.")
    {
        m_ctrListServer.SetItemText(Idx, 2, "체크완료");
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

//클라이언트 -> 서버간 파일 목록을 담아 보낸다.
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
        //0 파일명, 1 파일경로
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
    //누구에게 명령? 어떤 클라이언트에게???
    //x행 2열, 3열에 아이피와 포트번호를 통해서 정보를 알 수 있다.
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

