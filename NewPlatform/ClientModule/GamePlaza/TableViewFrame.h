#ifndef TABLE_VIEW_FRAME_HEAD_FILE
#define TABLE_VIEW_FRAME_HEAD_FILE

// ������
// ��ͨ��Ϸ��
// m_ImageBitGround �ȶ�ȡ��ԴĿ¼�µ�ground.bmp����ͼ
// ����ģʽ

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "GamePlazaHead.h"
#include "PlatformEvent.h"
#include "PlatformPublicize.h"
#include "DlgTipUserInfo.h"
#include "UserInfoDlgEx.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#define INDEX_ENTER_CHAIR			MAX_CHAIR							//��������

//��ť��ʶ
#define BT_ENTER_GAME				1									//������Ϸ
#define BT_LEAVE_MATCH				2									//�رշ���

//��������
struct tagTableAttribute
{
	//���ӱ�־
	bool							bLocker;							//�����־
	bool							bPlaying;							//��Ϸ��־
	bool							bFocusFrame;						//��ܱ�־

	//����״̬
	WORD							wWatchCount;						//�Թ���Ŀ
	DWORD							dwTableOwnerID;						//��������

	//���Ա���
	WORD							wTableID;							//���Ӻ���
	WORD							wChairCount;						//������Ŀ
	TCHAR							szTableName[LEN_TABLENAME];			//��������
	IClientUserItem *				pIClientUserItem[MAX_CHAIR];		//�û���Ϣ
};

//////////////////////////////////////////////////////////////////////////////////

//������Դ
class CTableResource
{
	//��������
public:
	CSize							m_SizeChair;						//���ӳߴ�
	CSize							m_SizeTable;						//���ӳߴ�
	CSize							m_SizeEnter;						//����ߴ�
	CSize                           m_SizeNumberBK;                     //���ųߴ�
	CSize                           m_SizeNumberNB;                     //���ųߴ�

	//���Ա���
public:
	bool							m_bShowUser;						//��ʾ�û�
	bool							m_bShowEnter;						//��ʾ����
	bool							m_bShowChair;						//��ʾ����
	bool							m_bShowTable;						//��ʾ����
	bool							m_bShowFocus;						//��ʾ����
	bool							m_bCustomUser;						//�Զ��û�
	bool							m_bRenderImage;						//��Ⱦ��־
	bool							m_b3DChair;							//3D����
	LONG							m_lDefaultFace;						//ȱʡ��ͷ��
	LONG							m_lShowTableID;						//������ʾģʽ

	//��Ŀ����
public:
	WORD							m_wTableItemCount;					//������Ŀ
	WORD							m_wChairItemCount;					//������Ŀ

	WORD							m_wChairCellCount;					//����ͼƬ��Ԫ����
	LONG							m_lChairCellIndex[MAX_CHAIR*2];		//����ͼƬ����

	//������Ŀ
public:
	WORD							m_wUserGenreCount;					//������Ŀ
	WORD							m_wTableGenreCount;					//������Ŀ
	WORD							m_wChairGenreCount;					//������Ŀ

	//λ������
public:
	CPoint							m_ptLock;							//����λ��
	CPoint							m_ptEnter;							//����λ��
	CPoint							m_ptTableID;						//����λ��
	CPoint							m_ptReadyArray[MAX_CHAIR];			//׼��λ��
	CPoint							m_ptAvatarArray[MAX_CHAIR];			//ͷ��λ��

	//ͷ������
public:
	int								m_ptAvatarIndex[MAX_CHAIR];			//ͷ������ 

	//���λ��
public:
	CRect							m_rcNameArray[MAX_CHAIR];			//����λ��
	CRect							m_rcChairArray[MAX_CHAIR];			//����λ��

	//��ɫ����
public:
	COLORREF						m_crName;							//������ɫ
	COLORREF						m_crMember;							//��Ա��ɫ
	COLORREF						m_crMaster;							//������ɫ

	//��������
public:
	INT								m_nDrawStyle[MAX_CHAIR];			//�����ʽ
	TCHAR							m_szDirectory[MAX_PATH];			//��Դ·��

	//�ⲿ��Դ
public:
	CPngImage						m_ImageEnter;						//����λͼ
	CPngImage						m_ImageChair;						//����λͼ
	CPngImage						m_ImageChairBOX;					//���ӿ�λͼ
	CPngImage						m_ImageTable;						//����λͼ
	CPngImage						m_ImageTableBOX;					//���ӿ�λͼ
	CPngImage						m_ImageGround;						//����λͼ
	CBitImage						m_ImageBitGround;					//����λͼ
	CPngImage						m_ImageCustomUser;					//�Զ�λͼ
	CBitImage						m_ImageWaitDistribute;				//����λͼ

	//�ڲ���Դ
public:
	CPngImage						m_ImageReady;						//׼��λͼ
	CPngImage						m_ImageLocker;						//����λͼ
	CPngImage						m_ImageNumberNB;					//����λͼ
	CPngImage						m_ImageNumberBK;					//���뱳��
	CPngImage						m_ImageUserFace_2D;					//456��ͷ��ȱʡ��
	CPngImage						m_ImageUserFace_3D;					//3Dͷ��ȱʡ��

	//��������
public:
	//���캯��
	CTableResource();
	//��������
	virtual ~CTableResource();

	//���ܺ���
public:
	//�ͷ���Դ
	VOID DestroyResource();
	//������Դ
	bool LoadResource(LPCTSTR pszResDirectory, WORD wChairCount, bool bAvertCheatMode);
	//������Դ
	bool UpdateResource(CSkinRenderManager * pSkinRenderManager);

	//���ܺ���
public:
	//�������ͼƬ������
	LONG GetTableCellIndex(WORD wChairID, bool bHasUser);

	//��������
protected:
	//��ȡ��ֵ
	LONG ReadStringValue(LPCTSTR & pszSring);
	//��ȡ����
	bool ReadRect(RECT & ValueRect, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile);
	//��ȡ����
	bool ReadPoint(POINT & ValuePoint, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile);
	//��ȡ��ɫ
	bool ReadColor(COLORREF & ValueColor, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile);
};

//////////////////////////////////////////////////////////////////////////////////

//������ͼ
class CTableView : public ITableView
{
	//״̬����
protected:
	bool							m_bMouseDown;						//�Ƿ���
	WORD							m_wHoverChairID;					//��������
	tagTableAttribute				m_TableAttribute;					//��������

	//����ӿ�
protected:
	CDFontEx*						m_pDrawFont;						//����ӿ�

	//����ӿ�
protected:
	ITableViewFrame *				m_pITableViewFrame;					//���ӽӿ�

	//��������
public:
	//���캯��
	CTableView();
	//��������
	virtual ~CTableView();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ܽӿ�
public:
	//��������
	virtual WORD GetNullChairCount(WORD & wNullChairID);
	//���ú���
	virtual VOID InitTableView(WORD wTableID, WORD wChairCount, TCHAR szTableName[LEN_TABLENAME], ITableViewFrame * pITableViewFrame);

	//�û��ӿ�
public:
	//��ȡ�û�
	virtual IClientUserItem * GetClientUserItem(WORD wChairID);
	//������Ϣ
	virtual bool SetClientUserItem(WORD wChairID, IClientUserItem * pIClientUserItem);

	//״̬�ӿ�
public:
	//��갴��
	virtual VOID SetMouseDown(bool bMouseDown);
	//������
	virtual VOID SetFocusFrame(bool bFocusFrame);
	//����λ��
	virtual VOID SetHoverStatus(WORD wHoverChairID);
	//����״̬ 
	virtual VOID SetTableStatus(bool bPlaying, bool bLocker);

	//��ѯ�ӿ�
public:
	//��Ϸ��־
	virtual bool GetPlayFlag() { return m_TableAttribute.bPlaying; }
	//�����־
	virtual bool GetLockerFlag() { return m_TableAttribute.bLocker; }

	//���ú���
public:
	//��������
	void SetFont( CDFontEx* pFont ) { m_pDrawFont = pFont; }

