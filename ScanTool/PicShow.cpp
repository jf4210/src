// PicShow.cpp : 实现文件
//

#include "stdafx.h"
#include "ScanTool.h"
#include "PicShow.h"
#include "afxdialogex.h"


const int nScrollH_H = 12;
const int nScrollV_W = 12;

using namespace cv;
using namespace std;
// CPicShow 对话框

IMPLEMENT_DYNAMIC(CPicShow, CDialog)

CPicShow::CPicShow(CWnd* pParent /*=NULL*/)
	: CDialog(CPicShow::IDD, pParent)
	, m_bShowScrolH(TRUE), m_bShowScrolV(TRUE), m_iX(0), m_iY(0), m_fScale(1.0)
{

}

CPicShow::~CPicShow()
{
}

void CPicShow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PicShow, m_picShow);
	DDX_Control(pDX, IDC_SCROLLBAR_H, m_scrollBarH);
	DDX_Control(pDX, IDC_SCROLLBAR_V, m_scrollBarV);
}


BEGIN_MESSAGE_MAP(CPicShow, CDialog)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_MESSAGE(WM_CV_MBtnWheel, &CPicShow::MBtnWheel)
	ON_MESSAGE(WM_CV_picture, &CPicShow::CvPaint)
	ON_MESSAGE(WM_CV_LBTNUP, &CPicShow::RoiLbtnUp)
	ON_MESSAGE(WM_CV_LBTNDOWN, &CPicShow::RoiLbtnDown)
	ON_MESSAGE(WM_CV_RBTNUP, &CPicShow::RoiRbtnUp)
END_MESSAGE_MAP()

BOOL CPicShow::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_picShow.OnInit(1);

	m_scrollBarV.ShowScrollBar(m_bShowScrolV);
	m_scrollBarH.ShowScrollBar(m_bShowScrolH);

	return TRUE;
}

// CPicShow 消息处理程序
void CPicShow::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);


#if 1
	CRect rt;
	GetClientRect(&rt);
	if (m_picShow.GetSafeHwnd())
	{
		if (!m_bShowScrolH && !m_bShowScrolV)
			m_picShow.MoveWindow(0, 0, rt.Width(), rt.Height());
		else if (!m_bShowScrolH)
			m_picShow.MoveWindow(0, 0, rt.Width() - nScrollV_W - 1, rt.Height());
		else if (!m_bShowScrolV)
			m_picShow.MoveWindow(0, 0, rt.Width(), rt.Height() - nScrollH_H - 1);
		else
			m_picShow.MoveWindow(0, 0, rt.Width() - nScrollV_W - 1, rt.Height() - nScrollH_H - 1);
	}

	if (m_bShowScrolH && m_scrollBarH.GetSafeHwnd())
		m_scrollBarH.MoveWindow(0, rt.Height() - nScrollH_H, rt.Width(), rt.Height());
	if (m_bShowScrolV && m_scrollBarV.GetSafeHwnd())
		m_scrollBarV.MoveWindow(rt.Width() - nScrollV_W, 0, rt.Width(), rt.Height());
#else
	if (m_picShow.GetSafeHwnd())
	{
		if (!m_bShowScrolH && !m_bShowScrolV)
			m_picShow.MoveWindow(0, 0, cx, cy);
		else if (!m_bShowScrolH)
			m_picShow.MoveWindow(0, 0, cx - nScrollV_W - 1, cy);
		else if (!m_bShowScrolV)
			m_picShow.MoveWindow(0, 0, cx, cy - nScrollH_H - 1);
		else
			m_picShow.MoveWindow(0, 0, cx - nScrollV_W - 1, cy - nScrollH_H - 1);
	}

	if (m_bShowScrolH && m_scrollBarH.GetSafeHwnd())
		m_scrollBarH.MoveWindow(0, cy - nScrollH_H, cx, cy);
	if (m_bShowScrolV && m_scrollBarV.GetSafeHwnd())
		m_scrollBarV.MoveWindow(cx - nScrollV_W, 0, cx, cy);
#endif
}

