
// IntelligentScissors_MFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IntelligentScissors_MFC.h"
#include "IntelligentScissors_MFCDlg.h"
#include "afxdialogex.h"

#include "ImageUtility.h"

#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define oriX_value 50
#define oriY_value 150

#define WINDOW_WIDTH_MIN 500
#define WINDOW_HEIGHT_MIN 400

// CIntelligentScissors_MFCDlg dialog



CIntelligentScissors_MFCDlg::CIntelligentScissors_MFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIntelligentScissors_MFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CIntelligentScissors_MFCDlg::~CIntelligentScissors_MFCDlg(){
	if (pImg){
		delete pImg;
		pImg = NULL;
	}
	if (pIntSci){
		delete pIntSci;
		pIntSci = NULL;
	}
	
}

void CIntelligentScissors_MFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_STATUS, status_CStatic);
	DDX_Control(pDX, IDC_PROGRESS_DP, DP_percent_CProgressCtrl);
	DDX_Control(pDX, IDC_STATIC_DP, DP_percent_CStatic);
}

BEGIN_MESSAGE_MAP(CIntelligentScissors_MFCDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD_IMG, &CIntelligentScissors_MFCDlg::OnBnClickedButtonLoadImg)
	ON_BN_CLICKED(IDC_BUTTON_START, &CIntelligentScissors_MFCDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_END, &CIntelligentScissors_MFCDlg::OnBnClickedButtonEnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CIntelligentScissors_MFCDlg::OnBnClickedButtonSave)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CIntelligentScissors_MFCDlg message handlers

BOOL CIntelligentScissors_MFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	pImg = NULL;
	pIntSci = NULL;

	status = INIT;

	pThread = NULL;

	status_CStatic.SetWindowTextW(LPCTSTR(CString("请载入图片并点击\"开始\"按钮")));

	DP_percent_CProgressCtrl.SetRange(0, 100);
	DP_percent_CProgressCtrl.ShowWindow(false);//隐藏该控件

	UpdateData(false);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIntelligentScissors_MFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		if (pImg!=NULL){
			SetWindowPos(NULL, 0, 0, max(pImg->GetWidth() + oriX_value * 2,WINDOW_WIDTH_MIN),
			max(pImg->GetHeight() + oriY_value * 3 / 2,WINDOW_HEIGHT_MIN), SWP_NOMOVE);

			DrawPara p;
			p.height = pImg->GetHeight();
			p.width = pImg->GetWidth();
			p.oriX = oriX_value;
			p.oriY = oriY_value;
			p.pDC = GetDC();
			p.pImg = pImg;

			DrawCImage(&p);
		}
	}
}

void CIntelligentScissors_MFCDlg::DrawCImage(DrawPara * p){
	CRect rect;
	GetClientRect(&rect);
	CDC     memDC;             // 用于缓冲绘图的内存画笔  
	CBitmap memBitmap;         // 用于缓冲绘图的内存画布
	memDC.CreateCompatibleDC(p->pDC);  // 创建与原画笔兼容的画笔
	memBitmap.CreateCompatibleBitmap(p->pDC, p->width, p->height);  // 创建与原位图兼容的内存画布
	memDC.SelectObject(&memBitmap);      // 创建画笔与画布的关联
	memDC.FillSolidRect(rect, p->pDC->GetBkColor());

	// 将pImg的内容缩放画到内存画布中
	p->pImg->StretchBlt(memDC.m_hDC, 0, 0, p->width, p->height);

	// 将已画好的画布复制到真正的缓冲区中
	p->pDC->BitBlt(p->oriX, p->oriY, p->width, p->height, &memDC, 0, 0, SRCCOPY);
	memBitmap.DeleteObject();
	memDC.DeleteDC();
}

//载入图片
cv::Mat CIntelligentScissors_MFCDlg::loadImage(string fileName, ATL::CImage** pImg){
	cv::Mat img = imread(fileName);

	ImageUtility::MatToCImage(&img, pImg);

	return img;
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIntelligentScissors_MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CIntelligentScissors_MFCDlg::OnBnClickedButtonLoadImg()
{
	if (pThread != NULL){
		status_CStatic.SetWindowTextW(LPCTSTR(CString("请等待后台计算！")));
		return;
	}

	if (pIntSci != NULL){
		delete pIntSci;
		pIntSci = NULL;
	}

	// TODO: Add your control notification handler code here
	TCHAR  szFilter[] = _T("JPEG(*.jpg)|*.jpg|BMP(*.jpg)|*.bmp|TIFF (*.tif)|*.tif|All Files (*.*)|*.*||");
	CFileDialog fileOpenDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (fileOpenDlg.DoModal() == IDOK)
	{
		CString imFilePath;
		VERIFY(imFilePath = fileOpenDlg.GetPathName());
		string fileName = (CStringA)imFilePath;
		if (fileName.size() >= fileName_max_length){
			status_CStatic.SetWindowTextW(LPCTSTR(CString("文件名过长")));
			UpdateData(false);
			return;
		}
		fileName.copy(_filename, fileName.size(), 0);
		_filename[fileName.size()] = '\0';

		mat = loadImage(fileName, &pImg);

		this->Invalidate();
	}
}


void CIntelligentScissors_MFCDlg::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here
	static int protectLock = 1;
	if (protectLock){
		protectLock = 0;
		if (mat.data){
			if (pThread != NULL){
				status_CStatic.SetWindowTextW(LPCTSTR(CString("请等待后台计算！")));
				protectLock = 1;
				return;
			}

			ImageUtility::MatToCImage(&mat, &pImg);
			OnPaint();

			if (result.data){
				result.release();
			}
			status = START;
			status_CStatic.SetWindowTextW(LPCTSTR(CString("请选择种子坐标")));
			UpdateData(false);
			pIntSci = new IntelligentScissors(mat);
		}
		protectLock = 1;
	}
}


