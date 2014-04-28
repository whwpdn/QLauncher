// ClientDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


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
    char File[10][256];     //파일명
    char FilePath[10][256]; //파일경로
    double FileSize;        //파일용량

    char ServerSendFile[256]; //서버가 실행하라고 명령한 파일명
    
    bool ProgramOn;         //프로그램이 실행중인가.

    int ServerToClientOrder;

}pack;
//=============================


class CServiceSocket;
// CClientDlg 대화 상자
class CClientDlg : public CDialog, public CSocket
{
// 생성입니다.
public:
	CClientDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CLIENT_DIALOG };

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
    CServiceSocket* m_pPublicServiceSocket;
    
    CIPAddressCtrl m_ctrPublicAdress;
    
    client_serv m_sClient_serv;
    filestring m_strPublicFileName; //파일명, 파일경로담을것
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

    bool m_bProcess; //CreateProcess를 통해 받은 m_pi의 최초 핸들값을 받아 정상적
    bool m_bProgramOn; //최종적으로 프로그램이 켜있는지에 대한 값 (1 on, 0 off)
};
