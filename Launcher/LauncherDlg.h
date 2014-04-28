// LauncherDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "reportctrl.h"
#include "vector"
#include "PackDefine.h"
#include "map"
using namespace std;
class CReactionSock;
// CLauncherDlg 대화 상자
class CLauncherDlg : public CDialog, public CSocket
{
// 생성입니다.
public:
	CLauncherDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LAUNCHER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

//===========함 수 부=========================================//
private: //MFC 
        virtual void OnConnect(int nErrorCode);
        virtual BOOL DestroyWindow();

        afx_msg void OnBnClickedConnect();
        afx_msg void OnBnClickedSelectMytype();
        afx_msg void OnBnClickedTypeCancel();
        afx_msg void OnNMDblclkListServerInfo(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnBnClickedDisconnect();
        afx_msg void OnBnClickedSendFilelist();
        afx_msg void OnBnClickedAddFileList();
        afx_msg void OnBnClickedDelList();
        virtual void OnClose(int nErrorCode);
        afx_msg void OnBnClickedLetsRunClient();
        afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
        void InitServer();
        void InitClient();

public: //사용자 정의
        void ProcessAccept();
        void ProcessReceive(CReactionSock* pSocket);
        void ProcessClose(CReactionSock* pSocket);
  


//===========변 수 부=========================================//
private:
        CReactionSock*  m_pMySocket;
        CComboBox       m_cbMyType; // 0 : server , 1 : client
        CReportCtrl     m_ctrListServer;
        CReportCtrl     m_ctrFileListClient;
        CReportCtrl     m_ctrListViewClientState;

        vector<CReactionSock*>      m_pVecServiceSocket; //다량의 소켓관리
        map<CReactionSock*, pack>   m_MmapManager; //소켓과 그 소켓의 pack정보를 담는다.
        SHELLEXECUTEINFO            m_si; //프로그램 실행관련
        pack                        m_onlyClientTempPack;
public:



    afx_msg void OnBnClickedLetsOffClient();
    afx_msg void OnBnClickedLetsExitClient();
};
