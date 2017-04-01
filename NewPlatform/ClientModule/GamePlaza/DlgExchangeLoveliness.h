#ifndef EXCHANGE_LOVELINESS_HEAD_FILE
#define EXCHANGE_LOVELINESS_HEAD_FILE

#pragma once


// CDlgExchangeLoveliness �Ի���

class CDlgExchangeLoveliness : public CSkinDialog, public CMissionItem
{
//	DECLARE_DYNAMIC(CDlgExchangeLoveliness)

public:
	CDlgExchangeLoveliness();   // ��׼���캯��
	virtual ~CDlgExchangeLoveliness();
public:
	CMissionManager					m_MissionManager;						// �������
	bool							m_bQueryMission;						// ��ȡ���ò���
	bool							m_bExchangeMisssion;					// �һ�����
protected:
	CSkinButton						m_btExchange;							// �һ���ť
	CSkinButton						m_btCancel;								// ȡ���һ�

	TCHAR							m_szTips[1024];							// ��ʾ			
	TCHAR							m_szExchangeInfo[1024];					// ����ת����Ϣ
// �Ի�������
//	enum { IDD = IDD_DLG_EXCHANGE_LOVELINESS };
	//�����¼�
protected:
	//��ѯ����
	VOID QueryExchangeLovelinessInfo();
	//��ʼ����
	VOID OnMissionStart();
	//��ֹ����
	VOID OnMissionConclude();

private:
	//�����¼�
	virtual bool OnEventMissionLink(INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);
protected:
	//��������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();
	// ȡ��
	virtual VOID OnCancel();
	// ���ּӶ��Ŵ���
	VOID SwitchScoreFormat(SCORE lScore, UINT uSpace, LPTSTR pszBuffer, WORD wBufferSize);
	
	//���ں���
protected:
	//�滭��Ϣ
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};

#endif
