#ifndef SERVICE_INDIVIDUAL_HEAD_FILE
#define SERVICE_INDIVIDUAL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgServiceItem.h"
#include "DlgExchangeLoveliness.h"
#include "CollocateRoom.h"

//////////////////////////////////////////////////////////////////////////////////

//�޸�����
class CDlgServiceIndividual : public CDlgServiceItem, public CMissionItem
{
	//��Ԫ����
	friend class CDlgService;

	//��������
protected:
	bool							m_bQueryMission;						//��ѯ����
	bool							m_bModifyMission;						//�޸�����
	CMissionManager					m_MissionManager;						//�������

	//�ʺ�����
protected:
	BYTE							m_cbGender;								//�û��Ա�
	TCHAR							m_szNickName[LEN_NICKNAME];				//��Ϸ�ǳ�
	TCHAR							m_szPassword[LEN_PASSWORD];				//�û�����
	TCHAR							m_szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��

	//��ϸ����
protected:
	TCHAR							m_szQQ[LEN_QQ];							//Q Q ����
	TCHAR							m_szEMail[LEN_EMAIL];					//�����ʼ�
	TCHAR							m_szUserNote[LEN_USER_NOTE];			//�û�˵��
	TCHAR							m_szSeatPhone[LEN_SEAT_PHONE];			//�̶��绰
	TCHAR							m_szMobilePhone[LEN_MOBILE_PHONE];		//�ƶ��绰
	TCHAR							m_szCompellation[LEN_COMPELLATION];		//��ʵ����
	TCHAR							m_szDwellingPlace[LEN_DWELLING_PLACE];	//��ϵ��ַ

	//�ؼ�����
protected:
	CSkinEditEx						m_edAccounts;							//�û��ʺ�
	CSkinEditEx						m_edNickName;							//�û��ǳ�
//	CSkinEdit						m_edPassword;							//�޸�����
//	CSkinEdit						m_edUnderWrite;							//����ǩ��

	//�ؼ�����
protected:
	CSkinEditEx						m_edQQ;									//Q Q ����
//	CSkinEdit						m_edEMail;								//�����ʼ�
//	CSkinEdit						m_edUserNote;							//�û�˵��
//	CSkinEdit						m_edSeatPhone;							//�̶��绰
	CSkinEditEx						m_edMobilePhone;						//�ƶ��绰
	CSkinEditEx						m_edPassPortID;							// ���֤
//	CSkinEdit						m_edCompellation;						//��ʵ����
//	CSkinEdit						m_edDwellingPlace;						//��ϵ��ַ
	CSkinEditEx						m_edGameID;								// �û�ID
	CSkinEditEx						m_edGender;								// �û��Ա�
	CSkinEditEx						m_edMember;								// ��Ա����
	CSkinEditEx						m_edUserMedal;							// �û�����
	CSkinEditEx						m_edUnderWrite;							// ����ǩ���༭��

	CToolTipCtrl					m_ToolTipCtrl;							//��ʾ�ؼ�

protected:
//	CSkinHyperLink					m_hlGameID;								// �û�ID
	CBrush							m_brBrush;								//������ˢ

	//�ؼ�����
protected:
	CSkinButton						m_btOk;									//�޸İ�ť
//	CSkinButton						m_btSelectFace;							//ѡ��ť

	CSkinButton						m_btUnderWrite;							// �༭����ǩ��
	CSkinButton						m_btExchangeLoveLiness;					// �һ�����

	//�������
protected:
//	CFaceItemView					m_FaceItemView;							//ͷ����ͼ
	CFaceSelectControlHelper		m_FaceSelectControl;					//ͷ��ѡ��

	//��������
public:
	//���캯��
	CDlgServiceIndividual();
	//��������
	virtual ~CDlgServiceIndividual();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//��������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK();
	//�ؼ���ɫ
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	//���غ���
public:
	//��ȡ����
	virtual LPCTSTR GetServiceItemName() { return TEXT("�û�����"); }

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

	//���غ���
protected:
	//�ʺ���Ϣ
	VOID LoadAccountsInfo();
	//��ѯ����
	VOID QueryIndividualInfo();
	//��������
	VOID UpdateIndividualInfo();

	//��Ϣӳ��
protected:
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//ͷ��ѡ��
	VOID OnBnClickedSelectFace();
	// �༭����ǩ��
	VOID OnBnClickedUnderWrite();
	// �ύ����ǩ��
	VOID OnBnClickedSubmit();
	// �һ�����
	VOID OnBnClickedExchangeLoveliness();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif