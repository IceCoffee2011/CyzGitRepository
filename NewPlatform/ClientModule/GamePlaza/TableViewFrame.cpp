#include "Stdafx.h"
#include "Resource.h"
//#include "TestTableView.h"
#include "GlobalUnits.h"
#include "TableViewFrame.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////////////

//横行格式
#define	NT_LEFT						0x01								//左对齐
#define	NT_RIGHT					0x02								//右对齐

//纵向格式
#define	NT_TOP						0x10								//上对齐
#define	NT_BOTTOM					0x20								//下对齐

//滚动定义
#define WHELL_TIMES					2									//滚轮倍数
#define SCROLL_POS_LINE				10									//滚动像素

#define IDC_BT_JOIN_MATCH			10012								//参赛按钮

//定时器
#define IDI_TICK_TIMER				1									//常规定时

//控件标识
#define IDC_WEB_PUBLICIZE			100									//浏览控件

//按钮位置
#define ENTER_MATCH_BUTTON_X		150									//按钮位置
#define ENTER_MATCH_BUTTON_Y		50									//按钮位置
#define LEAVE_MATCH_BUTTON_X		200									//按钮位置
#define LEAVE_MATCH_BUTTON_Y		70									//按钮位置

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTableViewFrame, CWnd)

	//系统消息
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

	//自定消息
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_PLATFORM_EVENT, OnMessagePlatformEvent)

	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CTableResource::CTableResource()
{
	//属性变量
	m_bShowUser=true;
	m_bShowChair=true;
	m_bShowTable=true;
	m_bShowFocus=true;
	m_bShowEnter=false;
	m_bCustomUser=false;
	m_lShowTableID=0;
	m_bRenderImage=true;
	m_lDefaultFace=0;

	//数目定义
	m_wTableItemCount=0;
	m_wChairItemCount=0;

	//类型数目
	m_wUserGenreCount=0;
	m_wTableGenreCount=0;
	m_wChairGenreCount=0;

	//颜色属性
	m_crName=RGB(0,0,0);
	m_crMember=RGB(0,0,0);
	m_crMaster=RGB(0,0,0);

	//桌子属性
	m_SizeChair.SetSize(0,0);
	m_SizeTable.SetSize(0,0);
	m_SizeEnter.SetSize(0,0);

	//位置变量
	m_ptLock.SetPoint(0,0);
	m_ptEnter.SetPoint(0,0);
	m_ptTableID.SetPoint(0,0);

	//位置变量
	for (WORD i=0;i<MAX_CHAIR;i++)
	{
		m_ptReadyArray[i].SetPoint(0,0);
		m_rcNameArray[i].SetRect(0,0,0,0);
		m_rcChairArray[i].SetRect(0,0,0,0);
		m_ptAvatarArray[i].SetPoint(0,0);  //yao
		m_ptAvatarIndex[i] = 0;
	}

	//辅助变量
	ZeroMemory(m_nDrawStyle,sizeof(m_nDrawStyle));
	ZeroMemory(m_szDirectory,sizeof(m_szDirectory));

	//加载资源
	HINSTANCE hins = AfxGetInstanceHandle();
	m_ImageReady.LoadImage(hins,TEXT("TABLE_READY"));
	m_ImageLocker.LoadImage(hins,TEXT("TABLE_LOCKER"));
	m_ImageNumberBK.LoadImage(hins,TEXT("TABLE_NUMBER_BK"));
	m_ImageNumberNB.LoadImage(hins,TEXT("TABLE_NUMBER_NB"));

	//设置变量
	m_SizeNumberBK.SetSize(m_ImageNumberBK.GetWidth(),m_ImageNumberBK.GetHeight());
	m_SizeNumberNB.SetSize(m_ImageNumberNB.GetWidth()/10L,m_ImageNumberNB.GetHeight());

	return;
}

//析构函数
CTableResource::~CTableResource()
{
	//释放资源
	DestroyResource();

	return;
}

//释放资源
VOID CTableResource::DestroyResource()
{
	//桌子属性
	m_SizeChair.SetSize(0,0);
	m_SizeTable.SetSize(0,0);

	//数目定义
	m_wTableItemCount=0;
	m_wChairItemCount=0;

	//输出控制
	ZeroMemory(m_nDrawStyle,sizeof(m_nDrawStyle));

	//位置属性
	ZeroMemory(&m_ptLock,sizeof(m_ptLock));
	ZeroMemory(m_rcNameArray,sizeof(m_rcNameArray));
	ZeroMemory(m_rcChairArray,sizeof(m_rcChairArray));
	ZeroMemory(m_ptReadyArray,sizeof(m_ptReadyArray));
	ZeroMemory(m_ptAvatarArray,sizeof(m_ptAvatarArray)); //yao
	ZeroMemory(m_ptAvatarIndex,sizeof(m_ptAvatarIndex));

	//颜色属性
	m_crName=RGB(0,0,0);
	m_crMember=RGB(0,0,0);
	m_crMaster=RGB(0,0,0);

	//释放位图
	m_ImageChair.DestroyImage();
	m_ImageChairBOX.DestroyImage();
	m_ImageTable.DestroyImage();
	m_ImageTableBOX.DestroyImage();
	m_ImageGround.DestroyImage();

	//释放位图
	m_ImageEnter.DestroyImage();
	m_ImageCustomUser.DestroyImage();

	return;
}

