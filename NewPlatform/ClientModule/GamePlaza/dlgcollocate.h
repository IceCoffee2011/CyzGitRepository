#ifndef DLG_COLLOCATE_HEAD_FILE
#define DLG_COLLOCATE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603��˵��
class CCollocateItem;

//6603����˵��
typedef CWHArray<CCollocateItem *> CCollocateItemArray;

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603ѡ�����
class CCollocateBar : public CWnd
{
	//6603���Ϣ
protected:
	bool							m_bHovering;						//6603������־
	WORD							m_wItemDown;						//6603��������
	WORD							m_wItemHover;						//6603��������
	WORD							m_wItemActive;						//6603��������

	//6603������Ϣ
protected:
	CSize							m_SizeButton;						//6603��ť��С
	CRect							m_rcItemCreate;						//6603����λ��
	CCollocateItemArray				m_CollocateItemArray;				//6603��������

	//6603��������
public:
	//6603���캯��
	CCollocateBar();
	//6603��������
	virtual ~CCollocateBar();

	//6603���غ���
public:
	//6603��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//6603��������
public:
	//6603�������
	bool SaveParameter();
	//6603Ĭ�ϲ���
	bool DefaultParameter();

	//6603���ܺ���
public:
	//6603��������
	bool SetItemCreateRect(CRect rcItemCreate);
	//6603��������
	bool InsertCollocateItem(CCollocateItem * pCollocateItem);
	//6603��������
	bool SetActiveCollocateItem(CCollocateItem * pCollocateItem);

	//6603�ڲ�����
private:
	//6603�����л�
	WORD SwitchToButtonIndex(CPoint MousePoint);

	//6603��Ϣ����
protected:
	//6603�ػ���Ϣ
	VOID OnPaint();
	//6603������Ϣ
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//6603����ƶ�
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//6603�����Ϣ
	VOID OnLButtonUp(UINT nFlags, CPoint Point);
	//6603�����Ϣ
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//6603����뿪
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//6603�����Ϣ
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603��������
class CCollocateItem : public CDialog
{
	//6603��Ԫ����
	friend class CCollocateBar;
	friend class CDlgCollocate;

	//6603��������
protected:
	BYTE							m_cbImageIndex;						//6603ͼ����Ҫ

	//6603��������
public:
	//6603���캯��
	CCollocateItem(UINT uIDTemplate);
	//6603��������
	virtual ~CCollocateItem();

	//6603���غ���
protected:
	//6603ȷ����Ϣ
	virtual VOID OnOK() { GetParent()->PostMessage(WM_COMMAND,IDOK,0); }
	//6603ȡ����Ϣ
	virtual VOID OnCancel() { GetParent()->PostMessage(WM_COMMAND,IDCANCEL,0); }

	//6603���غ���
public:
	//6603�������
	virtual bool SaveParameter()=NULL;
	//6603Ĭ�ϲ���
	virtual bool DefaultParameter()=NULL;
	//6603���¿���
	virtual bool UpdateControlStatus()=NULL;

	//6603���ܺ���
public:
	//6603��������
	bool CreateCollocateItem(CWnd * pParentWnd, CRect rcCreate);

	//6603��Ϣ����
protected:
	//6603������Ϣ
	BOOL OnEraseBkgnd(CDC * pDC);
	//6603��ɫ��Ϣ
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603ͬ������
class CCollocateServer : public CCollocateItem
{
	//6603ʤ������
public:
	WORD							m_wMinWinRate;						//6603���ʤ��
	bool							m_bLimitWinRate;					//6603����ʤ��

	//6603��������
public:
	WORD							m_wMaxFleeRate;						//6603�������
	bool							m_bLimitFleeRate;					//6603���ƶ���

	//6603��������
public:
	LONG							m_lMaxGameScore;					//6603��߷��� 
	LONG							m_lMinGameScore;					//6603��ͷ���
	bool							m_bLimitGameScore;					//6603���Ʒ���

	//6603��������
public:
	bool							m_bLimitDetest;						//6603�������
	bool							m_bLimitSameIP;						//6603Ч���ַ

	//6603Я������
public:
	bool							m_bTakePassword;					//6603Я������
	TCHAR							m_szPassword[LEN_PASSWORD];			//6603��������

	//6603�༭�ؼ�
protected:
	CSkinEdit						m_edWinRate;						//6603�û�ʤ��
	CSkinEdit						m_edFleeRate;						//6603�û�����
	CSkinEdit						m_edScoreMin;						//6603��ͻ���
	CSkinEdit						m_edScoreMax;						//6603��߻���
	CSkinEdit						m_edPassword;						//6603��������

	//6603��������
public:
	//6603���캯��
	CCollocateServer();
	//6603��������
	virtual ~CCollocateServer();

	//6603���غ���
public:
	//6603�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//6603���ú���
	virtual BOOL OnInitDialog();

	//6603���غ���
public:
	//6603�������
	virtual bool SaveParameter();
	//6603Ĭ�ϲ���
	virtual bool DefaultParameter();
	//6603���¿���
	virtual bool UpdateControlStatus();
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603��������
class CCollocateProxy : public CCollocateItem
{
	//6603���ñ���
public:
	BYTE							m_cbProxyType;						//6603��������
	tagProxyServer					m_ProxyServer;						//6603������Ϣ

