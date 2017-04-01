#ifndef DLG_SERVICE_HEAD_FILE
#define DLG_SERVICE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgServiceItem.h"
#include "DlgServicePassword.h"
#include "DlgServiceIndividual.h"
#include "DlgServiceLockMachine.h"
#include "DlgServiceBindMobile.h"

//////////////////////////////////////////////////////////////////////////////////

//�û�����
//#define SERVICE_ACCOUNTS			1									//�޸�����
#define SERVICE_INDIVIDUAL			0									//��������
#define SERVICE_PASSWORD			2									//�޸�����
// #define SERVICE_PASSPROTECT			4									//���뱣��
// #define SERVICE_SECURITY			5									//��ȫ����
// #define SERVICE_MEMBER_PAYMENT		6									//��Ա��ֵ

//��Ա����
// #define SERVICE_GAME_RECORD			10									//��Ϸ��¼
// #define SERVICE_PAY_RECORD			11									//��ֵ��¼
// #define SERVICE_CLEAN_FLEE			12									//��������
// #define SERVICE_CLEAN_SCORE			13									//��������
// #define SERVICE_MEMBER_INFO			14									//��Ա����

#define SERVICE_LOCK_MACHINE		1									// ��������
#define SERVICE_BIND_MOBILE			3									// ���ֻ�

//////////////////////////////////////////////////////////////////////////////////

//����˵��
typedef CWHArray<CDlgServiceItem *> CDlgServiceItemArray;

//////////////////////////////////////////////////////////////////////////////////

//ѡ�����
class CDlgServiceBar : public CWnd
{
	//���Ϣ
protected:
	bool							m_bHovering;						//������־
	WORD							m_wItemDown;						//��������
	WORD							m_wItemHover;						//��������
	WORD							m_wItemActive;						//��������

	//������Ϣ
protected:
	CSize							m_SizeButton;						//��ť��С
	CRect							m_rcItemCreate;						//����λ��
	CDlgServiceItemArray			m_DlgServiceItemArray;				//��������

	//��������
public:
	//���캯��
	CDlgServiceBar();
	//��������
	virtual ~CDlgServiceBar();

	//���غ���
public:
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//���ܺ���
public:
	//��������
	bool SetItemCreateRect(CRect rcItemCreate);
	//��������
	bool InsertServiceItem(CDlgServiceItem * pDlgServiceItem);
	//��������
	bool SetActiveServiceItem(CDlgServiceItem * pDlgServiceItem);

	//�ڲ�����
private:
	//�����л�
	WORD SwitchToButtonIndex(CPoint MousePoint);

	//��Ϣ����
protected:
	//�ػ���Ϣ
	VOID OnPaint();
	//������Ϣ
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//����ƶ�
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//�����Ϣ
	VOID OnLButtonUp(UINT nFlags, CPoint Point);
	//�����Ϣ
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//����뿪
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//�����Ϣ
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//����ר��
class CDlgService : public CSkinDialog
{
	//�������
protected:
	BYTE							m_cbServiceID;						//������ʶ

protected:
	CSkinTabCtrl					m_TabControl;						//����ѡ��
	//�ؼ�����
//protected:
//	CDlgServiceBar					m_DlgServiceBar;					//���ť
protected:
	BYTE							m_cbInitPageID;						// ��ʼ����ҳǩID
	CDlgServiceItemArray			m_DlgServiceItemArray;				// ��������			

	//��ҳ����
public:
//	CDlgServiceBrowser				m_DlgServiceBrowser1;				//��ҳ����
//	CDlgServiceBrowser				m_DlgServiceBrowser2;				//��ҳ����
//	CDlgServiceBrowser				m_DlgServiceBrowser3;				//��ҳ����
//	CDlgServiceBrowser				m_DlgServiceBrowser4;				//��ҳ����
//	CDlgServiceBrowser				m_DlgServiceBrowser5;				//��ҳ����
//	CDlgServiceBrowser				m_DlgServiceBrowser6;				//��ҳ����

	//�������
public:
	CDlgServicePassword				m_DlgServicePassword;				// �޸ĵ�¼����
	CDlgServiceIndividual			m_DlgServiceIndividual;				// ��������
	CDlgServiceLockMachine			m_DlgServiceLockMachine;			// ��������
	CDlgServiceBindMobile			m_DlgServiceBindMobile;				// ���ֻ�

	//��������
public:
	//���캯��
	CDlgService(BYTE cbPageID = SERVICE_INDIVIDUAL);
	//��������
	virtual ~CDlgService();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//��������
	virtual BOOL OnInitDialog();
	//ȷ����Ϣ
	virtual VOID OnOK();
	// ���ڴ�С����
	VOID OnSize(UINT nType, INT cx, INT cy);

	//���ܺ���
public:
	//������ʶ
	VOID SetExpectationID(BYTE cbServiceID);
	//��ȡ����
	CDlgServiceItem * GetServiceItem(BYTE cbServiceID);
	// �л���ʾҳ
	VOID SwitchPage(BYTE cbPageID);

	//��Ϣӳ��
protected:
	//�б���Ϣ
	VOID OnActionServiceUser(NMHDR * pNMHDR, LRESULT * pResult);
	//�б���Ϣ
	VOID OnActionServiceMember(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTcnSelchangeTabControl(NMHDR *pNMHDR, LRESULT *pResult);
	//�滭��Ϣ
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif