/*!
	\file MListBox.h
	\author unigauldoth@gmail.com
	\date 2014-8-9

	MListBox继承自的CListBox.
	支持显示图标.
*/
#pragma once

#include <afxwin.h>
#include <vector>

namespace Gdiplus
{
class Image;
}

class MListBox : public CListBox
{
public:
	MListBox();

	int AddString(LPCTSTR lpszItem, LPCTSTR lpszIconPath);
	int DeleteString(UINT nIndex);
	int InsertString(int nIndex, LPCTSTR lpszItem, LPCTSTR lpszIconPath);
	void ResetContent();
	int Dir(UINT attr, LPCTSTR lpszWildCard);

	//>>>Owner draw
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void DrawItem(int index,bool bSelected,bool bMouseOver);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
protected:
	DECLARE_MESSAGE_MAP()
private:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	std::vector<Gdiplus::Image *> m_normalImages;
	std::vector<Gdiplus::Image *> m_selectedImages;
	std::vector<Gdiplus::Image *> m_mouseOverImages;
	//std::vector<float> m_selectedAlpha;
	int m_currentMouseOverItem;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};