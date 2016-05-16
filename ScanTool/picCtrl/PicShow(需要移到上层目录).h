#pragma once
#include "CV_picture.h"

// CPicShow 对话框

class CPicShow : public CDialog
{
	DECLARE_DYNAMIC(CPicShow)

public:
	CPicShow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPicShow();

// 对话框数据
	enum { IDD = IDD_PICSHOW };

public:
	CV_picture	m_picShow;//picture control 控件变量
	CScrollBar	m_scrollBarH; //水平滚动条
	CScrollBar	m_scrollBarV; //垂直滚动条

	BOOL		m_bShowScrolH;
	BOOL		m_bShowScrolV;
	cv::Mat		m_src_img;
	cv::Mat		m_dst_img;

	CRect m_client;//picture control大小
	int m_picWidth;    //载入图片的宽
	int m_picHeight;   //载入图片的高

	int m_iShowWidth;   //要显示的宽
	int m_iShowHeight;   //要显示的高

	int m_iX;    //图较大时要显示的X坐标
	int m_iY;    //图较大时要显示的Y坐标
	float m_fScale;		//当前图像的缩放比

	void	ShowPic(cv::Mat imgMat);
	LRESULT CvPaint(WPARAM wParam, LPARAM lParam);
	LRESULT RoiLbtnUp(WPARAM wParam, LPARAM lParam);
	LRESULT MBtnWheel(WPARAM wParam, LPARAM lParam);
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
