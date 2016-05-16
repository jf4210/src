// CV_picture.cpp : ʵ���ļ�
//

#include "..\stdafx.h"		//������빤�̺�͵ð������ļ�
#include "CV_picture.h"
#include "..\Resource.h"
#include <opencv2\opencv.hpp>
using namespace cv;
using namespace std;

IMPLEMENT_DYNAMIC(CV_picture, CStatic)

CV_picture::CV_picture()
: m_nShowType(0), m_nX(0), m_nY(0), m_bShowRectTracker_H(FALSE), m_bShowRectTracker_V(FALSE)
{
}
//���ڹ����Զ�����ʱ���޷���������ʼ���ö����������һ����ʼ����������
void CV_picture::OnInit(int nShowType /*= 0*/, bool bBtnDown /*= true*/)
{
	this->GetClientRect(&m_rect);
	this->GetWindowRect(&m_rect_win);

	m_src_img=Mat(m_rect.Height(),m_rect.Width(),CV_8UC3);
	m_src_img=0;
	m_dst_img=m_src_img+0;
	m_drawing=m_dst_img+0;
	m_MouseDraw_img=Mat(m_dst_img.size(),m_dst_img.type(),Scalar(255,255,255));
	m_iDrawingMethod=0;
	m_ptMButtonDown=0;
	m_ptBeforeMove=Point2f(0);

	//��ʼ��roi��ز���
	m_fRoi_scale=1;
	m_rect_roi=Rect(0,0,m_dst_img.cols,m_dst_img.rows);
	m_rect_roi_center.x=(float)m_rect_roi.width/2.0f;
	m_rect_roi_center.y=(float)m_rect_roi.height/2.0f;
	m_MouseDraw_rect=Rect(m_rect_roi_center.x,m_rect_roi_center.y,50,50);
	m_dst_roi=m_dst_img(m_rect_roi);

	//Ĭ��״̬�¸ÿؼ����Ա����ֻ�пؼ�������ÿؼ��Ż���Ӧ������
	m_bEnableMSG=1;
	m_bActive=0;
	m_iDragFlag=0;
	m_iMouseDraw=1;
	m_bLButtonDown=0;

	//++liujf 2016.3.17
	m_bEnableBtnDown = bBtnDown;
	m_nShowType = nShowType;
	//--

	m_RectTrackerH.m_nStyle = CRectTracker::resizeInside | CRectTracker::solidLine;//����RectTracker��ʽ	CRectTracker::resizeInside
	m_RectTrackerH.m_nHandleSize = 5; //���Ʊ������ش�С
//	m_RectTrackerH.m_rect.SetRect(-10, -20, 150, 150); //��ʼ��m_rect��ֵ
	
	m_RectTrackerV.m_nStyle = CRectTracker::resizeInside | CRectTracker::solidLine;//����RectTracker��ʽ	CRectTracker::resizeInside
	m_RectTrackerV.m_nHandleSize = 5; //���Ʊ������ش�С
//	m_RectTrackerV.m_rect.SetRect(0, 0, 150, 150); //��ʼ��m_rect��ֵ
	///////////////��������Ӷ���ĳ�ʼ������////////////////

	m_bMouseMoveinfoEnable=0;
}

CV_picture::~CV_picture()
{
}


//����ӦͼƬ�ؼ�����ԭͼƬ���������س�ʼ����������ظ�������ͬ�ߴ��ͼ�����ұ������ڵ�״̬����ʼ����ֱ�Ӷ�m_dst_img��ֵ��
void CV_picture::cvLoadImage(cv::Mat &img)
{
	this->GetClientRect(&m_rect);
	this->GetWindowRect(&m_rect_win);

	m_src_img=img+0;
	m_dst_img=m_src_img+0;
	m_MouseDraw_img=Mat(m_dst_img.size(),m_dst_img.type(),Scalar(255,255,255));
	m_rect_roi=Rect(0,0,m_dst_img.cols,m_dst_img.rows);
	m_rect_roi_center.x=(float)m_rect_roi.width/2.0f;
	m_rect_roi_center.y=(float)m_rect_roi.height/2.0f;
	m_MouseDraw_rect=Rect(m_rect_roi_center.x,m_rect_roi_center.y,10,10);
	m_dst_roi=m_dst_img(m_rect_roi);
	m_fRoi_scale=1;
}

//����ӦͼƬ�ؼ�����ԭͼƬ���������س�ʼ��������Ϊ�ļ�·����
bool  CV_picture::cvLoadImage(CString &Path)
{
	this->GetClientRect(&m_rect);
	this->GetWindowRect(&m_rect_win);

	Mat checkopen;
	checkopen = imread((string)(CT2CA)Path);
	if (checkopen.data==NULL)
	{
		MessageBox(_T("�����ļ�������ȷ��·�����ļ������Ƿ���ȷ��"),_T("����ʧ��"),MB_ICONERROR);
		return 0;
	}
	m_src_img=checkopen+0;
	m_dst_img=m_src_img+0;
	m_MouseDraw_img=Mat(m_dst_img.size(),m_dst_img.type(),Scalar(255,255,255));
	m_rect_roi=Rect(0,0,m_dst_img.cols,m_dst_img.rows);
	m_rect_roi_center.x=(float)m_rect_roi.width/2.0f;
	m_rect_roi_center.y=(float)m_rect_roi.height/2.0f;
	m_MouseDraw_rect=Rect(m_rect_roi_center.x,m_rect_roi_center.y,10,10);
	m_dst_roi=m_dst_img(m_rect_roi);
	m_fRoi_scale=1;

	return 1;
}

//�Զ�����img��С����Ӧ��ӦͼƬ�ؼ���������������ʾ������ͬʱ����img��������m_dst_img��
//��ʼ��roi��ز����������������൱��������LoadImage��
//ע������ˢ�»���������ʾ��ͬ����ͼƬ��Ӧ�ȵ���һ�θú�����Ȼ��ʹ��ShowImage_roiװ��������ͼƬ���Ч��
//method���ŵķ�ʽ��0��ʾ����ͼƬԭ����ȥ��Ӧm_rect,1��ʾ����ͼƬȥ��Ӧm_rect
void CV_picture::ShowImage(Mat &img,int method)
{
	this->GetClientRect(&m_rect);
	this->GetWindowRect(&m_rect_win);

	//��img��������m_dst_img����ʼ��roi��ز���
	m_dst_img=img+0;
	m_MouseDraw_img=Mat(m_dst_img.size(),m_dst_img.type(),Scalar(255,255,255));
	m_rect_roi=Rect(0,0,m_dst_img.cols,m_dst_img.rows);
	m_rect_roi_center.x=(float)m_rect_roi.width/2.0f+0.5f;
	m_rect_roi_center.y=(float)m_rect_roi.height/2.0f+0.5f;
	m_MouseDraw_rect=Rect(m_rect_roi_center.x,m_rect_roi_center.y,10,10);
	m_dst_roi=m_dst_img(m_rect_roi);
	m_fRoi_scale=1;
	//ͼƬ��С��ͬ����ֱ������ResizeImage����imgԭ�����Ƶ�m_drawing���ټ�����
	if (m_dst_img.size!=m_drawing.size)
	{
		TRACE("ShowImage: img(%d,%d),m_rect_roi(%d,%d),m_dst_roi(%d,%d)\n", img.cols, img.rows, m_rect_roi.width, m_rect_roi.height, m_dst_roi.cols, m_dst_roi.rows);
		ResizeImage(m_dst_img,m_rect,m_drawing,method);
	}else
	{
		m_drawing=m_dst_img+0;
	}

	OnPaint();
}


//����m_dst_img��roi�������ʹ��ǰӦ����LoadImage��ShowImage���ع�һ��ͼ��
//���ø÷���ǰ������ShowImage�Ը����ڵ�m_dst_img��roi��ʼ��һ�Σ���������
//center_vecΪroiƫ������(ƫ�Ƶ�λΪm_dst_img�ĵ�λ)��m_fRoi_scaleΪroi���ԭͼ�����ű�(����-1���ʾ���ı�)
void CV_picture::SetImage_roi(cv::Point &center_vec,float &roi_scale)
{
	this->GetClientRect(&m_rect);
	this->GetWindowRect(&m_rect_win);

	if (roi_scale>=0)	//���m_fRoi_scale�仯�ˣ�����������ԭͼ
	{
		m_rect_roi_center=m_rect_roi_center+center_vec;
		//����m_rect_roi����ȡm_dst_roi
		Point tl,br;
		int roi_width,roi_height;
#if 1
		roi_height = roi_scale*((float)m_rect.Height());
		roi_width = roi_scale*((float)m_rect.Width());
		//ͼ��Ŵ���С�ı߽��⣬��ֹԽ��,roi����Խ��ͼ��ԽС
		//roi�����С���
		int nMinH = m_dst_img.rows / 5;
		int nMinW = m_dst_img.cols / 5;
		if ((roi_height <= roi_width) && (roi_height<nMinH))	//40
		{
			roi_height = nMinH;
			roi_scale = (float)roi_height / (float)m_rect.Height();
			roi_width = roi_scale*((float)m_rect.Width());
		}
		if ((roi_height>roi_width) && (roi_width<nMinW))		//40
		{
			roi_width = nMinW;
			roi_scale = (float)roi_width / (float)m_rect.Width();
			roi_height = roi_scale*((float)m_rect.Height());
		}
		//roi���������
		if (roi_height>m_dst_img.rows)
		{
			float scale1 = (float)((float)m_dst_img.rows / (float)m_rect.Height());
			float scale2 = (float)((float)m_dst_img.cols / (float)m_rect.Width());
			roi_scale= min(scale1, scale2);
			roi_height=m_dst_img.rows;
			roi_width = roi_scale*((float)m_rect.Width());
//			TRACE("******** height is max. roi_scale = %f\n", roi_scale);
		}
		if (roi_width>m_dst_img.cols)
		{
			float scale1 = (float)((float)m_dst_img.rows / (float)m_rect.Height());
			float scale2 = (float)((float)m_dst_img.cols / (float)m_rect.Width());
			roi_scale= min(scale1, scale2);
			roi_height=roi_scale*((float)m_rect.Height());
			roi_width=m_dst_img.cols;
//			TRACE("******** width is max. roi_scale = %f\n", roi_scale);
		}
//		TRACE("m_rect_roi_center(%d,%d), m_rect_roi(%d,%d,%d,%d), roi_scale = %f\n", m_rect_roi_center.x, m_rect_roi_center.y, m_rect_roi.tl().x, m_rect_roi.tl().y, m_rect_roi.width, m_rect_roi.height, roi_scale);
#else
		roi_height = roi_scale*((float)m_dst_img.rows);
		roi_width = roi_scale*((float)m_dst_img.cols);
		//ͼ��Ŵ���С�ı߽��⣬��ֹԽ��,roi����Խ��ͼ��ԽС
		//roi�����С���
		if ((roi_height<=roi_width)&&(roi_height<40))
		{
			roi_height=40;
			roi_scale=(float)roi_height/(float)m_dst_img.rows;
			roi_width=roi_scale*((float)m_dst_img.cols);
		}
		if ((roi_height>roi_width)&&(roi_width<40))
		{
			roi_width=40;
			roi_scale=(float)roi_width/(float)m_dst_img.cols;
			roi_height=roi_scale*((float)m_dst_img.rows);
		}
		//roi���������
		if (roi_height>m_dst_img.rows)
		{
			roi_scale=1;
			roi_height=m_dst_img.rows;
			roi_width=m_dst_img.cols;
		}
		if (roi_width>m_dst_img.cols)
		{
			roi_scale=1;
			roi_height=m_dst_img.rows;
			roi_width=m_dst_img.cols;
		}
#endif
		m_fRoi_scale=roi_scale;
		//�����µ�m_rect_roi
		tl.x=(float)m_rect_roi_center.x-(float)roi_width/2.0f+0.5f;
		tl.y=(float)m_rect_roi_center.y-(float)roi_height/2.0f+0.5f;
		m_rect_roi=Rect(tl.x,tl.y,roi_width,roi_height);
		//���µ�m_rect_roi���б߽��⣬ǰ��ֻ�Ƕ������ʽ��б��������m_rect_roi������Ҳ���ƶ�������Խǵ���б���
		boundary_protect(tl,m_dst_img);
		br=tl+Point(roi_width,roi_height);
		boundary_protect(br,m_dst_img);
		tl=br-Point(roi_width,roi_height);
		//�������յ���m_rect_roi
		m_rect_roi=Rect(tl,br);
		m_rect_roi_center=Point((float)(tl.x+br.x)/2.0f,(float)(tl.y+br.y)/2.0f);
		m_dst_roi=m_dst_img(m_rect_roi);
//		TRACE("m_rect_roi_center(%d,%d), m_rect_roi(%d,%d,%d,%d)\n", m_rect_roi_center.x, m_rect_roi_center.y, m_rect_roi.tl().x, m_rect_roi.tl().y, m_rect_roi.width, m_rect_roi.height);
	}else if (-1==roi_scale)	//���m_fRoi_scaleΪ-1���ʾ����ԭ���ʣ������ƶ�roi����
	{
		m_rect_roi_center=m_rect_roi_center+center_vec;
		//����m_rect_roi����ȡm_dst_roi
		Point tl,br;
		tl.x=(float)m_rect_roi_center.x-(float)m_rect_roi.width/2.0f+0.5f;
		tl.y=(float)m_rect_roi_center.y-(float)m_rect_roi.height/2.0f+0.5f;
		m_rect_roi=Rect(tl.x,tl.y,m_rect_roi.width,m_rect_roi.height);
		//���µ�m_rect_roi���б߽��⣬ǰ��ֻ�Ƕ������ʽ��б��������m_rect_roi������Ҳ���ƶ�������Խǵ���б���
		boundary_protect(tl,m_dst_img);
		br=tl+Point(m_rect_roi.width,m_rect_roi.height);
		boundary_protect(br,m_dst_img);
		tl=br-Point(m_rect_roi.width,m_rect_roi.height);
		//�������յ���m_rect_roi
		m_rect_roi=Rect(tl,br);
		m_rect_roi_center=Point((float)(tl.x+br.x)/2.0f,(float)(tl.y+br.y)/2.0f);
		m_dst_roi=m_dst_img(m_rect_roi);
	}
}

