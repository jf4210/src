#pragma once

class CNetUser;
class CMission
{
public:
	CMission();
	~CMission(void);

	//该mission所属的user
	CNetUser*	m_pUser;
	//存放mission的数据区
	char*		m_pMissionData;
	//数据区长度
	int			m_nDataSize;
	// 总共的数据量
	int			m_nTotalSize;
	bool SetData(char* pData, int nDataSize);
	void ReleaseDataBuf(void);
	int CreateDataBuf(int nSize);
	void SetNetUset(CNetUser* pUser);
};