//加载资源
bool CTableResource::LoadResource(LPCTSTR pszResDirectory, WORD wChairCount, bool bAvertCheatMode)
{
	//效验参数
	ASSERT((pszResDirectory!=NULL)&&(wChairCount<=MAX_CHAIR));
	if ((pszResDirectory==NULL)||(wChairCount>MAX_CHAIR)) return false;

	//资源路径
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));
	if(bAvertCheatMode)
		_sntprintf(m_szDirectory,CountArray(m_szDirectory),TEXT("%s\\%s\\AvertCheatMode"),szDirectory,pszResDirectory);
	else
		_sntprintf(m_szDirectory,CountArray(m_szDirectory),TEXT("%s\\%s"),szDirectory,pszResDirectory);

	//参数路径
	TCHAR szParameterFile[MAX_PATH]=TEXT("");
	_sntprintf(szParameterFile,CountArray(szParameterFile),TEXT("%s\\TableResource.INI"),m_szDirectory);

	//资源判断
	if(bAvertCheatMode)
	{
		//构造路径
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Table.png"),m_szDirectory);

		//加载资源
		CPngImage ImageTable;
		if (ImageTable.LoadImage(szImagePath)==false)
		{
			//资源路径
			ZeroMemory(m_szDirectory, sizeof(m_szDirectory));
			_sntprintf(m_szDirectory,CountArray(m_szDirectory),TEXT("%s\\AvertCheatMode"),szDirectory);

			//参数路径
			ZeroMemory(szParameterFile, sizeof(szParameterFile));
			_sntprintf(szParameterFile,CountArray(szParameterFile),TEXT("%s\\TableResource.INI"),m_szDirectory);
		}

		//构造路径
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\WaitDistribute.bmp"),m_szDirectory);

		//加载资源
		if(FAILED(m_ImageWaitDistribute.Load(szImagePath)))
		{
			return false;
		}
	}

	//颜色变量
	ReadColor(m_crName,TEXT("Color"),TEXT("Color_Name"),szParameterFile);
	ReadColor(m_crMember,TEXT("Color"),TEXT("Color_Member"),szParameterFile);
	ReadColor(m_crMaster,TEXT("Color"),TEXT("Color_Master"),szParameterFile);

	//位置坐标
	ReadPoint(m_ptLock,TEXT("Position"),TEXT("Point_Lock"),szParameterFile);
	ReadPoint(m_ptEnter,TEXT("Position"),TEXT("Point_Enter"),szParameterFile);
	ReadPoint(m_ptTableID,TEXT("Position"),TEXT("Point_TableID"),szParameterFile);

	//显示属性
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

	//数目属性
	m_wTableItemCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("TableItemCount"),2L,szParameterFile);
	m_wChairItemCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("ChairItemCount"),wChairCount,szParameterFile);
	m_wChairCellCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("ChairCellCount"),0,szParameterFile);

	//设置变量，读取椅子单元图片索引
	TCHAR szReadData[1024]=TEXT("");
	ZeroMemory(&m_lChairCellIndex,sizeof(m_lChairCellIndex));

	GetPrivateProfileString(TEXT("Attribute"),TEXT("ChairCellIndex"),TEXT(""),szReadData,CountArray(szReadData),szParameterFile);
	if (szReadData[0]!=0)
	{
		//读取变量
		LPCTSTR pszString=szReadData;
		for (WORD i=0; i<m_wChairItemCount*2;i++)
		{
			m_lChairCellIndex[i]=ReadStringValue(pszString);
		}
	}

	//类型数目
	m_wUserGenreCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("UserGenreCount"),1L,szParameterFile);
	m_wTableGenreCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("TableGenreCount"),1L,szParameterFile);
	m_wChairGenreCount=GetPrivateProfileInt(TEXT("Attribute"),TEXT("ChairGenreCount"),1L,szParameterFile);

	//更新资源
	if(UpdateResource(CSkinRenderManager::GetInstance())==false)
	{
		return false;
	}

	//进入尺寸
	if (m_ImageEnter.IsNull()==false)
	{
		m_SizeEnter.SetSize(m_ImageEnter.GetWidth()/4,m_ImageEnter.GetHeight());
	}

	//桌子尺寸
	if (m_ImageTable.IsNull()==false)
	{
		m_SizeTable.SetSize(m_ImageTable.GetWidth()/m_wTableItemCount,m_ImageTable.GetHeight()/m_wTableGenreCount);
	}

	//
	//椅子尺寸
	if (m_ImageChair.IsNull()==false)
	{
		m_SizeChair.SetSize(m_ImageChair.GetWidth()/m_wChairItemCount,m_ImageChair.GetHeight());
	}

	//椅子尺寸
	if (m_ImageChairBOX.IsNull()==false)
	{
		m_SizeChair.SetSize(m_ImageChairBOX.GetWidth()/m_wChairItemCount,m_ImageChairBOX.GetHeight());
	}

	//用户尺寸
	if (m_ImageCustomUser.IsNull()==false)
	{
		m_SizeChair.SetSize(m_ImageCustomUser.GetWidth()/m_wChairItemCount,m_ImageCustomUser.GetHeight()/m_wUserGenreCount);
	}

	//位置坐标
	if (m_bShowTable==true)
	{
		for (WORD i=0;i<wChairCount;i++)
		{
			//变量定义
			TCHAR szSubItem[64]=TEXT("");

			//用户属性
			if (m_bShowUser==true)
			{
				//名字位置
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Rect_Name%d"),i+1);
				ReadRect(m_rcNameArray[i],TEXT("Position"),szSubItem,szParameterFile);

				//准备位置
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Point_Ready%d"),i+1);
				ReadPoint(m_ptReadyArray[i],TEXT("Position"),szSubItem,szParameterFile);

				//控制输出
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("DrawStyle%d"),i+1);
				m_nDrawStyle[i]=GetPrivateProfileInt(TEXT("Position"),szSubItem,0L,szParameterFile);

				//椅子位置
				POINT ChairPoint;
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Point_Chair%d"),i+1);
				ReadPoint(ChairPoint,TEXT("Position"),szSubItem,szParameterFile);

				//椅子位置
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Point_P%d"),i+1);
				ReadPoint(m_ptAvatarArray[i],TEXT("Position"),szSubItem,szParameterFile);

				//头像属性 
				_sntprintf(szSubItem,CountArray(szSubItem),TEXT("Head_Index%d"),i+1);
				m_ptAvatarIndex[i]=GetPrivateProfileInt(TEXT("Position"),szSubItem,0L,szParameterFile);

				//椅子框架
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
#define MODULE_HANDLEL_NAME	TEXT("GamePlaza.exe")			//组件名字
#else
#define MODULE_HANDLEL_NAME	TEXT("GameManager.exe")			//组件名字
#endif
//更新资源
bool CTableResource::UpdateResource(CSkinRenderManager * pSkinRenderManager)
{
//	MessageBox(NULL, m_szDirectory, NULL, MB_OK);
	//构造路径
	TCHAR szImageGround[MAX_PATH]=TEXT("");
	_sntprintf(szImageGround,CountArray(szImageGround),TEXT("%s\\Ground.png"),m_szDirectory);
//	MessageBox(NULL, szImageGround, NULL, MB_OK);

	//加载背景
	if (!m_ImageGround.IsNull())
	{
		LOGI("CTableResource::UpdateResource, 销毁图片");
		MessageBox(NULL, TEXT("销毁图片"), NULL, MB_OK);
		m_ImageGround.DestroyImage();
	}
//	else
//	{
//		MessageBox(NULL, _T("m_ImageGround.IsNull() else"), NULL, MB_OK);
//	}

	//加载资源
	if ( m_ImageGround.LoadImage(szImageGround) == false)
	{
//		LOGI("载入图片失败， szImageGround="<<szImageGround);
//		MessageBox(NULL, TEXT("载入图片失败"), NULL, MB_OK);
		m_ImageBitGround.LoadFromResource(AfxGetInstanceHandle(),IDB_TABLE_GROUND);
	}
	else
	{
//		LOGI("载入图片成功, szImageGround="<<szImageGround);
//		MessageBox(NULL, TEXT("载入图片成功"), NULL, MB_OK);
	}

	//渲染资源
	if (m_bRenderImage==true)
	{
		pSkinRenderManager->RenderImage(m_ImageBitGround);
//		pSkinRenderManager->RenderImage(m_ImageGround);
	}

	//进入位图
	if ((m_bShowEnter==true)&&(m_ImageEnter.IsNull()==true))
	{
		//构造路径
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Enter.png"),m_szDirectory);

		//加载资源
		if (m_ImageEnter.LoadImage(szImagePath)==false) return false;
	}

	//用户位图
	if ((m_bCustomUser==true)&&(m_ImageCustomUser.IsNull()==true))
	{
		//构造路径
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\CustomUser.png"),m_szDirectory);

		//加载资源
		if (m_ImageCustomUser.LoadImage(szImagePath)==false) return false;
	}

	//构造桌子
	if (m_bShowTable==true)
	{
		//构造路径
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Table.png"),m_szDirectory);

		//加载资源
		if (m_ImageTable.LoadImage(szImagePath)==false) return false;
	}

	//构造桌子框
	if (m_bShowTable==true&&m_bShowFocus==true)
	{
		//构造路径
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Table_BOX.png"),m_szDirectory);

		//加载资源
		m_ImageTableBOX.LoadImage(szImagePath);
	}

	//创建椅子框
	if ((m_bShowChair==true)&&(m_bShowTable==true))
	{
		//构造路径
		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Chair_BOX.png"),m_szDirectory);

		//加载资源
		m_ImageChairBOX.LoadImage(szImagePath);

		//6603构造路径
//		TCHAR szImagePath[MAX_PATH]=TEXT("");
		_sntprintf(szImagePath,CountArray(szImagePath),TEXT("%s\\Chair.png"),m_szDirectory);

		//6603加载资源
		if (m_ImageChair.LoadImage(szImagePath)==false) return false;
	}

	//缺省的头像
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

//读取数值
LONG CTableResource::ReadStringValue(LPCTSTR & pszSring)
{
	//效验参数
	ASSERT((pszSring!=NULL)&&(pszSring[0]!=0));
	if ((pszSring==NULL)||(pszSring[0]==0)) return 0L;

	//寻找开始
	bool firstcharisnumber = true;
	while (((pszSring[0]>0)&&(pszSring[0]<TEXT('0')))||(pszSring[0]>TEXT('9')))
	{
		firstcharisnumber = false;
		pszSring++;
	}

	//是否有负号
	bool neg = false;
	if (!firstcharisnumber)
	{
		if (pszSring[-1] == '-')
			neg = true;
	}

	//读取数值
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

//读取矩形
bool CTableResource::ReadRect(RECT & ValueRect, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile)
{
	//设置变量
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValueRect,sizeof(ValueRect));

	//读取字符
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),pszIniFile);
	if (szReadData[0]!=0)
	{
		//读取变量
		LPCTSTR pszString=szReadData;
		ValueRect.left=ReadStringValue(pszString);
		ValueRect.top=ReadStringValue(pszString);
		ValueRect.right=ValueRect.left+ReadStringValue(pszString);
		ValueRect.bottom=ValueRect.top+ReadStringValue(pszString);

		return true;
	}

	return false;
}

//读取坐标
bool CTableResource::ReadPoint(POINT & ValuePoint, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile)
{
	//设置变量
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValuePoint,sizeof(ValuePoint));

	//读取字符
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),pszIniFile);
	if (szReadData[0]!=0)
	{
		//读取变量
		LPCTSTR pszString=szReadData;
		ValuePoint.x=ReadStringValue(pszString);
		ValuePoint.y=ReadStringValue(pszString);

		return true;
	}

	return false;
}

//读取颜色
bool CTableResource::ReadColor(COLORREF & ValueColor, LPCTSTR pszItem, LPCTSTR pszSubItem, LPCTSTR pszIniFile)
{
	//设置变量
	TCHAR szReadData[64]=TEXT("");
	ZeroMemory(&ValueColor,sizeof(ValueColor));

	//读取字符
	GetPrivateProfileString(pszItem,pszSubItem,TEXT(""),szReadData,CountArray(szReadData),pszIniFile);
	if (szReadData[0]!=0)
	{
		//读取变量
		LPCTSTR pszString=szReadData;
		ValueColor=RGB(ReadStringValue(pszString),ReadStringValue(pszString),ReadStringValue(pszString));

		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CTableView::CTableView()
{
	//组件接口
	m_pITableViewFrame=NULL;

	//状态变量
	m_bMouseDown=false;
	m_wHoverChairID=INVALID_CHAIR;
	ZeroMemory(&m_TableAttribute,sizeof(m_TableAttribute));

	return;
}

//析构函数
CTableView::~CTableView()
{
}

//接口查询
VOID * CTableView::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableView,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableView,Guid,dwQueryVer);
	return NULL;
}

//空椅子数
WORD CTableView::GetNullChairCount(WORD & wNullChairID)
{
	//设置变量
	wNullChairID=INVALID_CHAIR;

	//寻找位置
	WORD wNullCount=0;
	for (WORD i=0;i<m_TableAttribute.wChairCount;i++)
	{
		if (m_TableAttribute.pIClientUserItem[i]==NULL)
		{
			//设置数目
			wNullCount++;

			//设置结果
			if (wNullChairID==INVALID_CHAIR) wNullChairID=i;
		}
	}

	return wNullCount;
}

//配置函数
VOID CTableView::InitTableView(WORD wTableID, WORD wChairCount, TCHAR szTableName[LEN_TABLENAME], ITableViewFrame * pITableViewFrame)
{
	//设置属性
	m_TableAttribute.wTableID=wTableID;
	m_TableAttribute.wChairCount=wChairCount;
	CopyMemory(m_TableAttribute.szTableName, szTableName, CountArray(m_TableAttribute.szTableName));

	//设置接口
	m_pITableViewFrame=QUERY_OBJECT_PTR_INTERFACE(pITableViewFrame,ITableViewFrame);

	return;
}

//获取用户
IClientUserItem * CTableView::GetClientUserItem(WORD wChairID)
{
	//效验参数
	ASSERT(wChairID<m_TableAttribute.wChairCount);
	if (wChairID>=m_TableAttribute.wChairCount) return NULL;

	//获取用户
	return m_TableAttribute.pIClientUserItem[wChairID];
}

//设置信息
bool CTableView::SetClientUserItem(WORD wChairID, IClientUserItem * pIClientUserItem)
{
	//效验参数
	ASSERT(wChairID<m_TableAttribute.wChairCount);
	if (wChairID>=m_TableAttribute.wChairCount) return false;

	//设置用户
	m_TableAttribute.pIClientUserItem[wChairID]=pIClientUserItem;

	//更新界面
	m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);

	return true;
}

//鼠标按下
VOID CTableView::SetMouseDown(bool bMouseDown)
{
	//设置标志
	if (m_bMouseDown!=bMouseDown)
	{
		//设置变量
		m_bMouseDown=bMouseDown; 

		//更新界面
		m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);
	}

	return;
}

//焦点框架
VOID CTableView::SetFocusFrame(bool bFocusFrame)
{
	//设置标志
	if (m_TableAttribute.bFocusFrame!=bFocusFrame)
	{
		//设置变量
		m_TableAttribute.bFocusFrame=bFocusFrame;

		//更新界面
		m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);
	}

	return;
}

//盘旋位置
VOID CTableView::SetHoverStatus(WORD wHoverChairID)
{
	//设置标志
	if (m_wHoverChairID!=wHoverChairID)
	{
		//设置变量
		m_wHoverChairID=wHoverChairID;

		//更新界面
		m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);
	}

	return;
}

//桌子状态 
VOID CTableView::SetTableStatus(bool bPlaying, bool bLocker)
{
	//设置标志
	if ((m_TableAttribute.bLocker!=bLocker)||(m_TableAttribute.bPlaying!=bPlaying))
	{
		//设置变量
		m_TableAttribute.bLocker=bLocker; 
		m_TableAttribute.bPlaying=bPlaying;

		//更新界面
		m_pITableViewFrame->UpdateTableView(m_TableAttribute.wTableID);
	}

	return;
}

//#define SINGLE

