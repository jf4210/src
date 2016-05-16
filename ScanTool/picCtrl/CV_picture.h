/*M///////////////////////////////////////////////////////////////////////////////////////
//
//              自定义图片控件类，专用于关联MFC中的picture控件，形成与opencv的接口
//				2014.4.20	——Charles
// 
//第一部分定义了一些图片操作的方法及其相关变量
//第二部分为该控件的消息相应，大部分消息相应只有在m_bEnableMSG==1下才有效（默认为1）
//一个图片控件应对应一个该类的对象，本文件依赖于opencv1.x的CvvImage.h
//建立该对象由一下步骤完成：
//1.建立一个picture控件
//2.通过mfc的classwizard来将该picture控件关联并建立一个CStatic类的对象（例m_showing）
//3.在控件所在对话框的头文件(testDlg.h)中找到刚刚建立的对象（m_showing）,并将其类型改为本类
//4.在控件所在对话框的头文件(testDlg.h)中包含本头文件
//5.在控件所在对话框的CCP中(testDlg.ccp)找到OnInitDialog,并在其下的额外初始化代码中加入：
//			m_showImg.OnInit();
//以上步骤完成后则可直接使用了，如果需要添加额外的消息处理，可以新建消息或在已有的末尾加上额外代码
//
//Note：建立的工程使用的字符集必须为“多字节字符集”否则字符类型转换中会出错
/*////////////////////////////////////////////////////////////////////////////////////////



#pragma once

#include <opencv/cv.h>
#include "opencv/highgui.h"
#include "CvvImage.h"


#define WM_CV_picture (WM_USER + 0x001)			//鼠标移动事件
#define WM_CV_LBTNUP  (WM_USER + 0x002)			//鼠标左键抬起事件
#define WM_CV_MBtnWheel	(WM_USER + 0x003)		//中间滚轮滚动事件
#define WM_CV_LBTNDOWN	(WM_USER + 0x004)		//鼠标左键按下事件
#define WM_CV_RBTNUP	(WM_USER + 0x005)		//鼠标右键抬起事件

class CV_picture : public CStatic
{
	DECLARE_DYNAMIC(CV_picture)	//注册CV_picture类

public:
	CV_picture();
	//由于关联自定义类时，无法带参数初始化该对象，因而独立一个初始化函数出来
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
	//专用于窗口重绘，同时也可以更新图片控件的窗口坐标到该类的成员内（默认只绘制m_drawing到控件上）
	afx_msg void OnPaint();

public:
	BOOL m_bActive;		//激活标记，激活后才会响应鼠标消息，鼠标在控件内按下中键或左键会自动激活
	BOOL m_bEnableMSG;		//在激活状态下，只有该值为True才会响应鼠标消息，false相当于完全屏蔽鼠标消息
	int m_iMouseDraw;	//鼠标画图标记

	bool	m_bShowRectTracker_H;
	bool	m_bShowRectTracker_V;
	CRectTracker m_RectTrackerH;
	CRectTracker m_RectTrackerV;
	void SetShowRectTracker(bool bShowH, bool bShowV);
protected:
	int m_iDragFlag;
	BOOL m_bLButtonDown;

public:
	//往对应图片控件加载原图片，并完成相关初始化（如果是重复载入相同尺寸的图，并且保持现在的状态不初始化则直接对m_dst_img赋值）
	void cvLoadImage(cv::Mat &img);
	//往对应图片控件加载原图片，并完成相关初始化（输入为文件路径）
	bool cvLoadImage(CString &Path);

	//自动调节img大小以适应对应图片控件，并将其完整显示出来，同时将该img存入该类的m_dst_img并
	//初始化roi相关参数用于其它处理（相当于运行了LoadImage）
	//注：对于刷新画面或高速显示相同规格的图片，应先调用一次该函数，然后使用ShowImage_roi装载其他的图片提高效率
	//method缩放的方式：0表示保持图片原比例去适应m_rect,1表示拉伸图片去适应m_rect
	void ShowImage(cv::Mat &img,int method=0);

	//显示src的roi区域内的图像，因为不带任何初始化，src的图片规格必须与m_dst_img完全一致，同时这也是一个非常高效的显示图片函数，
	//适合用于刷新画面或高速显示相同规格的图片
	void ShowImage_roi(cv::Mat &src,int method=0);

	//显示图像的指定区域，对大图像，图片控件不足以显示完全，用此函数显示指定起点的区域
	void ShowImage_rect(cv::Mat &src, cv::Point pt);

private:
	//设置m_dst_img的roi区域，因此使用前应先用LoadImage或ShowImage加载过一个图像
	//调用该方法前需先用ShowImage对该类内的m_dst_img和roi初始化一次，否则会出错
	//center_vec为roi偏移向量(偏移单位为m_dst_img的单位)，m_fRoi_scale为roi相对原图的缩放比(传入-1则表示不改变)
	void SetImage_roi(cv::Point &center_vec,float &roi_scale);

	//img输入的图像，rect希望调整成的大小（类型为MFC类的CRect），dst_img调整后的图，
	//该函数先格式化dst_img,再将img画到dst_img,因此不能img与dst_img绝不能是同一个图
	//method缩放的方式：0表示保持图片原比例去适应rect,1表示拉伸图片去适应rect
	void ResizeImage(cv::Mat &img,CRect rect,cv::Mat &dst_img,int method=0);

	//点的边界保护，如果点超出了给定图像的范围，则将超出的坐标定位到对应边界,主要是对roi边界保护，
	//如果需要保护的点是矩阵数据坐标保护，则对于最大边界处的点会出错（roi的角点是到rols，而矩阵数据坐标只能到rols-1）
	//按9格方式以图片边界直线分割平面，并返回该点所处的区域1-9,返回-1表示错误
	int boundary_protect(cv::Point &pt,cv::Mat &in_img);


public:
	//输入的源图像，一般一经加载，在更换图片前都不对其进行任何操作
	cv::Mat m_src_img;
	//即将显示的图像
	cv::Mat m_dst_img;
	//m_dst_img的roi区域，由ShowImage初始化，由ShowImage_roi来设置，图片控件只显示m_m_src_roi区域的内容
	cv::Mat m_dst_roi;
	//图片控件对应的矩形框,只有大小信息没有位置信息
	CRect m_rect;
	//图片控件位于窗口的位置与大小
	CRect m_rect_win;
	//roi区域相对于m_dst_img的缩放比，长宽比与m_dst_img保持一致
	float m_fRoi_scale;
	//roi移动距离与鼠标移动距离的比
	cv::Point2f m_scale_move;	
	//roi在m_dst_img中的区域，包含区域大小与相对于m_dst_img的位置
	cv::Rect m_rect_roi;
	//鼠标画图所用的画板，如果需要用到该画板时，将待画图与之关联即可,作图时只在m_MouseDraw_img的ROI区域画图，也就是只在控件显示的区域画图
	cv::Mat m_MouseDraw_img;
	//鼠标画图所用画刷大小
	cv::Rect m_MouseDraw_rect;
	//显示图像的缩放方式：0表示保持图片原比例去适应m_rect,1表示拉伸图片去适应图片框（默认为1,0的时候画Items坐标转换有些问题）
	int m_iDrawingMethod;		
	//移动ROI前m_rect_roi的tl点
	cv::Point2f m_ptBeforeMove;
	//移动ROI前MButtonDown消息触发时鼠标的坐标点
	CPoint m_ptMButtonDown;

	//++liujf 2016.3.15
	int	 m_nShowType;			//显示类型:默认0-缩放显示，按比例显示图像，1-显示图像全部，不进行缩放，比如图像很大，则显示一部分
	bool m_bEnableBtnDown;		//是否响应鼠标按下操作
	cv::Point m_ptPrev;			//鼠标左键按下的坐标
	cv::Point m_ptCur;			//鼠标左键抬起的坐标
	cv::Point m_ptRBtnUp;		//鼠标右键抬起的坐标
	int m_nNewWidth;			//图像按矩形比例缩放后的宽度
	int m_nNewHeight;			//图像按矩形比例缩放后的高度
	int m_nX;					//图像按矩形比例缩放后的起点X坐标
	int m_nY;					//图像按矩形比例缩放后的起点Y坐标
	cv::Point m_ptOldTL;		//橡皮筋类按下时的原点坐标(0,0)相对于图片上的坐标点
	cv::Point m_ptOldBR;		//橡皮筋类按下时的原点坐标(0,0)相对于图片上的坐标点
	cv::Point m_ptHTracker1;	//水平橡皮筋按下时tl的实际坐标
	cv::Point m_ptHTracker2;	//水平橡皮筋按下时br的实际坐标
	cv::Point m_ptVTracker1;	//垂直橡皮筋按下时tl的实际坐标
	cv::Point m_ptVTracker2;	//垂直橡皮筋按下时br的实际坐标
	void setHTrackerPosition(cv::Point pt1, cv::Point pt2);
	void setVTrackerPosition(cv::Point pt1, cv::Point pt2);
	//--
protected:
	//画在画板上的Mat图,任何情况下都不应该直接修改m_drawing里的内容
	cv::Mat m_drawing;
	cv::Point m_rect_roi_center;

/////////////////////在这里添加额外的成员变量//////////////////////////
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags); 
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	CPoint m_ptMouseMove;		//鼠标移动时在控件中的坐标，m_bMouseMoveinfoEnable为False时无效
	bool m_bMouseMoveinfoEnable;	//是否获取当前鼠标位置，并显示（需要编辑额外控件）
};


