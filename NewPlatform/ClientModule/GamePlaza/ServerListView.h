#ifndef SERVER_LIST_VIEW_HEAD_FILE
#define SERVER_LIST_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServerListData.h"
#include "LastGameServerCtrl.h"

//////////////////////////////////////////////////////////////////////////////////

//����˵��
typedef CMap<WORD,WORD,UINT,UINT>	CGameImageMap;						//��Ϸͼ��
typedef CList<WORD>	                CLastGameServerList;				//��Ϸ����

//////////////////////////////////////////////////////////////////////////////////

//�����б�
class CServerListView : public CTreeCtrl, public IServerListDataSink/*, public ILastGameServerCtrlSink */
{
	//λ�ñ���
protected:
	INT								m_nXScroll;							//����ƫ��
	INT								m_nYScroll;							//����ƫ��

	//�б���
protected:
	HTREEITEM						m_TreeListRoot;						//������
	HTREEITEM						m_TreeAssistant;					//�������

	//��Դ����
protected:
	CFont							m_FontBold;							//��������
	CFont							m_FontUnderLine;					//���»��ߵ�����
	CFont							m_FontNormal;						//��ͨ����
	CBitImage						m_ImageTypeBkgnd;					//���͵ĵ�ͼ
	CPngImage						m_ImageArrow1;						//��ͷͼ�꣨������ͣ�
	CPngImage						m_ImageArrow2;						//��ͷͼ�꣨��Ϸ��
	CPngImage						m_ImageItemLine;					//װ����
	CPngImage						m_ImageStarLevel;					//��Ϸ�Ǽ�
	CPngImage						m_ImageHotLevel;					//��Ϸ�ȶ�
	CPngImage						m_ImageVolumnFlag;					//��������
	CSkinScrollBar					m_SkinScrollBar;					//��������

	//��Դ����
protected:
	ILastGameServerSink*			m_pILastGameServerSink;				

	//��Դ����
protected:
	CImageList						m_ImageList;						//ͼƬ��Դ
	CGameImageMap					m_GameImageMap;						//ͼ������
	CLastGameServerList             m_LastPlayGameList;                 //��������Ϸ

	//��������
protected:
	HTREEITEM						m_hItemMouseHover;					//��������
	HTREEITEM						m_hTreeClickExpand;					//��������
//	CGameTypeItem                   m_LastServerItem;                   //�����Ϸ

	//�������
protected:

	//��������
public:
	//���캯��
	CServerListView();
	//��������
	virtual ~CServerListView();

	//״̬֪ͨ
public:
	//��ȡ֪ͨ
	virtual VOID OnGameItemFinish();
	//��ȡ֪ͨ
	virtual VOID OnGameKindFinish(WORD wKindID);
	//����֪ͨ
	virtual VOID OnGameItemUpdateFinish();

	//����֪ͨ
public:
	//����֪ͨ
	virtual VOID OnGameItemInsert(CGameListItem * pGameListItem);
	//����֪ͨ
	virtual VOID OnGameItemUpdate(CGameListItem * pGameListItem);
	//ɾ��֪ͨ
	virtual VOID OnGameItemDelete(CGameListItem * pGameListItem);

//	// �����Ϸ�ؼ�λ���ƶ�
// public:
// 	virtual VOID OnMoveLastGameServerCtrl(int nWidth, int nHeight);

	//���غ���
protected:
	//�����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//���ں���
	virtual LRESULT DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

	//���ܺ���
public:
	//����ͨ��
	VOID InitAssistantItem();
	//���ú���
	VOID InitServerTreeView();

	//���ܺ���
public:
	//��ȡѡ��
	HTREEITEM GetCurrentSelectItem(bool bOnlyText);
	//��Ӽ�¼
	void AddLastPlayGame(WORD wServerID, CGameServerItem *pGameServerItem);
	//�жϼ�¼
	bool IsLastPlayGame(WORD wServerID);
	//ö�ټ�¼
	HTREEITEM EmunGameServerItem(HTREEITEM hParentItem, WORD wServerID);
	//����Ϊ��
	bool IsEmptySubitem(WORD wKindID);
	//��÷�������״̬
	int  GetLoadInfoOfServer(tagGameServer * pGameServer);

	//չ������
public:
	//չ���ж�
	bool ExpandVerdict(HTREEITEM hTreeItem);
	//չ���б�
	bool ExpandListItem(HTREEITEM hTreeItem);
	//չ���б�
	bool ExpandListItem(CGameListItem * pGameListItem);

	//���ú���
public:
	//���ýӿ�
	VOID SetLastGameServerSink(ILastGameServerSink * pILastGameServerSink) { m_pILastGameServerSink = pILastGameServerSink; };

	//�滭����
private:
	//�滭����
	VOID DrawTreeItem(CDC * pDC, CRect & rcClient, CRect & rcClipBox);
	//�滭����
	VOID DrawTreeBack(CDC * pDC, CRect & rcClient, CRect & rcClipBox);

	//�滭��
	VOID DrawTree(CDC * pDC, CRect & rcClient, CRect & rcClipBox);

	//�滭����
private:
	//�����б���
	VOID DrawItem(CDC * pDC, CRect rcRect, HTREEITEM hTreeItem,bool bHovered, bool bSelected);
	//����ͼ��
	VOID DrawListImage(CDC * pDC, CRect rcRect, HTREEITEM hTreeItem,bool bHovered, bool bSelected);
	//�����ı�
	VOID DrawItemString(CDC * pDC, CRect rcRect, HTREEITEM hTreeItem, bool bSelected);

	//ͼ�꺯��
private:
	//��ȡͼ��
	UINT GetGameImageIndex(CGameKindItem * pGameKindItem);
	//��ȡͼ��
	UINT GetGameImageIndex(CGameServerItem * pGameServerItem);

	//���⺯��
private:
	//��ȡ����
	LPCTSTR GetGameItemTitle(CGameKindItem * pGameKindItem, LPTSTR pszTitle, UINT uMaxCount);
	//��ȡ����
	LPCTSTR GetGameItemTitle(CGameServerItem * pGameServerItem, LPTSTR pszTitle, UINT uMaxCount);

	//��������
private:
	//ɾ������
	VOID DeleteUpdateItem(CGameListItem * pGameListItem);
	//�޸�����
	VOID ModifyGameListItem(HTREEITEM hTreeItem, LPCTSTR pszTitle, UINT uImage);
	//��������
	HTREEITEM InsertInsideItem(LPCTSTR pszTitle, UINT uImage, DWORD dwInsideID, HTREEITEM hParentItem);
	//��������
	HTREEITEM InsertGameListItem(LPCTSTR pszTitle, UINT uImage, CGameListItem * pGameListItem, HTREEITEM hParentItem);

	//ϵͳ��Ϣ
protected:
	//�ػ���Ϣ
	VOID OnPaint();
	//ʱ����Ϣ
	VOID OnTimer(UINT nIDEvent);
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);
	//�����Ϣ
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	//�б���Ϣ
protected:
	//�Ҽ��б�
	VOID OnNMRClick(NMHDR * pNMHDR, LRESULT * pResult);
	//����б�
	VOID OnNMLClick(NMHDR * pNMHDR, LRESULT * pResult);
	//�б�ı�
	VOID OnTvnSelchanged(NMHDR * pNMHDR, LRESULT * pResult);
	//�б�չ��
//	VOID OnTvnItemexpanding(NMHDR * pNMHDR, LRESULT * pResult);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

protected:
	//��÷��为����Ϣ
	LPCTSTR GetLoadInfoOfServer(DWORD dwOnLineCount, DWORD dwMaxCount, LPTSTR pszBuffer, WORD wBufferSize);
	//��÷��为����Ϣ
	LPCTSTR GetLoadInfoOfServer(tagGameServer * pGameServer, LPTSTR pszBuffer, WORD wBufferSize);
	//��÷��为����Ϣ
	LPCTSTR GetLoadInfoOfServer(tagGameKind * pGameKind, LPTSTR pszBuffer, WORD wBufferSize);
	//���ؼ�¼
	void LoadLastPlayGame();
	//�����¼
	void SaveLastPlayGame();
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

//////////////////////////////////////////////////////////////////////////////////

#endif