//绘画函数
VOID CTableView::DrawTableView(CDC * pDC, INT nXPos, INT nYPos, DWORD dwServerRule, CTableResource * pTableResource)
{
	CSize SizeChair=pTableResource->m_SizeChair;
	CSize SizeTable=pTableResource->m_SizeTable;

	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//变量定义
	bool bMySelfTable=false;
	bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(dwServerRule);

#ifdef SINGLE
	m_TableAttribute.bPlaying=true;
#endif

	//绘画桌子
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

	//绘画进入
	if ((pTableResource->m_bShowEnter==true)&&(pTableResource->m_ImageEnter.IsNull()==false))
	{
		//计算位置
		INT nImageIndex=0;
		if (m_TableAttribute.bPlaying==true) nImageIndex=1;
		if ((m_wHoverChairID==INDEX_ENTER_CHAIR)&&(m_TableAttribute.bPlaying==false)&&(m_bMouseDown==true)) nImageIndex=2;
		if ((m_wHoverChairID==INDEX_ENTER_CHAIR)&&(m_TableAttribute.bPlaying==false)&&(m_bMouseDown==false)) nImageIndex=3;

		//绘画标志
		CSize SizeEnter=pTableResource->m_SizeEnter;
		pTableResource->m_ImageEnter.DrawImage(pDC,pTableResource->m_ptEnter.x + nXPos,pTableResource->m_ptEnter.y + nYPos,SizeEnter.cx,SizeEnter.cy,
			SizeEnter.cx*nImageIndex,0);
	}

	//桌面信息
	if(m_TableAttribute.wChairCount < MAX_CHAIR)
	{
		for (WORD i=0;i<m_TableAttribute.wChairCount;i++)
		{
			//变量定义
			CRect rcChair=pTableResource->m_rcChairArray[i];

			//绘画用户
			if (pTableResource->m_bShowUser==true)
			{
				//6603绘画椅子
				if ((!bAvertCheatMode)&&(pTableResource->m_bShowChair))
				{
					ASSERT(pTableResource->m_ImageChair.IsNull()==false);
					LONG lIndex=pTableResource->GetTableCellIndex(i,m_TableAttribute.pIClientUserItem[i]!=NULL);
					if (lIndex!=-1)
						pTableResource->m_ImageChair.DrawImage(pDC,nXPos+rcChair.left,nYPos+rcChair.top,SizeChair.cx,SizeChair.cy,SizeChair.cx*lIndex,0);
				}

				//绘画高亮椅子
				if ((bAvertCheatMode==false)||(m_TableAttribute.wTableID==0))
				{
					//自定边框 
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

					//获取用户
					IClientUserItem * pIClientUserItem=m_TableAttribute.pIClientUserItem[i];
					tagCustomFaceInfo * pCustomFaceInfo=pIClientUserItem->GetCustomFaceInfo();

					//6603绘画椅子
					if ((bAvertCheatMode==true)&&(pTableResource->m_bShowChair==true)&&(m_TableAttribute.wTableID==0))
					{
						ASSERT(pTableResource->m_ImageChair.IsNull()==false);
						ASSERT(0);
						pTableResource->m_ImageChair.DrawImage(pDC,rcChair.left,rcChair.top,SizeChair.cx,SizeChair.cy,SizeChair.cx*i,0);
					}

					bMySelfTable=(m_TableAttribute.pIClientUserItem[i]->GetUserID()==pGlobalUserData->dwUserID);

					//绘画头像
					DrawUserAvatar(pDC,nXPos,nYPos,i,pIClientUserItem,pTableResource);

					//同位判断
					WORD wTableID=pIClientUserItem->GetTableID();
					WORD wChairID=pIClientUserItem->GetChairID();
					bool bSameStation=(wTableID==m_TableAttribute.wTableID)&&(wChairID==i);

					//同意标志
					if ((bSameStation==true)&&(m_TableAttribute.bPlaying==false)&&((pIClientUserItem->GetUserStatus()==US_READY)))
					{
						INT nXPosT=pTableResource->m_ptReadyArray[i].x;
						INT nYPosT=pTableResource->m_ptReadyArray[i].y;
						pTableResource->m_ImageReady.DrawImage(pDC,nXPosT+nXPos,nYPosT+nYPos);
					}

					//变量定义
					WORD wGoldLevel=m_pITableViewFrame->GetGameLevelParser()->GetGameLevel(pIClientUserItem->GetUserScore());

					COLORREF crName = RGB(255,255,255);
					if (pIClientUserItem->GetMemberOrder())
						crName = RGB(255,50,50);

					//用户名字
					DrawTableName(pDC, nXPos, nYPos, i, pIClientUserItem->GetNickName(), pTableResource, crName);
				}

				//隐藏模式
				if ((bAvertCheatMode==true)&&(pTableResource->m_bShowUser==true))
				{
					//用户头像
					if (m_TableAttribute.wTableID>0)
					{
						//绘画头像
						if (pTableResource->m_bCustomUser==false)
						{
							DrawUserAvatar(pDC,nXPos,nYPos,((m_TableAttribute.wTableID*m_TableAttribute.wChairCount)+i)%26, i%pTableResource->m_wChairItemCount, pTableResource);
						}
						else
						{
							//自定头像
							INT nXImagePos=i*rcChair.Width();
							INT nYImagePos=(((m_TableAttribute.wTableID*m_TableAttribute.wChairCount)+i)%2==GENDER_FEMALE)?rcChair.Height():0;
							pTableResource->m_ImageCustomUser.DrawImage(pDC,rcChair.left,rcChair.top,rcChair.Width(),rcChair.Height(),nXImagePos,nYImagePos);
						}
					}

					//用户名字
					WORD wTableID=m_TableAttribute.wTableID;
					DrawTableName(pDC,nXPos, nYPos,i,(wTableID==0)?TEXT("点击加入"):TEXT("分配位置"),pTableResource, pTableResource->m_crName);
				}

			}
		}
	}

	//绘画锁图
#ifdef SINGLE
#else
	if (m_TableAttribute.bLocker==true)
#endif
	{
		INT nXPosT=pTableResource->m_ptLock.x;
		INT nYPosT=pTableResource->m_ptLock.y;
		pTableResource->m_ImageLocker.DrawImage(pDC,nXPosT+nXPos,nYPosT+nYPos);
	}

	// 百人模式不显示桌号
	if (pTableResource->m_bShowEnter==false)
	{
		//桌子号码
		//0，显示立体的桌号
		if (pTableResource->m_lShowTableID==0)
		{
			//获取大小
			CSize SizeNumberBK=pTableResource->m_SizeNumberBK;
			CSize SizeNumberNB=pTableResource->m_SizeNumberNB;

			//变量定义
			WORD wItemCount=0;
			WORD wNumberTemp=m_TableAttribute.wTableID+1;
			WORD wTableNumber=m_TableAttribute.wTableID+1;

			//计算数目
			do
			{
				wItemCount++;
				wNumberTemp/=10;
			} while (wNumberTemp>0);

			//绘画背景
			INT nXNumberBK=pTableResource->m_ptTableID.x-SizeNumberBK.cx/2;
			INT nYNumberBK=pTableResource->m_ptTableID.y-SizeNumberBK.cy/2;
			pTableResource->m_ImageNumberBK.DrawImage(pDC,nXNumberBK+nXPos,nYNumberBK+nYPos);

			//位置定义
			INT nYNumberNB=pTableResource->m_ptTableID.y-SizeNumberNB.cy/2;
			INT nXNumberNB=pTableResource->m_ptTableID.x+(wItemCount*SizeNumberNB.cx)/2-SizeNumberNB.cx;

			//绘画桌号
			for (INT i=0;i<wItemCount;i++)
			{
				//绘画号码
				WORD wNumber=wTableNumber%10;
				pTableResource->m_ImageNumberNB.DrawImage(pDC,nXNumberNB+nXPos,nYNumberNB+nYPos,SizeNumberNB.cx,SizeNumberNB.cy,wNumber*SizeNumberNB.cx,0);

				//设置变量
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

	//绘画焦点
	if ((pTableResource->m_bShowFocus==true)&&(pParameterGlobal->m_bSalienceTable==true)&&(bMySelfTable==true))
	{
 		if( !pTableResource->m_ImageTableBOX.IsNull() )
 			pTableResource->m_ImageTableBOX.DrawImage(pDC, nXPos, nYPos);
//		LOGI("CTableView::DrawTableView(),SizeTable.cx="<<SizeTable.cx<<",SizeTable.cy="<<SizeTable.cy);
//		pDC->Draw3dRect(0,0,SizeTable.cx,SizeTable.cy,RGB(255,0,0),RGB(0,0,255));
	}

	//辅助线
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

//绘画名字
VOID CTableView::DrawTableName(CDC * pDC, INT nXPos, INT nYPos, WORD wChairID, LPCTSTR pszNickName, CTableResource * pTableResource, COLORREF crTextColor)
{
	//用户名字
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


	//用户名字
// 	CRect rcName=pTableResource->m_rcNameArray[wChairID];
// 
// 	//绘图计算
// 	CRect rcDrawRect=rcName;
// 	INT nNameLength=lstrlen(pszNickName);
// 	INT nDrawStyte=pTableResource->m_nDrawStyle[wChairID];
// 	INT nDrawFormat=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS;
// 	INT nNameHeight=nNameLength*12;
// 
// 	//横向位置
// 	if (nNameHeight<=12)
// 	{
// 		if (nDrawStyte&NT_LEFT) nDrawFormat|=DT_LEFT;
// 		else if (nDrawStyte&NT_RIGHT) nDrawFormat|=DT_RIGHT;
// 		else nDrawFormat|=(DT_CENTER|DT_SINGLELINE);
// 	}
// 	else nDrawFormat|=DT_LEFT;
// 
// 	//竖向位置
// 	INT nYPosT=0;
// 	if (nDrawStyte&NT_BOTTOM) nYPosT=rcName.Height()-nNameHeight;
// 	else if (nDrawStyte&NT_TOP) nYPosT=0;
// 	else nYPosT=(rcName.Height()-nNameHeight)/2;
// 
// 	//构造位置
// 	rcDrawRect=rcName;
// 	rcDrawRect.left=rcName.left;
// 	rcDrawRect.right=rcName.right;
// 	rcDrawRect.top=__max(rcName.top,rcName.top+nYPosT);
// 	rcDrawRect.bottom=__min(rcName.bottom,rcDrawRect.top+nNameHeight);
// 
// 	rcDrawRect.OffsetRect(nXPos, nYPos);
// 
// 	//用户名字
// 	m_pDrawFont->DrawText(pDC,pszNickName,&rcDrawRect,crTextColor,nDrawFormat);

	return;
}

//绘画头像
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
			//变量定义
			wDrawChairID=(wChairID)%m_TableAttribute.wChairCount;
			wGoldLevel=m_pITableViewFrame->GetGameLevelParser()->GetGameLevel(pIClientUserItem->GetUserScore());

//			LOGI("CTableView::DrawUserAvatar, wGoldLevel="<<wGoldLevel);
			//变量设置
			wFaceID=(wGoldLevel+12*((pIClientUserItem->GetGender()==GENDER_FEMALE)?0:1));
		}
		else if(pTableResource->m_lDefaultFace==3)
		{
		
		}
		else
		{
			//绘制玩家自选或者自定义头像
		}
	}

	//绘画头像
	DrawUserAvatar( pDC, nXPos, nYPos, wFaceID, wDrawChairID, pTableResource );

	return;
}

VOID CTableView::DrawUserAvatar( CDC * pDC, INT nXPos, INT nYPos, WORD wFaceID, WORD wChairID, CTableResource * pTableResource )
{
	//绘制缺省的3D头像
	if (pTableResource->m_lDefaultFace==3)
	{
		//设置变量
		WORD wLineCount=pTableResource->m_ImageUserFace_3D.GetWidth()/(FACE_CX_3D*ONE_FACE_COUNT_3D);
		WORD wFullCount=wLineCount*pTableResource->m_ImageUserFace_3D.GetHeight()/FACE_CY_3D;
		int nOffHeight = (wFaceID / 2) * FACE_CY_3D;
		int nOffWidth = (wFaceID % 2) * (FACE_CX_3D * ONE_FACE_COUNT_3D);

		//设置变量
		nOffWidth += pTableResource->m_ptAvatarIndex[wChairID] * FACE_CX_3D;
		nOffWidth += ((m_TableAttribute.bPlaying==true)?(FACE_CX_3D*4):0);

		//绘画人物
		pTableResource->m_ImageUserFace_3D.DrawImage(pDC,pTableResource->m_ptAvatarArray[wChairID].x + nXPos,
			pTableResource->m_ptAvatarArray[wChairID].y + nYPos,FACE_CX_3D,FACE_CY_3D,nOffWidth,nOffHeight);
	}
	//绘制缺省的2D头像
	else if(pTableResource->m_lDefaultFace==2)
	{
//		LOGI("CTableView::DrawUserAvatar, wFaceID="<<wFaceID);
		//设置变量
		WORD ww = pTableResource->m_ImageUserFace_2D.GetWidth();
		WORD wLineCount=pTableResource->m_ImageUserFace_2D.GetWidth()/FACE_CX_2D;
		int nOffWidth = wFaceID % 12 * FACE_CX_2D;
		int nOffHeight = wFaceID / 12 * FACE_CY_2D * 2;

		//绘画人物
		pTableResource->m_ImageUserFace_2D.DrawImage(pDC,pTableResource->m_ptAvatarArray[wChairID].x + nXPos,
			pTableResource->m_ptAvatarArray[wChairID].y + nYPos,FACE_CX_2D,FACE_CY_2D,nOffWidth,nOffHeight);
		
	}
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CTableViewFrame::CTableViewFrame()
{
	//状态变量
	m_bHovering=false;
	m_bLMouseDown=false;
	m_bRMouseDown=false;
	m_bWaitDistribute=false;
	m_cbButtonDown=0;
	m_cbButtonHover=0;

	//属性变量
	m_wTableCount=0;
	m_wChairCount=0;
	m_dwServerRule=0;
	m_wServerType=0;

	//滚动信息
	m_nXExcursion=0;
	m_nYExcursion=0;
	m_nXTableCount=0;
	m_nYTableCount=0;

	//滚动信息
	m_nScrollPos=0;
	m_nScrollPosMax=0;	
	m_nVisibleLineCount=0;

	//索引变量
	m_wDownChairID=INVALID_CHAIR;
	m_wDownTableID=INVALID_TABLE;

	//设置指针
	m_pITableViewFrameSink=NULL;

	//比赛变量
	m_dwTotalUser=0;
	m_dwWaitting=0;
	m_cbMatchStatus=MS_NULL;
	m_wTipTableID=INVALID_TABLE;
	m_wTipChairID=INVALID_CHAIR;
	m_wLastTipChairID=INVALID_CHAIR;

	return;
}

//析构函数
CTableViewFrame::~CTableViewFrame()
{
	//删除桌子
	for (INT_PTR i=0;i<m_TableViewArray.GetCount();i++)
	{
		CTableView * pTableView=m_TableViewArray[i];
		if (pTableView!=NULL) SafeDelete(pTableView);
	}

	//删除数组
	m_TableViewArray.RemoveAll();

	return;
}

//接口查询
VOID * CTableViewFrame::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableViewFrame,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableViewFrame,Guid,dwQueryVer);
	return NULL;
}

//创建函数
bool CTableViewFrame::CreateTableFrame(CWnd * pParentWnd, UINT uWndID, IUnknownEx * pIUnknownEx)
{
	//设置变量
	m_bHovering=false;
	m_bLMouseDown=false;
	m_bRMouseDown=false;
	m_wDownChairID=INVALID_CHAIR;
	m_wDownTableID=INVALID_TABLE;
	m_cbButtonDown=0;
	m_cbButtonHover=0;

	//设置接口
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableViewFrameSink)!=NULL);
	m_pITableViewFrameSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableViewFrameSink);

	//创建窗口
	CRect rcCreate(0,0,0,0);
	Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rcCreate,pParentWnd,uWndID);

	//创建字体
	m_DrawFont.CreateFont(this, TEXT("宋体"), 12, 400);

	//创建控件
	//m_DlgTipUserInfo.Create(IDD_DLG_TIP_USERINFOR,this);

    m_UserInfoDlgEx.Create(IDD_USER_INFO,this);
	return true;
}

