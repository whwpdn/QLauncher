// ServerDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "ListenSocket.h"
#include "ServiceSocket.h"
#include "vector"
using namespace std;

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
}client_serv;
//----------------------------------------//

//�������� ��Ʈ, ���ϸ�, �������, �����ϴ� �ε����� �ٷ��. 
typedef struct _savePortnumber
{
    UINT portNum;       //  �� �ش� ��Ʈ��?
    UINT index;         //  �� ����Ʈ�� �ε�����°�� �ִ�.
    CString filename;   //  �� ��Ʈ�� �� ���ϸ����� �ϰ��� �Ѵ�.
    bool processing;
}saveportnum;




// CServerDlg ��ȭ ����
class CServerDlg : public CDialog, public CSocket
{
// �����Դϴ�.
public:
	CServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SERVER_DIALOG };

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
    
    afx_msg void OnLbnSelchangeListClientlist();
    afx_msg void OnBnClickedButtonOut();
    afx_msg void OnBnClickedButtonGoprogram();
    afx_msg void OnLbnSelchangeListProgram();
    afx_msg void OnBnClickedButtonOutprogram();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedCheck();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    afx_msg void ProcessAccept();
    afx_msg void ProcessReceive(CServiceSocket* pServiceSocket);
    afx_msg void ProcessClose(CServiceSocket* pServiceSocket);
    afx_msg void ProcessSend(CListenSocket* pServiceSocket);

    CString GetIPAddressToPortnum(UINT iPortNum);
    CString GetFileNameToPortnum(UINT iPortNum);
    CString GetFileNameToIndex(UINT iIndex);
    DWORD GetMyIP();

public:
    // Ŭ���̾�Ʈ ���� ����Ʈ�ڽ�
 //   CEdit m_editPublicInfo;
    CListBox m_listPublicProgramList;
    CEdit       m_editPublicDebugInfo;

    bool        m_bNowTimer;
    CListenSocket*   m_pListenSocket;
    CServiceSocket*  m_pServiceSocket;

    vector<CServiceSocket*> m_pVecServiceSocket;
    vector<saveportnum>     m_vecSavePortNum;
    CListBox    m_listPublicClient;     //������Ȳ����Ʈ
    CListBox m_listPublicTotalMessage;  //��ü�޼�������Ʈ
};
