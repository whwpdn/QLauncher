// ClientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "ServiceSocket.h"
#include "shellapi.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CClientDlg 대화 상자




CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientDlg::IDD, pParent), m_bProcess(false), m_bProgramOn(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
        m_strPublicFileName.strCurrentPathname = "";
        m_strPublicFileName.strFilename = "";
        memset(&m_sClient_serv,0,sizeof(m_sClient_serv));
        m_bPublicFileAdd = false;


//PROCESS_INFORMATION m_pi
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_IPADDRESS, m_ctrPublicAdress);
    DDX_Control(pDX, IDC_LIST, m_listPublicFileInfo);
    DDX_Control(pDX, ID_BUTTON_ADD, m_buttonPublicAdd);
    DDX_Control(pDX, ID_BUTTON_CONNECT, m_buttonPublicConnect);
    DDX_Control(pDX, IDC_EDIT_INFO, m_editPublicInfo);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
        ON_WM_DESTROY()
        ON_BN_CLICKED(ID_BUTTON_CONNECT, &CClientDlg::OnBnClickedButtonConnect)
        ON_BN_CLICKED(IDCTEST, &CClientDlg::OnBnClickedCtest)
        ON_BN_CLICKED(ID_BUTTON_ADD, &CClientDlg::OnBnClickedButtonAdd)
        ON_BN_CLICKED(ID_BUTTON_SEND, &CClientDlg::OnBnClickedButtonSend)
        ON_WM_TIMER()
        ON_BN_CLICKED(ID_BUTTON_CANCEL, &CClientDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CClientDlg 메시지 처리기

BOOL CClientDlg::OnInitDialog()
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
        m_pPublicServiceSocket = NULL;


//묘하게 GetMyIP는 반대로 값을 잡는다. 어짜피 표시하는데 쓰이는 함수이기에 또 반대로작성한다
        DWORD myIP = GetMyIP();
        CString strIP;
        strIP.Format("%d.%d.%d.%d",
            FOURTH_IPADDRESS(myIP),THIRD_IPADDRESS(myIP),SECOND_IPADDRESS(myIP),FIRST_IPADDRESS(myIP));

        SetWindowText("클라이언트 IP : "+strIP);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CClientDlg::OnPaint()
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
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CClientDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Add your message handler code here
    if(m_pPublicServiceSocket)
    {
        m_pPublicServiceSocket->Close();
        delete m_pPublicServiceSocket;
        m_pPublicServiceSocket = NULL;
    }
}

void CClientDlg::OnBnClickedButtonConnect()
{
    // TODO: Add your control notification handler code here
    OnConnect(0);
}
void CClientDlg::OnConnect(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class
    if(m_pPublicServiceSocket == NULL)
    {

        m_pPublicServiceSocket = new CServiceSocket(this);
        m_pPublicServiceSocket->Create();
        
        DWORD dwIP;
        m_ctrPublicAdress.GetAddress(dwIP);

        CString strIP;
        strIP.Format("%d.%d.%d.%d",
            FIRST_IPADDRESS(dwIP),SECOND_IPADDRESS(dwIP),THIRD_IPADDRESS(dwIP),FOURTH_IPADDRESS(dwIP));

        if(m_pPublicServiceSocket->Connect(strIP,9000))
        {
            //MessageBox(L"서버에 접속");
            m_ctrPublicAdress.EnableWindow(false);
            m_buttonPublicConnect.EnableWindow(false);

            m_editPublicInfo.SetWindowText("접속 성공...");
        }
        else
        {
            //MessageBox(L"서버에 접속을 못했습니다.");
            delete m_pPublicServiceSocket;
            m_pPublicServiceSocket = NULL;
            m_editPublicInfo.SetWindowText("접속 실패...");
        }
    }
    else 
    {
        MessageBox("이미접속");
    }
    __super::OnConnect(nErrorCode);
}

DWORD CClientDlg::GetMyIP()
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



void CClientDlg::OnSend(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class
    if(!m_bPublicFileAdd)
    {//리스트에 파일명/경로 안올려있다면..
        //MessageBox(L"파일불러오기 요구됨");
        m_editPublicInfo.Clear();
        m_editPublicInfo.SetWindowText("Send버튼사용은 파일경로부터...");
    }
    else
    {
        if(m_pPublicServiceSocket == NULL)
        {
            //MessageBox(L"미접속 상태");
            m_editPublicInfo.SetWindowText("접속부터 하세요.");
        }
        else
        {
            if(m_strPublicFileName.strFilename != "")
            {
                //1차적으로 미실행상태(실행준비완료)

                m_sClient_serv.program_on = false;
             
                // CString->char[]
                // 여러방법을 써도 한글자만 채워진다. strcpy관련
                // 그래서 검색해본결과....
                // 해법 http://skql.tistory.com/559
                size_t charactersConverted = 0;



                //CString to wchar_t *
                wchar_t* szFileName;
                USES_CONVERSION;
                szFileName = A2W(m_strPublicFileName.strFilename.GetBuffer());


                wcstombs_s(&charactersConverted, m_sClient_serv.filename,
                    m_strPublicFileName.strFilename.GetLength()+1, szFileName,_TRUNCATE);

                //현 정보를 보낸다.
                m_pPublicServiceSocket->Send((char*)&m_sClient_serv, sizeof(client_serv));

                CString Info;
                Info.Format("on : %d, file : ", m_sClient_serv.program_on);
                m_editPublicInfo.SetWindowText(Info+m_strPublicFileName.strFilename+" 전송");
            }
            else
            {
                MessageBox("파일명/경로가 제대로 설정이 안됨");
            }
        }
    }
    __super::OnSend(nErrorCode);
}
//문제점 : 서버와 다르게 받을때 감지해서 받지를 못한다..
//이건 사용안한다. 서버에서 구현했던것과 마찬가지로
//소켓을 상속받은 클래스를 이용해서 그 내부에 리시브를 할때 
//이 cpp문서에서 ProcessReceive함수를 만듬으로써 이쪽 구문으로 입장하여 처리하는 것이다.

//리시브는 서버로부터 자동으로 받아서 감지를해야한다.
//하지만 센드는 직접적으로 버튼을눌러 전송하기에 리시브와 다르게 한다.
void CClientDlg::OnReceive(int nErrorCode)
{
    //// TODO: Add your specialized code here and/or call the base class
    __super::OnReceive(nErrorCode);
}


void CClientDlg::ProcessReceive(CServiceSocket* pServiceSocket)
{
    serv_client serv_client;
    ZeroMemory(&serv_client, sizeof(serv_client));
    m_pPublicServiceSocket->Receive(&serv_client, sizeof(serv_client));

    CString Info;
    Info.Format("receive : state값 : %d", serv_client.state);
    m_editPublicInfo.Clear();
    m_editPublicInfo.SetWindowText(Info);

    //state가 GOPROGRAM
    if(serv_client.state == GOPROGRAM)
    {
        if(!m_bProgramOn)
        {
            ZeroMemory(&m_si, sizeof(SHELLEXECUTEINFO));
            m_si.cbSize = sizeof(m_si);
            m_si.lpVerb = "open";

            m_si.lpFile = (LPSTR)(LPCTSTR)m_strPublicFileName.strCurrentPathname;

            m_si.nShow = SW_SHOWDEFAULT;
            m_si.fMask = SEE_MASK_FLAG_DDEWAIT | SEE_MASK_NOCLOSEPROCESS;

            if(ShellExecuteEx(&m_si))
            {
                m_editPublicInfo.Clear();
                m_editPublicInfo.SetWindowText("프로그램 실행");

                SetTimer(100, 1500, 0);
            }
        }
    }

    //state가 CLIENTOUT
    if(serv_client.state == CLIENTOUT)
    {
        this->OnClose(0);
        exit(0);
    }

    //state가 OUTPROGRAM
    if(serv_client.state == OUTPROGRAM)
    {
        if(TerminateProcess(m_si.hProcess,0))
        {
            KillTimer(100);
            SendProgramOnState(false);
            m_bProgramOn = false;
            m_sClient_serv.program_on = !m_sClient_serv.program_on;
        }
    }
}

