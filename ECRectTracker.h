#pragma once

#include <vector>

#define MSG_PAINT_TRACKER WM_USER+2001
#define MSG_TRACKER_RECTCHANGED WM_USER+2002


class ECRectTracker
{
public:
	// 鼠标点击位置
	/**
	 1_______2______3
	 |              |
	 |              |
	 8       0      |4
	 |              |
	 |              |
	 7------6-------5

	**/
	typedef enum
	{
		HT_OutSide = -1,
		HT_InSide = 0,
		HT_LeftTop = 1,
		HT_Top = 2,
		HT_RightTop = 3,
		HT_Right = 4,
		HT_RightBottom = 5,
		HT_Bottom = 6,
		HT_LeftBottom = 7,
		HT_Left = 8
	}HitPosition;

	typedef enum
	{
		TS_Init = 0,  // 未画
		TS_Drawing,   // 正在画
		TS_Tracking,  // 拖动
		TS_TrackEnd   // 结束，进入绘图状态
	}TrackerStatus;

	ECRectTracker(HWND hWnd,LPCTSTR lpszImage,const CDuiRect& initRect = CDuiRect(0,0,0,0));
	~ECRectTracker(void);

	virtual void DrawRect(HDC hDC,const CDuiRect& rect);
	virtual CDuiRect GetRect() const;
	virtual CDuiRect GetMaxRegion() const;
	virtual void SetMaxRegion(const CDuiRect& rect);
	void SetStatus(INT nStatus);
	INT GetStatus();
	virtual INT HitTest(const POINT &pt) const;
	virtual LRESULT TrackHandle(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void Reset();
	void PaintWindow();
	const tString& GetSrcImage() const;

	static BOOL SaveBitmapToFile(Bitmap* image,const tString& sPath,const WCHAR* format);
	static Bitmap* NewImage(const tString& sFile)
	{
		INT nwLen = MultiByteToWideChar(CP_ACP,0,sFile.c_str(),sFile.size(),NULL,0); 
		WCHAR lpszFile[256] = {0};
		MultiByteToWideChar(CP_ACP,0,sFile.c_str(),sFile.size(),lpszFile,nwLen);
		return new Bitmap(lpszFile);		
	}


protected:
	//void RedrawHitPos(HDC hDC,const CDuiRect& rect,UINT nPenWidth);
	void UpdateHitPos(const CDuiRect& rect,UINT nWidth);
	void UpdateCursor(const POINT& pt);	

	virtual void OnLButtonDown(const POINT& pt);
	virtual void OnLButtonUp(const POINT& pt);
	virtual void OnMouseMove(const POINT& pt,DWORD dwKeyStatus);

	typedef std::vector<RectF> RecFVec;

	CDuiRect m_rect;
	RecFVec m_vecHitPosRect;

	HWND m_hOwnerWindow; 
	CDuiRect m_DrawingRect;
	UINT m_status;
	HCURSOR m_hCursor;
	INT m_nCurHitPos;
	POINT m_MoveStartPos;
	CDuiRect m_MaxRegion;
	tString m_sImage;

private:
	ECRectTracker(void);
};