//��ʾsrc��roi�����ڵ�ͼ����Ϊ�����κγ�ʼ����src��ͼƬ��������m_dst_img��ȫһ�£�ͬʱ��Ҳ��һ���ǳ���Ч����ʾͼƬ������
//�ʺ�����ˢ�»���������ʾ��ͬ����ͼƬ
void CV_picture::ShowImage_roi(cv::Mat &src,int method)
{
	try
	{
		this->GetClientRect(&m_rect);
		this->GetWindowRect(&m_rect_win);

		m_dst_roi = src(m_rect_roi);

		//ͼƬ��С��ͬ����ֱ������ResizeImage����imgԭ�����Ƶ�m_drawing���ټ�����
		if (m_dst_roi.size != m_drawing.size)
		{
			//		TRACE("ShowImage_roi: src(%d,%d),m_rect_roi(%d,%d),m_dst_roi(%d,%d),m_drawing(%d,%d)\n", src.cols, src.rows, m_rect_roi.width, m_rect_roi.height, m_dst_roi.cols, m_dst_roi.rows, m_drawing.cols, m_drawing.rows);
			ResizeImage(m_dst_roi, m_rect, m_drawing, method);
		}
		else
		{
			m_drawing = m_dst_roi + 0;
		}

		if (m_bShowRectTracker_H)
		{
			int nX1 = (int)((float)(m_ptHTracker1.x - m_rect_roi.tl().x) / (float)m_rect_roi.width * m_rect.Width());
			int nY1 = (int)((float)(m_ptHTracker1.y - m_rect_roi.tl().y) / (float)m_rect_roi.height * m_rect.Height());
			int nX2 = (int)((float)(m_ptHTracker2.x - m_rect_roi.tl().x) / (float)m_rect_roi.width * m_rect.Width());
			int nY2 = (int)((float)(m_ptHTracker2.y - m_rect_roi.tl().y) / (float)m_rect_roi.height * m_rect.Height());
			m_RectTrackerH.m_rect.left = nX1;
			m_RectTrackerH.m_rect.top = nY1;
			m_RectTrackerH.m_rect.right = nX2;
			m_RectTrackerH.m_rect.bottom = nY2;
		}
		else if (m_bShowRectTracker_V)
		{
			int nX1 = (int)((float)(m_ptVTracker1.x - m_rect_roi.tl().x) / (float)m_rect_roi.width * m_rect.Width());
			int nY1 = (int)((float)(m_ptVTracker1.y - m_rect_roi.tl().y) / (float)m_rect_roi.height * m_rect.Height());
			int nX2 = (int)((float)(m_ptVTracker2.x - m_rect_roi.tl().x) / (float)m_rect_roi.width * m_rect.Width());
			int nY2 = (int)((float)(m_ptVTracker2.y - m_rect_roi.tl().y) / (float)m_rect_roi.height * m_rect.Height());
			m_RectTrackerV.m_rect.left = nX1;
			m_RectTrackerV.m_rect.top = nY1;
			m_RectTrackerV.m_rect.right = nX2;
			m_RectTrackerV.m_rect.bottom = nY2;
		}

		OnPaint();
	}
	catch (cv::Exception &exc)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::ShowImage_roi error. detail: %s\n", exc.msg);
		TRACE(szLog);
	}
	catch (...)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::ShowImage_roi error2. Unknown error.\n");
		TRACE(szLog);
	}
}

//��ʾͼ���ָ�����򣬶Դ�ͼ��ͼƬ�ؼ���������ʾ��ȫ���ô˺�����ʾָ����������
void CV_picture::ShowImage_rect(cv::Mat &src, cv::Point pt)
{
	try
	{
		this->GetClientRect(&m_rect);
		this->GetWindowRect(&m_rect_win);

		m_dst_img = src + 0;

		int nRoiW = m_rect.Width() * m_fRoi_scale;
		int nRoiH = m_rect.Height() * m_fRoi_scale;
// 		if (pt.x + nRoiW > m_dst_img.cols)
// 			nRoiW = m_dst_img.cols - pt.x;
// 		if (pt.y + nRoiH > m_dst_img.rows)
// 			nRoiH = m_dst_img.rows - pt.y;

		if (pt.x + nRoiW > m_dst_img.cols)
			pt.x = m_dst_img.cols - nRoiW;
		if (pt.y + nRoiH > m_dst_img.rows)
			pt.y = m_dst_img.rows - nRoiH;

		if (pt.x < 0)
		{
			pt.x = 0;
			if (pt.x + nRoiW > m_dst_img.cols)
				nRoiW = m_dst_img.cols - pt.x;
		}
		if (pt.y < 0)
		{
			pt.y = 0;
			if (pt.y + nRoiH > m_dst_img.rows)
				nRoiH = m_dst_img.rows - pt.y;
		}
		
		//	m_rect_roi = Rect(pt.x, pt.y, m_rect.Width(), m_rect.Height());
		m_rect_roi = Rect(pt.x, pt.y, nRoiW, nRoiH);
		m_rect_roi_center.x = pt.x + (float)m_rect_roi.width / 2.0f + 0.5f;
		m_rect_roi_center.y = pt.y + (float)m_rect_roi.height / 2.0f + 0.5f;
		m_dst_roi = m_dst_img(m_rect_roi);

		m_drawing = m_dst_roi + 0;

		if (m_bShowRectTracker_H)
		{
			int nX1 = (int)((float)(m_ptHTracker1.x - m_rect_roi.tl().x) / (float)m_rect_roi.width * m_rect.Width());
			int nY1 = (int)((float)(m_ptHTracker1.y - m_rect_roi.tl().y) / (float)m_rect_roi.height * m_rect.Height());
			int nX2 = (int)((float)(m_ptHTracker2.x - m_rect_roi.tl().x) / (float)m_rect_roi.width * m_rect.Width());
			int nY2 = (int)((float)(m_ptHTracker2.y - m_rect_roi.tl().y) / (float)m_rect_roi.height * m_rect.Height());
			m_RectTrackerH.m_rect.left = nX1;
			m_RectTrackerH.m_rect.top = nY1;
			m_RectTrackerH.m_rect.right = nX2;
			m_RectTrackerH.m_rect.bottom = nY2;
		}
		else if (m_bShowRectTracker_V)
		{
			int nX1 = (int)((float)(m_ptVTracker1.x - m_rect_roi.tl().x) / (float)m_rect_roi.width * m_rect.Width());
			int nY1 = (int)((float)(m_ptVTracker1.y - m_rect_roi.tl().y) / (float)m_rect_roi.height * m_rect.Height());
			int nX2 = (int)((float)(m_ptVTracker2.x - m_rect_roi.tl().x) / (float)m_rect_roi.width * m_rect.Width());
			int nY2 = (int)((float)(m_ptVTracker2.y - m_rect_roi.tl().y) / (float)m_rect_roi.height * m_rect.Height());
			m_RectTrackerV.m_rect.left = nX1;
			m_RectTrackerV.m_rect.top = nY1;
			m_RectTrackerV.m_rect.right = nX2;
			m_RectTrackerV.m_rect.bottom = nY2;
		}

		OnPaint();
	}
	catch (cv::Exception &exc)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::OnPaint error. detail: %s\n", exc.msg);
		TRACE(szLog);
	}
	catch (...)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::OnPaint error2. Unknown error.\n");
		TRACE(szLog);
	}
}


//img�����ͼ��rectϣ�������ɵĴ�С������ΪMFC���CRect����dst_img�������ͼ��
//�ú����ȸ�ʽ��dst_img,�ٽ�img����dst_img,��˲���img��dst_img��������ͬһ��ͼ
//method���ŵķ�ʽ��0��ʾ����ͼƬԭ����ȥ��Ӧrect,1��ʾ����ͼƬȥ��Ӧrect
void CV_picture::ResizeImage(Mat &img,CRect rect,Mat &dst_img,int method)
{
	try
	{
		// ��ȡͼƬ�Ŀ�͸�
		int h = img.rows;
		int w = img.cols;
		int nw, nh;

		//	TRACE("ResizeImage: img(%d,%d)��rect(%d,%d),dst_img(%d,%d), method=%d.\n", img.cols, img.rows, rect.Width(),rect.Height(),dst_img.cols,dst_img.rows,method);

		if (0 == method)
		{
			// ���㽫ͼƬ���ŵ�dst_drawing��������ı�������
			float scale1 = (float)((float)w / (float)rect.Width());
			float scale2 = (float)((float)h / (float)rect.Height());

			float scale = (scale1 > scale2) ? scale1 : scale2;

			// ���ź�ͼƬ�Ŀ�͸�
			nw = ((float)w) / scale;
			nh = ((float)h) / scale;

			//���ڸ���Ҳ�������ݽضϣ�Ϊ��ֹnw����Ŀ����С�������߽籣��
			if (nw > rect.Width())
			{
				nw = rect.Width();
			}
			if (nh > rect.Height())
			{
				nh = rect.Height();
			}
		}
		else if (1 == method)
		{
			// ���ź�ͼƬ�Ŀ�͸�
			nw = rect.Width();
			nh = rect.Height();
		}

		// Ϊ�˽����ź��ͼƬ���� dst_drawing �����в�λ�������ͼƬ�� dst_drawing ���Ͻǵ���������ֵ
		int tlx = (nw < rect.Width()) ? (((float)(rect.Width() - nw)) / 2.0f + 0.5f) : 0;
		int tly = (nh < rect.Height()) ? (((float)(rect.Height() - nh)) / 2.0f + 0.5f) : 0;

		//++liujf 2016.3.17
		m_nNewWidth = nw;
		m_nNewHeight = nh;
		m_nX = tlx;
		m_nY = tly;
		//--

		//���þ����С��ͼ�Ƚϴ�ʱҲ�ܷ�ʱ�䣬�Ӹ��жϿɱ����ظ������þ����С
		if (dst_img.rows != rect.Height() || dst_img.cols != rect.Width() || dst_img.type() != img.type())
		{
			dst_img = Mat(rect.Height(), rect.Width(), img.type());
		}
		if (0 == method)
		{
			dst_img = 0;
		}
		// ���� dst_img �� ROI ��������������ĺ��ͼƬ img
		Rect rect_roi = Rect(tlx, tly, nw, nh);

		// ��ͼƬ img �������ţ������뵽 src_drawing ��
		Mat dst_img_roi = dst_img(rect_roi);

		//��img�ߴ�������dst_img_roi�ߴ�,��ֵ����ѡ��INTER_NEAREST���ŵ������޽�����ʾ�ܽ�ÿ�����ؿ���
		resize(img, dst_img_roi, cv::Size(nw, nh), 0.0, 0.0, INTER_NEAREST);
	}
	catch (cv::Exception &exc)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::ResizeImage error. detail: %s\n", exc.msg);
		TRACE(szLog);
	}
	catch (...)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::ResizeImage error2. Unknown error.\n");
		TRACE(szLog);
	}
}