void CClientDlg::OnBnClickedCtest()
{
    // TODO: Add your control notification handler code here
    if(m_pPublicServiceSocket == NULL)
    {
        m_editPublicInfo.Clear();
        m_editPublicInfo.SetWindowText("현재 접속상태가 아닙니다.");
    }
    else
    {
        m_pPublicServiceSocket->Close();
        delete m_pPublicServiceSocket;
        m_pPublicServiceSocket = NULL;

        m_buttonPublicConnect.EnableWindow(true);
        m_buttonPublicAdd.EnableWindow(true);
        m_ctrPublicAdress.EnableWindow(true);
        m_bPublicFileAdd = false;
        m_listPublicFileInfo.DeleteString(0);
        m_editPublicInfo.Clear();
        m_editPublicInfo.SetWindowText("접속 해제...");
    }
}

void CClientDlg::OnBnClickedButtonAdd()
{
    // TODO: Add your control notification handler code here
    CFileDialog dlg(true);
    
    if(dlg.DoModal() == IDOK)
    {
        //dlg.getpathname 는 절대경로
        //dlg.GetFileName 는 파일명
        m_strPublicFileName.strCurrentPathname = dlg.GetPathName();
        m_strPublicFileName.strFilename = dlg.GetFileName();

        m_listPublicFileInfo.AddString(m_strPublicFileName.strFilename);
       
        //기능일부구현을 위해서 add버튼후 사라지기로..
        m_buttonPublicAdd.EnableWindow(false);
        m_bPublicFileAdd = true;
    }
}

void CClientDlg::OnBnClickedButtonSend()
{
    // TODO: Add your control notification handler code here
    OnSend(0);
}

void CClientDlg::OnClose(int nErrorCode)
{
    // TODO: Add your specialized code here and/or call the base class
    m_pPublicServiceSocket->Close();

    if(m_pPublicServiceSocket)
        delete m_pPublicServiceSocket;

    m_pPublicServiceSocket = NULL;
    __super::OnClose(nErrorCode);
}

void CClientDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    if(nIDEvent == 100)
    {
        DWORD ec;
        GetExitCodeProcess(m_si.hProcess, &ec);

        if(ec == STILL_ACTIVE)
        {
            m_editPublicInfo.SetWindowText("프로그램 실행중");
            m_bProgramOn = true;
            SendProgramOnState(m_bProgramOn);
        }
        else
        {
            KillTimer(100);
            m_editPublicInfo.SetWindowText("프로그램 X");
            m_bProgramOn = false;
            SendProgramOnState(m_bProgramOn);
        }
    }
    __super::OnTimer(nIDEvent);
}

void CClientDlg::OnBnClickedButtonCancel()
{
    // TODO: Add your control notification handler code here
    m_buttonPublicAdd.EnableWindow(true);

   // ZeroMemory(&m_strPublicFileName, sizeof(m_strPublicFileName));
    m_strPublicFileName.strCurrentPathname = "";
    m_strPublicFileName.strFilename = "";
    m_listPublicFileInfo.DeleteString(0);
    m_bPublicFileAdd = false;
}

//true로 인자를 넣고 하면 인자에따라 켜있는상태로 서버에 보낼것이다.
void CClientDlg::SendProgramOnState(bool onoff)
{
    m_sClient_serv.program_on = onoff;

    size_t charactersConverted = 0;

    //CString to wchar_t *
    wchar_t* szFileName;
    USES_CONVERSION;
    szFileName = A2W(m_strPublicFileName.strFilename.GetBuffer());


    wcstombs_s(&charactersConverted, m_sClient_serv.filename,
        m_strPublicFileName.strFilename.GetLength()+1, szFileName,_TRUNCATE);

    //현 정보를 보낸다.
    m_pPublicServiceSocket->Send((char*)&m_sClient_serv, sizeof(client_serv));
}
