#ifndef PLAZA_USER_INFO_DLG_HEAD_FILE
#define PLAZA_USER_INFO_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////
typedef CMap<BYTE,BYTE,CString,CString &> CMapMemberDescribe;			//��������

//�û���Ϣ
class CUserInfoDlg : public CDialog
{
	//״̬����
protected:
	IClientUserItem*			    m_pUserData;
	CRgn							m_DialogRgn;						//��������
	CBitImage						m_ImageBack;						//����λͼ
	IGameLevelParser *				m_pIGameLevelParser;				//�ȼ��ӿ�
	CMapMemberDescribe				m_MapMemberDescribe;				//��Ա����
public:
     WORD							m_wServerType;						//��������
	//��������
public:
	//���캯��
	CUserInfoDlg();
	//��������
	virtual ~CUserInfoDlg();
	//���ܺ���
public:
	//����λͼ
	bool LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans);

	//�ڲ����� 
private:
	//��������
	bool CreateControlRgn(COLORREF crTrans);
	//���غ���
protected:
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual VOID OnOK() { return; };
	//ȡ������
	virtual VOID OnCancel() { return; };

	//���ܺ���
public:
	//������Ϣ
	VOID UpdateUserInfo(IClientUserItem* pUserData);
    //���ýӿ�
	VOID SetGameLevelParser(IGameLevelParser * pIGameLevelParser) { m_pIGameLevelParser=pIGameLevelParser; }
	//��Ϣ����
public:
	//�ػ�����
	afx_msg void OnPaint();
	//����Ҽ�������Ϣ
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//������Ϣ
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);

	afx_msg void OnKillFocus( CWnd* pNewWnd );
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CUserInfoDlg)
};

//////////////////////////////////////////////////////////////////////////

#endif