// LauncherDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "reportctrl.h"
#include "vector"
#include "PackDefine.h"
#include "map"
using namespace std;
class CReactionSock;
// CLauncherDlg ��ȭ ����
class CLauncherDlg : public CDialog, public CSocket
{
// �����Դϴ�.
public:
	CLauncherDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LAUNCHER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

//===========�� �� ��=========================================//
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

public: //����� ����
        void ProcessAccept();
        void ProcessReceive(CReactionSock* pSocket);
        void ProcessClose(CReactionSock* pSocket);
  


//===========�� �� ��=========================================//
private:
        CReactionSock*  m_pMySocket;
        CComboBox       m_cbMyType; // 0 : server , 1 : client
        CReportCtrl     m_ctrListServer;
        CReportCtrl     m_ctrFileListClient;
        CReportCtrl     m_ctrListViewClientState;

        vector<CReactionSock*>      m_pVecServiceSocket; //�ٷ��� ���ϰ���
        map<CReactionSock*, pack>   m_MmapManager; //���ϰ� �� ������ pack������ ��´�.
        SHELLEXECUTEINFO            m_si; //���α׷� �������
        pack                        m_onlyClientTempPack;
public:



    afx_msg void OnBnClickedLetsOffClient();
    afx_msg void OnBnClickedLetsExitClient();
};
