#include "Stdafx.h"
#include "Resource.h"
//#include "TestTableView.h"
#include "GlobalUnits.h"
#include "TableViewFrame.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////////////

//���и�ʽ
#define	NT_LEFT						0x01								//�����
#define	NT_RIGHT					0x02								//�Ҷ���

//�����ʽ
#define	NT_TOP						0x10								//�϶���
#define	NT_BOTTOM					0x20								//�¶���

//��������
#define WHELL_TIMES					2									//���ֱ���
#define SCROLL_POS_LINE				10									//��������

#define IDC_BT_JOIN_MATCH			10012								//������ť

//��ʱ��
#define IDI_TICK_TIMER				1									//���涨ʱ

//�ؼ���ʶ
#define IDC_WEB_PUBLICIZE			100									//����ؼ�

//��ťλ��
#define ENTER_MATCH_BUTTON_X		150									//��ťλ��
#define ENTER_MATCH_BUTTON_Y		50									//��ťλ��
#define LEAVE_MATCH_BUTTON_X		200									//��ťλ��
#define LEAVE_MATCH_BUTTON_Y		70									//��ťλ��

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTableViewFrame, CWnd)

	//ϵͳ��Ϣ
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()

	//�Զ���Ϣ
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_PLATFORM_EVENT, OnMessagePlatformEvent)

	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CTableResource::CTableResource()
{
	//���Ա���
	m_bShowUser=true;
	m_bShowChair=true;
	m_bShowTable=true;
	m_bShowFocus=true;
	m_bShowEnter=false;
	m_bCustomUser=false;
	m_lShowTableID=0;
	m_bRenderImage=true;
	m_lDefaultFace=0;

	//��Ŀ����
	m_wTableItemCount=0;
	m_wChairItemCount=0;

	//������Ŀ
	m_wUserGenreCount=0;
	m_wTableGenreCount=0;
	m_wChairGenreCount=0;

	//��ɫ����
	m_crName=RGB(0,0,0);
	m_crMember=RGB(0,0,0);
	m_crMaster=RGB(0,0,0);

	//��������
	m_SizeChair.SetSize(0,0);
	m_SizeTable.SetSize(0,0);
	m_SizeEnter.SetSize(0,0);

	//λ�ñ���
	m_ptLock.SetPoint(0,0);
	m_ptEnter.SetPoint(0,0);
	m_ptTableID.SetPoint(0,0);

	//λ�ñ���
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		m_ptReadyArray[i].SetPoint(0,0);
		m_rcNameArray[i].SetRect(0,0,0,0);
		m_rcChairArray[i].SetRect(0,0,0,0);
		m_ptAvatarArray[i].SetPoint(0,0);  //yao
		m_ptAvatarIndex[i] = 0;
	}

	//��������
	ZeroMemory(m_nDrawStyle,sizeof(m_nDrawStyle));
	ZeroMemory(m_szDirectory,sizeof(m_szDirectory));

	//������Դ
	HINSTANCE hins = AfxGetInstanceHandle();
	m_ImageReady.LoadImage(hins,TEXT("TABLE_READY"));
	m_ImageLocker.LoadImage(hins,TEXT("TABLE_LOCKER"));
	m_ImageNumberBK.LoadImage(hins,TEXT("TABLE_NUMBER_BK"));
	m_ImageNumberNB.LoadImage(hins,TEXT("TABLE_NUMBER_NB"));

	//���ñ���
	m_SizeNumberBK.SetSize(m_ImageNumberBK.GetWidth(),m_ImageNumberBK.GetHeight());
	m_SizeNumberNB.SetSize(m_ImageNumberNB.GetWidth()/10L,m_ImageNumberNB.GetHeight());

	return;
}

//��������
CTableResource::~CTableResource()
{
	//�ͷ���Դ
	DestroyResource();

	return;
}

//�ͷ���Դ
VOID CTableResource::DestroyResource()
{
	//��������
	m_SizeChair.SetSize(0,0);
	m_SizeTable.SetSize(0,0);

	//��Ŀ����
	m_wTableItemCount=0;
	m_wChairItemCount=0;

	//�������
	ZeroMemory(m_nDrawStyle,sizeof(m_nDrawStyle));

	//λ������
	ZeroMemory(&m_ptLock,sizeof(m_ptLock));
	ZeroMemory(m_rcNameArray,sizeof(m_rcNameArray));
	ZeroMemory(m_rcChairArray,sizeof(m_rcChairArray));
	ZeroMemory(m_ptReadyArray,sizeof(m_ptReadyArray));
	ZeroMemory(m_ptAvatarArray,sizeof(m_ptAvatarArray)); //yao
	ZeroMemory(m_ptAvatarIndex,sizeof(m_ptAvatarIndex));

	//��ɫ����
	m_crName=RGB(0,0,0);
	m_crMember=RGB(0,0,0);
	m_crMaster=RGB(0,0,0);

	//�ͷ�λͼ
	m_ImageChair.DestroyImage();
	m_ImageChairBOX.DestroyImage();
	m_ImageTable.DestroyImage();
	m_ImageTableBOX.DestroyImage();
	m_ImageGround.DestroyImage();

	//�ͷ�λͼ
	m_ImageEnter.DestroyImage();
	m_ImageCustomUser.DestroyImage();

	return;
}

//������Դ
bool CTableResource::LoadResource(LPCTSTR pszResDirectory, WORD wChairCount, bool bAvertCheatMode)
{
	//Ч�����
	ASSERT((pszResDirectory!=NULL)&&(wChairCount<=MAX_CHAIR));
	if ((pszResDirectory==NULL)||(wChairCount>MAX_CHAIR)) return false;

	//��Դ·��
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));
	if(bAvertCheatMode)
		_sntprintf(m_szDirectory,CountArray(m_szDirectory),TEXT("%s\\%s\\AvertCheatMode"),szDirectory,pszResDirectory);
	else
		_sntprintf(m_szDirectory,CountArray(m_szDirectory),TEXT("%s\\%s"),szDirectory,pszResDirectory);

	//����·��
	TCHAR szParameterFile[MAX_PATH]=TEXT("");
	_sntprintf(szParameterFile,CountArray(szParameterFile),TEXT("%s\\TableResource.INI"),m_szDirectory);

	//��Դ�ж�
	if(bAvertCheatMode)
	{
		//����·��
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Table.png"),m_szDirectory);

		//������Դ
		CPngImage ImageTable;
		if (ImageTable.LoadImage(szImagePath)==false)
		{
			//��Դ·��
			ZeroMemory(m_szDirectory, sizeof(m_szDirectory));
			_sntprintf(m_szDirectory,CountArray(m_szDirectory),TEXT("%s\\AvertCheatMode"),szDirectory);

			//����·��
			ZeroMemory(szParameterFile, sizeof(szParameterFile));
			_sntprintf(szParameterFile,CountArray(szParameterFile),TEXT("%s\\TableResource.INI"),m_szDirectory);
		}

		//����·��
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\WaitDistribute.bmp"),m_szDirectory);

		//������Դ
		if(FAILED(m_ImageWaitDistribute.Load(szImagePath)))
		{
			return false;
		}
	}

	//��ɫ����
	ReadColor(m_crName,TEXT("Color"),TEXT("Color_Name"),szParameterFile);
	ReadColor(m_crMember,TEXT("Color"),TEXT("Color_Member"),szParameterFile);
	ReadColor(m_crMaster,TEXT("Color"),TEXT("Color_Master"),szParameterFile);

	//λ������
	ReadPoint(m_ptLock,TEXT("Position"),TEXT("Point_Lock"),szParameterFile);
	ReadPoint(m_ptEnter,TEXT("Position"),TEXT("Point_Enter"),szParameterFile);
	ReadPoint(m_ptTableID,TEXT("Position"),TEXT("Point_TableID"),szParameterFile);

	//��ʾ����
	m_bShowUser=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowUser"),1L,szParameterFile)==TRUE);
	m_bShowChair=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowChair"),1L,szParameterFile)==TRUE);
	m_bShowTable=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowTable"),1L,szParameterFile)==TRUE);
	m_bShowFocus=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowFocus"),1L,szParameterFile)==TRUE);
	m_bShowEnter=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowEnter"),0L,szParameterFile)==TRUE);
	m_bCustomUser=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("CustomUser"),0L,szParameterFile)==TRUE);
	m_lShowTableID=GetPrivateProfileInt(TEXT("Attribute"),TEXT("ShowTableID"),1L,szParameterFile);
	m_bRenderImage=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("RenderImage"),1L,szParameterFile)==TRUE);
	m_b3DChair=(GetPrivateProfileInt(TEXT("Attribute"),TEXT("3DChair"),1L,szParameterFile)==FALSE);
	m_lDefaultFace=GetPrivateProfileInt(TEXT("Attribute"),TEXT("DefaultFace"),0L,szParameterFile);

	//��Ŀ����
	m_wTableItemCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("TableItemCount"),2L,szParameterFile);
	m_wChairItemCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("ChairItemCount"),wChairCount,szParameterFile);
	m_wChairCellCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("ChairCellCount"),0,szParameterFile);

	//���ñ�������ȡ���ӵ�ԪͼƬ����
	TCHAR szReadData[1024]=TEXT("");
	ZeroMemory(&m_lChairCellIndex,sizeof(m_lChairCellIndex));

	GetPrivateProfileString(TEXT("Attribute"),TEXT("ChairCellIndex"),TEXT(""),szReadData,CountArray(szReadData),szParameterFile);
	if (szReadData[0]!=0)
	{
		//��ȡ����
		LPCTSTR pszString=szReadData;
		for (WORD i=0; i<m_wChairItemCount*2;i++)
		{
			m_lChairCellIndex[i]=ReadStringValue(pszString);
		}
	}

	//������Ŀ
	m_wUserGenreCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("UserGenreCount"),1L,szParameterFile);
	m_wTableGenreCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("TableGenreCount"),1L,szParameterFile);
	m_wChairGenreCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("ChairGenreCount"),1L,szParameterFile);

	//������Դ
	if(UpdateResource(CSkinRenderManager::GetInstance())==false)
	{
		return false;
	}

	//����ߴ�
	if (m_ImageEnter.IsNull()==false)
	{
		m_SizeEnter.SetSize(m_ImageEnter.GetWidth()/4,m_ImageEnter.GetHeight());
	}

	//���ӳߴ�
	if (m_ImageTable.IsNull()==false)
	{
		m_SizeTable.SetSize(m_ImageTable.GetWidth()/m_wTableItemCount,m_ImageTable.GetHeight()/m_wTableGenreCount);
	}

	//
	//���ӳߴ�
	if (m_ImageChair.IsNull()==false)
	{
		m_SizeChair.SetSize(m_ImageChair.GetWidth()/m_wChairItemCount,m_ImageChair.GetHeight());
	}

	//���ӳߴ�
	if (m_ImageChairBOX.IsNull()==false)
	{
		m_SizeChair.SetSize(m_ImageChairBOX.GetWidth()/m_wChairItemCount,m_ImageChairBOX.GetHeight());
	}

	//�û��ߴ�
	if (m_ImageCustomUser.IsNull()==false)
	{
		m_SizeChair.SetSize(m_ImageCustomUser.GetWidth()/m_wChairItemCount,m_ImageCustomUser.GetHeight()/m_wUserGenreCount);
	}

	//λ������
	if (m_bShowTable==true)
	{
		for (WORD i=0;i<wChairCount;i++)
		{
			//��������
			TCHAR szSubItem[64]=TEXT("");

			//�û�����
			if (m_bShowUser==true)
			{
				//����λ��
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Rect_Name%d"),i+1);
				ReadRect(m_rcNameArray[i],TEXT("Position"),szSubItem,szParameterFile);

				//׼��λ��
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Point_Ready%d"),i+1);
				ReadPoint(m_ptReadyArray[i],TEXT("Position"),szSubItem,szParameterFile);

				//�������
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("DrawStyle%d"),i+1);
				m_nDrawStyle[i]=GetPrivateProfileInt(TEXT("Position"),szSubItem,0L,szParameterFile);

				//����λ��
				POINT ChairPoint;
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Point_Chair%d"),i+1);
				ReadPoint(ChairPoint,TEXT("Position"),szSubItem,szParameterFile);

				//����λ��
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Point_P%d"),i+1);
				ReadPoint(m_ptAvatarArray[i],TEXT("Position"),szSubItem,szParameterFile);

				//ͷ������ 
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Head_Index%d"),i+1);
				m_ptAvatarIndex[i]=GetPrivateProfileInt(TEXT("Position"),szSubItem,0L,szParameterFile);

				//���ӿ��
				m_rcChairArray[i].top=ChairPoint.y;
				m_rcChairArray[i].left=ChairPoint.x;
				m_rcChairArray[i].right=m_rcChairArray[i].left+m_SizeChair.cx;
				m_rcChairArray[i].bottom=m_rcChairArray[i].top+m_SizeChair.cy;
			}
		}
	}

	return true;
}

#ifndef RELEASE_MANAGER
#define MODULE_HANDLEL_NAME	TEXT("GamePlaza.exe")			//�������
#else
#define MODULE_HANDLEL_NAME	TEXT("GameManager.exe")			//�������
#endif
//������Դ
bool CTableResource::UpdateResource(CSkinRenderManager * pSkinRenderManager)
{
//	MessageBox(NULL, m_szDirectory, NULL, MB_OK);
	//����·��
	TCHAR szImageGround[MAX_PATH]=TEXT("");
	_sntprintf(szImageGround,CountArray(szImageGround),TEXT("%s\\Ground.png"),m_szDirectory);
//	MessageBox(NULL, szImageGround, NULL, MB_OK);

	//���ر���
	if (!m_ImageGround.IsNull())
	{
		LOGI("CTableResource::UpdateResource, ����ͼƬ");
		MessageBox(NULL, TEXT("����ͼƬ"), NULL, MB_OK);
		m_ImageGround.DestroyImage();
	}
//	else
//	{
//		MessageBox(NULL, _T("m_ImageGround.IsNull() else"), NULL, MB_OK);
//	}

	//������Դ
	if ( m_ImageGround.LoadImage(szImageGround) == false)
	{
//		LOGI("����ͼƬʧ�ܣ� szImageGround="<<szImageGround);
//		MessageBox(NULL, TEXT("����ͼƬʧ��"), NULL, MB_OK);
		m_ImageBitGround.LoadFromResource(AfxGetInstanceHandle(),IDB_TABLE_GROUND);
	}
	else
	{
//		LOGI("����ͼƬ�ɹ�, szImageGround="<<szImageGround);
//		MessageBox(NULL, TEXT("����ͼƬ�ɹ�"), NULL, MB_OK);
	}

	//��Ⱦ��Դ
	if (m_bRenderImage==true)
	{
		pSkinRenderManager->RenderImage(m_ImageBitGround);
//		pSkinRenderManager->RenderImage(m_ImageGround);
	}

	//����λͼ
	if ((m_bShowEnter==true)&&(m_ImageEnter.IsNull()==true))
	{
		//����·��
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Enter.png"),m_szDirectory);

		//������Դ
		if (m_ImageEnter.LoadImage(szImagePath)==false) return false;
	}

	//�û�λͼ
	if ((m_bCustomUser==true)&&(m_ImageCustomUser.IsNull()==true))
	{
		//����·��
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\CustomUser.png"),m_szDirectory);

		//������Դ
		if (m_ImageCustomUser.LoadImage(szImagePath)==false) return false;
	}

	//��������
	if (m_bShowTable==true)
	{
		//����·��
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Table.png"),m_szDirectory);

		//������Դ
		if (m_ImageTable.LoadImage(szImagePath)==false) return false;
	}

	//�������ӿ�
	if (m_bShowTable==true&&m_bShowFocus==true)
	{
		//����·��
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Table_BOX.png"),m_szDirectory);

		//������Դ
		m_ImageTableBOX.LoadImage(szImagePath);
	}

	//�������ӿ�
	if ((m_bShowChair==true)&&(m_bShowTable==true))
	{
		//����·��
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Chair_BOX.png"),m_szDirectory);

		//������Դ
		m_ImageChairBOX.LoadImage(szImagePath);

		//6603����·��
//		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Chair.png"),m_szDirectory);

		//6603������Դ
		if (m_ImageChair.LoadImage(szImagePath)==false) return false;
	}

	//ȱʡ��ͷ��
	if(m_ImageUserFace_2D.LoadImage(GetModuleHandle(AVATAR_CONTROL_DLL_NAME),TEXT("USER_FACE_IMAGE_2D"))==false) return false;
	if(m_ImageUserFace_3D.LoadImage(GetModuleHandle(AVATAR_CONTROL_DLL_NAME),TEXT("USER_FACE_IMAGE_3D"))==false) return false;

	return true;
}

LONG CTableResource::GetTableCellIndex(WORD wChairID, bool bHasUser)
{
	ASSERT(wChairID<MAX_CHAIR);
	if (wChairID>=MAX_CHAIR) return 0L;

	return m_lChairCellIndex[wChairID*2+(WORD)bHasUser];
};

//��ȡ��ֵ
LONG CTableResource::ReadStringValue(LPCTSTR & pszSring)
{
	//Ч�����
	ASSERT((pszSring!=NULL)&&(pszSring[0]!=0));
	if ((pszSring==NULL)||(pszSring[0]==0)) return 0L;

	//Ѱ�ҿ�ʼ
	bool firstcharisnumber = true;
	while (((pszSring[0]>0)&&(pszSring[0]<TEXT('0')))||(pszSring[0]>TEXT('9')))
	{
		firstcharisnumber = false;
		pszSring++;
	}

	//�Ƿ��и���
	bool neg = false;
	if (!firstcharisnumber)
	{
		if (pszSring[-1] == '-')
			neg = true;
	}

	//��ȡ��ֵ
	LONG lValue=0L;
	while ((pszSring[0]>=TEXT('0'))&&(pszSring[0]<=TEXT('9')))
	{
		lValue=lValue*10L+pszSring[0]-TEXT('0');
		++pszSring;
	}

	if (neg)
		return -lValue;
	else
		return lValue;
}

//��ȡ����
bool CTableResource::ReadRect(RECT & ValueRect, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile)
{
	//���ñ���
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValueRect,sizeof(ValueRect));

	//��ȡ�ַ�
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),pszIniFile);
	if (szReadData[0]!=0)
	{
		//��ȡ����
		LPCTSTR pszString=szReadData;
		ValueRect.left=ReadStringValue(pszString);
		ValueRect.top=ReadStringValue(pszString);
		ValueRect.right=ValueRect.left+ReadStringValue(pszString);
		ValueRect.bottom=ValueRect.top+ReadStringValue(pszString);

		return true;
	}

	return false;
}

//��ȡ����
bool CTableResource::ReadPoint(POINT & ValuePoint, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile)
{
	//���ñ���
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValuePoint,sizeof(ValuePoint));

	//��ȡ�ַ�
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),pszIniFile);
	if (szReadData[0]!=0)
	{
		//��ȡ����
		LPCTSTR pszString=szReadData;
		ValuePoint.x=ReadStringValue(pszString);
		ValuePoint.y=ReadStringValue(pszString);

		return true;
	}

	return false;
}

//��ȡ��ɫ
bool CTableResource::ReadColor(COLORREF & ValueColor, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile)
{
	//���ñ���
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValueColor,sizeof(ValueColor));

	//��ȡ�ַ�
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),pszIniFile);
	if (szReadData[0]!=0)
	{
		//��ȡ����
		LPCTSTR pszString=szReadData;
		ValueColor=RGB(ReadStringValue(pszString),ReadStringValue(pszString),ReadStringValue(pszString));

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CTableView::CTableView()
{
	//����ӿ�
	m_pITableViewFrame=NULL;

	//״̬����
	m_bMouseDown=false;
	m_wHoverChairID=INVALID_CHAIR;
	ZeroMemory(&m_TableAttribute,sizeof(m_TableAttribute));

	return;
}

//��������
CTableView::~CTableView()
{
}

//�ӿڲ�ѯ
VOID * CTableView::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableView,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableView,Guid,dwQueryVer);
	return NULL;
}

//��������
WORD CTableView::GetNullChairCount(WORD & wNullChairID)
{
	//���ñ���
	wNullChairID=INVALID_CHAIR;

	//Ѱ��λ��
	WORD wNullCount=0;
	for (WORD i=0;i<m_TableAttribute.wChairCount;i++)
	{
		if (m_TableAttribute.pIClientUserItem[i]==NULL)
		{
			//������Ŀ
			wNullCount++;

			//���ý��
			if (wNullChairID==INVALID_CHAIR) wNullChairID=i;
		}
	}

	return wNullCount;
}

//���ú���
VOID CTableView::InitTableView(WORD wTableID, WORD wChairCount, TCHAR szTableName[LEN_TABLENAME], ITableViewFrame * pITableViewFrame)
{
	//��������
	m_TableAttribute.wTableID=wTableID;
	m_TableAttribute.wChairCount=wChairCount;
	CopyMemory(m_TableAttribute.szTableName, szTableName, CountArray(m_TableAttribute.szTableName));

	//���ýӿ�
	m_pITableViewFrame=QUERY_OBJECT_PTR_INTERFACE(pITableViewFrame,ITableViewFrame);

	return;
}

//��ȡ�û�
IClientUserItem * CTableView::GetClientUserItem(WORD wChairID)
{
	//Ч�����
	ASSERT(wChairID<m_TableAttribute.wChairCount);
	if (wChairID>=m_TableAttribute.wChairCount) return NULL;

	//��ȡ�û�
	return m_TableAttribute.pIClientUserItem[wChairID];
}

//������Ϣ
bool CTableView::SetClientUserItem(WORD wChairID, IClientUserItem * pIClientUserItem)
{
	//Ч�����
	ASSERT(wChairID<m_TableAttribute.wChairCount);
	if (wChairID>=m_TableAttribute.wChairCount) return false;

	//�����û�
	m_TableAttribute.pIClientUserItem[wChairID]=pIClientUserItem;

	//���½���
	m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);

	return true;
}

//��갴��
VOID CTableView::SetMouseDown(bool bMouseDown)
{
	//���ñ�־
	if (m_bMouseDown!=bMouseDown)
	{
		//���ñ���
		m_bMouseDown=bMouseDown; 

		//���½���
		m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);
	}

	return;
}

//������
VOID CTableView::SetFocusFrame(bool bFocusFrame)
{
	//���ñ�־
	if (m_TableAttribute.bFocusFrame!=bFocusFrame)
	{
		//���ñ���
		m_TableAttribute.bFocusFrame=bFocusFrame;

		//���½���
		m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);
	}

	return;
}

//����λ��
VOID CTableView::SetHoverStatus(WORD wHoverChairID)
{
	//���ñ�־
	if (m_wHoverChairID!=wHoverChairID)
	{
		//���ñ���
		m_wHoverChairID=wHoverChairID;

		//���½���
		m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);
	}

	return;
}

//����״̬ 
VOID CTableView::SetTableStatus(bool bPlaying, bool bLocker)
{
	//���ñ�־
	if ((m_TableAttribute.bLocker!=bLocker)||(m_TableAttribute.bPlaying!=bPlaying))
	{
		//���ñ���
		m_TableAttribute.bLocker=bLocker; 
		m_TableAttribute.bPlaying=bPlaying;

		//���½���
		m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);
	}

	return;
}

//#define SINGLE