	//���ܺ���
public:
	//�滭����
	VOID DrawTableView(CDC * pDC, INT nXPos, INT nYPos, DWORD dwServerRule, CTableResource * pTableResource);
	//�滭����
	VOID DrawTableName(CDC * pDC, INT nXPos, INT nYPos, WORD wChairID, LPCTSTR pszNickName, CTableResource * pTableResource, COLORREF crTextColor);
	//�滭ͷ��
	VOID DrawUserAvatar(CDC * pDC, INT nXPos, INT nYPos, WORD wChairID, IClientUserItem * pIClientUserItem, CTableResource * pTableResource);
	//�滭ͷ��
	VOID DrawUserAvatar(CDC * pDC, INT nXPos, INT nYPos, WORD wFaceID, WORD wChairID, CTableResource * pTableResource);
};

//////////////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<CTableView *>		CTableViewArray;					//��������

//���ӿ��
class CTableViewFrame : public CWnd, public ITableViewFrame
{
	//��Ԫ����
	friend class CTableView;

	//״̬��Ϣ
protected:
	bool							m_bHovering;						//������־
	bool							m_bLMouseDown;						//�Ҽ���־
	bool							m_bRMouseDown;						//�����־
	bool                            m_bWaitDistribute;                  //�ȴ�����
	BYTE							m_cbButtonDown;						//��ť����
	BYTE							m_cbButtonHover;					//��ť����

	//��������
protected:
	WORD							m_wDownTableID;						//���Ӻ���
	WORD							m_wDownChairID;						//���Ӻ���

	//���Ա���
protected:
	WORD							m_wTableCount;						//��Ϸ����
	WORD							m_wChairCount;						//������Ŀ
	DWORD							m_dwServerRule;						//�������
	WORD							m_wServerType;						//��������

	WORD							m_wTipTableID;						//��Ϸ����
	WORD							m_wTipChairID;						//������Ŀ
	WORD							m_wLastTipChairID;					//������Ŀ

	//������Ϣ
protected:
	INT								m_nXExcursion;						//����ƫ��
	INT								m_nYExcursion;						//����ƫ��
	INT								m_nXTableCount;						//��������
	INT								m_nYTableCount;						//��������

	//������Ϣ
protected:
	INT								m_nScrollPos;						//����λ��
	INT								m_nScrollPosMax;					//���λ��
	INT								m_nVisibleLineCount;				//�ɼ�����

	//�ؼ�����
public:
	CSkinScrollBar					m_SkinScrollBar;					//��������
	CTableResource					m_TableResource;					//������Դ
	CTableViewArray					m_TableViewArray;					//��������
	//CDlgTipUserInfo                 m_DlgTipUserInfo;                   //��ʾ���
    
	CUserInfoDlgEx                  m_UserInfoDlgEx;
	CString							m_strName;							//��������
	//��������
public:
	DWORD							m_dwWaitting;						//�ȴ�����
	DWORD							m_dwTotalUser;						//��������
	DWORD							m_dwBestRank;						//�����Ϸ��
	DWORD							m_dwMatchCount;						//��������
	CTime							m_tStartTime;						//��ʼʱ��
	BYTE							m_cbMatchStatus;					//����״̬
	CBitImage						m_ImageEnterMatch;					//�������
	CBitImage						m_ImageLeaveMatch;					//�˳�����
	CBitImage						m_ImageTableTL;						//ͼƬ��Դ
	CBitImage						m_ImageTableTM;						//ͼƬ��Դ
	CBitImage						m_ImageTableTR;						//ͼƬ��Դ
	CPngImage						m_ImageNumberGreen;					//��ɫ����
	CPngImage						m_ImageNumberYellow;				//��ɫ����
	CPngImage						m_ImageLine;						//ͼƬ��Դ
	CPlatformPublicize				m_BrowseMatchInfo;					//����ҳ��
	CMD_GR_MatchDesc				m_pMatchDesc;						//��Ϣ����

	//������Ϣ
public:
	CDFontEx						m_DrawFont;							//һ������

	//����ӿ�
public:
	ITableViewFrameSink *			m_pITableViewFrameSink;				//֪ͨ�ӿ�
	IGameLevelParser *				m_pIGameLevelParser;				//�ȼ��ӿ�

	//��������
public:
	//���캯��
	CTableViewFrame();
	//��������
	virtual ~CTableViewFrame();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ýӿ�
public:
	//��������
	virtual bool CreateTableFrame(CWnd * pParentWnd, UINT uWndID, IUnknownEx * pIUnknownEx);
	//��������
	virtual bool ConfigTableFrame(WORD wTableCount, WORD wChairCount, TCHAR(* szTableNameArray)[LEN_TABLENAME], DWORD dwServerRule, WORD wServerType, WORD wServerID, LPCTSTR pszResDirectory);

	//��Ϣ�ӿ�
public:
	//������Ŀ
	virtual WORD GetTableCount() { return m_wTableCount; }
	//������Ŀ
	virtual WORD GetChairCount() { return m_wChairCount; }
	//��ȡ��Դ
	virtual CTableResource * GetTableResource() { return &m_TableResource; }

	//�û��ӿ�
public:
	//��ȡ�û�
	virtual IClientUserItem * GetClientUserItem(WORD wTableID, WORD wChairID);
	//������Ϣ
	virtual bool SetClientUserItem(WORD wTableID, WORD wChairID, IClientUserItem * pIClientUserItem);

	//״̬�ӿ�
public:
	//��Ϸ��־
	virtual bool GetPlayFlag(WORD wTableID);
	//�����־
	virtual bool GetLockerFlag(WORD wTableID);
	//������
	virtual VOID SetFocusFrame(WORD wTableID, bool bFocusFrame);
	//����״̬ 
	virtual VOID SetTableStatus(WORD wTableID, bool bPlaying, bool bLocker);
	//����״̬ 
	virtual VOID SetTableStatus(bool bWaitDistribute);

	//��ͼ����
public:
	//���ӿ���
	virtual bool VisibleTable(WORD wTableID);
	//��������
	virtual bool FlashGameTable(WORD wTableID);
	//��������
	virtual bool FlashGameChair(WORD wTableID, WORD wChairID);

	//���ܽӿ�
public:
	//��������
	virtual bool UpdateTableView(WORD wTableID);
	//��ȡ����
	virtual ITableView * GetTableViewItem(WORD wTableID);
	//��������
	virtual WORD GetNullChairCount(WORD wTableID, WORD & wNullChairID);
	//��ȡ�ӿ�
	virtual IGameLevelParser * GetGameLevelParser() { return m_pIGameLevelParser; }
	//���ýӿ�
	VOID SetGameLevelParser(IGameLevelParser * pIGameLevelParser) { m_pIGameLevelParser=pIGameLevelParser; }

	//���غ���
protected:
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//��������
protected:
	//���Ӳ���
	WORD SwitchTableID(POINT MousePoint);
	//���Ӳ���
	WORD SwitchChairID(POINT MousePoint);
	//��þ��ο�
	CRect GetTableRect(WORD wTableID);
	//�����ؼ�
	VOID RectifyControl(INT nWidth, INT nHeight);
	//��ť����
	BYTE GetHoverButton(CPoint MousePoint);
	//�滭����
	VOID DrawNumberString(CDC * pDC, CPngImage &PngNumber, LONG lNumber, INT nXPos, INT nYPos,BYTE cbAlign=TA_LEFT);
	//�滭ͼƬ
	VOID SpreadDrawImage(CDC * pDC, CPngImage &PngImage, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight);
	//�滭ͼƬ
	VOID SpreadDrawImage(CDC * pDC, CBitImage &BitImage, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight);
	//��Ϣ����
protected:
	//�ػ���Ϣ
	VOID OnPaint();
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);
	//�����Ϣ
	VOID OnRButtonUp(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	VOID OnLButtonUp(UINT nFlags, CPoint MousePoint);
	//����뿪
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//�����Ϣ
	VOID OnRButtonDown(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	VOID OnLButtonDown(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	VOID OnLButtonDblClk(UINT nFlags, CPoint MousePoint);
	//�����Ϣ
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//�����Ϣ
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint MousePoint);
	//������Ϣ
	VOID OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	//��ʱ��Ϣ
	afx_msg void OnTimer(UINT nIDEvent);
	//�Զ���Ϣ
protected:
	//�¼���Ϣ
	LRESULT OnMessagePlatformEvent(WPARAM wParam, LPARAM lParam);

	//��������
public:
	//����״̬
	void SetMatchStatus(BYTE cbMatchStatus);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif
