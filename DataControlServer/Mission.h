#pragma once

class CNetUser;
class CMission
{
public:
	CMission();
	~CMission(void);

	//��mission������user
	CNetUser*	m_pUser;
	//���mission��������
	char*		m_pMissionData;
	//����������
	int			m_nDataSize;
	// �ܹ���������
	int			m_nTotalSize;
	bool SetData(char* pData, int nDataSize);
	void ReleaseDataBuf(void);
	int CreateDataBuf(int nSize);
	void SetNetUset(CNetUser* pUser);
};
