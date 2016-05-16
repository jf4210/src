#include "stdafx.h"
#include "RecognizeThread.h"
#include "ScanTool.h"


using namespace cv;
CRecognizeThread::CRecognizeThread()
{
}

CRecognizeThread::~CRecognizeThread()
{
	if (m_vecContours.size())
		m_vecContours.clear();
	g_pLogger->information("RecognizeThread exit.");
	TRACE("RecognizeThread exit1.\n");
}

void CRecognizeThread::run()
{
	g_pLogger->information("RecognizeThread start...");
	TRACE("RecognizeThread start...\n");
	while (!g_nExitFlag)
	{
		pRECOGTASK pTask = NULL;
		g_fmRecog.lock();
		RECOGTASKLIST::iterator it = g_lRecogTask.begin();
		for (; it != g_lRecogTask.end();)
		{
			pTask = *it;
			it = g_lRecogTask.erase(it);
			break;
		}
		g_fmRecog.unlock();
		if (NULL == pTask)
		{
			Poco::Thread::sleep(100);
			continue;
		}

		HandleTask(pTask);

		delete pTask;
		pTask = NULL;
	}

	std::map<pMODEL, pMODELINFO>::iterator it = _mapModel.begin();
	for (; it != _mapModel.end();)
	{
		pMODELINFO pModelInfo = NULL;
		pModelInfo = it->second;
		if (pModelInfo)
		{
			delete pModelInfo;
			pModelInfo = NULL;
		}
		it = _mapModel.erase(it);
	}

	TRACE("RecognizeThread exit 0\n");
}

bool CRecognizeThread::HandleTask(pRECOGTASK pTask)
{
	//进行任务识别
	pMODELINFO pModelInfo = NULL;
	std::map<pMODEL, pMODELINFO>::iterator it = _mapModel.find(pTask->pPaper->pModel);		//pTask->pModel
	if (it == _mapModel.end())
	{
		pModelInfo = new MODELINFO;
		_mapModel.insert(std::map<pMODEL, pMODELINFO>::value_type(pTask->pPaper->pModel, pModelInfo));	//pTask->pModel

		pModelInfo->pModel = pTask->pPaper->pModel;		//pTask->pModel;
		LoadModel(pModelInfo);
	}
	else
		pModelInfo = it->second;

	PaperRecognise(pTask->pPaper, pModelInfo);

	return true;
}
void CRecognizeThread::sharpenImage1(const cv::Mat &image, cv::Mat &result)
{
	//创建并初始化滤波模板
	cv::Mat kernel(3, 3, CV_32F, cv::Scalar(0));
	kernel.at<float>(1, 1) = 5;
	kernel.at<float>(0, 1) = -1.0;
	kernel.at<float>(1, 0) = -1.0;
	kernel.at<float>(1, 2) = -1.0;
	kernel.at<float>(2, 1) = -1.0;

	result.create(image.size(), image.type());

	//对图像进行滤波
	cv::filter2D(image, result, image.depth(), kernel);
}

bool CRecognizeThread::LoadModel(pMODELINFO pModelInfo)
{
	USES_CONVERSION;
	for (int i = 0; i < pModelInfo->pModel->nPicNum; i++)
	{
		std::string strModelPicPath = g_strModelSavePath + "\\" + T2A(pModelInfo->pModel->strModelName + _T("\\") + pModelInfo->pModel->vecPaperModel[i].strModelPicName);

		cv::Mat matSrc = cv::imread(strModelPicPath);
		pModelInfo->vecMatSrc.push_back(matSrc);
	}
	return true;
}

void CRecognizeThread::PaperRecognise(pST_PaperInfo pPaper, pMODELINFO pModelInfo)
{
	PIC_LIST::iterator itPic = pPaper->lPic.begin();
	for (int i = 0; itPic != pPaper->lPic.end(); itPic++, i++)
	{
		clock_t start_pic, end_pic;
		start_pic = clock();
#if 1
		int nCount = pModelInfo->pModel->vecPaperModel[i].lH_Head.size() + pModelInfo->pModel->vecPaperModel[i].lV_Head.size() + pModelInfo->pModel->vecPaperModel[i].lABModel.size()
			+ pModelInfo->pModel->vecPaperModel[i].lCourse.size() + pModelInfo->pModel->vecPaperModel[i].lQK_CP.size() + pModelInfo->pModel->vecPaperModel[i].lGray.size()
			+ pModelInfo->pModel->vecPaperModel[i].lWhite.size();
		if (!nCount)	//如果当前模板试卷没有校验点就不需要进行试卷打开操作，直接下一张试卷
			continue;
#else
		if (!pModelInfo->pModel->vecPaperModel[i].lCheckPoint.size())	//如果当前模板试卷没有校验点就不需要进行试卷打开操作，直接下一张试卷
			continue;
#endif
		std::string strPicFileName = (*itPic)->strPicName;
		Mat matCompPic = imread((*itPic)->strPicPath);			//imread((*itPic)->strPicPath);

		clock_t end1_pic = clock();

		bool bFind = false;
		int nPic = i;

		m_ptFixCP = Point(0, 0);
		bool bResult = RecogFixCP(nPic, matCompPic, *itPic, pModelInfo);
		if(bResult) bResult = RecogHHead(nPic, matCompPic, *itPic, pModelInfo);
		if(bResult) bResult = RecogVHead(nPic, matCompPic, *itPic, pModelInfo);
		if(bResult) bResult = RecogABModel(nPic, matCompPic, *itPic, pModelInfo);
		if(bResult) bResult = RecogCourse(nPic, matCompPic, *itPic, pModelInfo);
		if(bResult) bResult = RecogQKCP(nPic, matCompPic, *itPic, pModelInfo);
		if(bResult) bResult = RecogGrayCP(nPic, matCompPic, *itPic, pModelInfo);
		if(bResult) bResult = RecogWhiteCP(nPic, matCompPic, *itPic, pModelInfo);
		if(!bResult) bFind = true;
		if (bFind)
		{
			pPaper->bIssuePaper = true;				//标识此学生试卷属于问题试卷
			pPAPERSINFO pPapers = (pPAPERSINFO)pPaper->pPapers;

			pPapers->fmlPaper.lock();
			PAPER_LIST::iterator itPaper = pPapers->lPaper.begin();
			for (; itPaper != pPapers->lPaper.end();)
			{
				if (*itPaper == pPaper)
				{
					itPaper = pPapers->lPaper.erase(itPaper);
					break;
				}
				else
					itPaper++;
			}
			pPapers->fmlPaper.unlock();

			pPapers->fmlIssue.lock();
			pPapers->lIssue.push_back(pPaper);
			pPapers->nRecogErrCount++;
			pPapers->fmlIssue.unlock();

			((CDialog*)pPaper->pSrcDlg)->PostMessageW(MSG_ERR_RECOG, (WPARAM)pPaper, (LPARAM)pPapers);
			break;									//找到这张试卷有问题点，不进行下一张试卷的检测
		}	

		end_pic = clock();
		TRACE("试卷 %s 打开时间: %d, 识别总时间: %d\n", strPicFileName.c_str(), end1_pic - start_pic, end_pic - start_pic);
		char szLog[MAX_PATH] = { 0 };
		sprintf_s(szLog, "试卷 %s 打开时间: %d, 识别总时间: %d\n", strPicFileName.c_str(), end1_pic - start_pic, end_pic - start_pic);
		g_pLogger->information(szLog);
	}
}

