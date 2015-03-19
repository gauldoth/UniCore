
// UTestPainterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UTestPainter.h"
#include "UTestPainterDlg.h"
#include "afxdialogex.h"


//using namespace uni;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUTestPainterDlg 对话框




CUTestPainterDlg::CUTestPainterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUTestPainterDlg::IDD, pParent)
	,m_flame(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUTestPainterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUTestPainterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


unsigned __stdcall TestPainterFunc(void *p)
{
	CUTestPainterDlg *dlg = static_cast<CUTestPainterDlg *>(p);

	uni::CubicBezierLine a(29.869799,749.17999,82.259300,769.42804,126.58900,732.07898,96.247299,685.11597);
	uni::CubicBezierLine b(51.442501,774.73901,46.796101,729.59601,67.041199,693.41504,136.40599,706.02899);
	std::vector<uni::Point> result = uni::IntersectBezierAndBezierLine(a,b,dlg);
	int count = result.size();
	float x1 = a.getX(0.895);
	float y1 = a.getY(0.895);

	return 0;
}


// CUTestPainterDlg 消息处理程序

BOOL CUTestPainterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	_beginthreadex(0,0,TestPainterFunc,this,0,0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUTestPainterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUTestPainterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CDialogEx::OnPaint();

		CPaintDC dc(this);
		//dc.OffsetViewportOrg(250,250);
		static CBrush whiteBrush(RGB(0,0,0));
		static CPen pen(PS_SOLID,1,RGB(255,0,0));
		static CPen pen1(PS_SOLID,1,RGB(0,255,0));
		dc.SelectObject(&pen);
		//dc.SelectBrush(&whiteBrush);
		CRect rect;
		GetClientRect(&rect);
		dc.FillRect(rect,&whiteBrush);
		//
		for(int i = 0; i < clines.size(); i++)
		{
			dc.MoveTo(clines[i].getX(0.0)*2,(clines[i].getY(0.0)-550)*2);
			for(float t = 0.0; t <= 1.0; t+= 0.02)
			{
				dc.LineTo(clines[i].getX(t)*2,(clines[i].getY(t)-550)*2);
			}
		}
				dc.SelectObject(&pen1);
		for(int i = 0; i < slines.size(); i++)
		{
			dc.MoveTo(slines[i].points[0].x*2,(slines[i].points[0].y-550)*2);
			dc.LineTo(slines[i].points[1].x*2,(slines[i].points[1].y-550)*2);
		}
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUTestPainterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUTestPainterDlg::newFrame()
{
	clines.clear();
	slines.clear();
}

void CUTestPainterDlg::addBezier( uni::CubicBezierLine line )
{
	clines.push_back(line);
}

void CUTestPainterDlg::addLine( uni::StraightLine line )
{
	slines.push_back(line);
}

void CUTestPainterDlg::waitNextFrame()
{
	m_flame++;
	wchar_t buf[100];
	_itow_s(m_flame,buf,10);
	SetWindowText(buf);
	Invalidate(TRUE);
	Sleep(3000);
}

