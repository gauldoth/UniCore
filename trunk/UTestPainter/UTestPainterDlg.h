
// UTestPainterDlg.h : ͷ�ļ�
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


// CUTestPainterDlg �Ի���
class CUTestPainterDlg : public CDialogEx, public uni::TestPainter
{
// ����
public:
	CUTestPainterDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_UTESTPAINTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