	//6603��ť�ؼ�
protected:
	CSkinButton						m_btProxyTest;						//6603���԰�ť
	CSkinComboBox					m_cmProxyType;						//6603��������

	//6603�༭�ؼ�
protected:
	CSkinEdit						m_edProxyPort;						//6603����˿�
	CSkinEdit						m_edProxyServer;					//6603�����ַ
	CSkinEdit						m_edProxyUserName;					//6603�����û�
	CSkinEdit						m_edProxyPassword;					//6603��������

	//6603��������
public:
	//6603���캯��
	CCollocateProxy();
	//6603��������
	virtual ~CCollocateProxy();

	//6603���غ���
public:
	//6603�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//6603��������
	virtual BOOL OnInitDialog();

	//6603���غ���
public:
	//6603�������
	virtual bool SaveParameter();
	//6603Ĭ�ϲ���
	virtual bool DefaultParameter();
	//6603���¿���
	virtual bool UpdateControlStatus();

	//6603��Ϣ����
public:
	//6603�������
	VOID OnBnClickedProxyTest();

	DECLARE_MESSAGE_MAP()
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603��Ϣ����
class CCollocateNotify : public CCollocateItem
{
	//6603���ñ���
public:
	WORD							m_wWhisperHotKey;					//6603˽���ȼ�
	bool							m_bAutoShowWhisper;					//6603��ʾ��Ϣ

	//6603ö�ٱ���
public:
	BYTE							m_cbInviteMode;						//6603����ģʽ
	BYTE							m_cbMessageMode;					//6603��Ϣģʽ

	//6603��������
public:
	//6603���캯��
	CCollocateNotify();
	//6603��������
	virtual ~CCollocateNotify();

	//6603���غ���
public:
	//6603�������
	virtual bool SaveParameter();
	//6603Ĭ�ϲ���
	virtual bool DefaultParameter();
	//6603���¿���
	virtual bool UpdateControlStatus();
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603ϰ������
class CCollocateCustom : public CCollocateItem
{
	//6603���ñ���
public:
	bool							m_bSendWhisperByEnter;				//6603�س�����

	//6603��������
public:
	BYTE							m_cbActionHitAutoJoin;				//6603�Զ�����
	BYTE							m_cbActionLeftDoubleList;			//6603˫���б�

	//6603��������
public:
	//6603���캯��
	CCollocateCustom();
	//6603��������
	virtual ~CCollocateCustom();

	//6603���غ���
public:
	//6603�������
	virtual bool SaveParameter();
	//6603Ĭ�ϲ���
	virtual bool DefaultParameter();
	//6603���¿���
	virtual bool UpdateControlStatus();
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603ϵͳ����
class CCollocateSystem : public CCollocateItem
{
	//6603���ñ���
public:
	WORD							m_wBossHotKey;						//6603�ϰ��ȼ�

	//6603���ñ���
public:
	bool							m_bAllowSound;						//6603��������
	bool							m_bAllowBackMusic;					//6603����������
	bool							m_bAutoSitDown;						//6603�Զ�����
	bool							m_bSalienceTable;					//6603ͻ������
	bool							m_bSaveWhisperChat;					//6603����˽��
	bool							m_bNotifyUserInOut;					//6603������Ϣ
	bool							m_bNotifyFriendCome;				//6603������ʾ
	bool							m_bFullScreenBrowse;				//6603ȫ�����

	//6603��������
public:
	//6603���캯��
	CCollocateSystem();
	//6603��������
	virtual ~CCollocateSystem();

	//6603���غ���
public:
	//6603�������
	virtual bool SaveParameter();
	//6603Ĭ�ϲ���
	virtual bool DefaultParameter();
	//6603���¿���
	virtual bool UpdateControlStatus();
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603���ô���
class CDlgCollocate : public CSkinDialog
{
	//6603���ñ���
protected:
	CParameterGame *				m_pParameterGame;					//6603��Ϸ����
	CParameterServer *				m_pParameterServer;					//6603��������

	//6603�ؼ�����
protected:
	CSkinButton						m_btOk;								//6603ȷ����ť
	CSkinButton						m_btCancel;							//6603ȡ����ť
	CSkinButton						m_btDefault;						//6603Ĭ�ϰ�ť
	CCollocateBar					m_CollocateBar;						//6603���ť

	//6603��������
public:
	CCollocateProxy					m_CollocateProxy;					//6603��������
	CCollocateSystem				m_CollocateSystem;					//6603ϵͳ����
	CCollocateCustom				m_CollocateCustom;					//6603ϰ������
	CCollocateNotify				m_CollocateNotify;					//6603��Ϣ����
	CCollocateServer				m_CollocateServer;					//6603��������

	//6603��������
public:
	//6603���캯��
	CDlgCollocate();
	//6603��������
	virtual ~CDlgCollocate();

	//6603���غ���
protected:
	//6603�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//6603��������
	virtual BOOL OnInitDialog();
	//6603ȷ������
	virtual VOID OnOK();

	//6603���ܺ���
public:
	//6603���ò���
	bool InitCollocate(CParameterGame * pParameterGame, CParameterServer * pParameterServer);

	//6603��ť��Ϣ
protected:
	//6603Ĭ�ϰ�ť
	VOID OnBnClickedDefault();

	DECLARE_MESSAGE_MAP()
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

#endif