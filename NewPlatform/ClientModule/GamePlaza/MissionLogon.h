#ifndef MISSION_LOGON_HEAD_FILE
#define MISSION_LOGON_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgLogon.h"
#include "DlgStatus.h"
#include "DlgRegister.h"
#include "DlgProfile.h"

//////////////////////////////////////////////////////////////////////////////////

//��¼����
class CMissionLogon : public CDlgStatus, public CMissionItem, public IStatusViewSink
{
	//��Ԫ����
	friend class CDlgLogon;
	friend class CDlgRegister;
	friend class CDlgProfile;

	//��������
protected:
	bool							m_bRegister;						//ע���־
	bool							m_bProfile;							//
	bool							m_bRemPassword;						//��ס����
	bool                            m_bLowVerValidate;                  //�Ͱ汾Ч��

	//�ؼ�ָ��
protected:
	CDlgLogon *						m_pDlgLogon;						//��¼����
	CDlgRegister *					m_pDlgRegister;						//ע�ᴰ��
	CDlgProfile *					m_pDlgProfile;						//���ϴ���

	//��̬����
protected:
	static CMissionLogon *			m_pMissionLogon;					//����ָ��

	//��������
public:
	//���캯��
	CMissionLogon();
	//��������
	virtual ~CMissionLogon();

	//״̬�ӿ�
public:
	//ȡ������
	virtual VOID OnStatusCancel();

	//���غ���
public:
	//�����¼�
	virtual bool OnEventMissionLink(INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);

	//���ܺ���
public:
	//��ʾ��¼
	VOID ShowLogon();
	//��ʾע��
	VOID ShowRegister();
	//��ʾ������Ϣ
	VOID ShowProfile();
	
	//������Ϣ
	bool SendLogonPacket(BYTE cbValidateFlags);

	//��������
protected:
	//�����û�
	VOID UpdateUserInfo();
	//ִ�е�¼
	bool PerformLogonMission(bool bRemPassword);

	//�����¼�
protected:
	//��¼�ɹ�
	bool OnSocketSubLogonSuccess(VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnSocketSubLogonFailure(VOID * pData, WORD wDataSize);
	//��¼���
	bool OnSocketSubLogonFinish(VOID * pData, WORD wDataSize);
	//������ʾ
	bool OnSocketSubUpdateNotify(VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnSocketSubLogonValidateMBCard(VOID * pData, WORD wDataSize);
	//�������ϳɹ� add by chenj
	bool OnSocketSubProfileSuccess(VOID * pData, WORD wDataSize);
	//��������ʧ�� add by chenj
	bool OnSocketSubProfileFailure(VOID * pData, WORD wDataSize);
	//����ǳ�
	bool OnSocketSubCheckNick(VOID * pData, WORD wDataSize);

	//��̬����
public:
	//��ȡ����
	static CMissionLogon * GetInstance() { return m_pMissionLogon; }

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif