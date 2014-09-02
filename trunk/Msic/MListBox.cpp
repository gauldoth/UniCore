#include "stdafx.h"
#include "MListBox.h"

#include <gdiplus.h>
#include "..\UniCore\ULog.h"

using namespace Gdiplus;
using namespace uni;

MListBox::MListBox()
	:m_currentMouseOverItem(-1)
{

}

BEGIN_MESSAGE_MAP(MListBox, CListBox)
	//{{AFX_MSG_MAP(MListBox)
	//}}AFX_MSG_MAP
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int MListBox::AddString( LPCTSTR lpszItem, LPCTSTR lpszIconPath)
{
	int nIndex = CListBox::AddString(lpszItem);
	if (nIndex >= 0)
	{
		Image *normalImage = new Image(lpszIconPath);
		m_normalImages.push_back(normalImage);
		SetItemData(nIndex, m_normalImages.size()-1);
	}
	return nIndex;
}

int MListBox::InsertString(int nIndex, LPCTSTR lpszItem, LPCTSTR lpszIconPath)
{
	int nInsertedIndex = CListBox::InsertString(nIndex,lpszItem);
	if (nInsertedIndex >= 0)
	{
		Image *normalImage = new Image(lpszIconPath);
		m_normalImages.push_back(normalImage);
		SetItemData(nIndex, m_normalImages.size()-1);
	}
	return nIndex;
}

void MListBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	bool bSelected = false;
	if(lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		bSelected = true;
	}
	else if(lpDrawItemStruct->itemState & ODS_DISABLED)
	{
	}
	else
	{
	}

	bool bMouseOver = (lpDrawItemStruct->itemID == m_currentMouseOverItem);

	DrawItem(lpDrawItemStruct->itemID,bSelected,bMouseOver);
}

void MListBox::DrawItem( int index, bool bSelected, bool bMouseOver )
{
	assert(index > -1);
	//CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rcClient;
	GetClientRect(rcClient);
	UTRACE<<delim<<"DrawItem"<<rcClient.Width()<<rcClient.Height();

	Bitmap bmp(rcClient.Width(), rcClient.Height());
	Graphics graphics(&bmp);

	//>>>绘制选中项的背景.
	CRect tempRect;
	int ret = GetItemRect(index,tempRect);
	if(ret == LB_ERR)
	{
		UERROR<<"GetItemRect failed.";
		return;
	}
	Rect rcItem(tempRect.left,tempRect.top,
		tempRect.Width(),
		tempRect.Height());
	Rect rcText = rcItem;

	rcText.X += 32;

	LinearGradientBrush mouseOverBrush(rcItem,
		Color(155,145,176,249), Color(155,110,152,229),LinearGradientModeHorizontal);
	LinearGradientBrush selectedBrush(rcItem,
		Color(255,145,176,249), Color(255,110,152,229),LinearGradientModeHorizontal);
	REAL relativeIntensities[] = {0.0f, 0.5f, 1.0f};
	REAL relativePositions[]   = {0.0f, 0.2f, 1.0f};
	selectedBrush.SetBlend(relativeIntensities,relativePositions,3);
	mouseOverBrush.SetBlend(relativeIntensities,relativePositions,3);
	SolidBrush bgBrush(Color(255,255,255,255));

	if(bSelected)
	{
		graphics.FillRectangle(&selectedBrush,rcItem);
	}
	else if(bMouseOver)
	{
		graphics.FillRectangle(&bgBrush,rcItem);
		graphics.FillRectangle(&mouseOverBrush,rcItem);
	}
	else
	{
		graphics.FillRectangle(&bgBrush,rcItem);
	}

	//>>>绘制文本
	Color colorText;
	FontFamily fontFamily(L"Times New Roman");
	Gdiplus::Font font(&fontFamily, 20, FontStyleRegular, UnitPixel);
	if(bSelected)
	{
		colorText.SetFromCOLORREF(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else if(bMouseOver)
	{
		colorText.SetFromCOLORREF(RGB(255,0,0));
	}
	else
	{
		colorText.SetFromCOLORREF(::GetSysColor(COLOR_WINDOWTEXT));
	}
	SolidBrush  solidBrush(colorText);

	StringFormat stringFormat;
	//stringFormat.SetAlignment(StringAlignmentCenter);
	stringFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	stringFormat.SetLineAlignment(StringAlignmentCenter);

	RectF rcTextToUse(rcText.X, rcText.Y, rcText.Width, rcText.Height);
	CString text;
	GetText(index, text);
	graphics.DrawString(text, -1, &font, rcTextToUse, &stringFormat, &solidBrush);

	int imageIndex = (int)GetItemData(index);
	if(imageIndex != -1)
	{
		RectF rcDest(rcItem.X,rcItem.Y,32,32);
		graphics.DrawImage(m_normalImages[imageIndex],rcDest);
		//CPoint pt(rcItem.X,rcItem.Y);
		//m_pImageList->Draw(CDC::FromHandle(lpDrawItemStruct->hDC), nImage, pt, ILD_NORMAL);
	}


	Graphics realGraphics(GetDC()->GetSafeHdc());
	realGraphics.DrawImage(&bmp,0,0);

}

void MListBox::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	ASSERT(lpMeasureItemStruct->CtlType == ODT_LISTBOX);
	lpMeasureItemStruct->itemHeight = 32;
}


BOOL MListBox::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= LBS_OWNERDRAWVARIABLE | LBS_HASSTRINGS | LBS_NOTIFY;
	return CListBox::PreCreateWindow(cs);
}


void MListBox::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//HitTest
	//假如当前的Item有绘制MouseOver框,则继续绘制.
	//假如当前的Item未绘制MouseOver框,则擦除之前有绘制的Item,并未当前Item绘制MouseOver框.

	BOOL bOutside = FALSE;
	UINT mouseOverIndex = CListBox::ItemFromPoint(point,bOutside);
	assert(mouseOverIndex != -1);

	if(bOutside)
	{
		mouseOverIndex = -1;
	}

	int curSel = GetCurSel();
	bool mouseOverSelected = curSel != -1 && curSel == mouseOverIndex;
	bool prevMouseOverSelected = curSel != -1 && curSel == m_currentMouseOverItem;
	
	if(mouseOverIndex != -1)
	{
		//鼠标在某项上,如果之前鼠标不在该项,则绘制该项.
		if(m_currentMouseOverItem != -1)
		{
			if(m_currentMouseOverItem == mouseOverIndex)
			{

			}
			else
			{
				DrawItem(mouseOverIndex,mouseOverSelected,true);
				DrawItem(m_currentMouseOverItem,prevMouseOverSelected,false);
			}
		}
		else
		{
			DrawItem(mouseOverIndex,mouseOverSelected,true);
		}
		m_currentMouseOverItem = mouseOverIndex;
	}
	else
	{
		//鼠标不在任何项上,如果之前鼠标在某项上,清除该项的MouseOver状态.
		if(m_currentMouseOverItem != -1)
		{
			DrawItem(m_currentMouseOverItem,prevMouseOverSelected,false);
			m_currentMouseOverItem = -1;
		}
	}

	CListBox::OnMouseMove(nFlags, point);
}

int MListBox::DeleteString( UINT nIndex )
{
	int customDataIndex = GetItemData(nIndex);
	assert(customDataIndex >= 0);
	int count = CListBox::DeleteString(nIndex);
	if(count != LB_ERR)
	{
		assert(customDataIndex < m_normalImages.size());
		delete m_normalImages[customDataIndex];
		m_normalImages.erase(m_normalImages.begin()+customDataIndex);
	}

	return customDataIndex;
}

int MListBox::Dir( UINT attr, LPCTSTR lpszWildCard )
{
	return LB_ERR;
}

void MListBox::ResetContent()
{
	CListBox::ResetContent();
	for(int i = 0; i < m_normalImages.size(); i++)
	{
		delete m_normalImages[i];
	}
	m_normalImages.clear();
}


BOOL MListBox::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//擦除没有项的区域.
	//return CListBox::OnEraseBkgnd(pDC);

	Graphics graphics(pDC->GetSafeHdc());

	int count = GetCount();
	CRect rcClient;
	GetClientRect(rcClient);
	if(count > 0)
	{
		CRect rcLastItem;
		GetItemRect(count-1,rcLastItem);
		rcClient.top = rcLastItem.bottom;
	}

	SolidBrush bgBrush(Color(255,255,255,255));
	graphics.FillRectangle(&bgBrush,rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height());

	return TRUE;
}


void MListBox::OnSize(UINT nType, int cx, int cy)
{

	CListBox::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}
