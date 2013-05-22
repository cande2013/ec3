#include "StdAfx.h"
#include "UICapture.h"
#include "ECRectTracker.h"
#include "UICaptureToolBar.h"

CUICapture::CUICapture()
	: _tracker(NULL)
	, m_hCursor(NULL)
	, m_pToolBar(NULL)
{

}

CUICapture::~CUICapture(void)
{
	if (_tracker)
	{
		delete _tracker;
		_tracker = NULL;
	}
	if (m_pToolBar)
	{
		delete m_pToolBar;
		m_pToolBar = NULL;
	}
}

void CUICapture::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

void CUICapture::Init()
{
	//获取屏幕分辩率
	INT xScreen = GetSystemMetrics(SM_CXSCREEN);
	INT yScreen = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(m_hWnd,NULL,0,0,xScreen,yScreen,SWP_SHOWWINDOW);
	m_hCursor = ::LoadCursorFromFile(_T("arrow_m.cur"));
	__super::Init();
}

LRESULT CUICapture::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (_tracker)
	{
		BOOL bHandled = FALSE;
		_tracker->TrackHandle(uMsg,wParam,lParam,bHandled);
		if(bHandled) return 0;
	}

	switch (uMsg) 
	{
	case WM_RBUTTONUP:
		{
			if (_tracker && !(_tracker->GetRect().IsNull()))
			{
				_tracker->Reset();
				if (m_pToolBar)
				{
					m_pToolBar->Close();
					m_pToolBar = NULL;
				}
			} 
			else
			{				
				Close();
			}
		}
		break;
	case WM_LBUTTONDOWN:		
		if (m_pToolBar && _tracker && !(_tracker->GetRect().IsNull()))
		{
 			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			if (_tracker->HitTest(pt) == 0 && m_pToolBar->GetSel() == CUICaptureToolBar::TBS_Text)
 			{
 				_tracker->SetStatus(_tracker->TS_TrackEnd);
				CUIEditTest* pTestDlg = new CUIEditTest;
				pTestDlg->Create(m_hWnd,NULL,WS_POPUP,WS_EX_TOOLWINDOW);
				::MoveWindow(*pTestDlg,pt.x,pt.y,100,50,FALSE);
				pTestDlg->ShowModal();
				delete pTestDlg;
 			}				
		}
		break;
	case WM_MOUSEMOVE:
		{
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			if (wParam == MK_LBUTTON)
			{
				if (m_pToolBar && _tracker && !(_tracker->GetRect().IsNull()))
				{
					m_pToolBar->ShowWindow(false);
				}				
			}			
		}		
		break;
	case WM_LBUTTONUP:
		if (_tracker && !(_tracker->GetRect().IsNull()))
		{
			if (m_pToolBar == NULL)
			{
				m_pToolBar = new CUICaptureToolBar;
				m_pToolBar->Create(m_hWnd,_T("CaptureToolBar"),UI_WNDSTYLE_DIALOG,WS_EX_PALETTEWINDOW);
			}
			m_pToolBar->ShowWindow();
			CDuiRect rc = _tracker->GetRect();
			::MoveWindow(*m_pToolBar,rc.right-400,rc.bottom+5,400,35,FALSE);
		}
		break;
	case WM_SETCURSOR:
		{
			if (m_hCursor) ::SetCursor(m_hCursor);
			return 0;
		}
		break;
	default:
		break;
	}
	return __super::HandleMessage(uMsg,wParam,lParam);
}

void CUICapture::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType,_T("windowinit")) == 0)
	{
	}
	else if (_tcsicmp(msg.sType,_T("click")) == 0)
	{

	}
	else if (_tcsicmp(msg.sType,_T("killfocus")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(),_T("edit_test")) == 0)
		{
			tString txt = msg.pSender->GetText();
			msg.pSender->SetVisible(false);
			msg.pSender->SetText(_T(""));
			HDC hDC = ::GetDC(m_hWnd);
			CDuiRect rc = msg.pSender->GetPos();
			::TextOut(hDC,rc.left,rc.top,txt.c_str(),txt.size());
			::ReleaseDC(m_hWnd,hDC);
		}
	}
}

void CUICapture::SetTracker(ECRectTracker* pTracker) 
{ 
	_tracker = pTracker;
	_tracker->Reset();
}
ECRectTracker* CUICapture::GetTracker() 
{ 
	return _tracker;
}

