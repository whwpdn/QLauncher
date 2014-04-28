// ServerDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "ListenSocket.h"
#include "ServiceSocket.h"
#include "vector"
using namespace std;

//----------------------------------------//
//*서버
#define CLIENTOUT   100 //클라이언트 소켓제거하라
#define GOPROGRAM   101 //클라이언트 프로그램 실행하라(클라에게 받은 파일명)
#define OUTPROGRAM  102 //클라이언트 현 프로그램을 종료하라

//서버가 클라이언트에게...
typedef struct _serv_client
{
    int state;
    char filename[255];
}serv_client;
//----------------------------------------//
//클라이언트가 서버에게...
//enum CtoS_stateEnum{CLIENTOUT = -1, NOPROBLEM = 0, RUNPROGRAM = 1};
typedef struct _client_serv
{
    bool program_on; //나는 프로그램이 실행상태인가? false:아니요, true:예
    char filename[255]; //현재 프로그램 파일명만 (절대경로는 클라에서 보관 파일명만으로 검색해서 찾아읽는다.
}client_serv;
//----------------------------------------//

//이정보는 포트, 파일명, 실행상태, 관리하는 인덱스를 다룬다. 
typedef struct _savePortnumber
{
    UINT portNum;       //  이 해당 포트는?
    UINT index;         //  이 리스트의 인덱스번째에 있다.
    CString filename;   //  이 포트는 이 파일명으로 하고자 한다.
    bool processing;
}saveportnum;




// CServerDlg 대화 상자
class CServerDlg : public CDialog, public CSocket
{
// 생성입니다.
public:
	CServerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;
	// 생성된 메시지 맵 함수
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
    // 클라이언트 정보 에디트박스
 //   CEdit m_editPublicInfo;
    CListBox m_listPublicProgramList;
    CEdit       m_editPublicDebugInfo;

    bool        m_bNowTimer;
    CListenSocket*   m_pListenSocket;
    CServiceSocket*  m_pServiceSocket;

    vector<CServiceSocket*> m_pVecServiceSocket;
    vector<saveportnum>     m_vecSavePortNum;
    CListBox    m_listPublicClient;     //접속현황리스트
    CListBox m_listPublicTotalMessage;  //전체메세지리스트
};
