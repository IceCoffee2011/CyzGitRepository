#ifndef LAST_GAME_SERVER_CTRL_HEAD_FILE
#define LAST_GAME_SERVER_CTRL_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServerListData.h"

#define  WM_LASTGAMESERVERCTRL_MOVE	 WM_USER+777

// �б�֪ͨ�����Ϸ�ؼ��ӿ�
interface ILastGameServerSink
{
	//״̬֪ͨ
public:
	// �����������Ϸ
	virtual VOID OnAddLastPlayGame(WORD wServerID, CGameServerItem *pGameServerItem)=NULL;
	// ɾ����������Ϸ
	virtual VOID OnRemoveLastPlayGame(WORD wServerID)=NULL;
	// ������������Ϸ����
	virtual VOID OnUpdateLastPlayGame(WORD wServerID, CGameServerItem *pGameServerItem)=NULL;
};

class CLastGameServerCtrl : public CWnd, public ILastGameServerSink
{
public:
	CLastGameServerCtrl(void);
	virtual ~CLastGameServerCtrl(void);
protected:
	CBitImage						m_ImageTypeBkgnd;					//���͵ĵ�ͼ
	CPngImage						m_ImageArrow1;						//��ͷͼ�꣨������ͣ�
//	CPngImage						m_ImageArrow2;						//��ͷͼ�꣨��Ϸ��

	CFont							m_FontBold;							//��������
	CFont							m_FontUnderLine;					// �»�������

	WORD							m_wIndexBkgnd;						// ����ͼƬIndex
//	map<WORD, CGameServerItem*>		m_mapLastGameServer;				// �����Ϸ	
	vector<CGameServerItem*>		m_vectGameServerItem;				// �����Ϸ
	bool							m_bShowLastGame;					// �Ƿ���ʾ���������Ϸ
//	map<WORD, CPngImage*>			m_mapLastGameIcon;					// ��Ϸ��Icon	
	map<WORD, CImage*>				m_mapLastGameIcon;
protected:
	CRect							m_rcTypeBkgnd;						// ��������
	vector<CRect>					m_vectRectLastGame;					// �����Ϸ����
	WORD							m_wHoverRectIndex;

public:
	// �����������Ϸ
	virtual VOID OnAddLastPlayGame(WORD wServerID, CGameServerItem *pGameServerItem);
	// ɾ����������Ϸ
	virtual VOID OnRemoveLastPlayGame(WORD wServerID);
	// ������������Ϸ����
	virtual VOID OnUpdateLastPlayGame(WORD wServerID, CGameServerItem *pGameServerItem);

	//��Ϣ����
protected:
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);
	//������Ϣ
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//����ƶ�
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//�����Ϣ
	VOID OnLButtonUp(UINT nFlags, CPoint Point);
	//�����Ϣ
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	// �ػ���Ϣ
	afx_msg VOID OnPaint();

	DECLARE_MESSAGE_MAP()
};

#endif