inline bool CRecognizeThread::Recog(int nPic, RECTINFO& rc, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo)
{
	Mat matSrcRoi, matCompRoi;
	Rect rt = rc.rt;
//	GetPosition(pPic->lFix, pModelInfo->pModel->vecPaperModel[nPic].lFix, rt);
	bool bFindRect = false;
	try
	{
// 		RECTINFO rcModelFix = pModelInfo->pModel->vecPaperModel[nPic].lFix.front();
// 		cv::Point ptModelFix;
// 		ptModelFix.x = rcModelFix.rt.x + rcModelFix.rt.width / 2 + 0.5;
// 		ptModelFix.y = rcModelFix.rt.y + rcModelFix.rt.height / 2 + 0.5;
// 		Rect rtModel = rc.rt;
// 		rtModel.x = rtModel.x + ptModelFix.x;
// 		rtModel.y = rtModel.y + ptModelFix.y;
// 		matSrcRoi = pModelInfo->vecMatSrc[nPic](rtModel);	//rt
 		matCompRoi = matCompPic(rt);

		Mat imag_src, img_comp;
//		cv::cvtColor(matSrcRoi, matSrcRoi, CV_BGR2GRAY);
		cv::cvtColor(matCompRoi, matCompRoi, CV_BGR2GRAY);

		const int channels[1] = { 0 };
		const float* ranges[1];
		const int histSize[1] = { 1 };
		float hranges[2];
		if (rc.eCPType != WHITE_CP)
		{
			hranges[0] = 0;
			hranges[1] = rc.nThresholdValue;
			ranges[0] = hranges;
		}
		else
		{
			hranges[0] = rc.nThresholdValue;
			hranges[1] = 255;
			ranges[0] = hranges;
		}
		MatND src_hist, comp_hist;
//		cv::calcHist(&matSrcRoi, 1, channels, Mat(), src_hist, 1, histSize, ranges, false);
		cv::calcHist(&matCompRoi, 1, channels, Mat(), comp_hist, 1, histSize, ranges, false);

// 		double maxValSrc = 0;
// 		double minValSrc = 0;
		double maxValComp = 0;
		double minValComp = 0;

		//找到直方图中的最大值和最小值
//		cv::minMaxLoc(src_hist, &minValSrc, &maxValSrc, 0, 0);
		cv::minMaxLoc(comp_hist, &minValComp, &maxValComp, 0, 0);
		int nSize = comp_hist.rows;

		// 设置最大峰值为图像高度的90%
		int hpt = static_cast<int>(0.9 * 256);		//histSize
		//		TRACE("maxValSrc = %f, maxValComp = %f\n", maxValSrc, maxValComp);
		for (int h = 0; h < nSize; h++)	//histSize
		{
// 			float binValSrc = src_hist.at<float>(h);
// 			int intensitySrc = static_cast<int>(binValSrc*hpt / maxValSrc);
//			TRACE("h = %d. binValSrc = %f. intensitySrc = %d\n", h, binValSrc, intensitySrc);

			float binValComp = comp_hist.at<float>(h);
			int intensityComp = static_cast<int>(binValComp*hpt / maxValComp);
//			TRACE("h = %d. binValComp = %f. intensityComp = %d\n", h, binValComp, intensityComp);

//			rc.fRealValuePercent = binValComp / binValSrc;
			rc.fRealValue = binValComp;
			rc.fRealValuePercent = binValComp / rc.fStandardValue;
			if (rc.fRealValuePercent < rc.fStandardValuePercent)		//g_fSamePercent
			{
				bFindRect = true;
				TRACE("校验失败, 灰度百分比: %f, 问题点: (%d,%d,%d,%d)\n", rc.fRealValuePercent * 100, rc.rt.x, rc.rt.y, rc.rt.width, rc.rt.height);
				char szLog[MAX_PATH] = { 0 };
				sprintf_s(szLog, "校验失败, 灰度百分比: %f, 问题点: (%d,%d,%d,%d)\n", rc.fRealValuePercent * 100, rc.rt.x, rc.rt.y, rc.rt.width, rc.rt.height);
				g_pLogger->information(szLog);
			}
			break;
		}
	}
	catch (cv::Exception &exc)
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "CRecognizeThread::Recog error. detail: %s\n\n", exc.msg);
		TRACE(szLog);
	}
	
	return bFindRect;
}

bool CRecognizeThread::RecogFixCP(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo)
{
	bool bResult = true;
	RECTLIST::iterator itCP = pModelInfo->pModel->vecPaperModel[nPic].lSelROI.begin();
	for (; itCP != pModelInfo->pModel->vecPaperModel[nPic].lSelROI.end(); itCP++)
	{
		RECTINFO rc = *itCP;

		Mat matCompRoi;
		matCompRoi = matCompPic(rc.rt);

		cvtColor(matCompRoi, matCompRoi, CV_BGR2GRAY);

		GaussianBlur(matCompRoi, matCompRoi, cv::Size(5, 5), 0, 0);
		sharpenImage1(matCompRoi, matCompRoi);

		threshold(matCompRoi, matCompRoi, 60, 255, THRESH_BINARY);
		cv::Canny(matCompRoi, matCompRoi, 0, 90, 5);
		Mat element = getStructuringElement(MORPH_RECT, Size(6, 6));	//Size(6, 6)	普通空白框可识别
		dilate(matCompRoi, matCompRoi, element);

#if 1
//		std::vector<std::vector<cv::Point> > vecContours;		//轮廓信息存储
//		m_vecContours.clear();
//		std::vector<cv::Mat> vecContours;
//		cv::findContours(matCompRoi.clone(), vecContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);	//hsvRe.clone()		//CV_RETR_EXTERNAL	//CV_CHAIN_APPROX_SIMPLE
		
// 		vector<Mat> vecContours;
// 		Mat hierarchy;
// 		findContours(matCompRoi, vecContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
// 
// 		std::vector<Rect>RectCompList;
// 		for (int i = 0; i < vecContours.size(); i++)
// 		{
// 			cv::Mat matTmp = cv::Mat(vecContours[i]);
// 			Rect rm = cv::boundingRect(matTmp);
// 
// 			// 			Rect rm = cv::boundingRect(vecContours[i]);
// 			// 			vecContours[i].release();
// 
// 			RectCompList.push_back(rm);
// 			matTmp.release();
// 		}



		IplImage ipl_img(matCompRoi);

		//the parm. for cvFindContours  
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* contour = 0;

		//提取轮廓  
		cvFindContours(&ipl_img, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		std::vector<Rect>RectCompList;
		for (int iteratorIdx = 0; contour != 0; contour = contour->h_next, iteratorIdx++/*更新迭代索引*/)
		{
			CvRect aRect = cvBoundingRect(contour, 0);
			Rect rm = aRect;
			RectCompList.push_back(rm);
		}
#else
//		std::vector<std::vector<cv::Point> > vecContours;		//轮廓信息存储
		m_vecContours.clear();
//		std::vector<cv::Mat> vecContours;
		cv::findContours(matCompRoi.clone(), m_vecContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);	//hsvRe.clone()		//CV_RETR_EXTERNAL	//CV_CHAIN_APPROX_SIMPLE

//		bool bResult = false;
		std::vector<Rect>RectCompList;
		for (int i = 0; i < m_vecContours.size(); i++)
		{
			Rect rm = cv::boundingRect(cv::Mat(m_vecContours[i]));
//			Rect rm = cv::boundingRect(cv::Mat(vecContours[i]));

// 			std::vector<cv::Point> v;
// 			m_vecContours[i].swap(v);
// 			if (rm.width < 10 || rm.height < 7 || rm.width > 70 || rm.height > 50 || rm.area() < 70)
// 			{
// 				TRACE("抛弃不符合条件的矩形块,(%d, %d, %d, %d)\n", rm.x, rm.y, rm.width, rm.height);
// 				continue;
// 			}
			RectCompList.push_back(rm);
			
//			m_vecContours[i].clear();
//			bResult = true;
		}
#endif
		bool bFindRect = false;
		if(RectCompList.size() == 0)
			bFindRect = true;
		else
		{
			std::sort(RectCompList.begin(), RectCompList.end(), SortByArea);
			Rect& rtFix = RectCompList[0];
			m_ptFixCP.x = rtFix.x + rtFix.width / 2 + 0.5 + rc.rt.x;
			m_ptFixCP.y = rtFix.y + rtFix.height / 2 + 0.5 + rc.rt.y;

			rtFix.x = rtFix.x + rc.rt.x;
			rtFix.y = rtFix.y + rc.rt.y;

			RECTINFO rcFixInfo;
			rcFixInfo.rt = rtFix;
			pPic->lFix.push_back(rcFixInfo);
			TRACE("定点矩形: (%d,%d,%d,%d)\n", rtFix.x, rtFix.y, rtFix.width, rtFix.height);
// 			pPic->ptFix = m_ptFixCP;
// 			pPic->rtFix.x = rcFix.x + rc.rt.x;
// 			pPic->rtFix.y = rcFix.y + rc.rt.y;
// 			pPic->rtFix.width = rcFix.width;
// 			pPic->rtFix.height = rcFix.height;
		}
		if (bFindRect)
		{
			bResult = false;						//找到问题点
			pPic->bFindIssue = true;
			pPic->lIssueRect.push_back(rc);
		}
	}
	if (!bResult)
	{
		char szLog[MAX_PATH] = { 0 };
		sprintf_s(szLog, "识别定点失败, 图片名: %s\n", pPic->strPicName.c_str());
		g_pLogger->information(szLog);
		TRACE(szLog);
	}
	return bResult;
}

bool CRecognizeThread::RecogHHead(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo)
{
	bool bResult = true;
	RECTLIST::iterator itCP = pModelInfo->pModel->vecPaperModel[nPic].lH_Head.begin();
	for (; itCP != pModelInfo->pModel->vecPaperModel[nPic].lH_Head.end(); itCP++)
	{
		RECTINFO rc = *itCP;

		GetPosition(pPic->lFix, pModelInfo->pModel->vecPaperModel[nPic].lFix, rc.rt);
		bool bFindRect = Recog(nPic, rc, matCompPic, pPic, pModelInfo);

		if (bFindRect)
		{
			bResult = false;						//找到问题点
			pPic->bFindIssue = true;
			pPic->lIssueRect.push_back(rc);
		}
		else
			pPic->lNormalRect.push_back(rc);
	}
	if (!bResult)
	{
		char szLog[MAX_PATH] = { 0 };
		sprintf_s(szLog, "识别水平同步头失败, 图片名: %s\n", pPic->strPicName.c_str());
		g_pLogger->information(szLog);
		TRACE(szLog);
	}
	return bResult;
}

int CRecognizeThread::RecogVHead(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo)
{
	bool bResult = true;
	RECTLIST::iterator itCP = pModelInfo->pModel->vecPaperModel[nPic].lV_Head.begin();
	for (; itCP != pModelInfo->pModel->vecPaperModel[nPic].lV_Head.end(); itCP++)
	{
		RECTINFO rc = *itCP;

		GetPosition(pPic->lFix, pModelInfo->pModel->vecPaperModel[nPic].lFix, rc.rt);
		bool bFindRect = Recog(nPic, rc, matCompPic, pPic, pModelInfo);
		if (bFindRect)
		{
			bResult = false;						//找到问题点
			pPic->bFindIssue = true;
			pPic->lIssueRect.push_back(rc);
		}
		else
			pPic->lNormalRect.push_back(rc);
	}
	if (!bResult)
	{
		char szLog[MAX_PATH] = { 0 };
		sprintf_s(szLog, "识别垂直同步头失败, 图片名: %s\n", pPic->strPicName.c_str());
		g_pLogger->information(szLog);
		TRACE(szLog);
	}
	return bResult;
}

int CRecognizeThread::RecogABModel(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo)
{
	bool bResult = true;
	RECTLIST::iterator itCP = pModelInfo->pModel->vecPaperModel[nPic].lABModel.begin();
	for (; itCP != pModelInfo->pModel->vecPaperModel[nPic].lABModel.end(); itCP++)
	{
		RECTINFO rc = *itCP;

		GetPosition(pPic->lFix, pModelInfo->pModel->vecPaperModel[nPic].lFix, rc.rt);
		bool bFindRect = Recog(nPic, rc, matCompPic, pPic, pModelInfo);
		if (bFindRect)
		{
			bResult = false;						//找到问题点
			pPic->bFindIssue = true;
			pPic->lIssueRect.push_back(rc);
		}
		else
			pPic->lNormalRect.push_back(rc);
	}
	if (!bResult)
	{
		char szLog[MAX_PATH] = { 0 };
		sprintf_s(szLog, "识别AB卷型失败, 图片名: %s\n", pPic->strPicName.c_str());
		g_pLogger->information(szLog);
		TRACE(szLog);
	}
	return bResult;
}

int CRecognizeThread::RecogCourse(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo)
{
	bool bResult = true;
	RECTLIST::iterator itCP = pModelInfo->pModel->vecPaperModel[nPic].lCourse.begin();
	for (; itCP != pModelInfo->pModel->vecPaperModel[nPic].lCourse.end(); itCP++)
	{
		RECTINFO rc = *itCP;

		GetPosition(pPic->lFix, pModelInfo->pModel->vecPaperModel[nPic].lFix, rc.rt);
		bool bFindRect = Recog(nPic, rc, matCompPic, pPic, pModelInfo);

		if (bFindRect)
		{
			bResult = false;						//找到问题点
			pPic->bFindIssue = true;
			pPic->lIssueRect.push_back(rc);
		}
		else
			pPic->lNormalRect.push_back(rc);
	}
	if (!bResult)
	{
		char szLog[MAX_PATH] = { 0 };
		sprintf_s(szLog, "识别科目失败, 图片名: %s\n", pPic->strPicName.c_str());
		g_pLogger->information(szLog);
		TRACE(szLog);
	}
	return bResult;
}

int CRecognizeThread::RecogQKCP(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo)
{
	bool bResult = true;
	RECTLIST::iterator itCP = pModelInfo->pModel->vecPaperModel[nPic].lQK_CP.begin();
	for (; itCP != pModelInfo->pModel->vecPaperModel[nPic].lQK_CP.end(); itCP++)
	{
		RECTINFO rc = *itCP;

		GetPosition(pPic->lFix, pModelInfo->pModel->vecPaperModel[nPic].lFix, rc.rt);
		bool bFindRect = Recog(nPic, rc, matCompPic, pPic, pModelInfo);
		if (bFindRect)
		{
			bResult = false;						//找到问题点
			pPic->bFindIssue = true;
			pPic->lIssueRect.push_back(rc);
		}
		else
			pPic->lNormalRect.push_back(rc);
	}
	if (!bResult)
	{
		char szLog[MAX_PATH] = { 0 };
		sprintf_s(szLog, "识别缺考失败, 图片名: %s\n", pPic->strPicName.c_str());
		g_pLogger->information(szLog);
		TRACE(szLog);
	}
	return bResult;
}

int CRecognizeThread::RecogGrayCP(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo)
{
	bool bResult = true;
	RECTLIST::iterator itCP = pModelInfo->pModel->vecPaperModel[nPic].lGray.begin();
	for (; itCP != pModelInfo->pModel->vecPaperModel[nPic].lGray.end(); itCP++)
	{
		RECTINFO rc = *itCP;

		GetPosition(pPic->lFix, pModelInfo->pModel->vecPaperModel[nPic].lFix, rc.rt);
		bool bFindRect = Recog(nPic, rc, matCompPic, pPic, pModelInfo);
		if (bFindRect)
		{
			bResult = false;						//找到问题点
			pPic->bFindIssue = true;
			pPic->lIssueRect.push_back(rc);
		}
		else
			pPic->lNormalRect.push_back(rc);
	}
	if (!bResult)
	{
		char szLog[MAX_PATH] = { 0 };
		sprintf_s(szLog, "识别灰度校验点失败, 图片名: %s\n", pPic->strPicName.c_str());
		g_pLogger->information(szLog);
		TRACE(szLog);
	}
	return bResult;
}

int CRecognizeThread::RecogWhiteCP(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo)
{
	bool bResult = true;
	RECTLIST::iterator itCP = pModelInfo->pModel->vecPaperModel[nPic].lWhite.begin();
	for (; itCP != pModelInfo->pModel->vecPaperModel[nPic].lWhite.end(); itCP++)
	{
		RECTINFO rc = *itCP;

		GetPosition(pPic->lFix, pModelInfo->pModel->vecPaperModel[nPic].lFix, rc.rt);
		bool bFindRect = Recog(nPic, rc, matCompPic, pPic, pModelInfo);
		if (bFindRect)
		{
			bResult = false;						//找到问题点
			pPic->bFindIssue = true;
			pPic->lIssueRect.push_back(rc);
		}
		else
			pPic->lNormalRect.push_back(rc);
	}
	if (!bResult)
	{
		char szLog[MAX_PATH] = { 0 };
		sprintf_s(szLog, "识别空白校验点失败, 图片名: %s\n", pPic->strPicName.c_str());
		g_pLogger->information(szLog);
		TRACE(szLog);
	}
	return bResult;
}
