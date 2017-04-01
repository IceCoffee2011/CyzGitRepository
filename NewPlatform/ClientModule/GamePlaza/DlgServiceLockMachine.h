#pragma once


#include "DlgServiceItem.h"

class CDlgServiceLockMachine : public CDlgServiceItem, public CMissionItem
{

protected:
	CPasswordControl			m_edInsurePassword;			// ��������(�󶨻���ʱ��Ҫ����)
	CSkinButton					m_btLockMachine;			// ��������
	CSkinButton					m_btUnLockMachine;			// ��������

protected:
	TCHAR						m_szPassword[LEN_PASSWORD];	//�û�����
	CMissionManager				m_MissionManager;			//�������
	BYTE						m_cbMachine;				//�󶨱�־(�뵱ǰʵ��״̬�෴)

public:
	CDlgServiceLockMachine();
	~CDlgServiceLockMachine();

	//���غ���
public:
	//��ȡ����
	virtual LPCTSTR GetServiceItemName() { return TEXT("�󶨻���"); }

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��������
	virtual BOOL OnInitDialog();
	//�����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//�����¼�
private:
	//�����¼�
	virtual bool OnEventMissionLink(INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);

	//�¼�����
protected:
	//��ʼ����
	VOID OnMissionStart();
	//��ֹ����
	VOID OnMissionConclude();

	//��Ϣӳ��
protected:
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
};
