// ClientDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


//----------------------------------------//
//*����
#define CLIENTOUT   100 //Ŭ���̾�Ʈ ���������϶�
#define GOPROGRAM   101 //Ŭ���̾�Ʈ ���α׷� �����϶�(Ŭ�󿡰� ���� ���ϸ�)
#define OUTPROGRAM  102 //Ŭ���̾�Ʈ �� ���α׷��� �����϶�
//������ Ŭ���̾�Ʈ����...
typedef struct _serv_client
{
    int state;
    char filename[255];
}serv_client;
//----------------------------------------//
//Ŭ���̾�Ʈ�� ��������...
//enum CtoS_stateEnum{CLIENTOUT = -1, NOPROBLEM = 0, RUNPROGRAM = 1};
typedef struct _client_serv
{
    bool program_on; //���� ���α׷��� ��������ΰ�? false:�ƴϿ�, true:��
    char filename[255]; //���� ���α׷� ���ϸ� (�����δ� Ŭ�󿡼� ���� ���ϸ����� �˻��ؼ� ã���д´�.
   // int state;
}client_serv;
//----------------------------------------//

typedef struct _filestring
{
    CString strCurrentPathname;
    CString strFilename;
}filestring;



//=============================
#define SOKET_OUT       3428
#define PROGRAM_GO      3429
#define PROGRAM_EXIT    3430

typedef struct _pack
{
    char File[10][256];     //���ϸ�
    char FilePath[10][256]; //���ϰ��
    double FileSize;        //���Ͽ뷮

    char ServerSendFile[256]; //������ �����϶�� ����� ���ϸ�
    
    bool ProgramOn;         //���α׷��� �������ΰ�.

    int ServerToClientOrder;

}pack;
//=============================


class CServiceSocket;
// CClientDlg ��ȭ ����
class CClientDlg : public CDialog, public CSocket
{
// �����Դϴ�.
public:
	CClientDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CServiceSocket* m_pPublicServiceSocket;
    
    CIPAddressCtrl m_ctrPublicAdress;
    
    client_serv m_sClient_serv;
    filestring m_strPublicFileName; //���ϸ�, ���ϰ�δ�����
    CListBox m_listPublicFileInfo;
    //bool      m_bPublicFileOn;
    bool      m_bPublicFileAdd;
public:
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedButtonConnect();
    virtual void OnConnect(int nErrorCode);
    virtual void OnSend(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
    afx_msg void OnBnClickedCtest();
    afx_msg void OnBnClickedButtonAdd();
    void ProcessReceive(CServiceSocket* pServiceSocket);
    void SendProgramOnState(bool onoff);
    DWORD GetMyIP();
public:
    CButton m_buttonPublicAdd;
    afx_msg void OnBnClickedButtonSend();
    virtual void OnClose(int nErrorCode);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    CButton m_buttonPublicConnect;
    afx_msg void OnBnClickedButtonCancel();
    CEdit m_editPublicInfo;

 //   STARTUPINFO m_si;
  //  PROCESS_INFORMATION m_pi;
    SHELLEXECUTEINFO m_si;

    bool m_bProcess; //CreateProcess�� ���� ���� m_pi�� ���� �ڵ鰪�� �޾� ������
    bool m_bProgramOn; //���������� ���α׷��� ���ִ����� ���� �� (1 on, 0 off)
};
