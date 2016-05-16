// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ScanTool.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
//#include "global.h"
#include "Net_Cmd_Protocol.h"

// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(CString strIP, int nPort, CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
	, m_strUserName(_T(""))
	, m_strPwd(_T(""))
	, m_strServerIP(strIP)
	, m_nServerPort(nPort)
	, m_nRecvLen(0)
	, m_nWantLen(0)
{
	ZeroMemory(m_szRecvBuff, 2048);
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_UserName, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_Pwd, m_strPwd);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_Login, &CLoginDlg::OnBnClickedBtnLogin)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序


BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateData(FALSE);

	return TRUE;
}


void CLoginDlg::OnBnClickedBtnLogin()
{
	UpdateData(TRUE);
	USES_CONVERSION;

	Poco::Net::SocketAddress sa(T2A(m_strServerIP), m_nServerPort);
	try
	{
		m_ss.connect(sa);

		ST_CMD_HEADER stHead;
		stHead.usCmd = USER_LOGIN;
		stHead.uPackSize = sizeof(ST_LOGIN_INFO);
		ST_LOGIN_INFO stLogin;
		ZeroMemory(&stLogin, sizeof(ST_LOGIN_INFO));
		strcpy(stLogin.szUserNo, T2A(m_strUserName));
		strcpy(stLogin.szPWD, T2A(m_strPwd));

		char szSendBuf[4096] = { 0 };
		memcpy(szSendBuf, (char*)&stHead, HEAD_SIZE);
		memcpy(szSendBuf + HEAD_SIZE, (char*)&stLogin, sizeof(ST_LOGIN_INFO));
		m_ss.sendBytes(szSendBuf, HEAD_SIZE + stHead.uPackSize);

		if (RecvData())
		{
			OnOK();
		}
		else
		{
			AfxMessageBox(_T("登录失败"));
			OnCancel();
		}

	}
	catch (Poco::Exception& exc)
	{
		std::string strLog = "连接服务器失败，Detail: " + exc.displayText();
		g_pLogger->information(strLog);
		TRACE(strLog.c_str());
		AfxMessageBox(_T("登录失败"));
		OnCancel();
	}
}

int CLoginDlg::RecvData()
{
	int nBaseLen = HEAD_SIZE;
	while (!g_nExitFlag)
	{
		int nLen;
		if (m_nRecvLen < nBaseLen)
		{
			m_nWantLen = nBaseLen - m_nRecvLen;
			nLen = m_ss.receiveBytes(m_szRecvBuff + m_nRecvLen, m_nWantLen);
			if (nLen > 0)
			{
				m_nRecvLen += nLen;
				if (m_nRecvLen == nBaseLen)
				{
					ST_CMD_HEADER* pstHead = (ST_CMD_HEADER*)m_szRecvBuff;
					nBaseLen += pstHead->uPackSize;
				}
			}
			else if (nLen == 0)
			{
				TRACE("the peer has closed.\n");
				return 0;
			}
			else
				Poco::Thread::sleep(1);
		}
		else
			break;
	}
	
	int nResult = 0;
	ST_CMD_HEADER* pstHead = (ST_CMD_HEADER*)m_szRecvBuff;
	if (pstHead->usCmd == USER_RESPONSE_LOGIN)
	{
		switch (pstHead->usResult)
		{
		case RESULT_SUCCESS:
			nResult = 1;
			break;
		}
	}
	return nResult;
}
