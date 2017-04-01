#ifndef DLG_INSURE_COUNTER_HEAD_FILE
#define DLG_INSURE_COUNTER_HEAD_FILE

#pragma once

#include "MissionManager.h"
#include "PasswordControl.h"
#include "ShareControlHead.h"

//////////////////////////////////////////////////////////////////////////////////

//��˵��
class CDlgInsureItem;
class CDlgInsureMain;
class CDlgInsureSave;

//////////////////////////////////////////////////////////////////////////////////

//������ʽ
#define INSURE_SAVE					0									// ��ȡ��ʽ
#define INSURE_TRANSFER				1									// ת�˷�ʽ
#define TRANS_RECORD				2									// ת����ʷ��¼	
#define INSURE_MODIFY_PASSWORD		3									// �޸���������
#define INSURE_PASSWORD				4									// ��֤���������ǩ


#define EVENT_QUERYNICKNAME			100									// ����GameID��ѯ�ǳ�					

struct RealTransRecord
{
	DWORD                               dwGameID;
	TCHAR                               szSourceAccount[LEN_ACCOUNTS];
	TCHAR                               szTargetAccounts[LEN_ACCOUNTS];
	LONGLONG                            lSwapScore;
	LONGLONG                            lRevenue;
	DWORD								dwSourceID;
	DWORD								dwTargetID;
	TCHAR                               szTime[LEN_ACCOUNTS];
};

typedef CWHArray<RealTransRecord *>	VecTransRecord;					//ת�˼�¼����

//////////////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagUserInsureInfo
{
	WORD							wRevenueTake;						//˰�ձ���
	WORD							wRevenueTransfer;					//˰�ձ���
	WORD							wServerID;							//�����ʶ
	SCORE							lUserScore;							//�û���Ϸ��
	SCORE							lUserInsure;						//������Ϸ��
	SCORE							lTransferPrerequisite;				//ת������
};

//////////////////////////////////////////////////////////////////////////////////

//���нӿ�
interface IInsureCounterAction
{
	//��ѯ�¼�
	virtual VOID PerformQueryInfo()=NULL;
	//�����¼�
	virtual VOID PerformSaveScore(SCORE lSaveScore)=NULL;
	//ȡ���¼�
	virtual VOID PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass)=NULL;
	//ת���¼�
	virtual VOID PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass)=NULL;
    //ת�˼�¼
    virtual VOID PerformQueryTransRecord()=NULL;
	// ��ѯ����ǳ�
	virtual VOID PerformQueryUserNickName(DWORD dwGameID) = NULL;
	// ������������
	virtual VOID PerformInsurePassword(LPCTSTR pszInsurePass) = NULL;
	// �޸���������
	virtual VOID PerformModiyInsurePassword(LPCTSTR pszSrcPassword, LPCTSTR pszDesPassword) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////

//ҳ������
class CDlgInsureItem
{
	//��������
public:
	//���캯��
	CDlgInsureItem();
	//��������
	virtual ~CDlgInsureItem();

	//���ܺ���
public:
	//��ȡ��Ϸ��
	SCORE GetUserControlScore(CWnd * pWndScore);

	//���ܺ���
public:
	//�滭����
	VOID DrawNumberString(CDC * pDC, SCORE lScore, INT nXPos, INT nYPos);
	//ת���ַ�
	VOID SwitchScoreString(SCORE lScore, LPTSTR pszBuffer, WORD wBufferSize);
	//ת���ַ�
	VOID SwitchScoreFormat(SCORE lScore, UINT uSpace, LPTSTR pszBuffer, WORD wBufferSize);
};
//////////////////////////////////////////////////////////////////////////
// ��ʼ������������Ի���
class CDlgInsurePassword : public CDialog
{
	//��Ԫ����
	friend class CDlgInsureMain;
	//����ָ��
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//����ָ��
public:
	CSkinButton						m_btOk;								// ȷ����ť
	CSkinButton						m_btCancel;							// ȡ����ť
public:
	CPasswordControl				m_PasswordControl;					// �������봰��

public:
	CDlgInsurePassword();
	virtual ~CDlgInsurePassword();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//��������
	virtual BOOL OnInitDialog();
	// ȷ����ť
	virtual VOID OnOK();
	// ȡ����ť
	virtual VOID OnCancel();
protected:
	// ȷ�����밴ť
	VOID OnBnClickedOk();
	// ȡ����ť
	VOID OnBnClickedCancel();

protected:
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);


	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
class CTransRecordList:public CSkinListCtrl
{
    
    //��������
public:
    //���캯��
    CTransRecordList(){};
    //��������
    virtual ~CTransRecordList(){};
    //���غ���
public:
    //�滭����
    virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    DECLARE_MESSAGE_MAP()
};


//��ȡ��Ϸ��(��¼)
class CDlgTransRecord : public CDialog
{
	//��Ԫ����
	friend class CDlgInsureMain;
	//����ָ��
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//����ָ��
	tagUserInsureInfo *				m_pUserInsureInfo;					//�û���Ϣ

	//�ؼ�����
public:
	CSkinListCtrl					m_ListView;							
	CSkinButton						m_btQuery;						//��ѯ��ť
	CSkinButton						m_btFirst;						//��ҳ��ť
	CSkinButton						m_btPre;						//��һҳ��ť
	CSkinButton						m_btNext;						//��һҳ��ť
	CSkinButton						m_btLast;						//βҳ��ť
	CDlgInsureItem					m_DlgInsureItem;					//��������
	
    bool m_bQueryRecord;
	VecTransRecord					m_vecTransRecord;
	BYTE							m_cbCurPage;
	BYTE							m_cbPageCell;
    BYTE                            m_cbPageCount;


	//��������
public:
	//���캯��
	CDlgTransRecord();
	//��������
	virtual ~CDlgTransRecord();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��������
	virtual BOOL OnInitDialog();
	//ȷ����ť
	virtual VOID OnOK(){return;}
	//ȡ����ť
	virtual VOID OnCancel(){return;};
private:
	void ShowTransRecord();
public:
    void SetRecordList(CMD_GR_RealTransRecord* pRealTransRecord);
	void SetRecordList(CMD_GP_RealTransRecord* pRealTransRecord);
	//��Ϣ����
protected:
	afx_msg void OnTimer(UINT nIDEvent);
    //�����
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//�ؼ���ɫ
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};
//////////////////////////////////////////////////////////////////////////////////

//��ȡ��Ϸ��
class CDlgInsureSave : public CDialog
{
	//��Ԫ����
	friend class CDlgInsureMain;

	//��������
protected:
	bool							m_bSwitchIng;						//ת����־

	//����ָ��
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//����ָ��
	tagUserInsureInfo *				m_pUserInsureInfo;					//�û���Ϣ

	//�ؼ�����
public:
	CSkinEditEx						m_edScore;							//������Ϸ��
	CSkinButton						m_btTakeScore;						//ȡ�ť
	CSkinButton						m_btSaveScore;						//��ť

	//�����ؼ�
public:
//	CSkinHyperLink					m_ForgetInsure;						//��������
	CDlgInsureItem					m_DlgInsureItem;					//��������
//	CPasswordControl				m_PasswordControl;					//�û����� (���������û����룬�ڶԻ����ʱ�Ѿ������)
//	CDlgTransRecord					m_DlgTransRecord;

	// ��ݲ���
protected:
	CSkinHyperLink					m_hlOneMillion;					// 100����
	CSkinHyperLink					m_hlFiveMillion;				// 500����
	CSkinHyperLink					m_hlTenMillion;					// 1000����
	CSkinHyperLink					m_hlFiftyMillion;				// 5000����
	CSkinHyperLink					m_hlOneHundredMillion;			// 1��
	CSkinHyperLink					m_hlFiveHundredMillion;			// 5��
	CSkinHyperLink					m_hlAllScore;					// ȫ����Ϸ
	CSkinHyperLink					m_hlAllInsureScore;				// ȫ��������	
	
	//��������
public:
	//���캯��
	CDlgInsureSave();
	//��������
	virtual ~CDlgInsureSave();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��������
	virtual BOOL OnInitDialog();
	//ȷ����ť
	virtual VOID OnOK();
	//ȡ����ť
	virtual VOID OnCancel();

	//�ؼ�����
protected:
	//������Ϣ
	VOID OnEnChangeScore();
	//��ť
	VOID OnBnClickedSaveScore();
	//ȡ�ť
	VOID OnBnClickedTakeScore();
	//��������
	VOID OnBnClickedForgetInsure();

	// ���ֽ�����
	VOID OnBnClickedHyperLink1();
	VOID OnBnClickedHyperLink2();
	VOID OnBnClickedHyperLink3();
	VOID OnBnClickedHyperLink4();
	VOID OnBnClickedHyperLink5();
	VOID OnBnClickedHyperLink6();
	VOID OnBnClickedHyperLink7();
	VOID OnBnClickedHyperLink8();
	//��Ϣ����
protected:
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//�ؼ���ɫ
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//ת����Ϸ��
class CDlgInsureTransfer : public CDialog
{
	//��Ԫ����
	friend class CDlgInsureMain;

	//��������
protected:
	bool							m_bSwitchIng;						//ת����־

	//����ָ��
protected:
	CDlgInsureMain *				m_pDlgInsureMain;					//����ָ��
	tagUserInsureInfo *				m_pUserInsureInfo;					//�û���Ϣ

	//�ؼ�����
public:
	CSkinEditEx						m_edScore;							//������Ϸ��
	CSkinEditEx						m_edNickName;						//�û��ǳ�
	CSkinButton						m_btTransferScore;					//ת�˰�ť

	//�����ؼ�
public:
//	CSkinHyperLink					m_ForgetInsure;						//��������
	CDlgInsureItem					m_DlgInsureItem;					//��������
//	CPasswordControl				m_PasswordControl;					//�û�����

	CSkinHyperLink					m_hlOneMillion;						// һ��������
	CSkinHyperLink					m_hlFiveMillion;					// 5��������
	CSkinHyperLink					m_hlTenMillion;						// 1ǧ������
	CSkinHyperLink					m_hlFiftyMillion;					// 5ǧ������
	CSkinHyperLink					m_hlOneHundredMillion;				// 1�ڳ�����
	CSkinHyperLink					m_hlFiveHundredMillion;				// 5�ڳ�����


	//��������
public:
	//���캯��
	CDlgInsureTransfer();
	//��������
	virtual ~CDlgInsureTransfer();

public:
	// ������������ǳ�
	VOID SetUserNickName(LPCTSTR lpszNickName);

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��������
	virtual BOOL OnInitDialog();
	//ȷ����ť
	virtual VOID OnOK();
	//ȡ����ť
	virtual VOID OnCancel();

	//�ؼ�����
protected:
	//������Ϣ
	VOID OnEnChangeScore();
	//��������
	VOID OnBnClickedForgetInsure();
	//ת�˰�ť
	VOID OnBnClickedTransferScore();
	// GameID���봰��ʧȥ����
	VOID OnKillFocusNickName();
	// ���ֽ�����
	VOID OnBnClickedHyperLink1();
	VOID OnBnClickedHyperLink2();
	VOID OnBnClickedHyperLink3();
	VOID OnBnClickedHyperLink4();
	VOID OnBnClickedHyperLink5();
	VOID OnBnClickedHyperLink6();

	//��Ϣ����
protected:
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//�ؼ���ɫ
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);
	//ʱ����Ϣ
	VOID OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedById();
	afx_msg void OnBnClickedByName();
};

//////////////////////////////////////////////////////////////////////////////////

// �޸���������
class CDlgInsureModifyPassword : public CDialog
{
	//��Ԫ����
	friend class CDlgInsureMain;
public:
	CSkinButton					m_btModify;				// �޸İ�ť
	CPasswordControl			m_edInsurePassword1;	// ��������1
	CPasswordControl			m_edInsurePassword2;	// ��������2
	CPasswordControl			m_edInsurePassword3;	// ��������2

protected:
	CDlgInsureMain *			m_pDlgInsureMain;					//����ָ��

public:
	CDlgInsureModifyPassword();
	~CDlgInsureModifyPassword();
	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//��������
	virtual BOOL OnInitDialog();
	// ȷ����ť
	virtual VOID OnOK();
	// ȡ����ť
	virtual VOID OnCancel();
protected:
	// �޸����밴ť
	VOID OnBnClickedModifyPassword();
protected:
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//���չ���
class SHARE_CONTROL_CLASS CDlgInsureMain : public CSkinDialog, public IInsureCounterAction
{
	//��������
public:
	static TCHAR					m_szInsurePass[LEN_MD5];			// MD5���ܺ����������
	static bool						m_bAutoLogOff;						// �رնԻ���ʱ�Զ�ע��
protected:
	bool							m_bSaveMission;						//�������
	bool							m_bTakeMission;						//ȡ������
	bool							m_bQueryMission;					//��ѯ����
	bool							m_bTransferMission;					//ת������
	bool                            m_bTransRecordMission;              //ת�˼�¼
	bool							m_bQueryNickNameMission;			// ��ѯ�ǳ�
	bool							m_bVerifyInsurePassword;			// �����������룬���м��
	bool							m_bModifyInsurePassword;			// �޸���������

	//��ѯ����
protected:
	bool							m_bInitInfo;						//������Ϣ
	BYTE							m_cbInsureMode;						//����ģʽ
	tagUserInsureInfo				m_UserInsureInfo;					//������Ϣ
	HBRUSH							m_hBrush;

	//�����ؼ�
protected:
	CSkinTabCtrl					m_TabControl;						//����ѡ��
	CSkinButton						m_btQueryInfo;						//��ѯ��ť

	//�����
protected:
	CDlgInsureItem					m_DlgInsureItem;					//��������
	CDlgInsurePassword				m_DlgInsurePassword;				// �������������
	CDlgInsureSave					m_DlgInsureSave;					//��ȡ����
	CDlgInsureTransfer				m_DlgInsureTransfer;				//ת������
	CDlgTransRecord					m_DlgTransRecord;					// ת�˼�¼
	CDlgInsureModifyPassword		m_DlgInsureModifyPassword;			// �޸�����

	//��������
public:
	//���캯��
	CDlgInsureMain();
	//��������
	virtual ~CDlgInsureMain();

	//���غ���
protected:
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//��������
	virtual BOOL OnInitDialog();
	//ȷ����ť
	virtual VOID OnOK();

	//���ں���
protected:
	//�滭��Ϣ
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight);

	//�¼�����
public:
	//��ʼ����
	VOID OnMissionStart();
	//��ֹ����
	VOID OnMissionConclude();

	//���ܺ���
protected:
	//��ѯ����
	VOID QueryUserInsureInfo();
	//����ģʽ
	VOID SwitchInsureModeView(BYTE cbInsureMode);

	//��Ϣ����
protected:
	//��ѯ��ť
	VOID OnBnClickedQueryInfo();
	//���͸ı�
	VOID OnTcnSelchangeInsureMode(NMHDR * pNMHDR, LRESULT * pResult);
	// �ı�ؼ���ɫ
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);
	// ���ٴ���
//	afx_msg void OnDestory();
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//�㳡����
class SHARE_CONTROL_CLASS CDlgInsurePlaza : public CDlgInsureMain, public CMissionItem
{
	//��������
protected:
	BYTE                            m_cbByNickName;                      //�ǳ�����
	SCORE							m_lScore;							//��Ϸ����Ŀ
	TCHAR							m_szNickName[LEN_NICKNAME];			//�û��ǳ�
//	TCHAR							m_szInsurePass[LEN_PASSWORD];		//��������
	DWORD							m_dwGameID;

	TCHAR							m_szSrcInsurePass[LEN_PASSWORD];	// Ҫ�޸ĵ�ԭ��������	
	TCHAR							m_szDesInsurePass[LEN_PASSWORD];	// Ҫ�޸ĵ�����������	

	//�������
protected:
	CMissionManager					m_MissionManager;					//�������

	//��������
public:
	//���캯��
	CDlgInsurePlaza();
	//��������
	virtual ~CDlgInsurePlaza();

	//ִ�к���
protected:
	//��ѯ�¼�
	virtual VOID PerformQueryInfo();
	//�����¼�
	virtual VOID PerformSaveScore(SCORE lSaveScore);
	//ȡ���¼�
	virtual VOID PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass);
	//ת���¼�
	virtual VOID PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass);
	// ��ѯת���¼�
    virtual VOID PerformQueryTransRecord();
	// �������GameID��ѯ�ǳ�
	virtual VOID PerformQueryUserNickName(DWORD dwGameID);
	// ������������
	virtual VOID PerformInsurePassword(LPCTSTR pszInsurePass);
	// �޸���������
	virtual VOID PerformModiyInsurePassword(LPCTSTR pszSrcPassword, LPCTSTR pszDesPassword);

	//�����¼�
private:
	//�����¼�
	virtual bool OnEventMissionLink(INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

//��������
class SHARE_CONTROL_CLASS CDlgInsureServer : public CDlgInsureMain
{
	//��������
protected:
	BYTE                            m_cbByNickName;                     //�ǳ�����
	SCORE							m_lScore;							//��Ϸ����Ŀ
	TCHAR							m_szNickName[LEN_NICKNAME];			//�û��ǳ�
//	TCHAR							m_szInsurePass[LEN_PASSWORD];		//��������

	//��������
protected:
	ITCPSocket *					m_pITCPSocket;						//����ӿ�
	IClientUserItem *				m_pIMySelfUserItem;					//�Լ�ָ��

	//��������
public:
	//���캯��
	CDlgInsureServer();
	//��������
	virtual ~CDlgInsureServer();

	//ִ�к���
protected:
	//��ѯ�¼�
	virtual VOID PerformQueryInfo();
	//�����¼�
	virtual VOID PerformSaveScore(SCORE lSaveScore);
	//ȡ���¼�
	virtual VOID PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass);
	//ת���¼�
	virtual VOID PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass);
    // ת�˼�¼
	virtual VOID PerformQueryTransRecord();
	// ����GameID��ѯ����ǳ�
	virtual VOID PerformQueryUserNickName(DWORD dwGameID);
	// ������������
	virtual VOID PerformInsurePassword(LPCTSTR pszInsurePass);
	// �޸���������
	virtual VOID PerformModiyInsurePassword(LPCTSTR pszSrcPassword, LPCTSTR pszDesPassword);
	//���ܺ���
public:
	//��������
	VOID Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem);
	//������Ϣ
	bool OnServerInsureMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////////////

// ת�˳ɹ���ִ����
class CDlgInsureTransferReceipt : public CSkinDialog
{
protected:
	CMD_GR_UserTransferReceipt*	m_pUserTransferReceipt;

	CDlgInsureItem				m_DlgInsureItem;					//��������

	CSkinButton					m_btOk;								// ȷ����ť
public:
	CDlgInsureTransferReceipt(CWnd * pParentWnd=NULL);
	~CDlgInsureTransferReceipt();

public:
	INT ShowReceipt(CMD_GR_UserTransferReceipt * pUserTransferReceipt);
	//��Ϣӳ��
protected:
	//��ʼ����Ϣ
	virtual BOOL OnInitDialog();
	//�滭��Ϣ
	virtual VOID OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight);
	//�ؼ���
	virtual VOID DoDataExchange(CDataExchange * pDX);
protected:
	//�滭����
//	BOOL OnEraseBkgnd(CDC * pDC);
	//�ؼ���ɫ
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);
	// ȷ����ť
	VOID OnBnClickedOk();
	// �رմ���
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////////////

#endif
