#pragma once

class ECRectTracker;
class CUICaptureToolBar;
//template<class _TT>
class CUICapture : public CWindowImplBase
{
public:
	CUICapture();
	virtual ~CUICapture(void);

	virtual void OnFinalMessage(HWND hWnd);
	virtual void Init();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Notify(TNotifyUI& msg);

	void SetTracker(ECRectTracker* pTracker);
	ECRectTracker* GetTracker();

protected:
	virtual tString GetSkinFolder() { return _T("Skin\\");}
	virtual tString GetSkinFile() { return _T("UICapture.xml");}
	virtual LPCTSTR GetWindowClassName(void) const { return _T("UICapture");}
	virtual UILIB_RESOURCETYPE GetResourceType() const { return UILIB_FILE;}
	virtual tString GetZIPFileName() const { return _T("Skin.zip");}
	virtual LPCTSTR GetResourceID() const { return MAKEINTRESOURCE(102);}

private:
	//_TT _tracker;
	ECRectTracker* _tracker;
	HCURSOR m_hCursor;
	CUICaptureToolBar* m_pToolBar;
};
