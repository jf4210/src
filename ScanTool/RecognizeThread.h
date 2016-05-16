#pragma once
#include "global.h"

typedef struct _ModelInfo_
{
	pMODEL	pModel;
	cv::Mat matSrc;
	std::vector<cv::Mat> vecMatSrc;
}MODELINFO,*pMODELINFO;
class CRecognizeThread : public Poco::Runnable
{
public:
	CRecognizeThread();
	~CRecognizeThread();

	virtual void run();

	bool HandleTask(pRECOGTASK pTask);

	bool LoadModel(pMODELINFO pModelInfo);

	void PaperRecognise(pST_PaperInfo pPaper, pMODELINFO pModelInfo);

	void sharpenImage1(const cv::Mat &image, cv::Mat &result);
	inline bool Recog(int nPic, RECTINFO& rc, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo);

	bool RecogFixCP(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo);		//ʶ��ˮƽͬ��ͷ
	bool RecogHHead(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo);		//ʶ��ˮƽͬ��ͷ
	int RecogVHead(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo);			//ʶ��ֱͬ��ͷ
	int RecogABModel(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo);		//ʶ�����
	int RecogCourse(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo);		//ʶ���Ŀ
	int RecogQKCP(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo);			//ʶ��ȱ��
	int RecogGrayCP(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo);		//ʶ��Ҷ�У���
	int RecogWhiteCP(int nPic, cv::Mat& matCompPic, pST_PicInfo pPic, pMODELINFO pModelInfo);		//ʶ��հ�У���

private:
	std::map<pMODEL, pMODELINFO> _mapModel;		//���߳��Ѿ���ȡ��ģ���б���ֹͬ����ģ���ζ�ȡ�˷�ʱ��
	cv::Point m_ptFixCP;
	std::vector<std::vector<cv::Point>> m_vecContours;
};