//##############################################��Ϣ����##############################################################
BEGIN_MESSAGE_MAP(CV_picture, CStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// CV_picture ��Ϣ�������
void CV_picture::OnMouseMove(UINT nFlags, CPoint point)
{
// 	if (m_iMouseDraw == 2)
// 		TRACE("point(%d,%d)\n", point.x, point.y);

	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//����м䰴������¼������ν������Ϣʱָ��λ��ƫ��
	//(Ϊ�����ۼ��������ݽض��������㷨Ӧ�ü����������갴��ʱ������λ�ƶ�����ֱ�Ӽ���ÿ�ν������Ϣʱ��С��λ��)
	Point2f move_vec;	//������Ϣ�ƶ�ǰroi����ڳ�ʼλ�õ�λ��
	CPoint mouse_vec;	//������Ϣ���λ������ڳ�ʼ���λ�õ�λ��
	Point center_vec;	//������ϢroiӦ���ƶ���λ��
	if (1==m_iDragFlag)
	{
		move_vec=(Point2f)m_rect_roi.tl()-m_ptBeforeMove;
		mouse_vec=m_ptMButtonDown-point;
		//������������ģʽ�ֱ����roi�ƶ�����������ƶ�����ı�
		if (0==m_iDrawingMethod)
		{
			// ���㽫ͼƬ���ŵ�dst_drawing��������ı�������
			float scale1 = (float) ( (float)m_rect_roi.width /(float)m_rect.Width() );
			float scale2 = (float) ( (float)m_rect_roi.height /(float)m_rect.Height());
			m_scale_move.x=(scale1>scale2)?scale1:scale2;
			m_scale_move.y=m_scale_move.x;
		}
		else
		{
			m_scale_move.x=(float)m_rect_roi.width/(float)m_rect.Width();
			m_scale_move.y=(float)m_rect_roi.height/(float)m_rect.Height();
		}
		center_vec=Point((float)mouse_vec.x*m_scale_move.x-move_vec.x,(float)mouse_vec.y*m_scale_move.y-move_vec.y);
		//ֻ�����ƶ��������3�Ž���ͼƬ���ƶ�
		if( center_vec.dot(center_vec)>0.5 )
		{
			float scale_nochange=-1;
			SetImage_roi(center_vec,scale_nochange);
		}
	}

	//��ͼ����
	if (2==m_iMouseDraw)
	{
		//�������λ�ü��㻭ˢ����λ��
// 		Point tl,br;
// 		tl=Point(point.x-m_MouseDraw_rect.width/2,point.y-m_MouseDraw_rect.height/2);
// 		boundary_protect(tl,m_MouseDraw_img);
// 		br=tl+Point(m_MouseDraw_rect.width,m_MouseDraw_rect.height);
// 		boundary_protect(br,m_MouseDraw_img);
// 		tl=br-Point(m_MouseDraw_rect.width,m_MouseDraw_rect.height);
// 		m_MouseDraw_rect=Rect(tl,br);
		//���㻭ˢ��Ӧ��roi�ڵľ���
// 		Rect draw_rect_inROI;
// 		Point tl_inROI,br_inROI;
// 		static Point tl_inROI_last;
// 		tl_inROI.x=(float)tl.x/(float)m_rect.Width()*(float)m_rect_roi.width+0.5f;
// 		tl_inROI.y=(float)tl.y/(float)m_rect.Height()*(float)m_rect_roi.height+0.5f;
// 		br_inROI.x=(float)br.x/(float)m_rect.Width()*(float)m_rect_roi.width+0.5f;
// 		br_inROI.y=(float)br.y/(float)m_rect.Height()*(float)m_rect_roi.height+0.5f;

// 		if (tl_inROI!=tl_inROI_last)
// 		{
//			draw_rect_inROI=Rect(tl_inROI,br_inROI);
#if 1
			if (m_nShowType != 0)
			{
				int nX = (float)point.x / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
				int nY = (float)point.y / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;
				if (nX < m_rect_roi.tl().x)
					nX = m_rect_roi.tl().x;
				if (nY < m_rect_roi.tl().y)
					nY = m_rect_roi.tl().y;
				if (nX > m_rect_roi.br().x)
					nX = m_rect_roi.br().x;
				if (nY > m_rect_roi.br().y)
					nY = m_rect_roi.br().y;

				m_ptCur.x = nX;
				m_ptCur.y = nY;
			}
			else
			{
				if (m_iDrawingMethod == 0)
				{
					int nX = point.x - m_nX;
					int nY = point.y - m_nY;
					if (nX < 0)
						nX = 0;
					if (nY < 0)
						nY = 0;
					if (nX > m_nNewWidth)
						nX = m_nNewWidth;
					if (nY > m_nNewHeight)
						nY = m_nNewHeight;

					m_ptCur.x = (float)nX / (float)m_nNewWidth *(float)m_rect_roi.width;
					m_ptCur.y = (float)nY / (float)m_nNewHeight *(float)m_rect_roi.height;
				}
				else
				{
					m_ptCur.x = (float)point.x / (float)m_rect.Width()*(float)m_rect_roi.width;
					m_ptCur.y = (float)point.y / (float)m_rect.Height()*(float)m_rect_roi.height;
				}
			}
//			TRACE("OnMouseMove: m_dst_img(%d,%d), m_ptCur(%d,%d), point(%d,%d), m_rect_roi(%d,%d,%d,%d)\n", m_dst_img.cols, m_dst_img.rows, m_ptCur.x, m_ptCur.y, point.x, point.y, m_rect_roi.tl().x, m_rect_roi.tl().y, m_rect_roi.width, m_rect_roi.height);
			
			Rect draw_rect_inROI2;
			draw_rect_inROI2 = Rect(m_ptPrev, m_ptCur);
			Mat tmp = m_dst_img.clone();
			Mat tmp2 = m_dst_img.clone();

			rectangle(tmp, m_ptPrev, m_ptCur, CV_RGB(255, 0, 0), 2);
			rectangle(tmp2, m_ptPrev, m_ptCur, CV_RGB(255, 233, 10), -1);
			addWeighted(tmp, 0.5, tmp2, 0.5, 0, tmp);

//			cv::rectangle(tmp, draw_rect_inROI2, Scalar(0, 0, 0), -1);
//			rectangle(tmp, m_ptPrev, m_ptCur, CV_RGB(255, 0, 0));
			::SendMessageA(this->GetParent()->m_hWnd, WM_CV_picture, 0, LPARAM(&tmp));
//			tl_inROI_last = tl_inROI;
			CStatic::OnMouseMove(nFlags, point);
			return;
#else
			//ʹ�û�ˢ��ͼ����
			cv::rectangle(m_MouseDraw_img(m_rect_roi),draw_rect_inROI,Scalar(0,0,0),-1);
			Point nomove(0,0);
			float nochange=-1;
			::SendMessageA(this->GetParent()->m_hWnd,WM_CV_picture,0,LPARAM(&m_MouseDraw_img));
#endif
// 		}
// 		tl_inROI_last=tl_inROI;
	}
//	TRACE("OnMouseMove: m_dst_img(%d,%d)\n", m_dst_img.cols, m_dst_img.rows);
	ShowImage_roi(m_dst_img,m_iDrawingMethod);

	if (m_bMouseMoveinfoEnable)
	{
		m_ptMouseMove=point;
			//������ؼ�����ʾ��ǰ��������
			//�������λ�ü���λ��ͼ���ϵ�λ��
		Point point_in_img;
		point_in_img.x=(float)point.x/(float)m_rect.Width()*(float)m_rect_roi.width;
		point_in_img.y=(float)point.y/(float)m_rect.Height()*(float)m_rect_roi.height;
		point_in_img.x=point_in_img.x+m_rect_roi.x;
		point_in_img.y=point_in_img.y+m_rect_roi.y;
		//�����ǽ���ǰ������Ϣ��ʾ���Ի���ؼ���ģ��
// 		char x[10];
// 		sprintf(x,"%.5f",height);
// 		CWnd *pCWnd_parent;
// 		pCWnd_parent=GetParent();
// 		pCWnd_parent->GetDlgItem(IDC_EDIT1)->SetWindowTextA(x);
	}

	CStatic::OnMouseMove(nFlags, point);
}


//��ǰ�ؼ������ý��㣬���ܲ�׽��������Ϣ
BOOL CV_picture::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	try
	{
		//����ͼƬ�ؼ�������
		this->GetClientRect(&m_rect);
		this->GetWindowRect(&m_rect_win);

		if ((m_rect_win.PtInRect(pt)) && (1 == m_bActive))
		{
			//��������src_roi�����src�����ű�
			m_fRoi_scale = m_fRoi_scale + m_fRoi_scale*0.2f*(float)zDelta / 120.0f;
			//���㵱ǰ�����Ի������ĵ�ƫ��
			CPoint src_draw_tl = m_rect_win.TopLeft();
			Point pt_in_draw = Point((pt.x - src_draw_tl.x), (pt.y - src_draw_tl.y));
			Point vec(pt_in_draw.x - m_rect.CenterPoint().x, pt_in_draw.y - m_rect.CenterPoint().y);
			//��ǰ���űȵ����roi���ĵ�ƫ��
			Point center_vec = Point((float(vec.x)*m_fRoi_scale + 0.5f), (float(vec.y)*m_fRoi_scale + 0.5f));

//			TRACE("OnMouseWheel, pt(%d,%d), m_fRoi_scale = %f\n", pt.x, pt.y, m_fRoi_scale);
			//��ʾ�������roi����
			SetImage_roi(center_vec, m_fRoi_scale);
			//������������ģʽ�ֱ����roi�ƶ�����������ƶ�����ı�
			if (0 == m_iDrawingMethod)
			{
				// ���㽫ͼƬ���ŵ�dst_drawing��������ı�������
				float scale1 = (float)((float)m_rect_roi.width / (float)m_rect.Width());
				float scale2 = (float)((float)m_rect_roi.height / (float)m_rect.Height());
				m_scale_move.x = (scale1 > scale2) ? scale1 : scale2;
				m_scale_move.y = m_scale_move.x;
			}
			else{
				m_scale_move.x = (float)m_rect_roi.width / (float)m_rect.Width();
				m_scale_move.y = (float)m_rect_roi.height / (float)m_rect.Height();
			}
		}

		ShowImage_roi(m_dst_img, m_iDrawingMethod);
//		TRACE("��������ʾ��ͼ�����:(%d,%d,%d,%d), ����(%d,%d)\n", m_rect_roi.tl().x, m_rect_roi.tl().y, m_rect_roi.width, m_rect_roi.height, m_rect_roi_center.x, m_rect_roi_center.y);

		::SendMessageA(this->GetParent()->m_hWnd, WM_CV_MBtnWheel, (WPARAM)&m_fRoi_scale, (LPARAM)&m_rect_roi);
	}
	catch (cv::Exception &exc)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::OnMouseWheel error. detail: %s\n", exc.msg);
		TRACE(szLog);
	}
	catch (...)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::OnMouseWheel error2. Unknown error.\n");
		TRACE(szLog);
	}
	
	return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}