//配置函数
bool CTableViewFrame::ConfigTableFrame(WORD wTableCount, WORD wChairCount, TCHAR(* szTableNameArray)[LEN_TABLENAME], DWORD dwServerRule, WORD wServerType, WORD wServerID, LPCTSTR pszResDirectory)
{
	//效验参数
	ASSERT(wChairCount<=MAX_CHAIR);
	ASSERT((pszResDirectory!=NULL)&&(pszResDirectory[0]!=0));

	//设置滚动
	m_SkinScrollBar.InitScroolBar(this);

	//定义变量
	bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(dwServerRule);

	//加载资源
	if (m_TableResource.LoadResource(pszResDirectory,wChairCount, bAvertCheatMode)==false)
	{
		return false;
	}

	//设置变量
	m_wTableCount=wTableCount;
	m_wChairCount=wChairCount;
	m_dwServerRule=dwServerRule;
	m_wServerType=wServerType;
	m_TableViewArray.SetSize(m_wTableCount);
	ZeroMemory(m_TableViewArray.GetData(),m_wTableCount*sizeof(CTableView *));

	//创建桌子
	for (WORD i=0;i<m_wTableCount;i++)
	{
		m_TableViewArray[i]=new CTableView;
		m_TableViewArray[i]->InitTableView(i,wChairCount, szTableNameArray[i], this);
		m_TableViewArray[i]->SetFont(&m_DrawFont);
	}

	//调整位置
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	//注册事件
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
		
		//广告控件
		CRect rcCreate(0,0,0,0);
		m_BrowseMatchInfo.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,rcCreate,this,IDC_WEB_PUBLICIZE);

		//构造地址
		TCHAR szBillUrl[256]=TEXT("");
		_sntprintf(szBillUrl,CountArray(szBillUrl),TEXT("%s/Ads/MatchInfo.aspx?ServerID=%d"),szPlatformLink,wServerID);

		//设置广告
		m_BrowseMatchInfo.Navigate(szBillUrl);
		m_BrowseMatchInfo.SetWindowPos(NULL,0,m_ImageTableTL.GetHeight(),rcClient.Width(),rcClient.Height()-m_ImageTableTL.GetHeight(),SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE);

		//本地路径
		TCHAR szPath[MAX_PATH]=TEXT("");
		GetCurrentDirectory(MAX_PATH,szPath);
		_sntprintf(szBillUrl,CountArray(szBillUrl),TEXT("%s\\%s\\MatchInfo\\Match%d.html"),szPath,pszResDirectory,wServerID);
		m_BrowseMatchInfo.SetSpareURL(szBillUrl);
	}

	return true;
}

//获取用户
IClientUserItem * CTableViewFrame::GetClientUserItem(WORD wTableID, WORD wChairID)
{
	//获取桌子
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//获取用户
	if (pITableView!=NULL)
	{
		return pITableView->GetClientUserItem(wChairID);
	}

	return NULL;
}

//设置信息
bool CTableViewFrame::SetClientUserItem(WORD wTableID, WORD wChairID, IClientUserItem * pIClientUserItem)
{
	ITableView * pITableView=GetTableViewItem(wTableID);
	if (pITableView!=NULL) pITableView->SetClientUserItem(wChairID,pIClientUserItem);
	return true;
}

//游戏标志
bool CTableViewFrame::GetPlayFlag(WORD wTableID)
{
	//获取桌子
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//获取标志
	if (pITableView!=NULL)
	{
		return pITableView->GetPlayFlag();
	}

	return false;
}

//密码标志
bool CTableViewFrame::GetLockerFlag(WORD wTableID)
{
	//获取桌子
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//获取标志
	if (pITableView!=NULL)
	{
		return pITableView->GetLockerFlag();
	}

	return false;
}

//焦点框架
VOID CTableViewFrame::SetFocusFrame(WORD wTableID, bool bFocusFrame)
{
	//获取桌子
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//设置标志
	if (pITableView!=NULL) pITableView->SetFocusFrame(bFocusFrame);

	return;
}

//桌子状态 
VOID CTableViewFrame::SetTableStatus(WORD wTableID, bool bPlaying, bool bLocker)
{
	//获取桌子
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//设置标志
	if (pITableView!=NULL) pITableView->SetTableStatus(bPlaying,bLocker);

	return;
}

//桌子状态 
VOID CTableViewFrame::SetTableStatus(bool bWaitDistribute)
{
	m_bWaitDistribute=bWaitDistribute;
	RedrawWindow();
	return;
}

//桌子可视
bool CTableViewFrame::VisibleTable(WORD wTableID)
{
	//效验参数
	ASSERT(wTableID<m_wTableCount);
	if (wTableID>=m_wTableCount) return false;

	//计算位置
	INT nLastPos=m_nScrollPos;
	INT nXPos=(wTableID%m_nXTableCount)*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	INT nYPos=(wTableID/m_nXTableCount)*m_TableResource.m_SizeTable.cy-m_nScrollPos;

	//构造位置
	CRect rcTable,rcClient;
	GetClientRect(&rcClient);
	rcTable.SetRect(nXPos,nYPos,nXPos+m_TableResource.m_SizeTable.cx,nYPos+m_TableResource.m_SizeTable.cy);

	//位置调整
	if (rcTable.top<0L) m_nScrollPos+=rcTable.top;
	if ((rcTable.top>0)&&(rcTable.bottom>rcClient.bottom)) m_nScrollPos+=rcTable.bottom-rcClient.bottom;
	m_nScrollPos=__max(0,__min(m_nScrollPos,m_nScrollPosMax-m_nVisibleLineCount*m_TableResource.m_SizeTable.cy));

	//滚动视图
	if (nLastPos!=m_nScrollPos)
	{
		SetScrollPos(SB_VERT,m_nScrollPos);
		ScrollWindow(0,nLastPos-m_nScrollPos,NULL,NULL);
		InvalidateRect(NULL);	
	}

	return true;
}

//闪动桌子
bool CTableViewFrame::FlashGameTable(WORD wTableID)
{
	//获取桌子
	ITableView * pITableView=GetTableViewItem(wTableID);

	//错误判断
	if (pITableView==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//变量定义
	INT nXItem=wTableID%m_nXTableCount;
	INT nYItem=wTableID/m_nXTableCount;

	//计算位置
	CRect rcTable;
	rcTable.left=nXItem*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	rcTable.top=nYItem*m_TableResource.m_SizeTable.cy-m_nScrollPos;
	rcTable.right=(nXItem+1)*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	rcTable.bottom=(nYItem+1)*m_TableResource.m_SizeTable.cy+m_nYExcursion-m_nScrollPos;
	
	//定义参数
	INT nFlashTimes=20,nStepWidth=4;
	INT nExpandPos=nFlashTimes*nStepWidth;
	rcTable.InflateRect(nExpandPos,nExpandPos);

	//更新窗口
	UpdateWindow();

	//循环绘画
	CWindowDC WindowDC(this);
	CPen LinePen(PS_SOLID,0,GetSysColor(COLOR_BTNHILIGHT));

	//设置环境
	WindowDC.SetROP2(R2_XORPEN);
	WindowDC.SelectObject(&LinePen);

	//循环绘画
	for (INT nTimes=nFlashTimes;nTimes>=0;nTimes--)
	{
		//绘画线条
		WindowDC.MoveTo(rcTable.left,rcTable.top);
		WindowDC.LineTo(rcTable.right,rcTable.top);
		WindowDC.LineTo(rcTable.right,rcTable.bottom);
		WindowDC.LineTo(rcTable.left,rcTable.bottom);
		WindowDC.LineTo(rcTable.left,rcTable.top);

		//绘画暂停
		Sleep((nFlashTimes-nTimes)*2);

		//绘画线条
		WindowDC.MoveTo(rcTable.left,rcTable.top);
		WindowDC.LineTo(rcTable.right,rcTable.top);
		WindowDC.LineTo(rcTable.right,rcTable.bottom);
		WindowDC.LineTo(rcTable.left,rcTable.bottom);
		WindowDC.LineTo(rcTable.left,rcTable.top);

		//设置矩形
		rcTable.DeflateRect(nStepWidth,nStepWidth);
	}

	//释放资源
	WindowDC.DeleteDC();
	LinePen.DeleteObject();

	return true;
}

//闪动椅子
bool CTableViewFrame::FlashGameChair(WORD wTableID, WORD wChairID)
{
	//获取桌子
	ITableView * pITableView=GetTableViewItem(wTableID);

	//错误判断
	if (pITableView==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//变量定义
	INT nXItem=wTableID%m_nXTableCount;
	INT nYItem=wTableID/m_nXTableCount;
	CRect rcChair=m_TableResource.m_rcChairArray[wChairID];

	//获取位置
	INT nXTablePos=nXItem*m_TableResource.m_SizeTable.cx;
	INT nYTablePos=nYItem*m_TableResource.m_SizeTable.cy;
	rcChair.OffsetRect(nXTablePos+m_nXExcursion,nYTablePos+m_nYExcursion-m_nScrollPos);

	//定义参数
	INT nFlashTimes=20,nStepWidth=4;
	INT nExpandPos=nFlashTimes*nStepWidth;
	rcChair.InflateRect(nExpandPos,nExpandPos);

	//更新窗口
	UpdateWindow();

	//循环绘画
	CClientDC ClientDC(this);
	CPen LinePen(PS_SOLID,0,RGB(255,255,255));

	//设置环境
	ClientDC.SetROP2(R2_XORPEN);
	CPen * pOldPen=ClientDC.SelectObject(&LinePen);

	//循环绘画
	for (INT nTimes=nFlashTimes;nTimes>=0;nTimes--)
	{
		//绘画线条
		ClientDC.MoveTo(rcChair.left,rcChair.top);
		ClientDC.LineTo(rcChair.right,rcChair.top);
		ClientDC.LineTo(rcChair.right,rcChair.bottom);
		ClientDC.LineTo(rcChair.left,rcChair.bottom);
		ClientDC.LineTo(rcChair.left,rcChair.top);

		//绘画暂停
		Sleep((nFlashTimes-nTimes)*2);

		//绘画线条
		ClientDC.MoveTo(rcChair.left,rcChair.top);
		ClientDC.LineTo(rcChair.right,rcChair.top);
		ClientDC.LineTo(rcChair.right,rcChair.bottom);
		ClientDC.LineTo(rcChair.left,rcChair.bottom);
		ClientDC.LineTo(rcChair.left,rcChair.top);

		//设置矩形
		rcChair.DeflateRect(nStepWidth,nStepWidth);
	}

	//释放资源
	ClientDC.SelectObject(pOldPen);

	return true;
}

//更新桌子
bool CTableViewFrame::UpdateTableView(WORD wTableID)
{
	//获取桌子
	ITableView * pITableView=GetTableViewItem(wTableID);
	if ((pITableView==NULL)||(m_nXTableCount==0)) return false;

	//变量定义
	INT nXItem=wTableID%m_nXTableCount;
	INT nYItem=wTableID/m_nXTableCount;

	//更新桌子
	CRect rcTable;
	rcTable.left=nXItem*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	rcTable.right=(nXItem+1)*m_TableResource.m_SizeTable.cx+m_nXExcursion;
	rcTable.top=nYItem*m_TableResource.m_SizeTable.cy+m_nYExcursion-m_nScrollPos;
	rcTable.bottom=(nYItem+1)*m_TableResource.m_SizeTable.cy+m_nYExcursion-m_nScrollPos;

	//更新桌子
	RedrawWindow(&rcTable,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_ERASENOW|RDW_UPDATENOW);

	return true;
}

//获取桌子
ITableView * CTableViewFrame::GetTableViewItem(WORD wTableID)
{
	//获取桌子
	if (wTableID!=INVALID_TABLE)
	{
		//效验参数
		ASSERT(wTableID<m_TableViewArray.GetCount());
		if (wTableID>=m_TableViewArray.GetCount()) return NULL;

		//获取桌子
		ITableView * pITableView=m_TableViewArray[wTableID];

		return pITableView;
	}

	return NULL;
}

//空椅子数
WORD CTableViewFrame::GetNullChairCount(WORD wTableID, WORD & wNullChairID)
{
	//获取桌子
	ASSERT(GetTableViewItem(wTableID)!=NULL);
	ITableView * pITableView=GetTableViewItem(wTableID);

	//获取状态
	if (pITableView!=NULL)
	{
		return pITableView->GetNullChairCount(wNullChairID);
	}

	return 0;
}

//重画消息
VOID CTableViewFrame::OnPaint()
{
	//创建 DC
	CPaintDC dc(this);

	//获取区域
	CRect rcClip;
	CRect rcClient;
	dc.GetClipBox(&rcClip);
	GetClientRect(&rcClient);
	
	//创建缓冲
	CBitImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//创建设备
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(&dc);
	BufferDC.SelectObject(ImageBuffer);
	
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	// 比赛模式
	if((m_wServerType&GAME_GENRE_MATCH)!=0)
	{
		//获取区域
		CRect rcClient;
		GetClientRect(&rcClient);

		//绘画空隙
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
		
			/*  !!比赛模式这个地方再重写吧
			//创建字体
			CDFontEx DrawFont;
			DrawFont.CreateFont(this, TEXT("黑体"), 18, 300);
			pBufferDC->SetTextColor(RGB(255,255,255));
			
			int nXPos=__max(rcClient.Width()/2,400);
			int nYPos=50;

			DrawFont.DrawText(&BufferDC, TEXT("报名人数："), nXPos-100,20, RGB(255,255,255), DT_LEFT|DT_TOP);
			DrawNumberString(&BufferDC, m_ImageNumberYellow, m_dwWaitting, nXPos,nYPos+15,TA_LEFT);
			DrawNumberString(&BufferDC, m_ImageNumberGreen, m_dwTotalUser, nXPos+38,nYPos+55,TA_RIGHT);	
			m_ImageLine.DrawImage(&BufferDC,nXPos+22,nYPos+10);

			//释放资源
			DrawFont.DeleteFont();*/
		}

		//绘画进入
		if ((m_ImageEnterMatch.IsNull()==false) && m_ImageLeaveMatch.IsNull()==false)
		{
			//进入游戏
			INT nXEnterIndex=0;
			bool bEnterButtonDown=(m_cbButtonDown==BT_ENTER_GAME||m_cbButtonDown==BT_LEAVE_MATCH);
			bool bEnterButtonHover=(m_cbButtonHover==BT_ENTER_GAME||m_cbButtonDown==BT_LEAVE_MATCH);

			if ((bEnterButtonHover==true)&&(bEnterButtonDown==true)) nXEnterIndex=1;
			if ((bEnterButtonHover==true)&&(bEnterButtonDown==false)) nXEnterIndex=2;

			if(m_cbMatchStatus==MS_NULL)
			{
				//绘画标志
				CSize SizeEnter=CSize(m_ImageEnterMatch.GetWidth()/5,m_ImageEnterMatch.GetHeight());
				m_ImageEnterMatch.BitBlt(BufferDC.GetSafeHdc(),rcClient.Width()-ENTER_MATCH_BUTTON_X,ENTER_MATCH_BUTTON_Y,
					SizeEnter.cx,SizeEnter.cy,SizeEnter.cx*nXEnterIndex,0);
			}
			else if(m_cbMatchStatus==MS_SIGNUP)
			{
				//绘画标志
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

		//绘画界面
		dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

		return ;
	}

	//信息位置
	INT nXPos=0;
	INT nYPos=0;

	//绘画桌子
	if ((m_TableViewArray.GetCount()>0L)&&(m_TableResource.m_bShowTable==true))
	{
		//定义变量
		CSize SizeTable=m_TableResource.m_SizeTable;

		//定义变量
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

		//绘画准备
		INT nXStartItem=(rcClip.left-nXExcursion)/SizeTable.cx;
		INT nYStartItem=(m_nScrollPos-nYExcursion+rcClip.top)/SizeTable.cy;
		INT nXConcludeItem=(rcClip.right-nXExcursion+SizeTable.cx-1)/SizeTable.cx;
		INT nYConcludeItem=(m_nScrollPos-nYExcursion+rcClip.bottom+SizeTable.cy-1)/SizeTable.cy;

//		LOGI("index = " << nXStartItem << " " << nXConcludeItem << " " << nYStartItem << " " << nYConcludeItem)

		//绘画桌子
		for (INT nYIndex=nYStartItem;nYIndex<nYConcludeItem;nYIndex++)
		{
			for (INT nXIndex=nXStartItem;nXIndex<nXConcludeItem;nXIndex++)
			{
				//位置定义
				INT nIndex=nYIndex*m_nXTableCount+nXIndex;
				INT nXTablePos=nXIndex*SizeTable.cx+nXExcursion;
				INT nYTablePos=nYIndex*SizeTable.cy+nYExcursion-m_nScrollPos;


				//位置计算
				if(nIndex==m_wTipTableID && m_wTipChairID!=INVALID_CHAIR)
				{
					CRect &rc=m_TableResource.m_rcChairArray[m_wTipChairID];
					nXPos=nXTablePos+rc.left+rc.Width()+18;
					nYPos=nYTablePos+rc.top+rc.Height()+18;
				//	if(nXPos+TIP_USER_INFO_W > rcClient.right) nXPos -= (TIP_USER_INFO_W+rc.Width()+18);
				//	if(nYPos+TIP_USER_INFO_H > rcClient.bottom) nYPos -= (rc.Height()/2+TIP_USER_INFO_H);
				}

				//绘画桌子
				if ((nIndex>=0)&&(nXIndex<nXTableCount)&&(nIndex<wTableCount))
				{
					//绘画空隙
 					if (m_TableResource.m_ImageGround.IsNull())
 					{
 						SpreadDrawImage(&BufferDC,m_TableResource.m_ImageBitGround,nXTablePos,nYTablePos,SizeTable.cx,SizeTable.cy);
 					}
 					else
 					{
 						SpreadDrawImage(&BufferDC,m_TableResource.m_ImageGround,nXTablePos,nYTablePos,SizeTable.cx,SizeTable.cy);
 					}
					

					//绘画桌子
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
					//绘画空隙
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
		//绘画空隙
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

		//绘画空隙
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
		//绘画空隙
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

//位置消息
VOID CTableViewFrame::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;
}

//消息解释
BOOL CTableViewFrame::PreTranslateMessage(MSG * pMsg)
{
	//消息过滤
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

//滚动消息
VOID CTableViewFrame::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	LOGI("OnVScroll")
	//设置焦点
	SetFocus();

	//禁止滚动
	if((m_wServerType&GAME_GENRE_MATCH)!=0) return; 

	//获取参数
	CRect rcClient;
	GetClientRect(&rcClient);
	INT nLastPos=m_nScrollPos;

	//移动坐标
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
	
	//调整位置
	m_nScrollPos=__min(m_nScrollPos,m_nScrollPosMax-rcClient.bottom);
	if (m_nScrollPos<0) m_nScrollPos=0;

	//滚动窗口
	if (nLastPos!=m_nScrollPos)
	{
		SetScrollPos(SB_VERT,m_nScrollPos);
		ScrollWindow(0,nLastPos-m_nScrollPos,NULL,NULL);
		InvalidateRect(NULL);		
	}

	return;
}

//鼠标消息
VOID CTableViewFrame::OnLButtonDblClk(UINT nFlags, CPoint MousePoint)
{
	//设置焦点
	SetFocus();

	//事件通知
	m_pITableViewFrameSink->OnDButtonHitTable(m_wDownTableID);

	return;
}

//鼠标消息
LRESULT CTableViewFrame::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_bHovering=false;

	//设置界面
	if (m_wDownTableID!=INVALID_TABLE) 
	{
		//设置变量
		WORD wTableID=m_wDownTableID;
		m_wDownTableID=INVALID_TABLE;

		//设置界面
		ITableView * pITableView=GetTableViewItem(wTableID);
		if (pITableView!=NULL) pITableView->SetHoverStatus(INVALID_CHAIR);
	}

	return 0;
}

//鼠标消息
VOID CTableViewFrame::OnLButtonDown(UINT nFlags, CPoint MousePoint)
{
	//设置焦点
	SetFocus();

	//按下处理
	if (m_cbButtonHover!=0)
	{
		//鼠标扑获
		SetCapture();

		//设置变量
		m_cbButtonDown=m_cbButtonHover;

		//更新界面
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE);
	}

	//动作处理
	if ((m_bRMouseDown==false)&&(m_bLMouseDown==false))
	{
		//鼠标扑获
		SetCapture();

		//设置变量
		m_bLMouseDown=true;
		m_wDownTableID=SwitchTableID(MousePoint);

		//获取桌子
		ITableView * pITableView=NULL;
		if (m_wDownTableID!=INVALID_TABLE) pITableView=GetTableViewItem(m_wDownTableID);

		//动作处理
		if (pITableView!=NULL)
		{
			//位置定义
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y-m_nYExcursion+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//按键测试
			m_wDownChairID=SwitchChairID(TablePoint);

			//结果处理
			if (m_wDownChairID!=INVALID_CHAIR) 
			{
				pITableView->SetMouseDown(m_bLMouseDown);
				pITableView->SetHoverStatus(m_wDownChairID);
			}
		}
	}

	return;
}

//鼠标消息
VOID CTableViewFrame::OnLButtonUp(UINT nFlags, CPoint MousePoint)
{
	//结果动作
	BYTE cbButtonDown=0;
	WORD wViewItemDown=INVALID_WORD;

	//比赛按钮
	if ((m_wServerType&GAME_GENRE_MATCH)!=0)
	{
		//释放捕获
		ReleaseCapture();

		//设置变量
		if (m_cbButtonDown==m_cbButtonHover)
		{
			cbButtonDown=m_cbButtonDown;
		}

		//按起处理
		if (m_cbButtonDown!=0)
		{
			//设置变量
			m_cbButtonDown=0;

			//更新界面
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE);
		}

		//执行动作
		if (cbButtonDown!=0)
		{
			ASSERT(m_pITableViewFrameSink!=NULL);
			if (m_pITableViewFrameSink!=NULL) m_pITableViewFrameSink->OnHitTableButton(cbButtonDown);
		}

		return;
	}

	//点击判断
	if ((m_bLMouseDown==true)&&(m_bRMouseDown==false))
	{
		//释放捕获
		ReleaseCapture();

		//设置变量
		m_bLMouseDown=false;

		//变量定义
		WORD wChairID=INVALID_CHAIR;
		WORD wTableID=SwitchTableID(MousePoint);

		//清理桌子
		if (m_wDownTableID!=INVALID_TABLE)
		{
			//获取桌子
			ASSERT(GetTableViewItem(m_wDownTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(m_wDownTableID);

			//设置状态
			pITableView->SetMouseDown(false);
			pITableView->SetHoverStatus(INVALID_CHAIR);
		}

		//获取位置
		if ((wTableID!=INVALID_TABLE)&&(wTableID==m_wDownTableID))
		{
			//获取桌子
			ASSERT(GetTableViewItem(wTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(wTableID);

			//计算位置
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y-m_nYExcursion+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//椅子测试
			wChairID=SwitchChairID(TablePoint);
		}

		//结果处理
		if ((wTableID==m_wDownTableID)&&(wChairID==m_wDownChairID))
		{
			//设置变量
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;

			//事件通知
			m_pITableViewFrameSink->OnLButtonHitTable(wTableID,wChairID);
		}
		else
		{
			//设置变量
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;
		}
	}

	return;
}

//鼠标消息
VOID CTableViewFrame::OnRButtonDown(UINT nFlags, CPoint MousePoint)
{
	//设置焦点
	SetFocus();

	//动作处理
	if ((m_bRMouseDown==false)&&(m_bLMouseDown==false))
	{
		//鼠标扑获
		SetCapture();

		//设置变量
		m_bRMouseDown=true;

		//桌子测试
		m_wDownTableID=SwitchTableID(MousePoint);

		//设置状态
		if (m_wDownTableID!=INVALID_TABLE)
		{
			//获取桌子
			ASSERT(GetTableViewItem(m_wDownTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(m_wDownTableID);

			//计算位置
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y-m_nYExcursion+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//椅子测试
			m_wDownChairID=SwitchChairID(TablePoint);

			//结果处理
			if (m_wDownChairID!=INVALID_CHAIR)
			{
				pITableView->SetMouseDown(m_bRMouseDown);
				pITableView->SetHoverStatus(m_wDownChairID);
			}
		}
	}

	return;
}

//鼠标消息
VOID CTableViewFrame::OnRButtonUp(UINT nFlags, CPoint MousePoint)
{
	if (m_bRMouseDown==true)
	{
		//释放捕获
		ReleaseCapture();

		//设置变量
		m_bRMouseDown=false;

		//变量定义
		WORD wChairID=INVALID_CHAIR;
		WORD wTableID=SwitchTableID(MousePoint);

		//清理状态
		if (m_wDownTableID!=INVALID_TABLE)
		{
			//获取桌子
			ASSERT(GetTableViewItem(m_wDownTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(m_wDownTableID);

			//设置状态
			pITableView->SetMouseDown(false);
			pITableView->SetHoverStatus(INVALID_CHAIR);
		}

		//获取位置
		if ((wTableID!=INVALID_TABLE)&&(wTableID==m_wDownTableID))
		{
			//获取桌子
			ASSERT(GetTableViewItem(wTableID)!=NULL);
			ITableView * pITableView=GetTableViewItem(wTableID);

			//计算位置
			CPoint TablePoint;
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//椅子测试
			wChairID=SwitchChairID(TablePoint);
		}

		//结果处理
		if ((wTableID==m_wDownTableID)&&(wChairID==m_wDownChairID))
		{
			//设置变量
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;

			//事件通知
			m_UserInfoDlgEx.ShowWindow(SW_HIDE);
			m_pITableViewFrameSink->OnRButtonHitTable(wTableID,wChairID);
		}
		else
		{
			//设置变量
			m_wDownTableID=INVALID_TABLE;
			m_wDownChairID=INVALID_CHAIR;
		}
	}

	return;
}

//鼠标消息
BOOL CTableViewFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint MousePoint)
{
	LOGI("OnMouseWheel")
	//禁止滚动
	if((m_wServerType&GAME_GENRE_MATCH)!=0) return TRUE; 

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);
	LOGI("CTableViewFrame::OnMouseWheel, Step 2="<<GetTickCount());

	//设置滚动
	INT nLastPos=m_nScrollPos;
	INT nMaxPos=m_nScrollPosMax-rcClient.bottom;
	m_nScrollPos=__max(__min(m_nScrollPos-zDelta/WHELL_TIMES,nMaxPos),0L);

	//滚动窗口
	SetScrollPos(SB_VERT,m_nScrollPos);
	ScrollWindow(0,nLastPos-m_nScrollPos,NULL,NULL);
	InvalidateRect(NULL);	

	return TRUE;
}

//光标消息
BOOL CTableViewFrame::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//窗口位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//获取光标
	CPoint Point;
	GetCursorPos(&Point);
//	ScreenToClient(&Point);

	if((m_wServerType&GAME_GENRE_MATCH)!=0)
	{
		//位置计算
		BYTE cbButtonHover=GetHoverButton(Point);

		//数据调整
		if ((m_cbButtonDown!=0)&&(cbButtonHover!=m_cbButtonDown))
		{
			cbButtonHover=0;
		}

		//盘旋变量
		if (m_cbButtonHover!=cbButtonHover)
		{
			//设置变量
			m_cbButtonHover=cbButtonHover;

			//更新界面
			RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);			
		}
		//设置光标
		if(cbButtonHover!=0)
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
			return TRUE;
		}
		return __super::OnSetCursor(pWnd,nHitTest,uMessage);
	}

	//进入判断
	if (m_bHovering==false)
	{
		//设置变量
		m_bHovering=true;

		//变量定义
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//注册消息
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//注册事件
		_TrackMouseEvent(&TrackMouseEvent);
	}

	//状态判断
	CPoint TablePoint = CPoint(0, 0);
	if ((m_bRMouseDown==false)&&(m_bLMouseDown==false))
	{
		//获取光标
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);

		//变量定义
		WORD wNewChairID=INVALID_CHAIR;
		WORD wNewTableID=SwitchTableID(MousePoint);

		//获取椅子
		if (wNewTableID!=INVALID_TABLE)
		{
			//变量定义
			TablePoint.x=(MousePoint.x-m_nXExcursion)%m_TableResource.m_SizeTable.cx;
			TablePoint.y=(MousePoint.y-m_nYExcursion+m_nScrollPos)%m_TableResource.m_SizeTable.cy;

			//获取椅子
			wNewChairID=SwitchChairID(TablePoint);
		}

		//设置变量
		if(m_wTipChairID!=wNewChairID)
		{
			m_wLastTipChairID=m_wTipChairID;
			m_wTipTableID=wNewTableID;
			m_wTipChairID=wNewChairID;
			if(m_wTipChairID==INVALID_CHAIR)m_UserInfoDlgEx.ShowWindow(SW_HIDE);
		}
		
		//设置盘旋
		if (((wNewTableID!=m_wDownTableID)||(wNewChairID!=m_wDownChairID))&&((wNewTableID!=INVALID_CHAIR)||(m_wDownChairID!=INVALID_CHAIR)))
		{
			//清理盘旋
			if (m_wDownTableID!=INVALID_TABLE)
			{
				GetTableViewItem(m_wDownTableID)->SetHoverStatus(INVALID_CHAIR);
			}

			//设置盘旋
			if ((wNewTableID!=INVALID_TABLE)&&(wNewChairID!=INVALID_CHAIR))
			{
				GetTableViewItem(wNewTableID)->SetHoverStatus(wNewChairID);
                //获取用户
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

		//设置变量
		m_wDownTableID=wNewTableID;
		m_wDownChairID=wNewChairID;

		//设置光标
		if ((m_wDownTableID!=INVALID_TABLE)&&(m_wDownChairID!=INVALID_CHAIR))
		{
			SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND_CUR)));
			return TRUE;
		}
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//获得矩形框
CRect CTableViewFrame::GetTableRect(WORD wTableID)
{
	if (m_nXTableCount==0)
		return CRect(0,0,0,0);

	CRect rc = CRect(CPoint(m_TableResource.m_SizeTable.cx*wTableID/m_nXTableCount+m_nXExcursion, 
		                    m_TableResource.m_SizeTable.cy*wTableID%m_nXTableCount+m_nYExcursion), 
                     CSize(m_TableResource.m_SizeTable.cx, m_TableResource.m_SizeTable.cy));

	return rc;
};

//桌子测试
WORD CTableViewFrame::SwitchTableID(POINT MousePoint)
{
	//状态效验
	if (m_TableResource.m_SizeTable.cx==0) return INVALID_TABLE;
	if (m_TableResource.m_SizeTable.cy==0) return INVALID_TABLE;

	//位置计算
	INT nXItem=(MousePoint.x-m_nXExcursion)/m_TableResource.m_SizeTable.cx;
	INT nYItem=(MousePoint.y-m_nYExcursion+m_nScrollPos)/m_TableResource.m_SizeTable.cy;

	//结果判断
	if ((nYItem>=0)&&(nXItem>=0)&&(nYItem<m_nYTableCount)&&(nXItem<m_nXTableCount))
	{
		//设置结果
		WORD wResultItem=nYItem*m_nXTableCount+nXItem;

		//结果判断
		if (wResultItem<m_wTableCount)
		{
			bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(m_dwServerRule);
			if ((wResultItem==0)||(bAvertCheatMode==false)) return wResultItem;
		}
	}

	return INVALID_TABLE;
}

//椅子测试
WORD CTableViewFrame::SwitchChairID(POINT MousePoint)
{
	//位置搜索
	if ((m_TableResource.m_bShowUser==true)||(m_TableResource.m_bShowChair==true))
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//横向判断
			if (MousePoint.x<m_TableResource.m_rcChairArray[i].left) continue;
			if (MousePoint.x>m_TableResource.m_rcChairArray[i].right) continue;

			//竖向判断
			if (MousePoint.y<m_TableResource.m_rcChairArray[i].top) continue;
			if (MousePoint.y>m_TableResource.m_rcChairArray[i].bottom) continue;

			return i;
		}
	}

	//进入区域
	if (m_TableResource.m_bShowEnter==true)
	{
		//变量定义
		bool bOutSide=false;

		//横向判断
		if (MousePoint.x<m_TableResource.m_ptEnter.x) bOutSide=true;
		if (MousePoint.x>m_TableResource.m_ptEnter.x+m_TableResource.m_SizeEnter.cx) bOutSide=true;

		//竖向判断
		if (MousePoint.y<m_TableResource.m_ptEnter.y) bOutSide=true;
		if (MousePoint.y>m_TableResource.m_ptEnter.y+m_TableResource.m_SizeEnter.cy) bOutSide=true;

		//结果判断
		if (bOutSide==false)
		{
			return INDEX_ENTER_CHAIR;
		}
	}

	return INVALID_CHAIR;
}

//调整控件
VOID CTableViewFrame::RectifyControl(INT nWidth, INT nHeight)
{
	//调整位置
	if ((m_wTableCount!=0)&&(m_TableResource.m_SizeTable.cx>0L)&&(m_TableResource.m_SizeTable.cy>0L))
	{
		//变量定义
		INT nViewWidth=__max(m_TableResource.m_SizeTable.cx,nWidth);
		INT nViewHeight=__max(m_TableResource.m_SizeTable.cy,nHeight);

		//视图参数
		m_nXTableCount=nViewWidth/m_TableResource.m_SizeTable.cx;
		m_nYTableCount=(m_wTableCount+m_nXTableCount-1)/m_nXTableCount;
		m_nVisibleLineCount=nViewHeight/m_TableResource.m_SizeTable.cy;

		//定义变量
		bool bAvertCheatMode=CServerRule::IsAllowAvertCheatMode(m_dwServerRule);
		if(bAvertCheatMode)
		{
			m_nYTableCount=1;
			m_nVisibleLineCount=1;
		}

		//偏移位置
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
	
	//滚动数据
	m_nScrollPosMax=m_nYTableCount*m_TableResource.m_SizeTable.cy;
	m_nScrollPos=__max(0,__min(m_nScrollPos,m_nScrollPosMax-m_nVisibleLineCount*m_TableResource.m_SizeTable.cy));
	
	//设置滚动
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

//事件消息
LRESULT CTableViewFrame::OnMessagePlatformEvent(WPARAM wParam, LPARAM lParam)
{
	//界面更新
	if (wParam==EVENT_SKIN_CONFIG_UPDATE)
	{
		//渲染判断
		if (m_TableResource.m_bRenderImage==true)
		{
			//更新资源
			CSkinRenderManager * pSkinRenderManager=CSkinRenderManager::GetInstance();
			if (pSkinRenderManager!=NULL) m_TableResource.UpdateResource(pSkinRenderManager);

			//更新界面
			RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASENOW);
		}

		return 0;
	}

	return 0L;
}

//按钮测试
BYTE CTableViewFrame::GetHoverButton(CPoint MousePoint)
{
	//窗口位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//进入位置
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

//绘画数字
VOID CTableViewFrame::DrawNumberString(CDC * pDC, CPngImage &PngNumber, LONG lNumber, INT nXPos, INT nYPos,BYTE cbAlign)
{
	return;
	//加载资源
	INT nNumberHeight=PngNumber.GetHeight();
	INT nNumberWidth=PngNumber.GetWidth()/10;

	//计算数目
	LONG lNumberCount=0;
	LONG lNumberTemp=lNumber;
	do
	{
		lNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0);

	//位置定义
	INT nYDrawPos=nYPos-nNumberHeight/2;
	INT nXDrawPos=nXPos;
	if(cbAlign==TA_RIGHT)
		nXDrawPos+=(lNumberCount-1)*nNumberWidth;
	else if(cbAlign==TA_CENTER)
		nXDrawPos+=(lNumberCount-1)*nNumberWidth/2;

	//绘画桌号
	for (LONG i=0;i<lNumberCount;i++)
	{
		//绘画号码
		LONG lCellNumber=(LONG)(lNumber%10L);
		PngNumber.DrawImage(pDC,nXDrawPos,nYDrawPos,nNumberWidth,nNumberHeight,lCellNumber*nNumberWidth,0);

		//设置变量
		lNumber/=10;
		nXDrawPos-=nNumberWidth;
	};

	return;
}

//绘画图片
VOID CTableViewFrame::SpreadDrawImage(CDC * pDC, CPngImage &PngImage, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight)
{
	//加载资源
	INT nImageWidth=PngImage.GetWidth();
	INT nImageHeight=PngImage.GetHeight();
	INT nDrawRight=nXDest+nDestWidth;
	INT nDrawBottom=nYDest+nDestHeight;

	//绘画图片
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

//绘画图片
VOID CTableViewFrame::SpreadDrawImage(CDC * pDC, CBitImage &BitImage, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight)
{

	//加载资源
	INT nImageWidth=BitImage.GetWidth();
	INT nImageHeight=BitImage.GetHeight();
	INT nDrawRight=nXDest+nDestWidth;
	INT nDrawBottom=nYDest+nDestHeight;

	//绘画图片
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
//比赛状态
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
