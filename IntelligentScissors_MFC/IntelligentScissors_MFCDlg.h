
// IntelligentScissors_MFCDlg.h : header file
//

#pragma once
#include <string>
#include <cv.h>

#include "IntelligentScissors.h"
#include "afxwin.h"
#include "afxcmn.h"

#define fileName_max_length 512

struct DrawPara
{
	ATL::CImage* pImg;
	CDC* pDC;
	int oriX;
	int oriY;
	int width;
	int height;
};

enum INTSCI_STATUS{
	INIT = 0,
	START = 1,
	WAITING = 2,
	ALREADY = 4,
	END = 16
};


struct ThreadInfo
{
	IntelligentScissors* pIntSci;
	int x;
	int y;
	enum INTSCI_STATUS* status;
	DPPercent dPPercent;
};

// CIntelligentScissors_MFCDlg dialog
class CIntelligentScissors_MFCDlg : public CDialogEx
{
// Construction
public:
	CIntelligentScissors_MFCDlg(CWnd* pParent = NULL);	// standard constructor

	~CIntelligentScissors_MFCDlg();

// Dialog Data
	enum { IDD = IDD_INTELLIGENTSCISSORS_MFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	CWinThread* pThread;
public:
	afx_msg void OnBnClickedButtonLoadImg();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonEnd();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonSave();

	//自定义成员函数
	void DrawCImage(DrawPara* p);//绘制图片

	cv::Mat loadImage(std::string path, ATL::CImage** pImg);//载入图片

	//自定义数据成员
	ATL::CImage* pImg;
	cv::Mat mat;
	cv::Mat result;
	IntelligentScissors* pIntSci;
	char _filename[fileName_max_length];

	INTSCI_STATUS status;
	ThreadInfo threadInfo;
	CStatic status_CStatic;

	static UINT ThreadFunc(LPVOID IpParam);//线程回调函数
	
	void setDP_percent(float pPercent);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CProgressCtrl DP_percent_CProgressCtrl;
	CStatic DP_percent_CStatic;
};
