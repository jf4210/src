/*M///////////////////////////////////////////////////////////////////////////////////////
//
//              �Զ���ͼƬ�ؼ��࣬ר���ڹ���MFC�е�picture�ؼ����γ���opencv�Ľӿ�
//				2014.4.20	����Charles
// 
//��һ���ֶ�����һЩͼƬ�����ķ���������ر���
//�ڶ�����Ϊ�ÿؼ�����Ϣ��Ӧ���󲿷���Ϣ��Ӧֻ����m_bEnableMSG==1�²���Ч��Ĭ��Ϊ1��
//һ��ͼƬ�ؼ�Ӧ��Ӧһ������Ķ��󣬱��ļ�������opencv1.x��CvvImage.h
//�����ö�����һ�²�����ɣ�
//1.����һ��picture�ؼ�
//2.ͨ��mfc��classwizard������picture�ؼ�����������һ��CStatic��Ķ�����m_showing��
//3.�ڿؼ����ڶԻ����ͷ�ļ�(testDlg.h)���ҵ��ոս����Ķ���m_showing��,���������͸�Ϊ����
//4.�ڿؼ����ڶԻ����ͷ�ļ�(testDlg.h)�а�����ͷ�ļ�
//5.�ڿؼ����ڶԻ����CCP��(testDlg.ccp)�ҵ�OnInitDialog,�������µĶ����ʼ�������м��룺
//			m_showImg.OnInit();
//���ϲ�����ɺ����ֱ��ʹ���ˣ������Ҫ��Ӷ������Ϣ���������½���Ϣ�������е�ĩβ���϶������
//
//Note�������Ĺ���ʹ�õ��ַ�������Ϊ�����ֽ��ַ����������ַ�����ת���л����
/*////////////////////////////////////////////////////////////////////////////////////////



#pragma once

#include <opencv/cv.h>
#include "opencv/highgui.h"
#include "CvvImage.h"


#define WM_CV_picture (WM_USER + 0x001)			//����ƶ��¼�
#define WM_CV_LBTNUP  (WM_USER + 0x002)			//������̧���¼�
#define WM_CV_MBtnWheel	(WM_USER + 0x003)		//�м���ֹ����¼�
#define WM_CV_LBTNDOWN	(WM_USER + 0x004)		//�����������¼�
#define WM_CV_RBTNUP	(WM_USER + 0x005)		//����Ҽ�̧���¼�

class CV_picture : public CStatic
{
	DECLARE_DYNAMIC(CV_picture)	//ע��CV_picture��

public:
	CV_picture();
	//���ڹ����Զ�����ʱ���޷���������ʼ���ö����������һ����ʼ����������
	void OnInit(int nShowType = 0, bool bBtnDown = true);
	virtual ~CV_picture();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//ר���ڴ����ػ棬ͬʱҲ���Ը���ͼƬ�ؼ��Ĵ������굽����ĳ�Ա�ڣ�Ĭ��ֻ����m_drawing���ؼ��ϣ�
	afx_msg void OnPaint();

public:
	BOOL m_bActive;		//�����ǣ������Ż���Ӧ�����Ϣ������ڿؼ��ڰ����м���������Զ�����
	BOOL m_bEnableMSG;		//�ڼ���״̬�£�ֻ�и�ֵΪTrue�Ż���Ӧ�����Ϣ��false�൱����ȫ���������Ϣ
	int m_iMouseDraw;	//��껭ͼ���

	bool	m_bShowRectTracker_H;
	bool	m_bShowRectTracker_V;
	CRectTracker m_RectTrackerH;
	CRectTracker m_RectTrackerV;
	void SetShowRectTracker(bool bShowH, bool bShowV);
protected:
	int m_iDragFlag;
	BOOL m_bLButtonDown;

public:
	//����ӦͼƬ�ؼ�����ԭͼƬ���������س�ʼ����������ظ�������ͬ�ߴ��ͼ�����ұ������ڵ�״̬����ʼ����ֱ�Ӷ�m_dst_img��ֵ��
	void cvLoadImage(cv::Mat &img);
	//����ӦͼƬ�ؼ�����ԭͼƬ���������س�ʼ��������Ϊ�ļ�·����
	bool cvLoadImage(CString &Path);

	//�Զ�����img��С����Ӧ��ӦͼƬ�ؼ���������������ʾ������ͬʱ����img��������m_dst_img��
	//��ʼ��roi��ز����������������൱��������LoadImage��
	//ע������ˢ�»���������ʾ��ͬ����ͼƬ��Ӧ�ȵ���һ�θú�����Ȼ��ʹ��ShowImage_roiװ��������ͼƬ���Ч��
	//method���ŵķ�ʽ��0��ʾ����ͼƬԭ����ȥ��Ӧm_rect,1��ʾ����ͼƬȥ��Ӧm_rect
	void ShowImage(cv::Mat &img,int method=0);

	//��ʾsrc��roi�����ڵ�ͼ����Ϊ�����κγ�ʼ����src��ͼƬ��������m_dst_img��ȫһ�£�ͬʱ��Ҳ��һ���ǳ���Ч����ʾͼƬ������
	//�ʺ�����ˢ�»���������ʾ��ͬ����ͼƬ
	void ShowImage_roi(cv::Mat &src,int method=0);

	//��ʾͼ���ָ�����򣬶Դ�ͼ��ͼƬ�ؼ���������ʾ��ȫ���ô˺�����ʾָ����������
	void ShowImage_rect(cv::Mat &src, cv::Point pt);

private:
	//����m_dst_img��roi�������ʹ��ǰӦ����LoadImage��ShowImage���ع�һ��ͼ��
	//���ø÷���ǰ������ShowImage�Ը����ڵ�m_dst_img��roi��ʼ��һ�Σ���������
	//center_vecΪroiƫ������(ƫ�Ƶ�λΪm_dst_img�ĵ�λ)��m_fRoi_scaleΪroi���ԭͼ�����ű�(����-1���ʾ���ı�)
	void SetImage_roi(cv::Point &center_vec,float &roi_scale);

