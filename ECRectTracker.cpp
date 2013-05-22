#include "stdafx.h"
#include "ECRectTracker.h"

ECRectTracker::ECRectTracker(HWND hWnd,LPCTSTR lpszImage,const CDuiRect& initRect)
: m_rect(0,0,0,0)
, m_hOwnerWindow(hWnd)
, m_DrawingRect(initRect)
, m_status(TS_Init)
, m_hCursor(NULL)
, m_nCurHitPos(-1)
, m_MaxRegion(0,0,0,0)
, m_sImage(lpszImage)
{
	m_MoveStartPos.x = 0;
	m_MoveStartPos.y = 0;
	m_vecHitPosRect.clear();
}

ECRectTracker::~ECRectTracker(void)
{
	//清除 
	m_vecHitPosRect.clear();
	//::DeleteFile(m_sImage.c_str());
}

CDuiRect ECRectTracker::GetRect() const
{
	return m_rect;
}

CDuiRect ECRectTracker::GetMaxRegion() const
{
	return m_MaxRegion;
}

void ECRectTracker::SetMaxRegion(const CDuiRect& rect)
{
	m_MaxRegion = rect;
}

const tString& ECRectTracker::GetSrcImage() const
{
	return m_sImage;
}

void ECRectTracker::SetStatus(INT nStatus)
{
	m_status = nStatus;
}

INT ECRectTracker::GetStatus()
{
	return m_status;
}

/************************************************************************/
/* 使用双缓存方式画图                                                                     */
/************************************************************************/
void ECRectTracker::DrawRect(HDC hDC,const CDuiRect& rect)
{
	GdiplusStartupInput si;
	ULONG_PTR pGdiToken;
	GdiplusStartup(&pGdiToken,&si,NULL);

	// 更新矩形大小
	m_rect = rect;
	// 校正矩形（此处务必执行）
	m_rect.Normalize();

	CDuiRect rw;
	::GetWindowRect(m_hOwnerWindow,&rw);
	int cx = rw.GetWidth();
	int cy = rw.GetHeight();

	// 创建一个不含遮罩效果的图
	Bitmap* pImage = NewImage(m_sImage);

	try
	{
		Graphics gs(pImage);

		//画矩形
		gs.DrawRectangle(&Pen(Color(255,0,209,245),1),RectF(m_rect.left,m_rect.top,m_rect.GetWidth(),m_rect.GetHeight()));

		//遮罩
		RecFVec rv;
		rv.push_back(RectF(0,0,m_rect.right,m_rect.top));
		rv.push_back(RectF(m_rect.right,0,(cx-m_rect.right),m_rect.bottom));
		rv.push_back(RectF(m_rect.left,m_rect.bottom,(cx - m_rect.left),(cy - m_rect.bottom)));
		rv.push_back(RectF(0,m_rect.top,m_rect.left,(cy - m_rect.top)));
		gs.FillRectangles(&SolidBrush(Color(100,0,0,0)),&rv[0],rv.size());

		//画拉伸点
		UpdateHitPos(m_rect,5);
		gs.FillRectangles(&SolidBrush(Color(255,0,209,245)),&m_vecHitPosRect[0],m_vecHitPosRect.size());

		// 显示到屏幕中
		Graphics graphics(hDC);
		CachedBitmap cachedBmp(pImage,&graphics);
		graphics.DrawCachedBitmap(&cachedBmp,0,0);
	}
	catch (...)
	{
		delete pImage;
		GdiplusShutdown(pGdiToken);
		return;
	}

	delete pImage;
	GdiplusShutdown(pGdiToken);
}

INT ECRectTracker::HitTest(const POINT &pt) const
{
	INT nRet = HT_OutSide;
	CDuiRect rc;
	rc.left = m_rect.left + m_MaxRegion.left + 1;
	rc.top = m_rect.top + m_MaxRegion.top + 1;
	rc.right = rc.left + m_rect.GetWidth() - 1;
	rc.bottom = rc.top + m_rect.GetHeight() - 1;

	if (::PtInRect(&rc,pt))
	{
		nRet = HT_InSide;
	}
	else 
	{
		for (INT n = 0; n != m_vecHitPosRect.size(); ++n)
		{
			//CDuiRect rc(m_vecHitPosRect[n].GetLeft()-1,m_vecHitPosRect[n].GetTop()-1,m_vecHitPosRect[n].GetRight()+1,m_vecHitPosRect[n].GetBottom()+1);
			rc.left = m_vecHitPosRect[n].GetLeft() + m_MaxRegion.left - 1;
			rc.top = m_vecHitPosRect[n].GetTop() + m_MaxRegion.top - 1;
			rc.right = rc.left + m_vecHitPosRect[n].Width + 2;
			rc.bottom = rc.top + m_vecHitPosRect[n].Height + 2;
			if (::PtInRect(&rc,pt))
			{
				nRet = n + 1;
				break;
			}
		}
	}
	
	return nRet;
}

void ECRectTracker::UpdateHitPos(const CDuiRect& rect,UINT nWidth)
{
	m_vecHitPosRect.clear();

	// 队列的顺序务必和位置的枚举值一一对应

	// 左上
	LONG x = rect.left - nWidth + 2;
	LONG y = rect.top - nWidth + 2;
	m_vecHitPosRect.push_back(RectF(x,y,nWidth,nWidth));

	// 上
	x = rect.left + (rect.GetWidth() / 2);
	m_vecHitPosRect.push_back(RectF(x,y,nWidth,nWidth));

	// 右上
	x = rect.right - 2;
	m_vecHitPosRect.push_back(RectF(x,y,nWidth,nWidth));

	// 右
	y = rect.top + (rect.GetHeight() / 2);
	m_vecHitPosRect.push_back(RectF(x,y,nWidth,nWidth));

	// 右下
	y = rect.bottom - 1;
	m_vecHitPosRect.push_back(RectF(x,y,nWidth,nWidth));

	// 下
	x = rect.left + (rect.GetWidth() / 2);
	m_vecHitPosRect.push_back(RectF(x,y,nWidth,nWidth));

	// 左下
	x = rect.left - nWidth + 2;
	m_vecHitPosRect.push_back(RectF(x,y,nWidth,nWidth));

	// 左
	y = rect.top + (rect.GetHeight() / 2);
	m_vecHitPosRect.push_back(RectF(x,y,nWidth,nWidth));
}

LRESULT ECRectTracker::TrackHandle(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (uMsg) 
	{
	case WM_SETCURSOR:
		if (m_hCursor)
		{
			::SetCursor(m_hCursor);
			bHandled = TRUE;			
		}
		break;
	case WM_LBUTTONDOWN:
		{
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			OnLButtonDown(pt);
		}
		break;
	case WM_LBUTTONUP:
		{
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			OnLButtonUp(pt);
		}
		break;
	case WM_MOUSEMOVE:
		{
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			OnMouseMove(pt,wParam);
		}
		break;
	case WM_PAINT:
		{
			if (m_status == TS_Drawing || m_status == TS_Tracking)
			{
				::PostMessage(m_hOwnerWindow,MSG_PAINT_TRACKER,0,0);
			}			
		}		
		break;
	case MSG_PAINT_TRACKER:
		{
			HDC hDC = ::GetDC(m_hOwnerWindow);
			DrawRect(hDC,m_DrawingRect);
			::ReleaseDC(m_hOwnerWindow,hDC);
			bHandled = TRUE;
		}
		break;
	default:
		bHandled = FALSE;
		break;
	}
	
	return 0;
}

void ECRectTracker::UpdateCursor(const POINT& pt)
{
	switch(HitTest(pt))
	{
	case 0: 
		m_hCursor = ::LoadCursor(NULL,IDC_SIZEALL);
		break;
	case 1: 
	case 5:
		m_hCursor = ::LoadCursor(NULL,IDC_SIZENWSE);
		break;
	case 2: 
	case 6:
		m_hCursor = ::LoadCursor(NULL,IDC_SIZENS);
		break;
	case 3:
	case 7: 
		m_hCursor = ::LoadCursor(NULL,IDC_SIZENESW);
		break;
	case 4:
	case 8: 
		m_hCursor = ::LoadCursor(NULL,IDC_SIZEWE);
		break;
	default: 
		m_hCursor = NULL;
		break;
	}
}

void ECRectTracker::PaintWindow()
{
	::PostMessage(m_hOwnerWindow,MSG_PAINT_TRACKER,0,0);
	//CDuiRect rw;
	//::GetWindowRect(m_hOwnerWindow,&rw);
	//::InvalidateRect(m_hOwnerWindow,&rw,FALSE);
}

void ECRectTracker::Reset()
{
	m_status = TS_Init;
	m_rect.Empty();
	m_DrawingRect.Empty();
	PaintWindow();
	/*
	CDuiRect rw;
	::GetWindowRect(m_hOwnerWindow,&rw);
	int cx = rw.GetWidth(); //GetSystemMetrics(SM_CXSCREEN);
	int cy = rw.GetHeight(); //GetSystemMetrics(SM_CYSCREEN);
	HDC hDC = ::GetDC(m_hOwnerWindow);

	GdiplusStartupInput si;
	ULONG_PTR pGdiToken;
	GdiplusStartup(&pGdiToken,&si,NULL);

	try
	{
		INT nwLen = MultiByteToWideChar(CP_ACP,0,m_sImage.c_str(),m_sImage.size(),NULL,0); 
		WCHAR lpszFile[256] = {0};
		MultiByteToWideChar(CP_ACP,0,m_sImage.c_str(),m_sImage.size(),lpszFile,nwLen);
		Bitmap image(lpszFile);
		Graphics gs(&image);
		SolidBrush sb(Color(100,0,0,0));
		gs.FillRectangle(&sb,RectF(0,0,cx,cy));

		Graphics graphics(hDC);
		CachedBitmap cachedBmp(&image,&graphics);
		graphics.DrawCachedBitmap(&cachedBmp,0,0);
	}
	catch (...)
	{
		GdiplusShutdown(pGdiToken);
		::ReleaseDC(m_hOwnerWindow,hDC);
		return;
	}

	GdiplusShutdown(pGdiToken);
	::ReleaseDC(m_hOwnerWindow,hDC);*/
}

void ECRectTracker::OnLButtonDown(const POINT& pt)
{
	if (m_MaxRegion.IsNull()) ::GetWindowRect(m_hOwnerWindow,&m_MaxRegion);

	//::ClientToScreen(m_hOwnerWindow,&pt);
	if (m_rect.IsNull())
	{
		m_DrawingRect.left = pt.x;
		m_DrawingRect.top = pt.y;
		m_DrawingRect.right = m_DrawingRect.left;
		m_DrawingRect.bottom = m_DrawingRect.top;
		m_status = TS_Drawing;
	}
	else if (m_status == TS_Tracking)
	{
		m_nCurHitPos = HitTest(pt);
		m_MoveStartPos.x = pt.x;
		m_MoveStartPos.y = pt.y;
	}
}

void ECRectTracker::OnLButtonUp(const POINT& pt)
{
	if (m_rect.IsNull()) m_status = TS_Init;
	else m_status = TS_Tracking;

	m_DrawingRect.Normalize(); // 画完后校正
}