//�滭����
VOID CTableView::DrawTableView(CDC * pDC, INT nXPos, INT nYPos, DWORD dwServerRule, CTableResource * pTableResource)
{
	CSize SizeChair=pTableResource->m_SizeChair;
	CSize SizeTable=pTableResource->m_SizeTable;

	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//��������
	bool bMySelfTable=false;
	bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(dwServerRule);

#ifdef SINGLE
	m_TableAttribute.bPlaying=true;
#endif

	//�滭����
	if ((bAvertCheatMode==false)||(m_TableAttribute.wTableID==0))
	{
		INT nXPosTable=(m_TableAttribute.bPlaying==true)?SizeTable.cx:0;
		pTableResource->m_ImageTable.DrawImage(pDC,nXPos,nYPos,SizeTable.cx,SizeTable.cy,nXPosTable,0);
	}
	else
	{
		INT nXPosTable=SizeTable.cx;
		pTableResource->m_ImageTable.DrawImage(pDC,nXPos,nYPos,SizeTable.cx,SizeTable.cy,nXPosTable,0);
	}

	//�滭����
	if ((pTableResource->m_bShowEnter==true)&&(pTableResource->m_ImageEnter.IsNull()==false))
	{
		//����λ��
		INT nImageIndex=0;
		if (m_TableAttribute.bPlaying==true) nImageIndex=1;
		if ((m_wHoverChairID==INDEX_ENTER_CHAIR)&&(m_TableAttribute.bPlaying==false)&&(m_bMouseDown==true)) nImageIndex=2;
		if ((m_wHoverChairID==INDEX_ENTER_CHAIR)&&(m_TableAttribute.bPlaying==false)&&(m_bMouseDown==false)) nImageIndex=3;

		//�滭��־
		CSize SizeEnter=pTableResource->m_SizeEnter;
		pTableResource->m_ImageEnter.DrawImage(pDC,pTableResource->m_ptEnter.x + nXPos,pTableResource->m_ptEnter.y + nYPos,SizeEnter.cx,SizeEnter.cy,
			SizeEnter.cx*nImageIndex,0);
	}

	//������Ϣ
	if(m_TableAttribute.wChairCount < MAX_CHAIR)
	{
		for (WORD i=0;i<m_TableAttribute.wChairCount;i++)
		{
			//��������
			CRect rcChair=pTableResource->m_rcChairArray[i];

			//�滭�û�
			if (pTableResource->m_bShowUser==true)
			{
				//6603�滭����
				if ((!bAvertCheatMode)&&(pTableResource->m_bShowChair))
				{
					ASSERT(pTableResource->m_ImageChair.IsNull()==false);
					LONG lIndex=pTableResource->GetTableCellIndex(i,m_TableAttribute.pIClientUserItem[i]!=NULL);
					if (lIndex!=-1)
						pTableResource->m_ImageChair.DrawImage(pDC,nXPos+rcChair.left,nYPos+rcChair.top,SizeChair.cx,SizeChair.cy,SizeChair.cx*lIndex,0);
				}

				//�滭��������
				if ((bAvertCheatMode==false)||(m_TableAttribute.wTableID==0))
				{
					//�Զ��߿� 
					if (m_wHoverChairID==i)
					{
						if ( !pTableResource->m_ImageChairBOX.IsNull() )
						{
							LONG lIndex=pTableResource->GetTableCellIndex(i,m_TableAttribute.pIClientUserItem[i]!=NULL);
							if (lIndex!=-1)
								pTableResource->m_ImageChairBOX.DrawImage(pDC, rcChair.left + nXPos, rcChair.top + nYPos,SizeChair.cx,SizeChair.cy,SizeChair.cx*lIndex,0);
						}
					}
				}

				if ((bAvertCheatMode==false) &&(m_TableAttribute.pIClientUserItem[i]!=NULL))
				{

					//��ȡ�û�
					IClientUserItem * pIClientUserItem=m_TableAttribute.pIClientUserItem[i];
					tagCustomFaceInfo * pCustomFaceInfo=pIClientUserItem->GetCustomFaceInfo();

					//6603�滭����
					if ((bAvertCheatMode==true)&&(pTableResource->m_bShowChair==true)&&(m_TableAttribute.wTableID==0))
					{
						ASSERT(pTableResource->m_ImageChair.IsNull()==false);
						ASSERT(0);
						pTableResource->m_ImageChair.DrawImage(pDC,rcChair.left,rcChair.top,SizeChair.cx,SizeChair.cy,SizeChair.cx*i,0);
					}

					bMySelfTable=(m_TableAttribute.pIClientUserItem[i]->GetUserID()==pGlobalUserData->dwUserID);

					//�滭ͷ��
					DrawUserAvatar(pDC,nXPos,nYPos,i,pIClientUserItem,pTableResource);

					//ͬλ�ж�
					WORD wTableID=pIClientUserItem->GetTableID();
					WORD wChairID=pIClientUserItem->GetChairID();
					bool bSameStation=(wTableID==m_TableAttribute.wTableID)&&(wChairID==i);

					//ͬ���־
					if ((bSameStation==true)&&(m_TableAttribute.bPlaying==false)&&((pIClientUserItem->GetUserStatus()==US_READY)))
					{
						INT nXPosT=pTableResource->m_ptReadyArray[i].x;
						INT nYPosT=pTableResource->m_ptReadyArray[i].y;
						pTableResource->m_ImageReady.DrawImage(pDC,nXPosT+nXPos,nYPosT+nYPos);
					}

					//��������
					WORD wGoldLevel=m_pITableViewFrame->GetGameLevelParser()->GetGameLevel(pIClientUserItem->GetUserScore());

					COLORREF crName = RGB(255,255,255);
					if (pIClientUserItem->GetMemberOrder())
						crName = RGB(255,50,50);

					//�û�����
					DrawTableName(pDC, nXPos, nYPos, i, pIClientUserItem->GetNickName(), pTableResource, crName);
				}

				//����ģʽ
				if ((bAvertCheatMode==true)&&(pTableResource->m_bShowUser==true))
				{
					//�û�ͷ��
					if (m_TableAttribute.wTableID>0)
					{
						//�滭ͷ��
						if (pTableResource->m_bCustomUser==false)
						{
							DrawUserAvatar(pDC,nXPos,nYPos,((m_TableAttribute.wTableID*m_TableAttribute.wChairCount)+i)%26, i%pTableResource->m_wChairItemCount, pTableResource);
						}
						else
						{
							//�Զ�ͷ��
							INT nXImagePos=i*rcChair.Width();
							INT nYImagePos=(((m_TableAttribute.wTableID*m_TableAttribute.wChairCount)+i)%2==GENDER_FEMALE)?rcChair.Height():0;
							pTableResource->m_ImageCustomUser.DrawImage(pDC,rcChair.left,rcChair.top,rcChair.Width(),rcChair.Height(),nXImagePos,nYImagePos);
						}
					}

					//�û�����
					WORD wTableID=m_TableAttribute.wTableID;
					DrawTableName(pDC,nXPos, nYPos,i,(wTableID==0)?TEXT("�������"):TEXT("����λ��"),pTableResource, pTableResource->m_crName);
				}

			}
		}
	}

	//�滭��ͼ
#ifdef SINGLE
#else
	if (m_TableAttribute.bLocker==true)
#endif
	{
		INT nXPosT=pTableResource->m_ptLock.x;
		INT nYPosT=pTableResource->m_ptLock.y;
		pTableResource->m_ImageLocker.DrawImage(pDC,nXPosT+nXPos,nYPosT+nYPos);
	}

	// ����ģʽ����ʾ����
	if (pTableResource->m_bShowEnter==false)
	{
		//���Ӻ���
		//0����ʾ���������
		if (pTableResource->m_lShowTableID==0)
		{
			//��ȡ��С
			CSize SizeNumberBK=pTableResource->m_SizeNumberBK;
			CSize SizeNumberNB=pTableResource->m_SizeNumberNB;

			//��������
			WORD wItemCount=0;
			WORD wNumberTemp=m_TableAttribute.wTableID+1;
			WORD wTableNumber=m_TableAttribute.wTableID+1;

			//������Ŀ
			do
			{
				wItemCount++;
				wNumberTemp/=10;
			} while (wNumberTemp>0);

			//�滭����
			INT nXNumberBK=pTableResource->m_ptTableID.x-SizeNumberBK.cx/2;
			INT nYNumberBK=pTableResource->m_ptTableID.y-SizeNumberBK.cy/2;
			pTableResource->m_ImageNumberBK.DrawImage(pDC,nXNumberBK+nXPos,nYNumberBK+nYPos);

			//λ�ö���
			INT nYNumberNB=pTableResource->m_ptTableID.y-SizeNumberNB.cy/2;
			INT nXNumberNB=pTableResource->m_ptTableID.x+(wItemCount*SizeNumberNB.cx)/2-SizeNumberNB.cx;

			//�滭����
			for (INT i=0;i<wItemCount;i++)
			{
				//�滭����
				WORD wNumber=wTableNumber%10;
				pTableResource->m_ImageNumberNB.DrawImage(pDC,nXNumberNB+nXPos,nYNumberNB+nYPos,SizeNumberNB.cx,SizeNumberNB.cy,wNumber*SizeNumberNB.cx,0);

				//���ñ���
				wTableNumber/=10;
				nXNumberNB-=SizeNumberNB.cx;
			};
		}
		else
		{
			CString str;
			if (lstrlen(m_TableAttribute.szTableName) == 0)
			{
				str.Format(TEXT("- %ld -"), m_TableAttribute.wTableID+1, m_TableAttribute.szTableName);
			}
			else
			{
				str.Format(TEXT("- %ld. %s -"), m_TableAttribute.wTableID+1, m_TableAttribute.szTableName);
			}
			
			pDC->SetBkMode(TRANSPARENT);
			CRect rcDrawRect(CPoint(nXPos, nYPos + pTableResource->m_ptTableID.y), CSize(SizeTable.cx, 22));
			pDC->SetTextColor(RGB(255,255,255));
			pDC->DrawText(str,&rcDrawRect,DT_SINGLELINE|DT_VCENTER|DT_CENTER);

	//		pDC->FrameRect(rcDrawRect, &CBrush(RGB(255, 255, 255)));
		}
	}

	//�滭����
	if ((pTableResource->m_bShowFocus==true)&&(pParameterGlobal->m_bSalienceTable==true)&&(bMySelfTable==true))
	{
 		if( !pTableResource->m_ImageTableBOX.IsNull() )
 			pTableResource->m_ImageTableBOX.DrawImage(pDC, nXPos, nYPos);
//		LOGI("CTableView::DrawTableView(),SizeTable.cx="<<SizeTable.cx<<",SizeTable.cy="<<SizeTable.cy);
//		pDC->Draw3dRect(0,0,SizeTable.cx,SizeTable.cy,RGB(255,0,0),RGB(0,0,255));
	}

	//������
// #ifdef SINGLE
// 	CRect rcFrame = CRect(CPoint(nXPos, nYPos), SizeTable); 
// 	pDC->FrameRect(rcFrame, &CBrush(RGB(255, 0, 255))); 
// #endif

	return;
}

CRect ToReal(CRect orc, CRect rc, int alignpoint, DWORD& style)
{
	CRect rcret = rc;
	style = DT_LEFT|DT_TOP;
	switch (alignpoint)
	{
	case 0:
		{
			rcret = rc;
			style = DT_LEFT | DT_TOP;
		}
		break;
	case 1:
		{
			rcret = CRect(CPoint(orc.left + (orc.Width() - rc.Width()) / 2, orc.top), CSize(rc.Width(), rc.Height()));
			style = DT_CENTER | DT_TOP;
		}
		break;
	case 2:	
		{
			rcret = CRect(CPoint(orc.right - rc.Width(), rc.top), CSize(rc.Width(), rc.Height())); 
			style = DT_RIGHT | DT_TOP;
		}
		break;
	case 3: 
		{
			rcret = CRect(CPoint(orc.left, orc.top + (orc.Height() - rc.Height()) / 2), CSize(rc.Width(), rc.Height())); 
			style = DT_LEFT | DT_VCENTER;
		}
		break;
	case 4:
		{
			rcret = CRect(CPoint(orc.left + (orc.Width() - rc.Width()) / 2, orc.top + (orc.Height() - rc.Height()) / 2), CSize(rc.Width(), rc.Height()));
			style = DT_CENTER | DT_VCENTER;
		}	
		break;
	case 5:
		{
			rcret = CRect(CPoint(orc.right - rc.Width(), orc.top + (orc.Height() - rc.Height()) / 2), CSize(rc.Width(), rc.Height()));
			style = DT_RIGHT | DT_VCENTER;
		}
		break;
	case 6:	
		{
			rcret = CRect(CPoint(orc.left, orc.top + orc.Height() - rc.Height()), CSize(rc.Width(), rc.Height())); 
			style = DT_LEFT | DT_BOTTOM;
		}
		break;
	case 7:
		{
//			if (orc.Width() < rc.Width())
//			{
//				int linecount = rc.Width()/orc.Width() + 1;
//				rcret = CRect(CPoint(orc.left, orc.top + orc.Height() - (rc.Height()*linecount+3*(linecount-1)) ), CSize(orc.Width(), rc.Height()*linecount+3*(linecount-1)));
//			}
//			else
//			{
				rcret = CRect(CPoint(orc.left + (orc.Width() - rc.Width()) / 2, orc.top + orc.Height() - rc.Height()), CSize(rc.Width(), rc.Height()));
//			}
			style = DT_CENTER | DT_BOTTOM;
		}
		break;
	case 8:
		{
			rcret = CRect(CPoint(orc.right - rc.Width(), orc.top + orc.Height() - rc.Height()), CSize(rc.Width(), rc.Height()));
			style = DT_RIGHT | DT_BOTTOM;
		}
		break;
	default:
		{
			ASSERT(0);
		}
		break;
	}

	return rcret;
};

// void DrawAlginText(CDC* pDC, CRect rc, CString str, int point)
// {
// 	CRect temprc = rc;
// 
// 	DWORD style = 0;
// 	ToReal(rc, rc, point, style);
// 	pDC->DrawText(str, temprc, DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_RIGHT|DT_CALCRECT|style);
// 
// 	pDC->FrameRect(temprc, &CBrush(RGB(255, 0, 255)));
// 	temprc = ToReal(rc, temprc, point, style);
// 	pDC->FrameRect(rc, &CBrush(RGB(255, 0, 255)));
// 
// 	pDC->DrawText(str, temprc, DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|style);
// }; 

//�滭����
VOID CTableView::DrawTableName(CDC * pDC, INT nXPos, INT nYPos, WORD wChairID, LPCTSTR pszNickName, CTableResource * pTableResource, COLORREF crTextColor)
{
	//�û�����
	CRect rcName=pTableResource->m_rcNameArray[wChairID];
	CRect rcDrawName=rcName;
//	CRect rcDrawName=CRect(CPoint(rcName.left, rcName.top), pDC->GetTextExtent(pszNickName));
	INT nDrawStyte=pTableResource->m_nDrawStyle[wChairID];

	DWORD style = 0;
//	ToReal(rcName, rcDrawName, nDrawStyte, style);
//	pDC->DrawText(pszNickName, rcDrawName, DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_RIGHT|DT_CALCRECT|style);

//	LOGI("CTableView::DrawTableName, pszNickName="<<pszNickName<<", nDrawStyte="<<nDrawStyte<<", nXPos="<<nXPos<<", nYPos="<<nYPos);
//	LOGI("CTableView::DrawTableName, rcName.left="<<rcName.left<<", rcName.top="<<rcName.top<<", rcName.Width="<<rcName.Width()<<", rcName.Height="<<rcName.Height());
//	LOGI("CTableView::DrawTableName, rcDrawName.left="<<rcDrawName.left<<", rcDrawName.top="<<rcDrawName.top<<", rcDrawName.Width="<<rcDrawName.Width()<<", rcDrawName.Height="<<rcDrawName.Height());
	rcDrawName = ToReal(rcName, rcDrawName, nDrawStyte, style);

//	LOGI("CTableView::DrawTableName, rcDrawName.left="<<rcDrawName.left<<", rcDrawName.top="<<rcDrawName.top<<", rcDrawName.Width="<<rcDrawName.Width()<<", rcDrawName.Height="<<rcDrawName.Height());

	rcDrawName.OffsetRect(nXPos, nYPos);
//	LOGI("CTableView::DrawTableName, rcDrawName.left="<<rcDrawName.left<<", rcDrawName.top="<<rcDrawName.top<<", rcDrawName.Width="<<rcDrawName.Width()<<", rcDrawName.Height="<<rcDrawName.Height());
//	COLORREF colOld = pDC->SetTextColor(crTextColor);
//	pDC->DrawText(pszNickName, rcDrawName, DT_WORDBREAK|DT_END_ELLIPSIS|DT_EDITCONTROL|DT_CALCRECT|style);
//	pDC->SetTextColor(colOld);
	pDC->SetTextColor(crTextColor); 
	pDC->DrawText(pszNickName,rcDrawName,DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|style);

//	rcName.OffsetRect(nXPos, nYPos);
//	pDC->FrameRect(rcName, &CBrush(RGB(255, 0, 255)));


	//�û�����
// 	CRect rcName=pTableResource->m_rcNameArray[wChairID];
// 
// 	//��ͼ����
// 	CRect rcDrawRect=rcName;
// 	INT nNameLength=lstrlen(pszNickName);
// 	INT nDrawStyte=pTableResource->m_nDrawStyle[wChairID];
// 	INT nDrawFormat=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS;
// 	INT nNameHeight=nNameLength*12;
// 
// 	//����λ��
// 	if (nNameHeight<=12)
// 	{
// 		if (nDrawStyte&NT_LEFT) nDrawFormat|=DT_LEFT;
// 		else if (nDrawStyte&NT_RIGHT) nDrawFormat|=DT_RIGHT;
// 		else nDrawFormat|=(DT_CENTER|DT_SINGLELINE);
// 	}
// 	else nDrawFormat|=DT_LEFT;
// 
// 	//����λ��
// 	INT nYPosT=0;
// 	if (nDrawStyte&NT_BOTTOM) nYPosT=rcName.Height()-nNameHeight;
// 	else if (nDrawStyte&NT_TOP) nYPosT=0;
// 	else nYPosT=(rcName.Height()-nNameHeight)/2;
// 
// 	//����λ��
// 	rcDrawRect=rcName;
// 	rcDrawRect.left=rcName.left;
// 	rcDrawRect.right=rcName.right;
// 	rcDrawRect.top=__max(rcName.top,rcName.top+nYPosT);
// 	rcDrawRect.bottom=__min(rcName.bottom,rcDrawRect.top+nNameHeight);
// 
// 	rcDrawRect.OffsetRect(nXPos, nYPos);
// 
// 	//�û�����
// 	m_pDrawFont->DrawText(pDC,pszNickName,&rcDrawRect,crTextColor,nDrawFormat);

	return;
}

//�滭ͷ��
VOID CTableView::DrawUserAvatar(CDC * pDC, INT nXPos, INT nYPos, WORD wChairID, IClientUserItem * pIClientUserItem, CTableResource * pTableResource)
{
	WORD wDrawChairID=0, wGoldLevel=0, wFaceID=0;
	if (pIClientUserItem==NULL)
	{
		wDrawChairID=wChairID;
		wFaceID=rand()%12;
	}
	else
	{
		if (pTableResource->m_lDefaultFace==2)
		{
			//��������
			wDrawChairID=(wChairID)%m_TableAttribute.wChairCount;
			wGoldLevel=m_pITableViewFrame->GetGameLevelParser()->GetGameLevel(pIClientUserItem->GetUserScore());

//			LOGI("CTableView::DrawUserAvatar, wGoldLevel="<<wGoldLevel);
			//��������
			wFaceID=(wGoldLevel+12*((pIClientUserItem->GetGender()==GENDER_FEMALE)?0:1));
		}
		else if(pTableResource->m_lDefaultFace==3)
		{
		
		}
		else
		{
			//���������ѡ�����Զ���ͷ��
		}
	}

	//�滭ͷ��
	DrawUserAvatar( pDC, nXPos, nYPos, wFaceID, wDrawChairID, pTableResource );

	return;
}

VOID CTableView::DrawUserAvatar( CDC * pDC, INT nXPos, INT nYPos, WORD wFaceID, WORD wChairID, CTableResource * pTableResource )
{
	//����ȱʡ��3Dͷ��
	if (pTableResource->m_lDefaultFace==3)
	{
		//���ñ���
		WORD wLineCount=pTableResource->m_ImageUserFace_3D.GetWidth()/(FACE_CX_3D*ONE_FACE_COUNT_3D);
		WORD wFullCount=wLineCount*pTableResource->m_ImageUserFace_3D.GetHeight()/FACE_CY_3D;
		int nOffHeight = (wFaceID / 2) * FACE_CY_3D;
		int nOffWidth = (wFaceID % 2) * (FACE_CX_3D * ONE_FACE_COUNT_3D);

		//���ñ���
		nOffWidth += pTableResource->m_ptAvatarIndex[wChairID] * FACE_CX_3D;
		nOffWidth += ((m_TableAttribute.bPlaying==true)?(FACE_CX_3D*4):0);

		//�滭����
		pTableResource->m_ImageUserFace_3D.DrawImage(pDC,pTableResource->m_ptAvatarArray[wChairID].x + nXPos,
			pTableResource->m_ptAvatarArray[wChairID].y + nYPos,FACE_CX_3D,FACE_CY_3D,nOffWidth,nOffHeight);
	}
	//����ȱʡ��2Dͷ��
	else if(pTableResource->m_lDefaultFace==2)
	{
//		LOGI("CTableView::DrawUserAvatar, wFaceID="<<wFaceID);
		//���ñ���
		WORD ww = pTableResource->m_ImageUserFace_2D.GetWidth();
		WORD wLineCount=pTableResource->m_ImageUserFace_2D.GetWidth()/FACE_CX_2D;
		int nOffWidth = wFaceID % 12 * FACE_CX_2D;
		int nOffHeight = wFaceID / 12 * FACE_CY_2D * 2;

		//�滭����
		pTableResource->m_ImageUserFace_2D.DrawImage(pDC,pTableResource->m_ptAvatarArray[wChairID].x + nXPos,
			pTableResource->m_ptAvatarArray[wChairID].y + nYPos,FACE_CX_2D,FACE_CY_2D,nOffWidth,nOffHeight);
		
	}
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CTableViewFrame::CTableViewFrame()
{
	//״̬����
	m_bHovering=false;
	m_bLMouseDown=false;
	m_bRMouseDown=false;
	m_bWaitDistribute=false;
	m_cbButtonDown=0;
	m_cbButtonHover=0;

	//���Ա���
	m_wTableCount=0;
	m_wChairCount=0;
	m_dwServerRule=0;
	m_wServerType=0;

	//������Ϣ
	m_nXExcursion=0;
	m_nYExcursion=0;
	m_nXTableCount=0;
	m_nYTableCount=0;

	//������Ϣ
	m_nScrollPos=0;
	m_nScrollPosMax=0;	
	m_nVisibleLineCount=0;

	//��������
	m_wDownChairID=INVALID_CHAIR;
	m_wDownTableID=INVALID_TABLE;

	//����ָ��
	m_pITableViewFrameSink=NULL;

	//��������
	m_dwTotalUser=0;
	m_dwWaitting=0;
	m_cbMatchStatus=MS_NULL;
	m_wTipTableID=INVALID_TABLE;
	m_wTipChairID=INVALID_CHAIR;
	m_wLastTipChairID=INVALID_CHAIR;

	return;
}

//��������
CTableViewFrame::~CTableViewFrame()
{
	//ɾ������
	for (INT_PTR i=0;i<m_TableViewArray.GetCount();i++)
	{
		CTableView * pTableView=m_TableViewArray[i];
		if (pTableView!=NULL) SafeDelete(pTableView);
	}

	//ɾ������
	m_TableViewArray.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CTableViewFrame::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableViewFrame,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableViewFrame,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CTableViewFrame::CreateTableFrame(CWnd * pParentWnd, UINT uWndID, IUnknownEx * pIUnknownEx)
{
	//���ñ���
	m_bHovering=false;
	m_bLMouseDown=false;
	m_bRMouseDown=false;
	m_wDownChairID=INVALID_CHAIR;
	m_wDownTableID=INVALID_TABLE;
	m_cbButtonDown=0;
	m_cbButtonHover=0;

	//���ýӿ�
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableViewFrameSink)!=NULL);
	m_pITableViewFrameSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableViewFrameSink);

	//��������
	CRect rcCreate(0,0,0,0);
	Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,pParentWnd,uWndID);

	//��������
	m_DrawFont.CreateFont(this, TEXT("����"), 12, 400);

	//�����ؼ�
	//m_DlgTipUserInfo.Create(IDD_DLG_TIP_USERINFOR,this);

    m_UserInfoDlgEx.Create(IDD_USER_INFO,this);
	return true;
}

//���ú���
bool CTableViewFrame::ConfigTableFrame(WORD wTableCount, WORD wChairCount, TCHAR(* szTableNameArray)[LEN_TABLENAME], DWORD dwServerRule, WORD wServerType, WORD wServerID, LPCTSTR pszResDirectory)
{
	//Ч�����
	ASSERT(wChairCount<=MAX_CHAIR);
	ASSERT((pszResDirectory!=NULL)&&(pszResDirectory[0]!=0));

	//���ù���
	m_SkinScrollBar.InitScroolBar(this);

	//�������
	bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(dwServerRule);

	//������Դ
	if (m_TableResource.LoadResource(pszResDirectory,wChairCount, bAvertCheatMode)==false)
	{
		return false;
	}

	//���ñ���
	m_wTableCount=wTableCount;
	m_wChairCount=wChairCount;
	m_dwServerRule=dwServerRule;
	m_wServerType=wServerType;
	m_TableViewArray.SetSize(m_wTableCount);
	ZeroMemory(m_TableViewArray.GetData(),m_wTableCount*sizeof(CTableView *));

	//��������
	for (WORD i=0;i<m_wTableCount;i++)
	{
		m_TableViewArray[i]=new CTableView;
		m_TableViewArray[i]->InitTableView(i,wChairCount, szTableNameArray[i], this);
		m_TableViewArray[i]->SetFont(&m_DrawFont);
	}

	//����λ��
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	//ע���¼�
	CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
	if (pPlatformEvent!=NULL) pPlatformEvent->RegisterEventWnd(m_hWnd);

	if((m_wServerType&GAME_GENRE_MATCH)!=0)
	{
		HINSTANCE hInstance=AfxGetInstanceHandle();
		m_ImageEnterMatch.LoadFromResource(hInstance,IDB_MATCH_BT_JOIN);
		m_ImageLeaveMatch.LoadFromResource(hInstance,IDB_MATCH_BT_LEAVE);
		m_ImageTableTL.LoadFromResource(hInstance,IDB_MATCH_TABLE_TL);
		m_ImageTableTM.LoadFromResource(hInstance,IDB_MATCH_TABLE_TM);
		m_ImageTableTR.LoadFromResource(hInstance,IDB_MATCH_TABLE_TR);

		m_ImageNumberGreen.LoadImage(hInstance, TEXT("MATCH_NUM_GREEN"));
		m_ImageNumberYellow.LoadImage(hInstance, TEXT("MATCH_NUM_ORANGE"));
		m_ImageLine.LoadImage(hInstance,TEXT("MATCH_LINE"));
		
		//���ؼ�
		CRect rcCreate(0,0,0,0);
		m_BrowseMatchInfo.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,rcCreate,this,IDC_WEB_PUBLICIZE);

		//�����ַ
		TCHAR szBillUrl[256]=TEXT("");
		_sntprintf(szBillUrl,CountArray(szBillUrl),TEXT("%s/Ads/MatchInfo.aspx?ServerID=%d"),szPlatformLink,wServerID);

		//���ù��
		m_BrowseMatchInfo.Navigate(szBillUrl);
		m_BrowseMatchInfo.SetWindowPos(NULL,0,m_ImageTableTL.GetHeight(),rcClient.Width(),rcClient.Height()-m_ImageTableTL.GetHeight(),SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE);

		//����·��
		TCHAR szPath[MAX_PATH]=TEXT("");
		GetCurrentDirectory(MAX_PATH,szPath);
		_sntprintf(szBillUrl,CountArray(szBillUrl),TEXT("%s\\%s\\MatchInfo\\Match%d.html"),szPath,pszResDirectory,wServerID);
		m_BrowseMatchInfo.SetSpareURL(szBillUrl);
	}

	return true;
}