	//img�����ͼ��rectϣ�������ɵĴ�С������ΪMFC���CRect����dst_img�������ͼ��
	//�ú����ȸ�ʽ��dst_img,�ٽ�img����dst_img,��˲���img��dst_img��������ͬһ��ͼ
	//method���ŵķ�ʽ��0��ʾ����ͼƬԭ����ȥ��Ӧrect,1��ʾ����ͼƬȥ��Ӧrect
	void ResizeImage(cv::Mat &img,CRect rect,cv::Mat &dst_img,int method=0);

	//��ı߽籣��������㳬���˸���ͼ��ķ�Χ���򽫳��������궨λ����Ӧ�߽�,��Ҫ�Ƕ�roi�߽籣����
	//�����Ҫ�����ĵ��Ǿ����������걣������������߽紦�ĵ�����roi�Ľǵ��ǵ�rols����������������ֻ�ܵ�rols-1��
	//��9��ʽ��ͼƬ�߽�ֱ�߷ָ�ƽ�棬�����ظõ�����������1-9,����-1��ʾ����
	int boundary_protect(cv::Point &pt,cv::Mat &in_img);


public:
	//�����Դͼ��һ��һ�����أ��ڸ���ͼƬǰ������������κβ���
	cv::Mat m_src_img;
	//������ʾ��ͼ��
	cv::Mat m_dst_img;
	//m_dst_img��roi������ShowImage��ʼ������ShowImage_roi�����ã�ͼƬ�ؼ�ֻ��ʾm_m_src_roi���������
	cv::Mat m_dst_roi;
	//ͼƬ�ؼ���Ӧ�ľ��ο�,ֻ�д�С��Ϣû��λ����Ϣ
	CRect m_rect;
	//ͼƬ�ؼ�λ�ڴ��ڵ�λ�����С
	CRect m_rect_win;
	//roi���������m_dst_img�����űȣ��������m_dst_img����һ��
	float m_fRoi_scale;
	//roi�ƶ�����������ƶ�����ı�
	cv::Point2f m_scale_move;	
	//roi��m_dst_img�е����򣬰��������С�������m_dst_img��λ��
	cv::Rect m_rect_roi;
	//��껭ͼ���õĻ��壬�����Ҫ�õ��û���ʱ��������ͼ��֮��������,��ͼʱֻ��m_MouseDraw_img��ROI����ͼ��Ҳ����ֻ�ڿؼ���ʾ������ͼ
	cv::Mat m_MouseDraw_img;
	//��껭ͼ���û�ˢ��С
	cv::Rect m_MouseDraw_rect;
	//��ʾͼ������ŷ�ʽ��0��ʾ����ͼƬԭ����ȥ��Ӧm_rect,1��ʾ����ͼƬȥ��ӦͼƬ��Ĭ��Ϊ1,0��ʱ��Items����ת����Щ���⣩
	int m_iDrawingMethod;		
	//�ƶ�ROIǰm_rect_roi��tl��
	cv::Point2f m_ptBeforeMove;
	//�ƶ�ROIǰMButtonDown��Ϣ����ʱ���������
	CPoint m_ptMButtonDown;

	//++liujf 2016.3.15
	int	 m_nShowType;			//��ʾ����:Ĭ��0-������ʾ����������ʾͼ��1-��ʾͼ��ȫ�������������ţ�����ͼ��ܴ�����ʾһ����
	bool m_bEnableBtnDown;		//�Ƿ���Ӧ��갴�²���
	cv::Point m_ptPrev;			//���������µ�����
	cv::Point m_ptCur;			//������̧�������
	cv::Point m_ptRBtnUp;		//����Ҽ�̧�������
	int m_nNewWidth;			//ͼ�񰴾��α������ź�Ŀ��
	int m_nNewHeight;			//ͼ�񰴾��α������ź�ĸ߶�
	int m_nX;					//ͼ�񰴾��α������ź�����X����
	int m_nY;					//ͼ�񰴾��α������ź�����Y����
	cv::Point m_ptOldTL;		//��Ƥ���ఴ��ʱ��ԭ������(0,0)�����ͼƬ�ϵ������
	cv::Point m_ptOldBR;		//��Ƥ���ఴ��ʱ��ԭ������(0,0)�����ͼƬ�ϵ������
	cv::Point m_ptHTracker1;	//ˮƽ��Ƥ���ʱtl��ʵ������
	cv::Point m_ptHTracker2;	//ˮƽ��Ƥ���ʱbr��ʵ������
	cv::Point m_ptVTracker1;	//��ֱ��Ƥ���ʱtl��ʵ������
	cv::Point m_ptVTracker2;	//��ֱ��Ƥ���ʱbr��ʵ������
	void setHTrackerPosition(cv::Point pt1, cv::Point pt2);
	void setVTrackerPosition(cv::Point pt1, cv::Point pt2);
	//--
protected:
	//���ڻ����ϵ�Matͼ,�κ�����¶���Ӧ��ֱ���޸�m_drawing�������
	cv::Mat m_drawing;
	cv::Point m_rect_roi_center;

/////////////////////��������Ӷ���ĳ�Ա����//////////////////////////
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags); 
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	CPoint m_ptMouseMove;		//����ƶ�ʱ�ڿؼ��е����꣬m_bMouseMoveinfoEnableΪFalseʱ��Ч
	bool m_bMouseMoveinfoEnable;	//�Ƿ��ȡ��ǰ���λ�ã�����ʾ����Ҫ�༭����ؼ���
};


