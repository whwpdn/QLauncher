// ClientDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "ServiceSocket.h"
#include "shellapi.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CClientDlg ��ȭ ����




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


// CClientDlg �޽��� ó����

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
        m_pPublicServiceSocket = NULL;


//���ϰ� GetMyIP�� �ݴ�� ���� ��´�. ��¥�� ǥ���ϴµ� ���̴� �Լ��̱⿡ �� �ݴ���ۼ��Ѵ�
        DWORD myIP = GetMyIP();
        CString strIP;
        strIP.Format("%d.%d.%d.%d",
            FOURTH_IPADDRESS(myIP),THIRD_IPADDRESS(myIP),SECOND_IPADDRESS(myIP),FIRST_IPADDRESS(myIP));

        SetWindowText("Ŭ���̾�Ʈ IP : "+strIP);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CClientDlg::OnPaint()
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
            //MessageBox(L"������ ����");
            m_ctrPublicAdress.EnableWindow(false);
            m_buttonPublicConnect.EnableWindow(false);

            m_editPublicInfo.SetWindowText("���� ����...");
        }
        else
        {
            //MessageBox(L"������ ������ ���߽��ϴ�.");
            delete m_pPublicServiceSocket;
            m_pPublicServiceSocket = NULL;
            m_editPublicInfo.SetWindowText("���� ����...");
        }
    }
    else 
    {
        MessageBox("�̹�����");
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
    {//����Ʈ�� ���ϸ�/��� �ȿ÷��ִٸ�..
        //MessageBox(L"���Ϻҷ����� �䱸��");
        m_editPublicInfo.Clear();
        m_editPublicInfo.SetWindowText("Send��ư����� ���ϰ�κ���...");
    }
    else
    {
        if(m_pPublicServiceSocket == NULL)
        {
            //MessageBox(L"������ ����");
            m_editPublicInfo.SetWindowText("���Ӻ��� �ϼ���.");
        }
        else
        {
            if(m_strPublicFileName.strFilename != "")
            {
                //1�������� �̽������(�����غ�Ϸ�)

                m_sClient_serv.program_on = false;
             
                // CString->char[]
                // ��������� �ᵵ �ѱ��ڸ� ä������. strcpy����
                // �׷��� �˻��غ����....
                // �ع� http://skql.tistory.com/559
                size_t charactersConverted = 0;



                //CString to wchar_t *
                wchar_t* szFileName;
                USES_CONVERSION;
                szFileName = A2W(m_strPublicFileName.strFilename.GetBuffer());


                wcstombs_s(&charactersConverted, m_sClient_serv.filename,
                    m_strPublicFileName.strFilename.GetLength()+1, szFileName,_TRUNCATE);

                //�� ������ ������.
                m_pPublicServiceSocket->Send((char*)&m_sClient_serv, sizeof(client_serv));

                CString Info;
                Info.Format("on : %d, file : ", m_sClient_serv.program_on);
                m_editPublicInfo.SetWindowText(Info+m_strPublicFileName.strFilename+" ����");
            }
            else
            {
                MessageBox("���ϸ�/��ΰ� ����� ������ �ȵ�");
            }
        }
    }
    __super::OnSend(nErrorCode);
}
//������ : ������ �ٸ��� ������ �����ؼ� ������ ���Ѵ�..
//�̰� �����Ѵ�. �������� �����ߴ��Ͱ� ����������
//������ ��ӹ��� Ŭ������ �̿��ؼ� �� ���ο� ���ú긦 �Ҷ� 
//�� cpp�������� ProcessReceive�Լ��� �������ν� ���� �������� �����Ͽ� ó���ϴ� ���̴�.

//���ú�� �����κ��� �ڵ����� �޾Ƽ� �������ؾ��Ѵ�.
//������ ����� ���������� ��ư������ �����ϱ⿡ ���ú�� �ٸ��� �Ѵ�.
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
    Info.Format("receive : state�� : %d", serv_client.state);
    m_editPublicInfo.Clear();
    m_editPublicInfo.SetWindowText(Info);

    //state�� GOPROGRAM
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
                m_editPublicInfo.SetWindowText("���α׷� ����");

                SetTimer(100, 1500, 0);
            }
        }
    }

    //state�� CLIENTOUT
    if(serv_client.state == CLIENTOUT)
    {
        this->OnClose(0);
        exit(0);
    }

    //state�� OUTPROGRAM
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
        m_editPublicInfo.SetWindowText("���� ���ӻ��°� �ƴմϴ�.");
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
        m_editPublicInfo.SetWindowText("���� ����...");
    }
}

void CClientDlg::OnBnClickedButtonAdd()
{
    // TODO: Add your control notification handler code here
    CFileDialog dlg(true);
    
    if(dlg.DoModal() == IDOK)
    {
        //dlg.getpathname �� ������
        //dlg.GetFileName �� ���ϸ�
        m_strPublicFileName.strCurrentPathname = dlg.GetPathName();
        m_strPublicFileName.strFilename = dlg.GetFileName();

        m_listPublicFileInfo.AddString(m_strPublicFileName.strFilename);
       
        //����Ϻα����� ���ؼ� add��ư�� ��������..
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
            m_editPublicInfo.SetWindowText("���α׷� ������");
            m_bProgramOn = true;
            SendProgramOnState(m_bProgramOn);
        }
        else
        {
            KillTimer(100);
            m_editPublicInfo.SetWindowText("���α׷� X");
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

//true�� ���ڸ� �ְ� �ϸ� ���ڿ����� ���ִ»��·� ������ �������̴�.
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

    //�� ������ ������.
    m_pPublicServiceSocket->Send((char*)&m_sClient_serv, sizeof(client_serv));
}