//��ȡ�û�
IClientUserItem * CTableViewFrame::GetClientUserItem(WORD wTableID, WORD wChairID)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//��ȡ�û�
	if (pITableView!=NULL)
	{
		return pITableView->GetClientUserItem(wChairID);
	}

	return NULL;
}

//������Ϣ
bool CTableViewFrame::SetClientUserItem(WORD wTableID, WORD wChairID, IClientUserItem * pIClientUserItem)
{
	ITableView * pITableView=GetTableViewItem(wTableID);
	if (pITableView!=NULL) pITableView->SetClientUserItem(wChairID,pIClientUserItem);
	return true;
}

//��Ϸ��־
bool CTableViewFrame::GetPlayFlag(WORD wTableID)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//��ȡ��־
	if (pITableView!=NULL)
	{
		return pITableView->GetPlayFlag();
	}

	return false;
}

//�����־
bool CTableViewFrame::GetLockerFlag(WORD wTableID)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//��ȡ��־
	if (pITableView!=NULL)
	{
		return pITableView->GetLockerFlag();
	}

	return false;
}

//������
VOID CTableViewFrame::SetFocusFrame(WORD wTableID, bool bFocusFrame)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//���ñ�־
	if (pITableView!=NULL) pITableView->SetFocusFrame(bFocusFrame);

	return;
}

//����״̬ 
VOID CTableViewFrame::SetTableStatus(WORD wTableID, bool bPlaying, bool bLocker)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//���ñ�־
	if (pITableView!=NULL) pITableView->SetTableStatus(bPlaying,bLocker);

	return;
}

//����״̬ 
VOID CTableViewFrame::SetTableStatus(bool bWaitDistribute)
{
	m_bWaitDistribute=bWaitDistribute;
	RedrawWindow();
	return;
}

//���ӿ���
bool CTableViewFrame::VisibleTable(WORD wTableID)
{
	//Ч�����
	ASSERT(wTableID<m_wTableCount);
	if (wTableID>=m_wTableCount) return false;

	//����λ��
	INT nLastPos=m_nScrollPos;
	INT nXPos=(wTableID%m_nXTableCount)*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	INT nYPos=(wTableID/m_nXTableCount)*m_TableResource.m_SizeTable.cy-m_nScrollPos;

	//����λ��
	CRect rcTable,rcClient;
	GetClientRect(&rcClient);
	rcTable.SetRect(nXPos,nYPos,nXPos+m_TableResource.m_SizeTable.cx,nYPos+m_TableResource.m_SizeTable.cy);

	//λ�õ���
	if (rcTable.top<0L) m_nScrollPos+=rcTable.top;
	if ((rcTable.top>0)&&(rcTable.bottom>rcClient.bottom)) m_nScrollPos+=rcTable.bottom-rcClient.bottom;
	m_nScrollPos=__max(0,__min(m_nScrollPos,m_nScrollPosMax-m_nVisibleLineCount*m_TableResource.m_SizeTable.cy));

	//������ͼ
	if (nLastPos!=m_nScrollPos)
	{
		SetScrollPos(SB_VERT,m_nScrollPos);
		ScrollWindow(0,nLastPos-m_nScrollPos,NULL,NULL);
		InvalidateRect(NULL);	
	}

	return true;
}

//��������
bool CTableViewFrame::FlashGameTable(WORD wTableID)
{
	//��ȡ����
	ITableView * pITableView=GetTableViewItem(wTableID);

	//�����ж�
	if (pITableView==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	INT nXItem=wTableID%m_nXTableCount;
	INT nYItem=wTableID/m_nXTableCount;

	//����λ��
	CRect rcTable;
	rcTable.left=nXItem*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	rcTable.top=nYItem*m_TableResource.m_SizeTable.cy-m_nScrollPos;
	rcTable.right=(nXItem+1)*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	rcTable.bottom=(nYItem+1)*m_TableResource.m_SizeTable.cy+m_nYExcursion-m_nScrollPos;
	
	//�������
	INT nFlashTimes=20,nStepWidth=4;
	INT nExpandPos=nFlashTimes*nStepWidth;
	rcTable.InflateRect(nExpandPos,nExpandPos);

	//���´���
	UpdateWindow();

	//ѭ���滭
	CWindowDC WindowDC(this);
	CPen LinePen(PS_SOLID,0,GetSysColor(COLOR_BTNHILIGHT));

	//���û���
	WindowDC.SetROP2(R2_XORPEN);
	WindowDC.SelectObject(&LinePen);

	//ѭ���滭
	for (INT nTimes=nFlashTimes;nTimes>=0;nTimes--)
	{
		//�滭����
		WindowDC.MoveTo(rcTable.left,rcTable.top);
		WindowDC.LineTo(rcTable.right,rcTable.top);
		WindowDC.LineTo(rcTable.right,rcTable.bottom);
		WindowDC.LineTo(rcTable.left,rcTable.bottom);
		WindowDC.LineTo(rcTable.left,rcTable.top);

		//�滭��ͣ
		Sleep((nFlashTimes-nTimes)*2);

		//�滭����
		WindowDC.MoveTo(rcTable.left,rcTable.top);
		WindowDC.LineTo(rcTable.right,rcTable.top);
		WindowDC.LineTo(rcTable.right,rcTable.bottom);
		WindowDC.LineTo(rcTable.left,rcTable.bottom);
		WindowDC.LineTo(rcTable.left,rcTable.top);

		//���þ���
		rcTable.DeflateRect(nStepWidth,nStepWidth);
	}

	//�ͷ���Դ
	WindowDC.DeleteDC();
	LinePen.DeleteObject();

	return true;
}

//��������
bool CTableViewFrame::FlashGameChair(WORD wTableID, WORD wChairID)
{
	//��ȡ����
	ITableView * pITableView=GetTableViewItem(wTableID);

	//�����ж�
	if (pITableView==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	INT nXItem=wTableID%m_nXTableCount;
	INT nYItem=wTableID/m_nXTableCount;
	CRect rcChair=m_TableResource.m_rcChairArray[wChairID];

	//��ȡλ��
	INT nXTablePos=nXItem*m_TableResource.m_SizeTable.cx;
	INT nYTablePos=nYItem*m_TableResource.m_SizeTable.cy;
	rcChair.OffsetRect(nXTablePos+m_nXExcursion,nYTablePos+m_nYExcursion-m_nScrollPos);

	//�������
	INT nFlashTimes=20,nStepWidth=4;
	INT nExpandPos=nFlashTimes*nStepWidth;
	rcChair.InflateRect(nExpandPos,nExpandPos);

	//���´���
	UpdateWindow();

	//ѭ���滭
	CClientDC ClientDC(this);
	CPen LinePen(PS_SOLID,0,RGB(255,255,255));

	//���û���
	ClientDC.SetROP2(R2_XORPEN);
	CPen * pOldPen=ClientDC.SelectObject(&LinePen);

	//ѭ���滭
	for (INT nTimes=nFlashTimes;nTimes>=0;nTimes--)
	{
		//�滭����
		ClientDC.MoveTo(rcChair.left,rcChair.top);
		ClientDC.LineTo(rcChair.right,rcChair.top);
		ClientDC.LineTo(rcChair.right,rcChair.bottom);
		ClientDC.LineTo(rcChair.left,rcChair.bottom);
		ClientDC.LineTo(rcChair.left,rcChair.top);

		//�滭��ͣ
		Sleep((nFlashTimes-nTimes)*2);

		//�滭����
		ClientDC.MoveTo(rcChair.left,rcChair.top);
		ClientDC.LineTo(rcChair.right,rcChair.top);
		ClientDC.LineTo(rcChair.right,rcChair.bottom);
		ClientDC.LineTo(rcChair.left,rcChair.bottom);
		ClientDC.LineTo(rcChair.left,rcChair.top);

		//���þ���
		rcChair.DeflateRect(nStepWidth,nStepWidth);
	}

	//�ͷ���Դ
	ClientDC.SelectObject(pOldPen);

	return true;
}

//��������
bool CTableViewFrame::UpdateTableView(WORD wTableID)
{
	//��ȡ����
	ITableView * pITableView=GetTableViewItem(wTableID);
	if ((pITableView==NULL)||(m_nXTableCount==0)) return false;

	//��������
	INT nXItem=wTableID%m_nXTableCount;
	INT nYItem=wTableID/m_nXTableCount;

	//��������
	CRect rcTable;
	rcTable.left=nXItem*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	rcTable.right=(nXItem+1)*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	rcTable.top=nYItem*m_TableResource.m_SizeTable.cy+m_nYExcursion-m_nScrollPos;
	rcTable.bottom=(nYItem+1)*m_TableResource.m_SizeTable.cy+m_nYExcursion-m_nScrollPos;

	//��������
	RedrawWindow(&rcTable,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW|RDW_UPDATENOW);

	return true;
}

//��ȡ����
ITableView * CTableViewFrame::GetTableViewItem(WORD wTableID)
{
	//��ȡ����
	if (wTableID!=INVALID_TABLE)
	{
		//Ч�����
		ASSERT(wTableID<m_TableViewArray.GetCount());
		if (wTableID>=m_TableViewArray.GetCount()) return NULL;

		//��ȡ����
		ITableView * pITableView=m_TableViewArray[wTableID];

		return pITableView;
	}

	return NULL;
}

//��������
WORD CTableViewFrame::GetNullChairCount(WORD wTableID, WORD & wNullChairID)
{
	//��ȡ����
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//��ȡ״̬
	if (pITableView!=NULL)
	{
		return pITableView->GetNullChairCount(wNullChairID);
	}

	return 0;
}

//�ػ���Ϣ
VOID CTableViewFrame::OnPaint()
{
	//���� DC
	CPaintDC dc(this);

	//��ȡ����
	CRect rcClip;
	CRect rcClient;
	dc.GetClipBox(&rcClip);
	GetClientRect(&rcClient);
	
	//��������
	CBitImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//�����豸
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(&dc);
	BufferDC.SelectObject(ImageBuffer);
	
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	// ����ģʽ
	if((m_wServerType&GAME_GENRE_MATCH)!=0)
	{
		//��ȡ����
		CRect rcClient;
		GetClientRect(&rcClient);

		//�滭��϶
		if ( m_TableResource.m_ImageGround.IsNull() )
		{
			LOGI("m_TableResource.m_ImageGround.IsNull() 1");
//			m_TableResource.m_ImageBitGround.DrawImageTile(pBufferDC, rcClip);
			SpreadDrawImage(&BufferDC,m_TableResource.m_ImageBitGround,rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height());
		}
		else
		{
//			LOGI("m_TableResource.m_ImageGround.IsNull() 2");
			SpreadDrawImage(&BufferDC,m_TableResource.m_ImageGround,rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height());
		}

		if(!m_ImageTableTL.IsNull() && !m_ImageTableTM.IsNull() && !m_ImageTableTR.IsNull())
		{
			CRect rcRect(0,0,(rcClient.Width()-m_ImageTableTM.GetWidth())/2, m_ImageTableTL.GetHeight());
			m_ImageTableTL.DrawImageTile(&BufferDC, rcRect);
			m_ImageTableTM.BitBlt(BufferDC.GetSafeHdc(), (rcClient.Width()-m_ImageTableTM.GetWidth())/2,0);
			rcRect=CRect((rcClient.Width()+m_ImageTableTM.GetWidth())/2,0,rcClient.Width(), m_ImageTableTL.GetHeight());
			m_ImageTableTR.DrawImageTile(&BufferDC, rcRect);
		
			/*  !!����ģʽ����ط�����д��
			//��������
			CDFontEx DrawFont;
			DrawFont.CreateFont(this, TEXT("����"), 18, 300);
			pBufferDC->SetTextColor(RGB(255,255,255));
			
			int nXPos=__max(rcClient.Width()/2,400);
			int nYPos=50;

			DrawFont.DrawText(&BufferDC, TEXT("����������"), nXPos-100,20, RGB(255,255,255), DT_LEFT|DT_TOP);
			DrawNumberString(&BufferDC, m_ImageNumberYellow, m_dwWaitting, nXPos,nYPos+15,TA_LEFT);
			DrawNumberString(&BufferDC, m_ImageNumberGreen, m_dwTotalUser, nXPos+38,nYPos+55,TA_RIGHT);	
			m_ImageLine.DrawImage(&BufferDC,nXPos+22,nYPos+10);

			//�ͷ���Դ
			DrawFont.DeleteFont();*/
		}

		//�滭����
		if ((m_ImageEnterMatch.IsNull()==false) && m_ImageLeaveMatch.IsNull()==false)
		{
			//������Ϸ
			INT nXEnterIndex=0;
			bool bEnterButtonDown=(m_cbButtonDown==BT_ENTER_GAME||m_cbButtonDown==BT_LEAVE_MATCH);
			bool bEnterButtonHover=(m_cbButtonHover==BT_ENTER_GAME||m_cbButtonDown==BT_LEAVE_MATCH);

			if ((bEnterButtonHover==true)&&(bEnterButtonDown==true)) nXEnterIndex=1;
			if ((bEnterButtonHover==true)&&(bEnterButtonDown==false)) nXEnterIndex=2;

			if(m_cbMatchStatus==MS_NULL)
			{
				//�滭��־
				CSize SizeEnter=CSize(m_ImageEnterMatch.GetWidth()/5,m_ImageEnterMatch.GetHeight());
				m_ImageEnterMatch.BitBlt(BufferDC.GetSafeHdc(),rcClient.Width()-ENTER_MATCH_BUTTON_X,ENTER_MATCH_BUTTON_Y,
					SizeEnter.cx,SizeEnter.cy,SizeEnter.cx*nXEnterIndex,0);
			}
			else if(m_cbMatchStatus==MS_SIGNUP)
			{
				//�滭��־
				CSize SizeQuit=CSize(m_ImageLeaveMatch.GetWidth()/5,m_ImageLeaveMatch.GetHeight());
				m_ImageLeaveMatch.BitBlt(BufferDC.GetSafeHdc(),rcClient.Width()-ENTER_MATCH_BUTTON_X,ENTER_MATCH_BUTTON_Y,
					SizeQuit.cx,SizeQuit.cy,SizeQuit.cx*nXEnterIndex,0);
			}
		}

		for(BYTE i=0; i<MAX_MATCH_DESC; i++)
		{
			if(m_pMatchDesc.szTitle[i][0]!='\0')
			{
					m_DrawFont.DrawText(&BufferDC, m_pMatchDesc.szTitle[i],&CRect(10,15+i*30,80,35+i*30),m_pMatchDesc.crTitleColor,DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
					m_DrawFont.DrawText(&BufferDC, m_pMatchDesc.szDescribe[i],&CRect(80,15+i*30,280,35+i*30),m_pMatchDesc.crDescribeColor,DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
			}
			else
				break;
		}

		//�滭����
		dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

		return ;
	}

	//��Ϣλ��
	INT nXPos=0;
	INT nYPos=0;

	//�滭����
	if ((m_TableViewArray.GetCount()>0L)&&(m_TableResource.m_bShowTable==true))
	{
		//�������
		CSize SizeTable=m_TableResource.m_SizeTable;

		//�������
		bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(m_dwServerRule);
		WORD wTableCount=m_wTableCount;
		WORD nXTableCount=m_nXTableCount;
		INT  nXExcursion=m_nXExcursion;
		INT  nYExcursion=m_nYExcursion;
		if(bAvertCheatMode)
		{
			wTableCount=1;
			nXTableCount=1;

			if(bAvertCheatMode && m_bWaitDistribute)
			{
				SizeTable.cx=m_TableResource.m_ImageWaitDistribute.GetWidth();
				SizeTable.cy=m_TableResource.m_ImageWaitDistribute.GetHeight();
				nXExcursion=(__max(SizeTable.cx,rcClient.Width()) - SizeTable.cx)/2;
				nYExcursion=(rcClient.Height() - SizeTable.cy)/2;//(__max(SizeTable.cy,rcClient.Height()) - SizeTable.cy)/2;
			}
		}

		//�滭׼��
		INT nXStartItem=(rcClip.left-nXExcursion)/SizeTable.cx;
		INT nYStartItem=(m_nScrollPos-nYExcursion+rcClip.top)/SizeTable.cy;
		INT nXConcludeItem=(rcClip.right-nXExcursion+SizeTable.cx-1)/SizeTable.cx;
		INT nYConcludeItem=(m_nScrollPos-nYExcursion+rcClip.bottom+SizeTable.cy-1)/SizeTable.cy;

//		LOGI("index = " << nXStartItem << " " << nXConcludeItem << " " << nYStartItem << " " << nYConcludeItem)

		//�滭����
		for (INT nYIndex=nYStartItem;nYIndex<nYConcludeItem;nYIndex++)
		{
			for (INT nXIndex=nXStartItem;nXIndex<nXConcludeItem;nXIndex++)
			{
				//λ�ö���
				INT nIndex=nYIndex*m_nXTableCount+nXIndex;
				INT nXTablePos=nXIndex*SizeTable.cx+nXExcursion;
				INT nYTablePos=nYIndex*SizeTable.cy+nYExcursion-m_nScrollPos;


				//λ�ü���
				if(nIndex==m_wTipTableID && m_wTipChairID!=INVALID_CHAIR)
				{
					CRect &rc=m_TableResource.m_rcChairArray[m_wTipChairID];
					nXPos=nXTablePos+rc.left+rc.Width()+18;
					nYPos=nYTablePos+rc.top+rc.Height()+18;
				//	if(nXPos+TIP_USER_INFO_W > rcClient.right) nXPos -= (TIP_USER_INFO_W+rc.Width()+18);
				//	if(nYPos+TIP_USER_INFO_H > rcClient.bottom) nYPos -= (rc.Height()/2+TIP_USER_INFO_H);
				}

				//�滭����
				if ((nIndex>=0)&&(nXIndex<nXTableCount)&&(nIndex<wTableCount))
				{
					//�滭��϶
 					if (m_TableResource.m_ImageGround.IsNull())
 					{
 						SpreadDrawImage(&BufferDC,m_TableResource.m_ImageBitGround,nXTablePos,nYTablePos,SizeTable.cx,SizeTable.cy);
 					}
 					else
 					{
 						SpreadDrawImage(&BufferDC,m_TableResource.m_ImageGround,nXTablePos,nYTablePos,SizeTable.cx,SizeTable.cy);
 					}
					

					//�滭����
					if(bAvertCheatMode && m_bWaitDistribute)
					{
						int nImageWidth=m_TableResource.m_ImageWaitDistribute.GetWidth();
						int nImageHeight=m_TableResource.m_ImageWaitDistribute.GetWidth();
						m_TableResource.m_ImageWaitDistribute.BitBlt(BufferDC.GetSafeHdc(),(rcClient.Width()-nImageWidth)/2,0);
					}
					else
					{
						m_TableViewArray[nIndex]->DrawTableView(&BufferDC,nXTablePos,nYTablePos,m_dwServerRule,&m_TableResource);
					}
				}
				else
				{
					//�滭��϶
					if ( (m_TableResource.m_ImageGround.IsNull()))
					{
//						LOGI("m_TableResource.m_ImageGround.IsNull(), 3");
						SpreadDrawImage(&BufferDC,m_TableResource.m_ImageBitGround,nXTablePos,nYTablePos,SizeTable.cx,SizeTable.cy);
					}
					else
					{
//						LOGI("m_TableResource.m_ImageGround.IsNull(), 4");
						SpreadDrawImage(&BufferDC,m_TableResource.m_ImageGround,nXTablePos,nYTablePos,SizeTable.cx,SizeTable.cy);
					}
				}
			}
		}
		//�滭��϶
		if (nXExcursion>rcClip.left)
		{
			INT nTileWidth=nXExcursion-rcClip.left;
			if ( m_TableResource.m_ImageGround.IsNull() == false )
			{
				SpreadDrawImage(&BufferDC,m_TableResource.m_ImageGround,rcClip.left,rcClip.top,nTileWidth,rcClip.Height());
//				m_TableResource.m_ImageGround.DrawImageTile(&dc,rcClip.left,rcClip.top,nTileWidth,rcClip.Height());
			}
			else
			{
				SpreadDrawImage(&BufferDC,m_TableResource.m_ImageBitGround,rcClip.left,rcClip.top,nTileWidth,rcClip.Height());
//				m_TableResource.m_ImageBitGround.DrawImageTile(&dc,rcClip.left,rcClip.top,nTileWidth,rcClip.Height());
			}
		}

		//�滭��϶
		if ((nYExcursion-m_nScrollPos)>rcClip.top)
		{
			INT nTileHeight=(nYExcursion-m_nScrollPos)-rcClip.top;
			if (m_TableResource.m_ImageGround.IsNull() == false)
			{
				SpreadDrawImage(&BufferDC,m_TableResource.m_ImageGround,rcClip.left,rcClip.top,rcClip.Width(),nTileHeight);
//				m_TableResource.m_ImageGround.DrawImageTile(&dc,rcClip.left,rcClip.top,rcClip.Width(),nTileHeight);
			}
			else
			{
				SpreadDrawImage(&BufferDC,m_TableResource.m_ImageBitGround,rcClip.left,rcClip.top,rcClip.Width(),nTileHeight);
//				m_TableResource.m_ImageBitGround.DrawImageTile(&dc,rcClip.left,rcClip.top,rcClip.Width(),nTileHeight);
			}
		}
	}
	else 
	{
		//�滭��϶
		if ( m_TableResource.m_ImageGround.IsNull() == false )
		{
//			LOGI("m_TableResource.m_ImageGround.Width="<<m_TableResource.m_ImageGround.GetWidth()<<", m_TableResource.m_ImageGround.Height="<<m_TableResource.m_ImageGround.GetHeight());
			SpreadDrawImage(&BufferDC,m_TableResource.m_ImageGround,rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height());
		}
		else
		{
			SpreadDrawImage(&BufferDC,m_TableResource.m_ImageBitGround,rcClip.left,rcClip.top,rcClip.Width(),rcClip.Height());
		}
	}

	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	return;

}

//λ����Ϣ
VOID CTableViewFrame::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//��Ϣ����
BOOL CTableViewFrame::PreTranslateMessage(MSG * pMsg)
{
	//��Ϣ����
	if (pMsg->message==WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_UP:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_LINEUP,m_nScrollPos),NULL);
				return TRUE;
			}
		case VK_DOWN:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_LINEDOWN,m_nScrollPos),NULL);
				return TRUE;
			}
		case VK_PRIOR:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_PAGEUP,m_nScrollPos),NULL);
				return TRUE;
			}
		case VK_NEXT:
		case VK_SPACE:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_PAGEDOWN,m_nScrollPos),NULL);
				return TRUE;
			}
		case VK_HOME:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_TOP,m_nScrollPos),NULL);
				return TRUE;
			}
		case VK_END:
			{
				SendMessage(WM_VSCROLL,MAKELONG(SB_BOTTOM,m_nScrollPos),NULL);
				return TRUE;
			}
		}
	}

	return __super::PreTranslateMessage(pMsg);
}

//������Ϣ
VOID CTableViewFrame::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	LOGI("OnVScroll")
	//���ý���
	SetFocus();

	//��ֹ����
	if((m_wServerType&GAME_GENRE_MATCH)!=0) return; 

	//��ȡ����
	CRect rcClient;
	GetClientRect(&rcClient);
	INT nLastPos=m_nScrollPos;

	//�ƶ�����
	switch (nSBCode)
	{
	case SB_TOP:
		{
			m_nScrollPos=0;
			break;
		}
	case SB_BOTTOM:
		{
			m_nScrollPos=m_nScrollPosMax-m_nVisibleLineCount*m_TableResource.m_SizeTable.cy;
			break;
		}
	case SB_LINEUP:
		{
			m_nScrollPos-=SCROLL_POS_LINE;
			break;
		}
	case SB_PAGEUP:
		{
			m_nScrollPos-=rcClient.bottom;
			break;
		}
	case SB_LINEDOWN:
		{
			m_nScrollPos+=SCROLL_POS_LINE;
			break;
		}
	case SB_PAGEDOWN:
		{
			m_nScrollPos+=rcClient.bottom;
			break;
		}
	case SB_THUMBTRACK:
		{
			m_nScrollPos=nPos;
			break;
		}
	}
	
	//����λ��
	m_nScrollPos=__min(m_nScrollPos,m_nScrollPosMax-rcClient.bottom);
	if (m_nScrollPos<0) m_nScrollPos=0;

	//��������
	if (nLastPos!=m_nScrollPos)
	{
		SetScrollPos(SB_VERT,m_nScrollPos);
		ScrollWindow(0,nLastPos-m_nScrollPos,NULL,NULL);
		InvalidateRect(NULL);		
	}

	return;
}

//�����Ϣ
VOID CTableViewFrame::OnLButtonDblClk(UINT nFlags, CPoint MousePoint)
{
	//���ý���
	SetFocus();

	//�¼�֪ͨ
	m_pITableViewFrameSink->OnDButtonHitTable(m_wDownTableID);

	return;
}

//�����Ϣ
LRESULT CTableViewFrame::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	//���ñ���
	m_bHovering=false;

	//���ý���
	if (m_wDownTableID!=INVALID_TABLE) 
	{
		//���ñ���
		WORD wTableID=m_wDownTableID;
		m_wDownTableID=INVALID_TABLE;

		//���ý���
		ITableView * pITableView=GetTableViewItem(wTableID);
		if (pITableView!=NULL) pITableView->SetHoverStatus(INVALID_CHAIR);
	}

	return 0;
}

//�����Ϣ
VOID CTableViewFrame::OnLButtonDown(UINT nFlags, CPoint MousePoint)
{
	//���ý���
	SetFocus();

	//���´���
	if (m_cbButtonHover!=0)
	{
		//����˻�
		SetCapture();

		//���ñ���
		m_cbButtonDown=m_cbButtonHover;

		//���½���
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE);
	}

	//��������
	if ((m_bRMouseDown==false)&&(m_bLMouseDown==false))
	{
		//����˻�
		SetCapture();

		//���ñ���
		m_bLMouseDown=true;
		m_wDownTableID=SwitchTableID(MousePoint);

		//��ȡ����
		ITableView * pITableView=NULL;
		if (m_wDownTableID!=INVALID_TABLE) pITableView=GetTableViewItem(m_wDownTableID);

		//��������
		if (pITableView!=NULL)
		{
			//λ�ö���
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y-m_nYExcursion+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//��������
			m_wDownChairID=SwitchChairID(TablePoint);

			//�������
			if (m_wDownChairID!=INVALID_CHAIR) 
			{
				pITableView->SetMouseDown(m_bLMouseDown);
				pITableView->SetHoverStatus(m_wDownChairID);
			}
		}
	}

	return;
}

