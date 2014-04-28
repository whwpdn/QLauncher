// ServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//-----------------------------------------------------//
void ErrorMsg(char *str);
bool InitLog();
void FreeLog();
void OutLog(char *str, ...);
extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow();
//-----------------------------------------------------//
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CServerDlg 대화 상자

CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDlg::IDD, pParent)
{
        m_pListenSocket = NULL;
        m_pServiceSocket = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
        m_bNowTimer = false;
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_DEBUGINFO, m_editPublicDebugInfo);
    DDX_Control(pDX, IDC_LIST_CLIENTLIST, m_listPublicClient);
//    DDX_Control(pDX, IDC_EDIT_CLIENTINFO, m_editPublicInfo);
    DDX_Control(pDX, IDC_LIST_PROGRAM, m_listPublicProgramList);
    DDX_Control(pDX, IDC_LIST_TOTAL_MESSAGE, m_listPublicTotalMessage);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
        ON_WM_TIMER()
        ON_WM_DESTROY()
        ON_BN_CLICKED(IDCHECK, &CServerDlg::OnBnClickedCheck)
        ON_LBN_SELCHANGE(IDC_LIST_CLIENTLIST, &CServerDlg::OnLbnSelchangeListClientlist)
        ON_BN_CLICKED(ID_BUTTON_OUT, &CServerDlg::OnBnClickedButtonOut)
        ON_BN_CLICKED(ID_BUTTON_GOPROGRAM, &CServerDlg::OnBnClickedButtonGoprogram)
        ON_LBN_SELCHANGE(IDC_LIST_PROGRAM, &CServerDlg::OnLbnSelchangeListProgram)
        ON_BN_CLICKED(ID_BUTTON_OUTPROGRAM, &CServerDlg::OnBnClickedButtonOutprogram)
END_MESSAGE_MAP()


// CServerDlg 메시지 처리기

BOOL CServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
        InitLog();

        if (!AfxSocketInit())
        {
            AfxMessageBox("소켓 생성오류");
        }
        m_pListenSocket = new CListenSocket(this);

        if (!m_pListenSocket->Create(9000))
        {
            AfxMessageBox("소켓 생성오류");
        }
        else
        {
            if (!m_pListenSocket->Listen())
            {
                AfxMessageBox("리슨오류");
            }
        }

        //묘하게 퍼온함수 GetMyIP는 반대로 값을 잡는다. 어짜피 표시하는데 쓰이는 함수이기에 여기에 또 반대로작성한다
        DWORD myIP = GetMyIP();
        CString strIP;
        strIP.Format("%d.%d.%d.%d",
            FOURTH_IPADDRESS(myIP),THIRD_IPADDRESS(myIP),SECOND_IPADDRESS(myIP),FIRST_IPADDRESS(myIP));

        SetWindowText("서버 IP : "+strIP);


        //m_listPublicTotalMessage 는 전체메세지버튼과 관련하여 작동
        m_listPublicTotalMessage.AddString("프로그램 실행");
        m_listPublicTotalMessage.AddString("프로그램 종료");
        m_listPublicTotalMessage.AddString("소켓종료");
        
        m_listPublicProgramList.EnableWindow(false);

        SetTimer(100, 1000, NULL);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServerDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (nIDEvent == 100)
    {
       int index = m_listPublicClient.GetCurSel() + 1;

       //CString str;
       //str.Format("%d", index);

       m_editPublicDebugInfo.SetWindowText("");

       //==========================================================

        vector<saveportnum>::iterator pos;
        for(pos = m_vecSavePortNum.begin(); pos != m_vecSavePortNum.end(); ++pos)
        {
            if(pos->index == index)
            {
                OutLog("타이머에 index = %d", index);
                CString str;
                if(pos->processing)
                {
                    str.Format("index = %d, 현재 실행중",pos->index);
                    m_editPublicDebugInfo.SetWindowText(str);
                }
                else
                {   str.Format("index = %d, 미실행중", pos->index);
                    m_editPublicDebugInfo.SetWindowText(str);
                }
            }
        }
    }


    CDialog::OnTimer(nIDEvent);
}

void CServerDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.

    if(m_pListenSocket)     delete m_pListenSocket;
    if(m_pServiceSocket)    delete m_pServiceSocket;
    FreeLog();
}

void CServerDlg::ProcessAccept()
{
    OutLog("accept"); 
   
    CServiceSocket* pServiceSocket = NULL;
    pServiceSocket = new CServiceSocket(this);
 
    if (m_pListenSocket->Accept(*pServiceSocket))
    {
        m_pVecServiceSocket.push_back(pServiceSocket);

        //현 접속자 현황
        vector<CServiceSocket*>::iterator pos;
        CString PeerAddress;
        UINT PeerPort;
        for(pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
        {
            (*pos)->GetPeerName(PeerAddress, PeerPort);
            OutLog("ip : %s, port : %d",(LPCTSTR)PeerAddress, PeerPort);
        }
        CString strPort;
        strPort.Format("포트 : %d    ",PeerPort);
        
        //접속인원 리스트박스에 추가
        m_listPublicClient.AddString(strPort + (LPCTSTR)PeerAddress);

        //더붙여 포트번호와 인덱스(리스트박스의 위치인덱스)를 저장하는 구조체
        //그리고 그걸 담는 벡터컨테이너로써 관리를 하면 되겠다.
        saveportnum tempPortNum; //포트번호와 해당인덱스를 넣는다.
        tempPortNum.portNum = PeerPort;
        tempPortNum.index = m_listPublicClient.GetListBoxInfo();
        tempPortNum.processing = false;
        tempPortNum.filename = "";
        m_vecSavePortNum.push_back(tempPortNum);
    }

    OutLog("접속인원 : %d 명", m_pVecServiceSocket.size());
}

void CServerDlg::ProcessReceive(CServiceSocket* pServiceSocket)
{
    client_serv client_serv;
    ZeroMemory(&client_serv, sizeof(client_serv));

    pServiceSocket->Receive(&client_serv, sizeof(client_serv));

    //받고 그 해당 클라포트에 맞는데에 다음 정보를 넣어주면 되겠다.
    //vector<saveportnum>     m_vecSavePortNum;

    // 클라이언트에서 받은 데이터를 accept할때 저장했던 포트번호를 통해
    // 인원정보와 일치하는 정보를 찾아
    // 단순히 어떤 파일을 사용할 것인지 체크를 해서 saveportnum구조체의 마지막 
    // 데이터 filename에 파일명을 넣어두어 서버에서 어떤 파일인지 알게끔 한다.

    vector<saveportnum>::iterator pos;
    for(pos = m_vecSavePortNum.begin(); pos != m_vecSavePortNum.end(); ++pos)
    {
        CString strIpAdress;
        UINT Port;
        pServiceSocket->GetPeerName(strIpAdress, Port);

        if(pos->portNum == Port)
        {
            //char[] -> CString
            pos->filename = (CString)client_serv.filename;
            pos->processing = client_serv.program_on;
            OutLog("인덱스!? %d",pos->index);

            //m_editPublicDebugInfo = ""
        }
    }
}

void CServerDlg::ProcessClose(CServiceSocket* pServiceSocket)
{
    vector<CServiceSocket*>::iterator pos;
    bool closeCheck = false;
    CString PeerAddress;
    UINT PeerPort;

    for(pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
    {
        (*pos)->GetPeerName(PeerAddress, PeerPort);

        CString tempPeerAddress;
        UINT tempPeerPort;

        //현재 끝 낼 서비스소켓 주소와 포트번호를 찾는다.
        pServiceSocket->GetPeerName(tempPeerAddress, tempPeerPort);
        
        //둘이포트번호가 같으면 지워라
        if(PeerPort == tempPeerPort)
        {
            pServiceSocket->Close();
            //※※※여기에서 pos로 지우면 에러난다.※※※※※//
            closeCheck = true;
            break;
        }
    }

    if(closeCheck)
    {
        ASSERT((*pos) != NULL);
        delete (*pos);
        (*pos) = NULL;

        //소켓해제 및 컨테이너에서도 제거
        m_pVecServiceSocket.erase(pos);
        OutLog("접속인원 : %d 명", m_pVecServiceSocket.size());

        //그리고 리스트에서도 삭제 1순위 : 포트번호를 판단하여 제거
        //해당 포트번호가 있는 인덱스를 찾아 제거해야한다.
        vector<saveportnum>::iterator portPos;

        vector<saveportnum>::iterator portDeletePos;
        bool listCheck      = false;
        for(portPos = m_vecSavePortNum.begin(); portPos != m_vecSavePortNum.end(); ++portPos)
        {
            if(PeerPort == portPos->portNum)
            {
                portDeletePos = portPos;
                listCheck = true;
            }
        }
        
        if(listCheck)
        {
            //※※※위에서 찾은 이터레이터 위치를 토대로 앞으로 끌어오기※※※※※//
            for(portPos = m_vecSavePortNum.begin(); portPos != m_vecSavePortNum.end(); ++portPos)
            {
                if(portPos->index > portDeletePos->index)
                {
                    portPos->index -= 1;
                }
            }
            //위에서 지정했던 위치의 리스트컨트롤은 지워준다.
            m_listPublicClient.DeleteString(portDeletePos->index - 1);// 인덱스0 == 첫번째  

            //그 위치를 담당했던 컨테이너도 지워준다.
            m_vecSavePortNum.erase(portDeletePos);
            OutLog("리스트컨테이너 %d 개",m_vecSavePortNum.size());
        }
    }
}

void CServerDlg::ProcessSend(CListenSocket* pServiceSocket)
{
    OutLog("!!!!");
}

//////////////////////////////////////////////////////////////////////////
// 에러 메시지 박스 함수
//////////////////////////////////////////////////////////////////////////
void ErrorMsg(char *str)
{
	MessageBox(NULL, str, str, MB_OK);
}

//////////////////////////////////////////////////////////////////////////
// 디버그창 초기화 함수.  
//////////////////////////////////////////////////////////////////////////
bool InitLog()
{
	//사용에 앞서서 헤더에 추가할부분
	//extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow();

	AllocConsole();
	freopen( "CONOUT$", "wt", stdout );
	HWND ConsolehWnd = GetConsoleWindow();
	SetWindowPos( ConsolehWnd, HWND_TOPMOST ,
		850, 300, 400, 400, SWP_SHOWWINDOW );

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 디버그창 해제
//////////////////////////////////////////////////////////////////////////
void FreeLog()
{
	FreeConsole();
}

//////////////////////////////////////////////////////////////////////////
// 디버그창 출력
//////////////////////////////////////////////////////////////////////////
void OutLog(char *str, ...)
{
	char msg1[1024];
	static char msg2[1024];
	
	va_list va;
	va_start(va, str);
	vsprintf(msg1, str, va);
	va_end(va);
	
	if( stricmp( msg1, msg2) != 0)
		printf("%s\n", msg1);
	
	strcpy( msg2, msg1);
}

//전체 소켓해제메세지를 보낸다.
void CServerDlg::OnBnClickedCheck()
{
    // TODO: Add your control notification handler code here
    OutLog("send");
    serv_client serv_client;
    ZeroMemory(&serv_client, sizeof(serv_client));

    CString str = GetFileNameToIndex(m_listPublicClient.GetCurSel() + 1);

    if(str != "")
    {
        size_t charactersConverted = 0;

        wchar_t* szFileName;
        USES_CONVERSION;
        szFileName = A2W(str.GetBuffer());

        wcstombs_s(&charactersConverted, serv_client.filename,
            str.GetLength()+1, szFileName,_TRUNCATE);


        OutLog("토탈메시지index : %d",m_listPublicTotalMessage.GetCurSel() + 1);

        if(m_listPublicTotalMessage.GetCurSel() + 1 == 1)
        {
            serv_client.state = GOPROGRAM;
        }

        if(m_listPublicTotalMessage.GetCurSel() + 1 == 2)
        {
            serv_client.state = OUTPROGRAM;
        }

        if(m_listPublicTotalMessage.GetCurSel() + 1 == 3)
        {
            serv_client.state = CLIENTOUT;
        }
        
        vector<CServiceSocket*>::iterator pos;
        for(pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
        {
            (*pos)->Send(&serv_client,sizeof(serv_client));
        }

        //※※※이유는 삭제후 계속 설렉터 시에 남아있던 에디터의 잔 텍스트문구※※※//
        //삭제 후 사라진 index는 셀렉트가 사라졌다면 분명 0의 값을 가질것
        int index = m_listPublicClient.GetCurSel() + 1; //커서의 정보를 가져옴
  //      m_editPublicInfo.SetWindowText(""); //지워준다.
        m_listPublicProgramList.DeleteString(0);
    }
    else
    {
        MessageBox("GetFileNameToIndex목록중에  ( 임시... ) ");
    }
}

//m_listPublicClient(접속인원) 셀렉과정
void CServerDlg::OnLbnSelchangeListClientlist()
{
    // TODO: Add your control notification handler code here

    int index = m_listPublicClient.GetCurSel() + 1; //커서의 정보를 가져옴
    OutLog("select index(접속인원) =  %d", index);
    vector<saveportnum>::iterator pos;
    for(pos = m_vecSavePortNum.begin(); pos != m_vecSavePortNum.end(); ++pos)
    {
        //m_listPublicClient셀렉의 index와 저장된 portNum컨테이너의 정보가 일치된다면...
        if(index == pos->index)
        {
            CString strIp = GetIPAddressToPortnum(pos->portNum);

            //DeleteString을 썻다는것도 문제점(수정사항)

            m_listPublicProgramList.DeleteString(0);
            m_listPublicProgramList.AddString(pos->filename);
        }
    }
}

void CServerDlg::OnBnClickedButtonOut()
{
    // TODO: Add your control notification handler code here
    //지정된 셀의 소켓과 리스트를 꺼버리고 삭제시킨다.
    //커서의 정보를 인덱스로 가져옴
    int index = m_listPublicClient.GetCurSel() + 1; 
    OutLog("index -> %d", index);
    if(index > 0)
    {
        //인덱스의 정보(벡터컨테이너)로 포트번호를 가져옴
        UINT PeerPort;
        vector<saveportnum>::iterator pos_port;
        bool find_port = false;
        
        for(pos_port = m_vecSavePortNum.begin(); pos_port != m_vecSavePortNum.end(); ++pos_port)
        {
            if(index == pos_port->index)
            {
                PeerPort = pos_port->portNum;
                find_port = true;
                break;
            }
        }

        //포트번호를 통해 해당소켓을 찾아 지운다.
        //vector<CServiceSocket*> m_pVecServiceSocket;
        vector<CServiceSocket*>::iterator pos;
        int tempNum = 0; //-1은 종료메시지
        if(find_port)
        {
            for(pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
            {
                CString strPeerIp;
                UINT    iPeerPort;
                (*pos)->GetPeerName(strPeerIp, iPeerPort);

                serv_client serv_client;
                serv_client.state = CLIENTOUT;

                if(iPeerPort == PeerPort)
                {
                    (*pos)->Send(&serv_client, sizeof(serv_client));
                }
            }
        }
    }
    //※※※이유는 삭제후 계속 남아있는 에디터의 잔 텍스트문구※※※//
    //삭제 후 사라진 index는 셀렉트가 사라졌다면 분명 0의 값을 가질것
    index = m_listPublicClient.GetCurSel() + 1; //커서의 정보를 가져옴
//    m_editPublicInfo.SetWindowText(""); //지워준다.
    
    
    m_listPublicProgramList.DeleteString(0); //임시방편 ver0.1
}

void CServerDlg::OnBnClickedButtonGoprogram()
{
    // TODO: Add your control notification handler code here
    OutLog("select_program_go");

    int index = m_listPublicClient.GetCurSel() + 1; 
    OutLog("index -> %d", index);
    if(index >= 0)
    {
        //인덱스의 정보(벡터컨테이너)로 포트번호를 가져옴
        UINT PeerPort;
        vector<saveportnum>::iterator pos_port;
        bool find_port = false;
        
        for(pos_port = m_vecSavePortNum.begin(); pos_port != m_vecSavePortNum.end(); ++pos_port)
        {
            if(index == pos_port->index)
            {
                PeerPort = pos_port->portNum;
                find_port = true;
                break;
            }
        }

        //포트번호를 통해 해당소켓을 찾아 지운다.
        //vector<CServiceSocket*> m_pVecServiceSocket;
        vector<CServiceSocket*>::iterator pos;
        int tempNum = 0; //-1은 종료메시지
        if(find_port)
        {
            for(pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
            {
                CString strPeerIp;
                UINT    iPeerPort;
                (*pos)->GetPeerName(strPeerIp, iPeerPort);

                serv_client serv_client;
                ZeroMemory(&serv_client, sizeof(serv_client));

                serv_client.state = GOPROGRAM; // 1/2 클라이언트명령값(UINT)

                CString fileName = GetFileNameToPortnum(iPeerPort);

                if(fileName != "")
                {
                    //CString -> char[]
                    size_t charactersConverted = 0;

                    wchar_t* szFileName;
                    USES_CONVERSION;
                    szFileName = A2W(fileName.GetBuffer());

                    wcstombs_s(&charactersConverted, serv_client.filename,
                        fileName.GetLength()+1, szFileName,_TRUNCATE);

                    // 2/2 클라이언트명령값과 같이들어가는 파일명

                    if(iPeerPort == PeerPort)
                    {
                        (*pos)->Send(&serv_client, sizeof(serv_client));
                    }
                }
            }
        }
    }

    //※※※이유는 삭제후 계속 남아있는 에디터의 잔 텍스트문구※※※//
    //삭제 후 사라진 index는 셀렉트가 사라졌다면 분명 0의 값을 가질것
    //index = m_listPublicClient.GetCurSel() + 1; //커서의 정보를 가져옴
    //m_editPublicInfo.SetWindowText(""); //지워준다.
    //m_listPublicProgramList.DeleteString(0); //임시방편 ver0.1
}

void CServerDlg::OnBnClickedButtonOutprogram()
{
    // TODO: Add your control notification handler code here
    //GOPROGRAM, OUTPROGRAM부분 참고
    OutLog("select_Outprogram");

    int index = m_listPublicClient.GetCurSel() + 1; 
    OutLog("index -> %d", index);
    if(index > 0)
    {
        //인덱스의 정보(벡터컨테이너)로 포트번호를 가져옴
        UINT PeerPort;
        vector<saveportnum>::iterator pos_port;
        bool find_port = false;
        
        for(pos_port = m_vecSavePortNum.begin(); pos_port != m_vecSavePortNum.end(); ++pos_port)
        {
            if(index == pos_port->index)
            {
                PeerPort = pos_port->portNum;
                find_port = true;
                break;
            }
        }

        //포트번호를 통해 해당소켓을 찾아 지운다.
        //vector<CServiceSocket*> m_pVecServiceSocket;
        vector<CServiceSocket*>::iterator pos;
        int tempNum = 0; //-1은 종료메시지
        if(find_port)
        {
            for(pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
            {
                CString strPeerIp;
                UINT    iPeerPort;
                (*pos)->GetPeerName(strPeerIp, iPeerPort);

                serv_client serv_client;
                ZeroMemory(&serv_client, sizeof(serv_client));

                serv_client.state = OUTPROGRAM; // 1/2 클라이언트명령값(UINT)

                CString fileName = GetFileNameToPortnum(iPeerPort);
                //CString -> char[]
                size_t charactersConverted = 0;

                wchar_t* szFileName;
                USES_CONVERSION;
                szFileName = A2W(fileName.GetBuffer());

                wcstombs_s(&charactersConverted, serv_client.filename,
                    fileName.GetLength()+1, szFileName,_TRUNCATE);

                // 2/2 클라이언트명령값과 같이들어가는 파일명

                if(iPeerPort == PeerPort)
                {
                    (*pos)->Send(&serv_client, sizeof(serv_client));
                }
            }
        }
    }
    //※※※이유는 삭제후 계속 남아있는 에디터의 잔 텍스트문구※※※//
    //삭제 후 사라진 index는 셀렉트가 사라졌다면 분명 0의 값을 가질것
    //index = m_listPublicClient.GetCurSel() + 1; //커서의 정보를 가져옴
    //m_editPublicInfo.SetWindowText(""); //지워준다.
    //m_listPublicProgramList.DeleteString(0); //임시방편 ver0.1
}



//포트번호를 통해 아이피를 가져온다.
CString CServerDlg::GetIPAddressToPortnum(UINT iPortNum)
{
    //현재 접속자 모두중 일치하는 포트번호를 토대로 아이피를 찾아낸다.
    CString str = "no ip str";

    vector<CServiceSocket*>::iterator pos;
    CString PeerAddress;
    UINT PeerPort;
    for(pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
    {
        (*pos)->GetPeerName(PeerAddress, PeerPort);

        if(PeerPort == iPortNum)
        {
            return PeerAddress;
        }
    }
    return str;
}

//포트번호를 통해 해당포트의 파일명을 가져온다.
//다중셀렉팅이되면 반드시 바뀌어야할 부분
CString CServerDlg::GetFileNameToPortnum(UINT iPortNum)
{
    //현재 접속자 모두중 일치하는 클라이언트 포트번호를 토대로 파일명(CString) 찾아낸다.
   // vector<saveportnum>     m_vecSavePortNum;
    vector<saveportnum>::iterator pos;
    for(pos= m_vecSavePortNum.begin(); pos != m_vecSavePortNum.end(); ++pos)
    {
        if(pos->portNum == iPortNum)
        {
            //다중셀렉팅이되면 반드시 바뀌어야할 부분
            return pos->filename;
        }
    }
    MessageBox("m_vecSavePortNum 컨테이너에 값을 못찾음");
    CString strEmpty;

    return strEmpty;
}

//인덱스번호 통해 해당포트의 파일명을 가져온다.
CString CServerDlg::GetFileNameToIndex(UINT iIndex)
{
    //현재 접속자 모두중 일치하는 클라이언트 포트번호를 토대로 파일명(CString) 찾아낸다.
   // vector<saveportnum>     m_vecSavePortNum;
    vector<saveportnum>::iterator pos;
    for(pos= m_vecSavePortNum.begin(); pos != m_vecSavePortNum.end(); ++pos)
    {
        if(pos->index == iIndex)
        {
            //다중셀렉팅이되면 반드시 바뀌어야할 부분
            return pos->filename;
        }
    }
    MessageBox("m_vecSavePortNum 컨테이너에 값을 못찾음");
    CString strEmpty;

    return strEmpty;
}

void CServerDlg::OnLbnSelchangeListProgram()
{
    // TODO: Add your control notification handler code here
    int index = m_listPublicProgramList.GetCurSel() + 1; 
    OutLog("select index(FILE) -> %d", index);
}



DWORD CServerDlg::GetMyIP()
{
         char buf[ 256 ] = "";
         gethostname( buf, 256 );

         HOSTENT *p = gethostbyname( buf );
 
         in_addr *addr;
 
         char **addrList = p->h_addr_list;
 
         if( p->h_addrtype == AF_INET )
         {
                  while( 1 )
                  {
                           addr=(( in_addr * )*addrList );
                           if( !addr )
                                    break;
                           addrList++;
                             return inet_addr( inet_ntoa( *addr ) );
                  }
         }
         return 0;
}
