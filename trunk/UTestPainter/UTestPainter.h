
// UTestPainter.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUTestPainterApp:
// �йش����ʵ�֣������ UTestPainter.cpp
//

class CUTestPainterApp : public CWinApp
{
public:
	CUTestPainterApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CUTestPainterApp theApp;