//�����Ϣ
VOID CTableViewFrame::OnLButtonUp(UINT nFlags, CPoint MousePoint)
{
	//�������
	BYTE cbButtonDown=0;
	WORD wViewItemDown=INVALID_WORD;

	//������ť
	if ((m_wServerType&GAME_GENRE_MATCH)!=0)
	{
		//�ͷŲ���
		ReleaseCapture();

		//���ñ���
		if (m_cbButtonDown==m_cbButtonHover)
		{
			cbButtonDown=m_cbButtonDown;
		}

		//������
		if (m_cbButtonDown!=0)
		{
			//���ñ���
			m_cbButtonDown=0;

			//���½���
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE);
		}

		//ִ�ж���
		if (cbButtonDown!=0)
		{
			ASSERT(m_pITableViewFrameSink!=NULL);
			if (m_pITableViewFrameSink!=NULL) m_pITableViewFrameSink->OnHitTableButton(cbButtonDown);
		}

		return;
	}

	//����ж�
	if ((m_bLMouseDown==true)&&(m_bRMouseDown==false))
	{
		//�ͷŲ���
		ReleaseCapture();

		//���ñ���
		m_bLMouseDown=false;

		//��������
		WORD wChairID=INVALID_CHAIR;
		WORD wTableID=SwitchTableID(MousePoint);

		//��������
		if (m_wDownTableID!=INVALID_TABLE)
		{
			//��ȡ����
			ASSERT(GetTableViewItem(m_wDownTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(m_wDownTableID);

			//����״̬
			pITableView->SetMouseDown(false);
			pITableView->SetHoverStatus(INVALID_CHAIR);
		}

		//��ȡλ��
		if ((wTableID!=INVALID_TABLE)&&(wTableID==m_wDownTableID))
		{
			//��ȡ����
			ASSERT(GetTableViewItem(wTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(wTableID);

			//����λ��
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y-m_nYExcursion+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//���Ӳ���
			wChairID=SwitchChairID(TablePoint);
		}

		//�������
		if ((wTableID==m_wDownTableID)&&(wChairID==m_wDownChairID))
		{
			//���ñ���
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;

			//�¼�֪ͨ
			m_pITableViewFrameSink->OnLButtonHitTable(wTableID,wChairID);
		}
		else
		{
			//���ñ���
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;
		}
	}

	return;
}

//�����Ϣ
VOID CTableViewFrame::OnRButtonDown(UINT nFlags, CPoint MousePoint)
{
	//���ý���
	SetFocus();

	//��������
	if ((m_bRMouseDown==false)&&(m_bLMouseDown==false))
	{
		//����˻�
		SetCapture();

		//���ñ���
		m_bRMouseDown=true;

		//���Ӳ���
		m_wDownTableID=SwitchTableID(MousePoint);

		//����״̬
		if (m_wDownTableID!=INVALID_TABLE)
		{
			//��ȡ����
			ASSERT(GetTableViewItem(m_wDownTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(m_wDownTableID);

			//����λ��
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y-m_nYExcursion+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//���Ӳ���
			m_wDownChairID=SwitchChairID(TablePoint);

			//�������
			if (m_wDownChairID!=INVALID_CHAIR)
			{
				pITableView->SetMouseDown(m_bRMouseDown);
				pITableView->SetHoverStatus(m_wDownChairID);
			}
		}
	}

	return;
}

//�����Ϣ
VOID CTableViewFrame::OnRButtonUp(UINT nFlags, CPoint MousePoint)
{
	if (m_bRMouseDown==true)
	{
		//�ͷŲ���
		ReleaseCapture();

		//���ñ���
		m_bRMouseDown=false;

		//��������
		WORD wChairID=INVALID_CHAIR;
		WORD wTableID=SwitchTableID(MousePoint);

		//����״̬
		if (m_wDownTableID!=INVALID_TABLE)
		{
			//��ȡ����
			ASSERT(GetTableViewItem(m_wDownTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(m_wDownTableID);

			//����״̬
			pITableView->SetMouseDown(false);
			pITableView->SetHoverStatus(INVALID_CHAIR);
		}

		//��ȡλ��
		if ((wTableID!=INVALID_TABLE)&&(wTableID==m_wDownTableID))
		{
			//��ȡ����
			ASSERT(GetTableViewItem(wTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(wTableID);

			//����λ��
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//���Ӳ���
			wChairID=SwitchChairID(TablePoint);
		}

		//�������
		if ((wTableID==m_wDownTableID)&&(wChairID==m_wDownChairID))
		{
			//���ñ���
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;

			//�¼�֪ͨ
			m_UserInfoDlgEx.ShowWindow(SW_HIDE);
			m_pITableViewFrameSink->OnRButtonHitTable(wTableID,wChairID);
		}
		else
		{
			//���ñ���
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;
		}
	}

	return;
}

//�����Ϣ
BOOL CTableViewFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint MousePoint)
{
	LOGI("OnMouseWheel")
	//��ֹ����
	if((m_wServerType&GAME_GENRE_MATCH)!=0) return TRUE; 

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);
	LOGI("CTableViewFrame::OnMouseWheel, Step 2="<<GetTickCount());

	//���ù���
	INT nLastPos=m_nScrollPos;
	INT nMaxPos=m_nScrollPosMax-rcClient.bottom;
	m_nScrollPos=__max(__min(m_nScrollPos-zDelta/WHELL_TIMES,nMaxPos),0L);

	//��������
	SetScrollPos(SB_VERT,m_nScrollPos);
	ScrollWindow(0,nLastPos-m_nScrollPos,NULL,NULL);
	InvalidateRect(NULL);	

	return TRUE;
}

//�����Ϣ
BOOL CTableViewFrame::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//����λ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��ȡ���
	CPoint Point;
	GetCursorPos(&Point);
//	ScreenToClient(&Point);

	if((m_wServerType&GAME_GENRE_MATCH)!=0)
	{
		//λ�ü���
		BYTE cbButtonHover=GetHoverButton(Point);

		//���ݵ���
		if ((m_cbButtonDown!=0)&&(cbButtonHover!=m_cbButtonDown))
		{
			cbButtonHover=0;
		}

		//��������
		if (m_cbButtonHover!=cbButtonHover)
		{
			//���ñ���
			m_cbButtonHover=cbButtonHover;

			//���½���
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);			
		}
		//���ù��
		if(cbButtonHover!=0)
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
			return TRUE;
		}
		return __super::OnSetCursor(pWnd,nHitTest,uMessage);
	}

	//�����ж�
	if (m_bHovering==false)
	{
		//���ñ���
		m_bHovering=true;

		//��������
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//ע����Ϣ
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//ע���¼�
		_TrackMouseEvent(&TrackMouseEvent);
	}

	//״̬�ж�
	CPoint TablePoint = CPoint(0, 0);
	if ((m_bRMouseDown==false)&&(m_bLMouseDown==false))
	{
		//��ȡ���
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);

		//��������
		WORD wNewChairID=INVALID_CHAIR;
		WORD wNewTableID=SwitchTableID(MousePoint);

		//��ȡ����
		if (wNewTableID!=INVALID_TABLE)
		{
			//��������
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y-m_nYExcursion+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//��ȡ����
			wNewChairID=SwitchChairID(TablePoint);
		}

		//���ñ���
		if(m_wTipChairID!=wNewChairID)
		{
			m_wLastTipChairID=m_wTipChairID;
			m_wTipTableID=wNewTableID;
			m_wTipChairID=wNewChairID;
			if(m_wTipChairID==INVALID_CHAIR)m_UserInfoDlgEx.ShowWindow(SW_HIDE);
		}
		
		//��������
		if (((wNewTableID!=m_wDownTableID)||(wNewChairID!=m_wDownChairID))&&((wNewTableID!=INVALID_CHAIR)||(m_wDownChairID!=INVALID_CHAIR)))
		{
			//��������
			if (m_wDownTableID!=INVALID_TABLE)
			{
				GetTableViewItem(m_wDownTableID)->SetHoverStatus(INVALID_CHAIR);
			}

			//��������
			if ((wNewTableID!=INVALID_TABLE)&&(wNewChairID!=INVALID_CHAIR))
			{
				GetTableViewItem(wNewTableID)->SetHoverStatus(wNewChairID);
                //��ȡ�û�
        		IClientUserItem *pUserItem=m_TableViewArray[m_wTipTableID]->GetClientUserItem(wNewChairID);
        		if(pUserItem==NULL)
        		{
                    m_UserInfoDlgEx.UpdateUserInfo(NULL);
        		}
        		else
        		{
					INT nXTablePos=m_wTipTableID%m_nXTableCount*m_TableResource.m_SizeTable.cx+m_nXExcursion;
					INT nYTablePos=m_wTipTableID/m_nXTableCount*m_TableResource.m_SizeTable.cy+m_nYExcursion-m_nScrollPos;

	                 CRect rc;
	       			 m_UserInfoDlgEx.GetClientRect(rc);
		             m_UserInfoDlgEx.UpdateUserInfo(pUserItem);
	                 m_UserInfoDlgEx.SetGameLevelParser(m_pIGameLevelParser);


					 //CPoint pt;
					 //pt.x = TablePoint.x+m_TableResource.m_rcChairArray[m_wTipChairID].right + 10;
					 //\pt.y = TablePoint.y+m_TableResource.m_rcChairArray[m_wTipChairID].bottom + 10;
					 CRect rcTmp = rcClient;
					 ClientToScreen(rcTmp);
//	                 m_UserInfoDlgEx.MoveWindow(nXTablePos+m_TableResource.m_rcChairArray[m_wTipChairID].right,nYTablePos+m_TableResource.m_rcChairArray[m_wTipChairID].bottom,rc.Width(),rc.Height());
	                 m_UserInfoDlgEx.MoveWindow(rcTmp.left+nXTablePos+m_TableResource.m_rcChairArray[m_wTipChairID].right,rcTmp.top+nYTablePos+m_TableResource.m_rcChairArray[m_wTipChairID].bottom,rc.Width(),rc.Height());
	      			 m_UserInfoDlgEx.ShowWindow(SW_SHOW);
        		}
			}
            else
            {
	             m_UserInfoDlgEx.UpdateUserInfo(NULL);
            }
		}

		//���ñ���
		m_wDownTableID=wNewTableID;
		m_wDownChairID=wNewChairID;

		//���ù��
		if ((m_wDownTableID!=INVALID_TABLE)&&(m_wDownChairID!=INVALID_CHAIR))
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
			return TRUE;
		}
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//��þ��ο�
CRect CTableViewFrame::GetTableRect(WORD wTableID)
{
	if (m_nXTableCount==0)
		return CRect(0,0,0,0);

	CRect rc = CRect(CPoint(m_TableResource.m_SizeTable.cx*wTableID/m_nXTableCount+m_nXExcursion, 
		                    m_TableResource.m_SizeTable.cy*wTableID%m_nXTableCount+m_nYExcursion), 
                     CSize(m_TableResource.m_SizeTable.cx, m_TableResource.m_SizeTable.cy));

	return rc;
};

//���Ӳ���
WORD CTableViewFrame::SwitchTableID(POINT MousePoint)
{
	//״̬Ч��
	if (m_TableResource.m_SizeTable.cx==0) return INVALID_TABLE;
	if (m_TableResource.m_SizeTable.cy==0) return INVALID_TABLE;

	//λ�ü���
	INT nXItem=(MousePoint.x-m_nXExcursion)/m_TableResource.m_SizeTable.cx;
	INT nYItem=(MousePoint.y-m_nYExcursion+m_nScrollPos)/m_TableResource.m_SizeTable.cy;

	//����ж�
	if ((nYItem>=0)&&(nXItem>=0)&&(nYItem<m_nYTableCount)&&(nXItem<m_nXTableCount))
	{
		//���ý��
		WORD wResultItem=nYItem*m_nXTableCount+nXItem;

		//����ж�
		if (wResultItem<m_wTableCount)
		{
			bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(m_dwServerRule);
			if ((wResultItem==0)||(bAvertCheatMode==false)) return wResultItem;
		}
	}

	return INVALID_TABLE;
}

//���Ӳ���
WORD CTableViewFrame::SwitchChairID(POINT MousePoint)
{
	//λ������
	if ((m_TableResource.m_bShowUser==true)||(m_TableResource.m_bShowChair==true))
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//�����ж�
			if (MousePoint.x<m_TableResource.m_rcChairArray[i].left) continue;
			if (MousePoint.x>m_TableResource.m_rcChairArray[i].right) continue;

			//�����ж�
			if (MousePoint.y<m_TableResource.m_rcChairArray[i].top) continue;
			if (MousePoint.y>m_TableResource.m_rcChairArray[i].bottom) continue;

			return i;
		}
	}

	//��������
	if (m_TableResource.m_bShowEnter==true)
	{
		//��������
		bool bOutSide=false;

		//�����ж�
		if (MousePoint.x<m_TableResource.m_ptEnter.x) bOutSide=true;
		if (MousePoint.x>m_TableResource.m_ptEnter.x+m_TableResource.m_SizeEnter.cx) bOutSide=true;

		//�����ж�
		if (MousePoint.y<m_TableResource.m_ptEnter.y) bOutSide=true;
		if (MousePoint.y>m_TableResource.m_ptEnter.y+m_TableResource.m_SizeEnter.cy) bOutSide=true;

		//����ж�
		if (bOutSide==false)
		{
			return INDEX_ENTER_CHAIR;
		}
	}

	return INVALID_CHAIR;
}

//�����ؼ�
VOID CTableViewFrame::RectifyControl(INT nWidth, INT nHeight)
{
	//����λ��
	if ((m_wTableCount!=0)&&(m_TableResource.m_SizeTable.cx>0L)&&(m_TableResource.m_SizeTable.cy>0L))
	{
		//��������
		INT nViewWidth=__max(m_TableResource.m_SizeTable.cx,nWidth);
		INT nViewHeight=__max(m_TableResource.m_SizeTable.cy,nHeight);

		//��ͼ����
		m_nXTableCount=nViewWidth/m_TableResource.m_SizeTable.cx;
		m_nYTableCount=(m_wTableCount+m_nXTableCount-1)/m_nXTableCount;
		m_nVisibleLineCount=nViewHeight/m_TableResource.m_SizeTable.cy;

		//�������
		bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(m_dwServerRule);
		if(bAvertCheatMode)
		{
			m_nYTableCount=1;
			m_nVisibleLineCount=1;
		}

		//ƫ��λ��
		if (m_wTableCount==1 || bAvertCheatMode)
		{
			m_nXExcursion=(nViewWidth-m_TableResource.m_SizeTable.cx)/2;
			m_nYExcursion=(m_nVisibleLineCount>=m_nYTableCount)?(nHeight-m_nYTableCount*m_TableResource.m_SizeTable.cy)/2:0;
		}
		else
		{
			m_nYExcursion=0L;
			m_nXExcursion=(nViewWidth-m_nXTableCount*m_TableResource.m_SizeTable.cx)/2;
		}
	}
	
	//��������
	m_nScrollPosMax=m_nYTableCount*m_TableResource.m_SizeTable.cy;
	m_nScrollPos=__max(0,__min(m_nScrollPos,m_nScrollPosMax-m_nVisibleLineCount*m_TableResource.m_SizeTable.cy));
	
	//���ù���
	if((m_wServerType&GAME_GENRE_MATCH)==0)
	{
		SCROLLINFO ScrollInfoVert;
		ScrollInfoVert.cbSize=sizeof(ScrollInfoVert);
		ScrollInfoVert.fMask=SIF_RANGE|SIF_PAGE|SIF_POS;
		ScrollInfoVert.nMin=0;
		ScrollInfoVert.nMax=m_nScrollPosMax;
		ScrollInfoVert.nPage=nHeight;
		ScrollInfoVert.nPos=m_nScrollPos;
		SetScrollInfo(SB_VERT,&ScrollInfoVert,FALSE);
	}
	else
	{
		ShowScrollBar(SB_BOTH, FALSE);
		if(m_BrowseMatchInfo.m_hWnd!=NULL)
			m_BrowseMatchInfo.SetWindowPos(NULL,0,m_ImageTableTL.GetHeight(),nWidth,nHeight-m_ImageTableTL.GetHeight(),SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE);
	}


	return;
}

//�¼���Ϣ
LRESULT CTableViewFrame::OnMessagePlatformEvent(WPARAM wParam, LPARAM lParam)
{
	//�������
	if (wParam==EVENT_SKIN_CONFIG_UPDATE)
	{
		//��Ⱦ�ж�
		if (m_TableResource.m_bRenderImage==true)
		{
			//������Դ
			CSkinRenderManager * pSkinRenderManager=CSkinRenderManager::GetInstance();
			if (pSkinRenderManager!=NULL) m_TableResource.UpdateResource(pSkinRenderManager);

			//���½���
			RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASENOW);
		}

		return 0;
	}

	return 0L;
}

//��ť����
BYTE CTableViewFrame::GetHoverButton(CPoint MousePoint)
{
	//����λ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//����λ��
	CRect rcEnterGame;
	rcEnterGame.top=ENTER_MATCH_BUTTON_Y;
	rcEnterGame.bottom=rcEnterGame.top+m_ImageEnterMatch.GetHeight();
	rcEnterGame.left=rcClient.Width()-ENTER_MATCH_BUTTON_X;
	rcEnterGame.right=rcEnterGame.left+m_ImageEnterMatch.GetWidth()/5;
	
	if (rcEnterGame.PtInRect(MousePoint)==TRUE) 
	{
		if (m_cbMatchStatus==MS_NULL)
		{
			return BT_ENTER_GAME;
		}
		else if (m_cbMatchStatus==MS_SIGNUP)
		{
			return BT_LEAVE_MATCH;
		}
	}

	return 0;
}

//�滭����
VOID CTableViewFrame::DrawNumberString(CDC * pDC, CPngImage &PngNumber, LONG lNumber, INT nXPos, INT nYPos,BYTE cbAlign)
{
	return;
	//������Դ
	INT nNumberHeight=PngNumber.GetHeight();
	INT nNumberWidth=PngNumber.GetWidth()/10;

	//������Ŀ
	LONG lNumberCount=0;
	LONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0);

	//λ�ö���
	INT nYDrawPos=nYPos-nNumberHeight/2;
	INT nXDrawPos=nXPos;
	if(cbAlign==TA_RIGHT)
		nXDrawPos+=(lNumberCount-1)*nNumberWidth;
	else if(cbAlign==TA_CENTER)
		nXDrawPos+=(lNumberCount-1)*nNumberWidth/2;

	//�滭����
	for (LONG i=0;i<lNumberCount;i++)
	{
		//�滭����
		LONG lCellNumber=(LONG)(lNumber%10L);
		PngNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,lCellNumber*nNumberWidth,0);

		//���ñ���
		lNumber/=10;
		nXDrawPos-=nNumberWidth;
	};

	return;
}

//�滭ͼƬ
VOID CTableViewFrame::SpreadDrawImage(CDC * pDC, CPngImage &PngImage, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight)
{
	//������Դ
	INT nImageWidth=PngImage.GetWidth();
	INT nImageHeight=PngImage.GetHeight();
	INT nDrawRight=nXDest+nDestWidth;
	INT nDrawBottom=nYDest+nDestHeight;

	//�滭ͼƬ
	for (INT nXPos=nXDest;nXPos<nDrawRight;nXPos+=nImageWidth)
	{
		for (INT nYPos=nYDest;nYPos<nDrawBottom;nYPos+=nImageHeight)
		{
			INT nDrawW=(((nDrawRight-(nXPos))>nImageWidth)?nImageWidth:(nDrawRight-(nXPos)));
			INT nDrawH=(((nDrawBottom-(nYPos))>nImageHeight)?nImageHeight:(nDrawBottom-(nYPos)));
			PngImage.DrawImage(pDC,nXPos,nYPos,nDrawW,nDrawH,0,0);
		}
	}
}

//�滭ͼƬ
VOID CTableViewFrame::SpreadDrawImage(CDC * pDC, CBitImage &BitImage, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight)
{

	//������Դ
	INT nImageWidth=BitImage.GetWidth();
	INT nImageHeight=BitImage.GetHeight();
	INT nDrawRight=nXDest+nDestWidth;
	INT nDrawBottom=nYDest+nDestHeight;

	//�滭ͼƬ
	for (INT nXPos=nXDest;nXPos<nDrawRight;nXPos+=nImageWidth)
	{
		for (INT nYPos=nYDest;nYPos<nDrawBottom;nYPos+=nImageHeight)
		{
			INT nDrawW=(((nDrawRight-(nXPos))>nImageWidth)?nImageWidth:(nDrawRight-(nXPos)));
			INT nDrawH=(((nDrawBottom-(nYPos))>nImageHeight)?nImageHeight:(nDrawBottom-(nYPos)));
			BitImage.DrawImageTile(pDC,nXPos,nYPos,nDrawW,nDrawH);
		}
	}
}
//����״̬
void CTableViewFrame::SetMatchStatus(BYTE cbMatchStatus)
{
	LOGI("SetMatchStatus")
	m_cbMatchStatus=cbMatchStatus;
    if(cbMatchStatus==MS_MATCHING)
	{
		m_tStartTime=CTime::GetCurrentTime();
		SetTimer(IDI_TICK_TIMER,1000, NULL);
		InvalidateRect(NULL);
	}
	else
	{
		KillTimer(IDI_TICK_TIMER);
	}
}

void CTableViewFrame::OnTimer(UINT nIDEvent)
{
	TRACE("CTableViewFrame::OnTimer\n");

	if(nIDEvent==IDI_TICK_TIMER)
	{
		LOGI("OnTimer")
		CRect rcClient;
		GetClientRect(&rcClient);

		int nXPos=rcClient.Width()/2;
		int nYPos=20;

		if ( CD2DEngine::GetD2DEngine() )
		{
			InvalidateRect(NULL);
		}
		else
		{
			InvalidateRect(&CRect(nXPos, nYPos,nXPos+400,nYPos+50));
		}
		return;
	}
	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////////////
