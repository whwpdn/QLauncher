// Launcher.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CLauncherApp:
// �� Ŭ������ ������ ���ؼ��� Launcher.cpp�� �����Ͻʽÿ�.
//

class CLauncherApp : public CWinApp
{
public:
	CLauncherApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CLauncherApp theApp;