void ECRectTracker::OnMouseMove(const POINT& pt,DWORD dwKeyStatus)
{
	//::ClientToScreen(m_hOwnerWindow,&pt);
	UpdateCursor(pt);
	if (dwKeyStatus == MK_LBUTTON)
	{		
		if (m_status == TS_Drawing)
		{
			m_DrawingRect.right = pt.x;
			m_DrawingRect.bottom = pt.y;
		} 
		else if(m_status == TS_Tracking)
		{
			switch(m_nCurHitPos)
			{
			case HT_InSide:
				{
					// 移动
					CDuiRect rc = m_DrawingRect;
					rc.left += pt.x - m_MoveStartPos.x;
					rc.right += pt.x - m_MoveStartPos.x;
					rc.top += pt.y - m_MoveStartPos.y;
					rc.bottom += pt.y - m_MoveStartPos.y;

					if (rc.left >= m_MaxRegion.left && rc.right <= m_MaxRegion.right)
					{
						m_DrawingRect.left += pt.x - m_MoveStartPos.x;
						m_DrawingRect.right += pt.x - m_MoveStartPos.x;								
					}
					else if (rc.left < m_MaxRegion.left)
					{
						LONG nTmp = rc.GetWidth();
						m_DrawingRect.left = m_MaxRegion.left;
						m_DrawingRect.right = m_DrawingRect.left + nTmp;
					}
					else if (rc.right > m_MaxRegion.right)
					{
						LONG nTmp = rc.GetWidth();
						m_DrawingRect.right = m_MaxRegion.right;
						m_DrawingRect.left = m_DrawingRect.right - nTmp;
					}

					if (rc.top >= m_MaxRegion.top && rc.bottom <= m_MaxRegion.bottom)
					{
						m_DrawingRect.top += pt.y - m_MoveStartPos.y;
						m_DrawingRect.bottom += pt.y - m_MoveStartPos.y;
					}
					else if (rc.top < m_MaxRegion.top)
					{
						LONG nTmp = rc.GetHeight();
						m_DrawingRect.top = m_MaxRegion.top;
						m_DrawingRect.bottom = m_DrawingRect.top + nTmp;
					}
					else if (rc.bottom > m_MaxRegion.bottom)
					{
						LONG nTmp = rc.GetHeight();
						m_DrawingRect.bottom = m_MaxRegion.bottom;
						m_DrawingRect.top = m_DrawingRect.bottom - nTmp;
					}

					m_MoveStartPos.x = pt.x;
					m_MoveStartPos.y = pt.y;
				}
				break;
			case HT_LeftTop:
				m_DrawingRect.left = pt.x;
				m_DrawingRect.top = pt.y;
				break;
			case HT_RightBottom:
				m_DrawingRect.right = pt.x;
				m_DrawingRect.bottom = pt.y;
				break;
			case HT_Top:
				m_DrawingRect.top = pt.y;
				break;
			case HT_Bottom:
				m_DrawingRect.bottom = pt.y;
				break;
			case HT_RightTop:
				m_DrawingRect.right = pt.x;
				m_DrawingRect.top = pt.y;
				break;
			case HT_LeftBottom:
				m_DrawingRect.left = pt.x;
				m_DrawingRect.bottom = pt.y;
				break;
			case HT_Right:
				m_DrawingRect.right = pt.x;
				break;
			case HT_Left:
				m_DrawingRect.left = pt.x;
				break;
			default: break;
			}

			if(m_DrawingRect.top < m_MaxRegion.top) m_DrawingRect.top = m_MaxRegion.top;
			if(m_DrawingRect.left < m_MaxRegion.left) m_DrawingRect.left = m_MaxRegion.left;
			if(m_DrawingRect.right > m_MaxRegion.right)	m_DrawingRect.right = m_MaxRegion.right;
			if(m_DrawingRect.bottom > m_MaxRegion.bottom) m_DrawingRect.bottom = m_MaxRegion.bottom;
		}	

		PaintWindow();
	}
}

BOOL ECRectTracker::SaveBitmapToFile(Bitmap* image,const tString& sPath,const WCHAR* format)
{
	// 获取对应格式的CLSID
	CLSID clsid;
	UINT  codenum = 0;   
	UINT  size = 0;   
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&codenum, &size);
	if(size == 0) return FALSE;  
	pImageCodecInfo = new ImageCodecInfo[size];
	if(pImageCodecInfo == NULL) return FALSE; 
	GetImageEncoders(codenum, size, pImageCodecInfo);
	UINT nIdex = 0;
	for(; nIdex != codenum; ++nIdex)
	{
		if( wcscmp(pImageCodecInfo[nIdex].MimeType, format) == 0 )
		{
			clsid = pImageCodecInfo[nIdex].Clsid;
			break;  
		}    
	} 
	delete []pImageCodecInfo;
	if (nIdex == codenum) return FALSE;

	INT nwLen = MultiByteToWideChar(CP_ACP,0,sPath.c_str(),sPath.size(),NULL,0); 
	WCHAR lpszFile[256] = {0};
	MultiByteToWideChar(CP_ACP,0,sPath.c_str(),sPath.size(),lpszFile,nwLen);
	return (image->Save(lpszFile,&clsid) == Ok);
}