void CV_picture::OnMButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	//����ͼƬ�ؼ�������
	this->GetClientRect(&m_rect);
	this->GetWindowRect(&m_rect_win);

	if (1==m_bEnableMSG)
	{
		m_bActive=1;
		m_iDragFlag=1;
		m_ptMButtonDown=point;
		m_ptBeforeMove=m_rect_roi.tl();
		//ʹ��ǰ�ؼ���ý��㣬�����Ŀؼ����ܲ�׽��������Ϣ
		this->SetFocus();
	}
	TRACE("OnMButtonDown\n");
	//////////// TODO: �ڴ���ӿؼ�����Ĵ���������/////////////////////////////

	CStatic::OnMButtonDown(nFlags, point);
}


void CV_picture::OnMButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	m_iDragFlag=0;
	m_ptMButtonDown=0;
	m_ptBeforeMove=m_rect_roi.tl();
	//////////// TODO: �ڴ���ӿؼ�����Ĵ���������/////////////////////////////
	TRACE("OnMButtonUp\n");
	CStatic::OnMButtonUp(nFlags, point);
}


void CV_picture::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!m_bEnableBtnDown)
		return CStatic::OnLButtonDown(nFlags, point);

#if 1
	if (m_bShowRectTracker_H)
	{
		if (m_RectTrackerH.HitTest(point) < 0)     //���δ���о���ѡ���,���»�ѡ���
		{
// 			m_RectTrackerH.TrackRubberBand(this, point, TRUE);
// 			m_RectTrackerH.m_rect.NormalizeRect();   //���滯���Σ��������滯���������н��ܣ�
		}
		else           //������о���ѡ���
		{
			m_RectTrackerH.Track(this, point, TRUE);
			m_RectTrackerH.m_rect.NormalizeRect();   //���滯����
		}

		int nX = (float)m_RectTrackerH.m_rect.left / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
		int nY = (float)m_RectTrackerH.m_rect.top / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;
		int nX2 = (float)m_RectTrackerH.m_rect.right / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
		int nY2 = (float)m_RectTrackerH.m_rect.bottom / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;

		m_ptHTracker1.x = nX;
		m_ptHTracker1.y = nY;
		m_ptHTracker2.x = nX2;
		m_ptHTracker2.y = nY2;
		Invalidate();
	}
	else if (m_bShowRectTracker_V)
	{
		if (m_RectTrackerV.HitTest(point) < 0)     //���δ���о���ѡ���,���»�ѡ���
		{
			m_RectTrackerV.TrackRubberBand(this, point, TRUE);
			m_RectTrackerV.m_rect.NormalizeRect();   //���滯���Σ��������滯���������н��ܣ�
		}
		else           //������о���ѡ���
		{
			m_RectTrackerV.Track(this, point, TRUE);
			m_RectTrackerV.m_rect.NormalizeRect();   //���滯����
		}
		
		int nX = (float)m_RectTrackerV.m_rect.left / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
		int nY = (float)m_RectTrackerV.m_rect.top / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;
		int nX2 = (float)m_RectTrackerV.m_rect.right / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
		int nY2 = (float)m_RectTrackerV.m_rect.bottom / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;

		m_ptVTracker1.x = nX;
		m_ptVTracker1.y = nY;
		m_ptVTracker2.x = nX2;
		m_ptVTracker2.y = nY2;
		Invalidate();
	}
	else
	{
		try
		{
			SetCapture();
			m_bLButtonDown = 1;

			//����ͼƬ�ؼ�������
			this->GetClientRect(&m_rect);
			this->GetWindowRect(&m_rect_win);

			if (1 == m_bEnableMSG)
			{
				m_bActive = 1;
				//ʹ��ǰ�ؼ���ý��㣬�����Ŀؼ����ܲ�׽��������Ϣ
				this->SetFocus();
			}
			if (1 == m_iMouseDraw)
			{
				m_iMouseDraw = 2;
			}

			//++liujf 2016.3.15
			if (m_nShowType != 0)
			{
				int nX = (float)point.x / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
				int nY = (float)point.y / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;
				if (nX < m_rect_roi.tl().x)
					nX = m_rect_roi.tl().x;
				if (nY < m_rect_roi.tl().y)
					nY = m_rect_roi.tl().y;
				if (nX > m_rect_roi.br().x)
					nX = m_rect_roi.br().x;
				if (nY > m_rect_roi.br().y)
					nY = m_rect_roi.br().y;

				m_ptPrev.x = nX;
				m_ptPrev.y = nY;
			}
			else
			{
				if (m_iDrawingMethod == 0)
				{
					int nX = point.x - m_nX;
					int nY = point.y - m_nY;
					if (nX < 0)
						nX = 0;
					if (nY < 0)
						nY = 0;
					if (nX > m_nNewWidth)
						nX = m_nNewWidth;
					if (nY > m_nNewHeight)
						nY = m_nNewHeight;

					m_ptPrev.x = (float)nX / (float)m_nNewWidth *(float)m_rect_roi.width;
					m_ptPrev.y = (float)nY / (float)m_nNewHeight *(float)m_rect_roi.height;
				}
				else
				{
					m_ptPrev.x = (float)point.x / (float)m_rect.Width()*(float)m_rect_roi.width;
					m_ptPrev.y = (float)point.y / (float)m_rect.Height()*(float)m_rect_roi.height;
				}
			}		
			::SendMessageA(this->GetParent()->m_hWnd, WM_CV_LBTNDOWN, WPARAM(&m_ptPrev), NULL);
			//--
			TRACE("��갴�¼�¼���(nX, nY) = (%d, %d)\n", m_ptPrev.x, m_ptPrev.y);
//			TRACE("OnLButtonDown: m_dst_img(%d,%d), m_ptPrev(%d,%d), point(%d,%d), m_rect_roi(%d,%d,%d,%d)\n", m_dst_img.cols, m_dst_img.rows, m_ptPrev.x, m_ptPrev.y, point.x, point.y, m_rect_roi.tl().x, m_rect_roi.tl().y, m_rect_roi.width, m_rect_roi.height);
			ShowImage_roi(m_dst_img, m_iDrawingMethod);
		}
		catch (cv::Exception &exc)
		{
			char szLog[300] = { 0 };
			sprintf_s(szLog, "CV_picture::OnLButtonDown error. detail: %s\n", exc.msg);
			TRACE(szLog);
		}
		catch (...)
		{
			char szLog[300] = { 0 };
			sprintf_s(szLog, "CV_picture::OnLButtonDown error2. Unknown error.\n");
			TRACE(szLog);
		}
	}
#else
	try
	{
		SetCapture();
		m_bLButtonDown = 1;

		//����ͼƬ�ؼ�������
		this->GetClientRect(&m_rect);
		this->GetWindowRect(&m_rect_win);

		if (1 == m_bEnableMSG)
		{
			m_bActive = 1;
			//ʹ��ǰ�ؼ���ý��㣬�����Ŀؼ����ܲ�׽��������Ϣ
			this->SetFocus();
		}
		if (1 == m_iMouseDraw)
		{
			m_iMouseDraw = 2;
		}

		//++liujf 2016.3.15
		if (m_nShowType != 0)
		{
			int nX = (float)point.x / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
			int nY = (float)point.y / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;
			if (nX < m_rect_roi.tl().x)
				nX = m_rect_roi.tl().x;
			if (nY < m_rect_roi.tl().y)
				nY = m_rect_roi.tl().y;
			if (nX > m_rect_roi.br().x)
				nX = m_rect_roi.br().x;
			if (nY > m_rect_roi.br().y)
				nY = m_rect_roi.br().y;

			m_ptPrev.x = nX;
			m_ptPrev.y = nY;
		}
		else
		{
			if (m_iDrawingMethod == 0)
			{
				int nX = point.x - m_nX;
				int nY = point.y - m_nY;
				if (nX < 0)
					nX = 0;
				if (nY < 0)
					nY = 0;
				if (nX > m_nNewWidth)
					nX = m_nNewWidth;
				if (nY > m_nNewHeight)
					nY = m_nNewHeight;

				m_ptPrev.x = (float)nX / (float)m_nNewWidth *(float)m_rect_roi.width;
				m_ptPrev.y = (float)nY / (float)m_nNewHeight *(float)m_rect_roi.height;
			}
			else
			{
				m_ptPrev.x = (float)point.x / (float)m_rect.Width()*(float)m_rect_roi.width;
				m_ptPrev.y = (float)point.y / (float)m_rect.Height()*(float)m_rect_roi.height;
			}
		}		
		::SendMessageA(this->GetParent()->m_hWnd, WM_CV_LBTNDOWN, WPARAM(&m_ptPrev), NULL);
		//--
//		TRACE("OnLButtonDown: m_dst_img(%d,%d), m_ptPrev(%d,%d), point(%d,%d), m_rect_roi(%d,%d,%d,%d)\n", m_dst_img.cols, m_dst_img.rows, m_ptPrev.x, m_ptPrev.y, point.x, point.y, m_rect_roi.tl().x, m_rect_roi.tl().y, m_rect_roi.width, m_rect_roi.height);
		ShowImage_roi(m_dst_img, m_iDrawingMethod);
	}
	catch (cv::Exception &exc)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::OnLButtonDown error. detail: %s\n", exc.msg);
		TRACE(szLog);
	}
	catch (...)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::OnLButtonDown error2. Unknown error.\n");
		TRACE(szLog);
	}
#endif
	CStatic::OnLButtonDown(nFlags, point);
}


void CV_picture::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bEnableBtnDown)
		return CStatic::OnLButtonUp(nFlags, point);

	try
	{
		ReleaseCapture();

		m_bLButtonDown = 0;

		if (2 == m_iMouseDraw)
		{
			m_iMouseDraw = 1;
		}
		//++liujf 2016.3.15
		if (m_nShowType != 0)
		{
			int nX = (float)point.x / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
			int nY = (float)point.y / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;
			if (nX < m_rect_roi.tl().x)
				nX = m_rect_roi.tl().x;
			if (nY < m_rect_roi.tl().y)
				nY = m_rect_roi.tl().y;
			if (nX > m_rect_roi.br().x)
				nX = m_rect_roi.br().x;
			if (nY > m_rect_roi.br().y)
				nY = m_rect_roi.br().y;

			m_ptCur.x = nX;
			m_ptCur.y = nY;
		}
		else
		{
			if (m_iDrawingMethod == 0)
			{
				int nX = point.x - m_nX;
				int nY = point.y - m_nY;
				if (nX < 0)
					nX = 0;
				if (nY < 0)
					nY = 0;
				if (nX > m_nNewWidth)
					nX = m_nNewWidth;
				if (nY > m_nNewHeight)
					nY = m_nNewHeight;

				m_ptCur.x = (float)nX / (float)m_nNewWidth *(float)m_rect_roi.width;
				m_ptCur.y = (float)nY / (float)m_nNewHeight *(float)m_rect_roi.height;
			}
			else
			{
				m_ptCur.x = (float)point.x / (float)m_rect.Width()*(float)m_rect_roi.width;
				m_ptCur.y = (float)point.y / (float)m_rect.Height()*(float)m_rect_roi.height;
			}
		}		
//		TRACE("OnLButtonUp: m_dst_img(%d,%d), m_ptCur(%d,%d), point(%d,%d), m_rect_roi(%d,%d,%d,%d)\n", m_dst_img.cols, m_dst_img.rows, m_ptCur.x, m_ptCur.y, point.x, point.y, m_rect_roi.tl().x, m_rect_roi.tl().y, m_rect_roi.width, m_rect_roi.height);

		Rect rt = Rect(m_ptPrev, m_ptCur);
		if (rt.width == 0 || rt.height == 0)
			return CStatic::OnLButtonUp(nFlags, point);
		// 	m_ptCur.x = (float)point.x / (float)m_rect.Width()*(float)m_rect_roi.width;
		// 	m_ptCur.y = (float)point.y / (float)m_rect.Height()*(float)m_rect_roi.height;
		//	rectangle(m_dst_img, m_ptPrev, m_ptCur, CV_RGB(255, 0, 0));
		Mat tmp = m_dst_img.clone();
		Mat tmp2 = m_dst_img.clone();

		rectangle(tmp, m_ptPrev, m_ptCur, CV_RGB(255, 0, 0), 2);
		rectangle(tmp2, m_ptPrev, m_ptCur, CV_RGB(255, 233, 10), -1);
		addWeighted(tmp, 0.5, tmp2, 0.5, 0, tmp);
//		rectangle(tmp, m_ptPrev, m_ptCur, CV_RGB(255, 0, 0));
		::SendMessageA(this->GetParent()->m_hWnd, WM_CV_LBTNUP, WPARAM(&rt), LPARAM(&tmp));
		//--
//		TRACE("OnLButtonUp: m_dst_img(%d,%d), m_ptCur(%d,%d)\n", m_dst_img.cols, m_dst_img.rows, m_ptCur.x, m_ptCur.y);
//		ShowImage_roi(m_dst_img, m_iDrawingMethod);
	}
	catch (cv::Exception &exc)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::OnLButtonUp error. detail: %s\n", exc.msg);
		TRACE(szLog);
	}
	catch (...)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::OnLButtonUp error2. Unknown error.\n");
		TRACE(szLog);
	}
	
	CStatic::OnLButtonUp(nFlags, point);
}


void CV_picture::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ShowImage_roi(m_dst_img,m_iDrawingMethod);

	CStatic::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CV_picture::OnPaint()
{
	try
	{
		//������Ӧ�����ػ���Ϣ,ɾ���� WM_PAINT��Ϣ�޷�����Ϣ����������������𲻶ϵĴ����ػ�
		CPaintDC dc(this); // device context for painting
		// TODO: �ڴ˴������Ϣ����������
		CDC* pDC = this->GetDC();
		HDC hDC = pDC->GetSafeHdc();
#if 1
		
		this->GetClientRect(&m_rect);
		this->GetWindowRect(&m_rect_win);

		//��Matͼ���ڻ�����������м����

		IplImage* drawing_ipl = &IplImage(m_drawing);
		CvvImage Cvvimg;
		Cvvimg.CopyOf(drawing_ipl);
		// ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������
		Cvvimg.DrawToHDC(hDC, &m_rect);

		if (m_bShowRectTracker_H)
		{
			m_RectTrackerH.Draw(pDC);
		}
		if(m_bShowRectTracker_V)
		{
			m_RectTrackerV.Draw(pDC);
		}
		ReleaseDC(pDC);		//һ��GetDC�����Ӧһ��ReleaseDC������������ص��ڴ�й¶
#else
		this->GetClientRect(&m_rect);
		this->GetWindowRect(&m_rect_win);

		//��Matͼ���ڻ�����������м����

		IplImage* drawing_ipl = &IplImage(m_drawing);
		CvvImage Cvvimg;
		Cvvimg.CopyOf(drawing_ipl);
		// ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������
		Cvvimg.DrawToHDC(hDC, &m_rect);

		ReleaseDC(pDC);		//һ��GetDC�����Ӧһ��ReleaseDC������������ص��ڴ�й¶
#endif
		// ��Ϊ��ͼ��Ϣ���� CStatic::OnPaint()

		///////////////////��������Ӷ���Ļ��ƴ���///////////////////////
	}
	catch (cv::Exception &exc)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::OnPaint error. detail: %s\n", exc.msg);
		TRACE(szLog);
	}
	catch (...)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CV_picture::OnPaint error2. Unknown error.\n");
		TRACE(szLog);
	}
}


//��ı߽籣��������㳬���˸���ͼ��ķ�Χ���򽫳��������궨λ����Ӧ�߽�,��Ҫ�Ƕ�roi�߽籣����
//�����Ҫ�����ĵ��Ǿ����������걣������������߽紦�ĵ�����roi�Ľǵ��ǵ�rols����������������ֻ�ܵ�rols-1��
//��9��ʽ��ͼƬ�߽�ֱ�߷ָ�ƽ�棬�����ظõ�����������1-9,����-1��ʾ����
int CV_picture::boundary_protect(Point &pt,Mat &in_img)
{
	int x_area=1,y_area=1;
	int area;
	if((pt.x<0)||(pt.x>in_img.cols+20000))
	{
		pt.x=0;
		x_area=0;
	}else if(pt.x>in_img.cols)
	{
		pt.x=in_img.cols;
		x_area=2;
	}
	if ((pt.y<0)||(pt.y>in_img.rows+20000))
	{
		pt.y=0;
		y_area=0;
	}else if (pt.y>in_img.rows)
	{
		pt.y=in_img.rows;
		y_area=2;
	}
	area=x_area+y_area*10;

	switch (area)
	{
	case 0 : return 1;
	case 01: return 2;
	case 02: return 3;
	case 10: return 4;
	case 11: return 5;
	case 12: return 6;
	case 20: return 7;
	case 21: return 8;
	case 22: return 9;
	default: return -1;
	}
}

void CV_picture::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_nShowType != 0)
	{
		int nX = (float)point.x / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
		int nY = (float)point.y / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;
		if (nX < m_rect_roi.tl().x)
			nX = m_rect_roi.tl().x;
		if (nY < m_rect_roi.tl().y)
			nY = m_rect_roi.tl().y;
		if (nX > m_rect_roi.br().x)
			nX = m_rect_roi.br().x;
		if (nY > m_rect_roi.br().y)
			nY = m_rect_roi.br().y;

		m_ptRBtnUp.x = nX;
		m_ptRBtnUp.y = nY;
	}
	else
	{
		if (m_iDrawingMethod == 0)
		{
			int nX = point.x - m_nX;
			int nY = point.y - m_nY;
			if (nX < 0)
				nX = 0;
			if (nY < 0)
				nY = 0;
			if (nX > m_nNewWidth)
				nX = m_nNewWidth;
			if (nY > m_nNewHeight)
				nY = m_nNewHeight;

			m_ptRBtnUp.x = (float)nX / (float)m_nNewWidth *(float)m_rect_roi.width;
			m_ptRBtnUp.y = (float)nY / (float)m_nNewHeight *(float)m_rect_roi.height;
		}
		else
		{
			m_ptRBtnUp.x = (float)point.x / (float)m_rect.Width()*(float)m_rect_roi.width;
			m_ptRBtnUp.y = (float)point.y / (float)m_rect.Height()*(float)m_rect_roi.height;
		}
	}
	::SendMessageA(this->GetParent()->m_hWnd, WM_CV_RBTNUP, WPARAM(&m_ptRBtnUp), NULL);
	CStatic::OnRButtonUp(nFlags, point);
}

void CV_picture::SetShowRectTracker(bool bShowH, bool bShowV)
{
	m_bShowRectTracker_H = bShowH;
	m_bShowRectTracker_V = bShowV;
}

BOOL CV_picture::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
// 	if (pWnd == this && m_RectTrackerH.SetCursor(this, nHitTest))
//  		return TRUE;
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

void CV_picture::setHTrackerPosition(cv::Point pt1, cv::Point pt2)
{
	int nX = (float)pt1.x / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
	int nY = (float)pt1.y / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;
	int nX2 = (float)pt2.x / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
	int nY2 = (float)pt2.y / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;

	m_ptHTracker1.x = nX;
	m_ptHTracker1.y = nY;
	m_ptHTracker2.x = nX2;
	m_ptHTracker2.y = nY2;
	m_RectTrackerH.m_rect.SetRect(m_ptHTracker1.x, m_ptHTracker1.y, m_ptHTracker2.x, m_ptHTracker2.y);
}

void CV_picture::setVTrackerPosition(cv::Point pt1, cv::Point pt2)
{
	int nX = (float)pt1.x / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
	int nY = (float)pt1.y / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;
	int nX2 = (float)pt2.x / (float)m_rect.Width() * m_rect_roi.width + m_rect_roi.tl().x;
	int nY2 = (float)pt2.y / (float)m_rect.Height() * m_rect_roi.height + m_rect_roi.tl().y;

	m_ptVTracker1.x = nX;
	m_ptVTracker1.y = nY;
	m_ptVTracker2.x = nX2;
	m_ptVTracker2.y = nY2;
	m_RectTrackerV.m_rect.SetRect(m_ptVTracker1.x, m_ptVTracker1.y, m_ptVTracker2.x, m_ptVTracker2.y);
}
