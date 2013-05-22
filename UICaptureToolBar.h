#pragma once

class CUICaptureToolBar : public CWindowImplBase
{
public:
	typedef enum
	{
		TBS_None = 0,
		TBS_Text,
		TBS_Rectangle,
		TBS_Ellipse
	};


	CUICaptureToolBar(void);
	virtual ~CUICaptureToolBar(void);

	virtual void OnFinalMessage(HWND hWnd);
	virtual void Init();
	void Notify(TNotifyUI& msg);
	
	UINT GetSel() { return m_nCurSel;}

protected:
	virtual tString GetSkinFolder() { return _T("Skin\\");}
	virtual tString GetSkinFile() { return _T("UICaptureToolBar.xml");}
	virtual LPCTSTR GetWindowClassName(void) const { return _T("UICaptureToolBar");}
	virtual UILIB_RESOURCETYPE GetResourceType() const { return UILIB_FILE;}
	virtual tString GetZIPFileName() const { return _T("Skin.zip");}
	virtual LPCTSTR GetResourceID() const { return MAKEINTRESOURCE(102);}

private:
	UINT m_nCurSel;
};

class CUIEditTest : public CWindowImplBase
{
public:
	CUIEditTest(void):m_sText(_T("")) {}
	virtual ~CUIEditTest(void){}

	virtual void OnFinalMessage(HWND hWnd)
	{
		__super::OnFinalMessage(hWnd);
		//delete this;
	}
	virtual void Init()
	{
		__super::Init();
	}
	void Notify(TNotifyUI& msg)
	{
		if (_tcsicmp(msg.sType,_T("windowinit")) == 0)
		{
			CEditUI* pEdit = static_cast<CEditUI*>(paint_manager_.FindControl(_T("edit_test")));
			ASSERT(pEdit);
			pEdit->SetFocus();
			pEdit->SetSelAll();
		}
		/*else if (_tcsicmp(msg.sType,_T("killfocus")) == 0)
		{
			if (_tcsicmp(msg.pSender->GetName(),_T("edit_test")) == 0)
			{
				m_sText = msg.pSender->GetText();
				Close();
			}
		}*/
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if( uMsg == WM_NCACTIVATE )
		{
			if (wParam == 0)
			{
				PostMessage(WM_CLOSE);
			}		
		}
		else if( uMsg == WM_KEYDOWN ) 
		{
			switch( wParam ) 
			{
			case VK_RETURN:
				PostMessage(WM_CLOSE);
				break;
			default:
				break;
			}
		}
		else if( uMsg == WM_KILLFOCUS ) 
		{
			if( m_hWnd != (HWND) wParam ) 
			{
				PostMessage(WM_CLOSE);
			}
		}

		return __super::HandleMessage(uMsg,wParam,lParam);
	}
	tString GetText() { return m_sText;}

protected:
	virtual tString GetSkinFolder() { return _T("Skin\\");}
	virtual tString GetSkinFile() { return _T("EditTestUI.xml");}
	virtual LPCTSTR GetWindowClassName(void) const { return _T("UIEditTest");}
	virtual UILIB_RESOURCETYPE GetResourceType() const { return UILIB_FILE;}
	virtual tString GetZIPFileName() const { return _T("Skin.zip");}
	virtual LPCTSTR GetResourceID() const { return MAKEINTRESOURCE(102);}

private:
	tString m_sText;
};
