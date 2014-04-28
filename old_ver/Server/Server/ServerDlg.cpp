// ServerDlg.cpp : ���� ����
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


// CServerDlg ��ȭ ����

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


// CServerDlg �޽��� ó����

BOOL CServerDlg::OnInitDialog()
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
        InitLog();

        if (!AfxSocketInit())
        {
            AfxMessageBox("���� ��������");
        }
        m_pListenSocket = new CListenSocket(this);

        if (!m_pListenSocket->Create(9000))
        {
            AfxMessageBox("���� ��������");
        }
        else
        {
            if (!m_pListenSocket->Listen())
            {
                AfxMessageBox("��������");
            }
        }

        //���ϰ� �ۿ��Լ� GetMyIP�� �ݴ�� ���� ��´�. ��¥�� ǥ���ϴµ� ���̴� �Լ��̱⿡ ���⿡ �� �ݴ���ۼ��Ѵ�
        DWORD myIP = GetMyIP();
        CString strIP;
        strIP.Format("%d.%d.%d.%d",
            FOURTH_IPADDRESS(myIP),THIRD_IPADDRESS(myIP),SECOND_IPADDRESS(myIP),FIRST_IPADDRESS(myIP));

        SetWindowText("���� IP : "+strIP);


        //m_listPublicTotalMessage �� ��ü�޼�����ư�� �����Ͽ� �۵�
        m_listPublicTotalMessage.AddString("���α׷� ����");
        m_listPublicTotalMessage.AddString("���α׷� ����");
        m_listPublicTotalMessage.AddString("��������");
        
        m_listPublicProgramList.EnableWindow(false);

        SetTimer(100, 1000, NULL);
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CServerDlg::OnPaint()
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
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServerDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
                OutLog("Ÿ�̸ӿ� index = %d", index);
                CString str;
                if(pos->processing)
                {
                    str.Format("index = %d, ���� ������",pos->index);
                    m_editPublicDebugInfo.SetWindowText(str);
                }
                else
                {   str.Format("index = %d, �̽�����", pos->index);
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

    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

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

        //�� ������ ��Ȳ
        vector<CServiceSocket*>::iterator pos;
        CString PeerAddress;
        UINT PeerPort;
        for(pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
        {
            (*pos)->GetPeerName(PeerAddress, PeerPort);
            OutLog("ip : %s, port : %d",(LPCTSTR)PeerAddress, PeerPort);
        }
        CString strPort;
        strPort.Format("��Ʈ : %d    ",PeerPort);
        
        //�����ο� ����Ʈ�ڽ��� �߰�
        m_listPublicClient.AddString(strPort + (LPCTSTR)PeerAddress);

        //���ٿ� ��Ʈ��ȣ�� �ε���(����Ʈ�ڽ��� ��ġ�ε���)�� �����ϴ� ����ü
        //�׸��� �װ� ��� ���������̳ʷν� ������ �ϸ� �ǰڴ�.
        saveportnum tempPortNum; //��Ʈ��ȣ�� �ش��ε����� �ִ´�.
        tempPortNum.portNum = PeerPort;
        tempPortNum.index = m_listPublicClient.GetListBoxInfo();
        tempPortNum.processing = false;
        tempPortNum.filename = "";
        m_vecSavePortNum.push_back(tempPortNum);
    }

    OutLog("�����ο� : %d ��", m_pVecServiceSocket.size());
}

void CServerDlg::ProcessReceive(CServiceSocket* pServiceSocket)
{
    client_serv client_serv;
    ZeroMemory(&client_serv, sizeof(client_serv));

    pServiceSocket->Receive(&client_serv, sizeof(client_serv));

    //�ް� �� �ش� Ŭ����Ʈ�� �´µ��� ���� ������ �־��ָ� �ǰڴ�.
    //vector<saveportnum>     m_vecSavePortNum;

    // Ŭ���̾�Ʈ���� ���� �����͸� accept�Ҷ� �����ߴ� ��Ʈ��ȣ�� ����
    // �ο������� ��ġ�ϴ� ������ ã��
    // �ܼ��� � ������ ����� ������ üũ�� �ؼ� saveportnum����ü�� ������ 
    // ������ filename�� ���ϸ��� �־�ξ� �������� � �������� �˰Բ� �Ѵ�.

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
            OutLog("�ε���!? %d",pos->index);

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

        //���� �� �� ���񽺼��� �ּҿ� ��Ʈ��ȣ�� ã�´�.
        pServiceSocket->GetPeerName(tempPeerAddress, tempPeerPort);
        
        //������Ʈ��ȣ�� ������ ������
        if(PeerPort == tempPeerPort)
        {
            pServiceSocket->Close();
            //�ءءؿ��⿡�� pos�� ����� ��������.�ءءءء�//
            closeCheck = true;
            break;
        }
    }

    if(closeCheck)
    {
        ASSERT((*pos) != NULL);
        delete (*pos);
        (*pos) = NULL;

        //�������� �� �����̳ʿ����� ����
        m_pVecServiceSocket.erase(pos);
        OutLog("�����ο� : %d ��", m_pVecServiceSocket.size());

        //�׸��� ����Ʈ������ ���� 1���� : ��Ʈ��ȣ�� �Ǵ��Ͽ� ����
        //�ش� ��Ʈ��ȣ�� �ִ� �ε����� ã�� �����ؾ��Ѵ�.
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
            //�ءء������� ã�� ���ͷ����� ��ġ�� ���� ������ �������ءءءء�//
            for(portPos = m_vecSavePortNum.begin(); portPos != m_vecSavePortNum.end(); ++portPos)
            {
                if(portPos->index > portDeletePos->index)
                {
                    portPos->index -= 1;
                }
            }
            //������ �����ߴ� ��ġ�� ����Ʈ��Ʈ���� �����ش�.
            m_listPublicClient.DeleteString(portDeletePos->index - 1);// �ε���0 == ù��°  

            //�� ��ġ�� ����ߴ� �����̳ʵ� �����ش�.
            m_vecSavePortNum.erase(portDeletePos);
            OutLog("����Ʈ�����̳� %d ��",m_vecSavePortNum.size());
        }
    }
}

void CServerDlg::ProcessSend(CListenSocket* pServiceSocket)
{
    OutLog("!!!!");
}

//////////////////////////////////////////////////////////////////////////
// ���� �޽��� �ڽ� �Լ�
//////////////////////////////////////////////////////////////////////////
void ErrorMsg(char *str)
{
	MessageBox(NULL, str, str, MB_OK);
}

//////////////////////////////////////////////////////////////////////////
// �����â �ʱ�ȭ �Լ�.  
//////////////////////////////////////////////////////////////////////////
bool InitLog()
{
	//��뿡 �ռ��� ����� �߰��Һκ�
	//extern "C" WINBASEAPI HWND WINAPI GetConsoleWindow();

	AllocConsole();
	freopen( "CONOUT$", "wt", stdout );
	HWND ConsolehWnd = GetConsoleWindow();
	SetWindowPos( ConsolehWnd, HWND_TOPMOST ,
		850, 300, 400, 400, SWP_SHOWWINDOW );

	return true;
}

//////////////////////////////////////////////////////////////////////////
// �����â ����
//////////////////////////////////////////////////////////////////////////
void FreeLog()
{
	FreeConsole();
}

//////////////////////////////////////////////////////////////////////////
// �����â ���
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

//��ü ���������޼����� ������.
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


        OutLog("��Ż�޽���index : %d",m_listPublicTotalMessage.GetCurSel() + 1);

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

        //�ءء������� ������ ��� ������ �ÿ� �����ִ� �������� �� �ؽ�Ʈ�����ءء�//
        //���� �� ����� index�� ����Ʈ�� ������ٸ� �и� 0�� ���� ������
        int index = m_listPublicClient.GetCurSel() + 1; //Ŀ���� ������ ������
  //      m_editPublicInfo.SetWindowText(""); //�����ش�.
        m_listPublicProgramList.DeleteString(0);
    }
    else
    {
        MessageBox("GetFileNameToIndex����߿�  ( �ӽ�... ) ");
    }
}

//m_listPublicClient(�����ο�) ��������
void CServerDlg::OnLbnSelchangeListClientlist()
{
    // TODO: Add your control notification handler code here

    int index = m_listPublicClient.GetCurSel() + 1; //Ŀ���� ������ ������
    OutLog("select index(�����ο�) =  %d", index);
    vector<saveportnum>::iterator pos;
    for(pos = m_vecSavePortNum.begin(); pos != m_vecSavePortNum.end(); ++pos)
    {
        //m_listPublicClient������ index�� ����� portNum�����̳��� ������ ��ġ�ȴٸ�...
        if(index == pos->index)
        {
            CString strIp = GetIPAddressToPortnum(pos->portNum);

            //DeleteString�� ���ٴ°͵� ������(��������)

            m_listPublicProgramList.DeleteString(0);
            m_listPublicProgramList.AddString(pos->filename);
        }
    }
}

void CServerDlg::OnBnClickedButtonOut()
{
    // TODO: Add your control notification handler code here
    //������ ���� ���ϰ� ����Ʈ�� �������� ������Ų��.
    //Ŀ���� ������ �ε����� ������
    int index = m_listPublicClient.GetCurSel() + 1; 
    OutLog("index -> %d", index);
    if(index > 0)
    {
        //�ε����� ����(���������̳�)�� ��Ʈ��ȣ�� ������
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

        //��Ʈ��ȣ�� ���� �ش������ ã�� �����.
        //vector<CServiceSocket*> m_pVecServiceSocket;
        vector<CServiceSocket*>::iterator pos;
        int tempNum = 0; //-1�� ����޽���
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
    //�ءء������� ������ ��� �����ִ� �������� �� �ؽ�Ʈ�����ءء�//
    //���� �� ����� index�� ����Ʈ�� ������ٸ� �и� 0�� ���� ������
    index = m_listPublicClient.GetCurSel() + 1; //Ŀ���� ������ ������
//    m_editPublicInfo.SetWindowText(""); //�����ش�.
    
    
    m_listPublicProgramList.DeleteString(0); //�ӽù��� ver0.1
}

void CServerDlg::OnBnClickedButtonGoprogram()
{
    // TODO: Add your control notification handler code here
    OutLog("select_program_go");

    int index = m_listPublicClient.GetCurSel() + 1; 
    OutLog("index -> %d", index);
    if(index >= 0)
    {
        //�ε����� ����(���������̳�)�� ��Ʈ��ȣ�� ������
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

        //��Ʈ��ȣ�� ���� �ش������ ã�� �����.
        //vector<CServiceSocket*> m_pVecServiceSocket;
        vector<CServiceSocket*>::iterator pos;
        int tempNum = 0; //-1�� ����޽���
        if(find_port)
        {
            for(pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
            {
                CString strPeerIp;
                UINT    iPeerPort;
                (*pos)->GetPeerName(strPeerIp, iPeerPort);

                serv_client serv_client;
                ZeroMemory(&serv_client, sizeof(serv_client));

                serv_client.state = GOPROGRAM; // 1/2 Ŭ���̾�Ʈ��ɰ�(UINT)

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

                    // 2/2 Ŭ���̾�Ʈ��ɰ��� ���̵��� ���ϸ�

                    if(iPeerPort == PeerPort)
                    {
                        (*pos)->Send(&serv_client, sizeof(serv_client));
                    }
                }
            }
        }
    }

    //�ءء������� ������ ��� �����ִ� �������� �� �ؽ�Ʈ�����ءء�//
    //���� �� ����� index�� ����Ʈ�� ������ٸ� �и� 0�� ���� ������
    //index = m_listPublicClient.GetCurSel() + 1; //Ŀ���� ������ ������
    //m_editPublicInfo.SetWindowText(""); //�����ش�.
    //m_listPublicProgramList.DeleteString(0); //�ӽù��� ver0.1
}

void CServerDlg::OnBnClickedButtonOutprogram()
{
    // TODO: Add your control notification handler code here
    //GOPROGRAM, OUTPROGRAM�κ� ����
    OutLog("select_Outprogram");

    int index = m_listPublicClient.GetCurSel() + 1; 
    OutLog("index -> %d", index);
    if(index > 0)
    {
        //�ε����� ����(���������̳�)�� ��Ʈ��ȣ�� ������
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

        //��Ʈ��ȣ�� ���� �ش������ ã�� �����.
        //vector<CServiceSocket*> m_pVecServiceSocket;
        vector<CServiceSocket*>::iterator pos;
        int tempNum = 0; //-1�� ����޽���
        if(find_port)
        {
            for(pos = m_pVecServiceSocket.begin(); pos != m_pVecServiceSocket.end(); ++pos)
            {
                CString strPeerIp;
                UINT    iPeerPort;
                (*pos)->GetPeerName(strPeerIp, iPeerPort);

                serv_client serv_client;
                ZeroMemory(&serv_client, sizeof(serv_client));

                serv_client.state = OUTPROGRAM; // 1/2 Ŭ���̾�Ʈ��ɰ�(UINT)

                CString fileName = GetFileNameToPortnum(iPeerPort);
                //CString -> char[]
                size_t charactersConverted = 0;

                wchar_t* szFileName;
                USES_CONVERSION;
                szFileName = A2W(fileName.GetBuffer());

                wcstombs_s(&charactersConverted, serv_client.filename,
                    fileName.GetLength()+1, szFileName,_TRUNCATE);

                // 2/2 Ŭ���̾�Ʈ��ɰ��� ���̵��� ���ϸ�

                if(iPeerPort == PeerPort)
                {
                    (*pos)->Send(&serv_client, sizeof(serv_client));
                }
            }
        }
    }
    //�ءء������� ������ ��� �����ִ� �������� �� �ؽ�Ʈ�����ءء�//
    //���� �� ����� index�� ����Ʈ�� ������ٸ� �и� 0�� ���� ������
    //index = m_listPublicClient.GetCurSel() + 1; //Ŀ���� ������ ������
    //m_editPublicInfo.SetWindowText(""); //�����ش�.
    //m_listPublicProgramList.DeleteString(0); //�ӽù��� ver0.1
}



//��Ʈ��ȣ�� ���� �����Ǹ� �����´�.
CString CServerDlg::GetIPAddressToPortnum(UINT iPortNum)
{
    //���� ������ ����� ��ġ�ϴ� ��Ʈ��ȣ�� ���� �����Ǹ� ã�Ƴ���.
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

//��Ʈ��ȣ�� ���� �ش���Ʈ�� ���ϸ��� �����´�.
//���߼������̵Ǹ� �ݵ�� �ٲ����� �κ�
CString CServerDlg::GetFileNameToPortnum(UINT iPortNum)
{
    //���� ������ ����� ��ġ�ϴ� Ŭ���̾�Ʈ ��Ʈ��ȣ�� ���� ���ϸ�(CString) ã�Ƴ���.
   // vector<saveportnum>     m_vecSavePortNum;
    vector<saveportnum>::iterator pos;
    for(pos= m_vecSavePortNum.begin(); pos != m_vecSavePortNum.end(); ++pos)
    {
        if(pos->portNum == iPortNum)
        {
            //���߼������̵Ǹ� �ݵ�� �ٲ����� �κ�
            return pos->filename;
        }
    }
    MessageBox("m_vecSavePortNum �����̳ʿ� ���� ��ã��");
    CString strEmpty;

    return strEmpty;
}

//�ε�����ȣ ���� �ش���Ʈ�� ���ϸ��� �����´�.
CString CServerDlg::GetFileNameToIndex(UINT iIndex)
{
    //���� ������ ����� ��ġ�ϴ� Ŭ���̾�Ʈ ��Ʈ��ȣ�� ���� ���ϸ�(CString) ã�Ƴ���.
   // vector<saveportnum>     m_vecSavePortNum;
    vector<saveportnum>::iterator pos;
    for(pos= m_vecSavePortNum.begin(); pos != m_vecSavePortNum.end(); ++pos)
    {
        if(pos->index == iIndex)
        {
            //���߼������̵Ǹ� �ݵ�� �ٲ����� �κ�
            return pos->filename;
        }
    }
    MessageBox("m_vecSavePortNum �����̳ʿ� ���� ��ã��");
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