void CIntelligentScissors_MFCDlg::OnBnClickedButtonEnd()
{
	// TODO: Add your control notification handler code here
	static int protectLock = 1;
	if (protectLock){
		protectLock = 0;
		if (pIntSci != NULL){
			if (result.data){
				result.release();
			}
			if (pThread != NULL){
				status_CStatic.SetWindowTextW(LPCTSTR(CString("请等待后台计算！")));
				protectLock = 1;
				return;
			}

			if (status == ALREADY ){
				result = pIntSci->getResult();
				ImageUtility::MatToCImage(&result, &pImg);
				OnPaint();

				status = INIT;
			}

			delete pIntSci;
			pIntSci = NULL;
		}
		protectLock = 1;
	}
}


void CIntelligentScissors_MFCDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnLButtonDown(nFlags, point);
	int x = point.x - oriX_value;
	int y = point.y - oriY_value;

	CEdit* cEdit_point = ((CEdit*)GetDlgItem(IDC_EDIT_POINT));
	if ((status == START || status == ALREADY) && pThread == NULL){
		if (x >= 0 && y >= 0 && x < mat.cols&& y < mat.rows){
			status = WAITING;

			status_CStatic.SetWindowTextW(LPCTSTR(CString("计算中，请稍后。。。")));

			DP_percent_CProgressCtrl.ShowWindow(true);//显示该控件
			DP_percent_CProgressCtrl.SetPos(0);
			DP_percent_CStatic.ShowWindow(true);
			DP_percent_CStatic.SetWindowTextW(LPCTSTR(CString("0.00%")));

			SetTimer(1, 10, NULL);
			UpdateData(false);

			threadInfo.pIntSci = pIntSci;
			threadInfo.status = &status;
			threadInfo.x = x;
			threadInfo.y = y;
			float temp = 0.0f;
			pThread = AfxBeginThread(CIntelligentScissors_MFCDlg::ThreadFunc, &threadInfo);
		}
	}
}


void CIntelligentScissors_MFCDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnMouseMove(nFlags, point);

	int x = point.x - oriX_value;
	int y = point.y - oriY_value;

	CEdit* cEdit_point = ((CEdit*)GetDlgItem(IDC_EDIT_POINT));
	if (status!=INIT){
		if (x >= 0 && y >= 0 && x < mat.cols&& y < mat.rows){

			CString temp;
			CString test("(%d,%d)");
			temp.Format(test, x, y);
			cEdit_point->SetWindowTextW(LPCTSTR(temp));

			if (status == ALREADY){
				KillTimer(1);
				pThread = NULL;
				DP_percent_CProgressCtrl.ShowWindow(false);//隐藏该控件
				DP_percent_CStatic.ShowWindow(false);
				status_CStatic.SetWindowTextW(LPCTSTR(CString("请选择路径")));

				pIntSci->MouseCallbackFunc(CV_EVENT_MOUSEMOVE, x, y, nFlags, NULL);

				Mat temp_mat = pIntSci->getTemp();
				//imshow("temp_mat", temp_mat);
				ImageUtility::MatToCImage(&temp_mat, &pImg);
				OnPaint();
			}

			UpdateData(false);
		}
	}
}

void CIntelligentScissors_MFCDlg::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	static int protectLock = 1;
	if (protectLock){
		protectLock = 0;
		if (result.data){
			char temp_fileName[fileName_max_length + 7];
			char temp_extension[10];
			int index = 0;
			while (_filename[index] != '.'){
				index++;
			}
			int null_index = index;
			while (_filename[null_index] != '\0') null_index++;

			memcpy(temp_extension, _filename + index, null_index - index + 1);

			memcpy(temp_fileName, _filename, index);
			memcpy(temp_fileName + index, "_result", 7);
			memcpy(temp_fileName + index + 7, temp_extension, null_index - index + 1);

			imwrite(temp_fileName, result);

			status_CStatic.SetWindowTextW(LPCTSTR(CString("保存成功！")));
			UpdateData(false);
		}
		protectLock = 1;
	}
}

UINT CIntelligentScissors_MFCDlg::ThreadFunc(LPVOID IpParam){
	ThreadInfo* p = (ThreadInfo*)IpParam;
	p->pIntSci->MouseCallbackFunc(CV_EVENT_LBUTTONDOWN, p->x, p->y, 0, &(p->dPPercent));

	*(p->status) = ALREADY;

	return 0;
}

void CIntelligentScissors_MFCDlg::setDP_percent(float percent){
	DP_percent_CProgressCtrl.SetPos(percent);

	CString temp;
	temp.Format(CString("%.2f%%"), percent);
	DP_percent_CStatic.SetWindowTextW(LPCTSTR(temp));
	UpdateData(false);
}

void CIntelligentScissors_MFCDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (pThread != NULL){
		status_CStatic.SetWindowTextW(LPCTSTR(CString("请等待后台计算！")));
		return;
	}
	CDialogEx::OnClose();
}


void CIntelligentScissors_MFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case 1:
	{
		if (pThread != NULL && status == WAITING && threadInfo.dPPercent.pDP_percent!=NULL){
			setDP_percent(*(threadInfo.dPPercent.pDP_percent));
		}
		else if (status == ALREADY){
			setDP_percent(100.00f);
			status_CStatic.SetWindowTextW(LPCTSTR(CString("已完成计算，请选择路径")));
		}
	}
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

