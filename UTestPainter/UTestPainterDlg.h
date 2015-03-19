
// UTestPainterDlg.h : 头文件
//

#pragma once
#include <vector>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include "..\UniCore\UGeometry.h"


// CUTestPainterDlg 对话框
class CUTestPainterDlg : public CDialogEx, public uni::TestPainter
{
// 构造
public:
	CUTestPainterDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_UTESTPAINTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	virtual void newFrame();

	virtual void addBezier( uni::CubicBezierLine line );

	virtual void addLine( uni::StraightLine line );

	virtual void waitNextFrame();

private:
	std::vector<uni::CubicBezierLine> clines;
	std::vector<uni::StraightLine> slines;
	int m_flame;

};