BOOL CPicShow::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
		if (pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CPicShow::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int pos;
	pos = m_scrollBarV.GetScrollPos();

	switch (nSBCode)
	{
	case SB_LINEUP:
		pos -= 1;
		break;
	case SB_LINEDOWN:
		pos += 1;
		break;
	case SB_PAGEUP:
		pos -= 10;
		break;
	case SB_PAGEDOWN:
		pos += 10;
		break;
	case SB_TOP:
		pos = 0;
		break;
	case SB_BOTTOM:
		pos = m_picHeight;
		break;
	case SB_THUMBPOSITION:
		pos = nPos;
		break;
	case SB_THUMBTRACK:
		pos = nPos;
		break;
	default:
		return;
	}
	if (pos<0)
		pos = 0;
	else if (pos > m_picHeight)
		pos = m_picHeight;
// 	else if (pos + m_client.Height()>m_picHeight - 1)
// 		pos = m_picHeight - 1 - m_client.Height();
	m_scrollBarV.SetScrollPos(pos, TRUE);
	m_iY = pos;


//	cv::Point pt(m_iX * m_fScale, m_iY * m_fScale);
	cv::Point pt(m_iX * 1, m_iY * 1);
//	TRACE("OnVScroll, pt(%d, %d), 缩放后:(%d,%d)\n", m_iX, m_iY, m_iX * m_fScale, m_iY * m_fScale);

	m_picShow.ShowImage_rect(m_src_img, pt);
}

void CPicShow::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int pos;
	pos = m_scrollBarH.GetScrollPos();

	switch (nSBCode)
	{
	case SB_LINEUP:
		pos -= 1;
		break;
	case SB_LINEDOWN:
		pos += 1;
		break;
	case SB_PAGEUP:
		pos -= 10;
		break;
	case SB_PAGEDOWN:
		pos += 10;
		break;
	case SB_TOP:
		pos = 0;
		break;
	case SB_BOTTOM:
		pos = m_picWidth;
		break;
	case SB_THUMBPOSITION:
		pos = nPos;
		break;
	case SB_THUMBTRACK:
		pos = nPos;
		break;
	default:
		TRACE("nSBCode = %d, nPos = %d\n", nSBCode, nPos);
		return;
	}
	if (pos<0)
		pos = 0;
	else if (pos > m_picWidth)
		pos = m_picWidth;
// 	else if (pos + m_client.Width()>m_picWidth - 1)
// 		pos = m_picWidth - 1 - m_client.Width();
	m_scrollBarH.SetScrollPos(pos, TRUE);
	m_iX = pos;

//	cv::Point pt(m_iX * m_fScale, m_iY * m_fScale);
	cv::Point pt(m_iX * 1, m_iY * 1);
//	TRACE("OnHScroll, pt(%d, %d), 缩放后:(%d,%d), m_fScale = %f.\n", m_iX, m_iY, m_iX * m_fScale, m_iY * m_fScale, m_fScale);

	m_picShow.ShowImage_rect(m_src_img, pt);
}

void CPicShow::ShowPic(cv::Mat& imgMat, cv::Point pt /*= cv::Point(0,0)*/)
{
	CRect rcDlg;
	this->GetClientRect(&rcDlg);

	CRect rcPic;
	m_picShow.GetClientRect(&m_client);
	m_picWidth  = imgMat.cols;
	m_picHeight = imgMat.rows;

	int nPageH, nPageV;
	if (m_picWidth < m_client.Width())
		nPageH = m_picWidth;
	else
		nPageH = m_client.Width();
	if (m_picHeight < m_client.Height())
		nPageV = m_picHeight;
	else
		nPageV = m_client.Height();

	int nWidthMax = m_picWidth;
	int nHeightMax = m_picHeight;

	if (pt.x < 0)
		pt.x = 0;
	if (pt.y < 0)
		pt.y = 0;
	if (pt.x + m_client.Width() > nWidthMax)
		pt.x = nWidthMax - m_client.Width();
	if (pt.y + m_client.Height() > nHeightMax)
		pt.y = nHeightMax - m_client.Height();

	if (pt.x == 0 && pt.y == 0)
	{
		pt.x = m_iX;
		pt.y = m_iY;
	}
	else
	{
		m_iX = pt.x;
		m_iY = pt.y;
	}
	
	if (m_picWidth >= m_client.Width())
	{
		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_ALL;
		info.nMin = 0;
		info.nMax = nWidthMax;
		info.nPage = nPageH;			//m_client.Width();
		info.nPos = pt.x;
		m_scrollBarH.SetScrollInfo(&info);
	}

	if (m_picHeight >= m_client.Height())
	{
		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_ALL;
		info.nMin = 0;
		info.nMax = nHeightMax;
		info.nPage = nPageV;			//m_client.Height();
		info.nPos = pt.y;
		m_scrollBarV.SetScrollInfo(&info);
	}

	m_src_img = imgMat;
//	cv::Point pt(0, 0);
	m_picShow.ShowImage_rect(m_src_img, pt);
}

LRESULT CPicShow::CvPaint(WPARAM wParam, LPARAM lParam)
{
	Mat* pShowMat = (Mat*)(lParam);
	m_picShow.ShowImage_roi(*pShowMat, 0);

	return TRUE;
}

LRESULT CPicShow::RoiLbtnUp(WPARAM wParam, LPARAM lParam)
{
	Rect* pRt = (Rect*)(wParam);
	Mat*  pShowMat = (Mat*)(lParam);

// 	m_dst_img = m_src_img(*pRt);
// 	m_picShow.ShowImage(m_dst_img, 0);

//	m_picShow.ShowImage(*pShowMat, 0);
	::SendMessageA(this->GetParent()->GetParent()->m_hWnd, WM_CV_LBTNUP, wParam, lParam);
	return TRUE;
}

LRESULT CPicShow::RoiLbtnDown(WPARAM wParam, LPARAM lParam)
{
	::SendMessageA(this->GetParent()->GetParent()->m_hWnd, WM_CV_LBTNDOWN, wParam, lParam);
	return TRUE;
}

LRESULT CPicShow::MBtnWheel(WPARAM wParam, LPARAM lParam)
{
	float fScale = *(float*)wParam;
	cv::Rect rcRoi = *(cv::Rect*)lParam;
	m_fScale = fScale;

	int nMaxH, nMaxV;
	nMaxH = m_picWidth;
	nMaxV = m_picHeight;
// 	nMaxH = m_picWidth / fScale;
// 	nMaxV = m_picHeight / fScale;
// 	if (nMaxH < m_client.Width())
// 		nMaxH = m_client.Width();
// 	if (nMaxV < m_client.Height())
// 		nMaxV = m_client.Height();

	int nX = m_scrollBarH.GetScrollPos();
	int nY = m_scrollBarV.GetScrollPos();
// 	m_iX = nX / fScale;
// 	m_iY = nY / fScale;
	m_iX = rcRoi.tl().x;
	m_iY = rcRoi.tl().y;
	if (m_iX > nMaxH)
		m_iX = nMaxH;
	if (m_iY > nMaxV)
		m_iY = nMaxV;
	
	SCROLLINFO infoH;
	infoH.cbSize = sizeof(SCROLLINFO);
	infoH.fMask = SIF_ALL;
	infoH.nMin = 0;
	infoH.nMax = nMaxH;
	infoH.nPage = rcRoi.width;//m_client.Width();
	infoH.nPos = m_iX;
	m_scrollBarH.SetScrollInfo(&infoH);

	SCROLLINFO infoV;
	infoV.cbSize = sizeof(SCROLLINFO);
	infoV.fMask = SIF_ALL;
	infoV.nMin = 0;
	infoV.nMax = nMaxV;
	infoV.nPage = rcRoi.height;// m_client.Height();
	infoV.nPos = m_iY;
	m_scrollBarV.SetScrollInfo(&infoV);

//	TRACE("MBtnWheel, fScale = %f, 滚动前起始位置(%d,%d), 滚动后位置(%d,%d),滚动范围(%d,%d)\n", fScale, nX, nY, m_iX, m_iY, nMaxH, nMaxV);

	return TRUE;
}

LRESULT CPicShow::RoiRbtnUp(WPARAM wParam, LPARAM lParam)
{
	::SendMessageA(this->GetParent()->GetParent()->m_hWnd, WM_CV_RBTNUP, wParam, lParam);
	return TRUE;
}

void CPicShow::SetShowTracker(bool bShowH, bool bShowV)
{
	m_picShow.SetShowRectTracker(bShowH, bShowV);
}

