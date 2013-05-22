#include "stdafx.h"
#include "UICaptureToolBar.h"

CUICaptureToolBar::CUICaptureToolBar(void)
: m_nCurSel(TBS_None)
{
}

CUICaptureToolBar::~CUICaptureToolBar(void)
{
}

void CUICaptureToolBar::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

void CUICaptureToolBar::Init()
{
	__super::Init();
}

void CUICaptureToolBar::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType,_T("windowinit")) == 0)
	{

	}
	else if (_tcsicmp(msg.sType,_T("click")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(),_T("btn_rectangle")) == 0)
		{
			m_nCurSel = TBS_Rectangle;
		}
		if (_tcsicmp(msg.pSender->GetName(),_T("btn_ellipse")) == 0)
		{
			m_nCurSel = TBS_Ellipse;
		}
		if (_tcsicmp(msg.pSender->GetName(),_T("btn_text")) == 0)
		{
			m_nCurSel = TBS_Text;
		}
	}
}