#pragma once
#include "CV_picture.h"

// CPicShow �Ի���

class CPicShow : public CDialog
{
	DECLARE_DYNAMIC(CPicShow)

public:
	CPicShow(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPicShow();

// �Ի�������
	enum { IDD = IDD_PICSHOW };

public:
	CV_picture	m_picShow;//picture control �ؼ�����
	CScrollBar	m_scrollBarH; //ˮƽ������
	CScrollBar	m_scrollBarV; //��ֱ������

	BOOL		m_bShowScrolH;
	BOOL		m_bShowScrolV;
	cv::Mat		m_src_img;
	cv::Mat		m_dst_img;

	CRect m_client;//picture control��С
	int m_picWidth;    //����ͼƬ�Ŀ�
	int m_picHeight;   //����ͼƬ�ĸ�

	int m_iShowWidth;   //Ҫ��ʾ�Ŀ�
	int m_iShowHeight;   //Ҫ��ʾ�ĸ�

	int m_iX;    //ͼ�ϴ�ʱҪ��ʾ��X����
	int m_iY;    //ͼ�ϴ�ʱҪ��ʾ��Y����
	float m_fScale;		//��ǰͼ������ű�

	void	ShowPic(cv::Mat imgMat);
	LRESULT CvPaint(WPARAM wParam, LPARAM lParam);
	LRESULT RoiLbtnUp(WPARAM wParam, LPARAM lParam);
	LRESULT MBtnWheel(WPARAM wParam, LPARAM lParam);
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
