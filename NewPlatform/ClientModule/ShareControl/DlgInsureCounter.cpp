#include "StdAfx.h"
#include "Resource.h"
#include "Information.h"
#include "DlgInsureCounter.h"
#include "UserItemElement.h"


//////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgInsurePassword, CDialog)
	ON_WM_ERASEBKGND()

	ON_BN_CLICKED(IDC_BT_OK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BT_CANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(CDlgInsureSave, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_EN_CHANGE(IDC_SCORE, OnEnChangeScore)
	ON_BN_CLICKED(IDC_BT_SAVE_SCORE, OnBnClickedSaveScore)
	ON_BN_CLICKED(IDC_BT_TAKE_SCORE, OnBnClickedTakeScore)
	ON_STN_CLICKED(IDC_FORGET_INSURE, OnBnClickedForgetInsure)

	ON_STN_CLICKED(IDC_HYPERLINK_1, OnBnClickedHyperLink1)
	ON_STN_CLICKED(IDC_HYPERLINK_2, OnBnClickedHyperLink2)
	ON_STN_CLICKED(IDC_HYPERLINK_3, OnBnClickedHyperLink3)
	ON_STN_CLICKED(IDC_HYPERLINK_4, OnBnClickedHyperLink4)
	ON_STN_CLICKED(IDC_HYPERLINK_5, OnBnClickedHyperLink5)
	ON_STN_CLICKED(IDC_HYPERLINK_6, OnBnClickedHyperLink6)
	ON_STN_CLICKED(IDC_HYPERLINK_7, OnBnClickedHyperLink7)
	ON_STN_CLICKED(IDC_HYPERLINK_8, OnBnClickedHyperLink8)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgInsureTransfer, CDialog)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_EN_CHANGE(IDC_SCORE, OnEnChangeScore)
	ON_STN_CLICKED(IDC_FORGET_INSURE, OnBnClickedForgetInsure)
	ON_BN_CLICKED(IDC_BT_TRANSFER_SCORE, OnBnClickedTransferScore)
	ON_BN_CLICKED(IDC_BY_ID, OnBnClickedById)
	ON_BN_CLICKED(IDC_BY_NAME, OnBnClickedByName)
	ON_EN_KILLFOCUS(IDC_NICKNAME, OnKillFocusNickName)

	ON_STN_CLICKED(IDC_HYPERLINK_1, OnBnClickedHyperLink1)
	ON_STN_CLICKED(IDC_HYPERLINK_2, OnBnClickedHyperLink2)
	ON_STN_CLICKED(IDC_HYPERLINK_3, OnBnClickedHyperLink3)
	ON_STN_CLICKED(IDC_HYPERLINK_4, OnBnClickedHyperLink4)
	ON_STN_CLICKED(IDC_HYPERLINK_5, OnBnClickedHyperLink5)
	ON_STN_CLICKED(IDC_HYPERLINK_6, OnBnClickedHyperLink6)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgInsureModifyPassword, CDialog)
	ON_WM_ERASEBKGND()
	ON_STN_CLICKED(IDC_MODIFY_INSURE_PASSWORD, OnBnClickedModifyPassword)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgInsureMain, CSkinDialog)
//	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BT_QUERY_INFO, OnBnClickedQueryInfo)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONTROL, OnTcnSelchangeInsureMode)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////

#define WM_INSUREPLAZA_UPDATA      (WM_USER+1680)             //银行更新消息

struct tagInsurePlazaUpdata
{
	SCORE  lUserScore;
	SCORE  lUserInsure;
};

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInsureItem::CDlgInsureItem()
{
}

//析构函数
CDlgInsureItem::~CDlgInsureItem()
{
}

//获取游戏币
SCORE CDlgInsureItem::GetUserControlScore(CWnd * pWndScore)
{
	//变量定义
	WORD wIndex=0;
	SCORE lScore=0L;

	//获取数值
	TCHAR szBuffer[32]=TEXT("");
	pWndScore->GetWindowText(szBuffer,CountArray(szBuffer));

	//构造数值
	while (szBuffer[wIndex]!=0)
	{
		//变量定义
		WORD wCurrentIndex=wIndex++;

		//构造数值
		if ((szBuffer[wCurrentIndex]>=TEXT('0'))&&(szBuffer[wCurrentIndex]<=TEXT('9')))
		{
			lScore=lScore*10L+(szBuffer[wCurrentIndex]-TEXT('0'));
		}
	}

	return lScore;
}

//绘画数字
VOID CDlgInsureItem::DrawNumberString(CDC * pDC, SCORE lScore, INT nXPos, INT nYPos)
{
	//转换逗号
	TCHAR szControl[128]=TEXT("");
	SwitchScoreFormat(lScore,3L,szControl,CountArray(szControl));

	//变量定义
	INT nXDrawPos=nXPos;
	INT nScoreLength=lstrlen(szControl);

	//绘画判断
	if (nScoreLength>0L)
	{
		//加载资源
		CPngImage ImageNumber;
		ImageNumber.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("SCORE_NUMBER"));

		//获取大小
		CSize SizeNumber;
		SizeNumber.SetSize(ImageNumber.GetWidth()/12L,ImageNumber.GetHeight());

		//绘画数字
		for (INT i=0;i<nScoreLength;i++)
		{
			//绘画逗号
			if (szControl[i]==TEXT(','))
			{
				ImageNumber.DrawImage(pDC,nXDrawPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*10L,0L);
			}

			//绘画点号
			if (szControl[i]==TEXT('.'))
			{
				ImageNumber.DrawImage(pDC,nXDrawPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*11L,0L);
			}

			//绘画数字
			if (szControl[i]>=TEXT('0')&&szControl[i]<=TEXT('9'))
			{
				ImageNumber.DrawImage(pDC,nXDrawPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*(szControl[i]-TEXT('0')),0L);
			}

			//设置位置
			nXDrawPos+=SizeNumber.cx;
		}
	}

	return;
}

//转换字符
VOID CDlgInsureItem::SwitchScoreString(SCORE lScore, LPTSTR pszBuffer, WORD wBufferSize)
{
	//变量定义
	LPCTSTR pszNumber[]={TEXT("零"),TEXT("壹"),TEXT("贰"),TEXT("叁"),TEXT("肆"),TEXT("伍"),TEXT("陆"),TEXT("柒"),TEXT("捌"),TEXT("玖")};
	LPCTSTR pszWeiName[]={TEXT("拾"),TEXT("佰"),TEXT("仟"),TEXT("万"),TEXT("拾"),TEXT("佰"),TEXT("仟"),TEXT("亿"),TEXT("拾"),TEXT("佰"),TEXT("仟"),TEXT("兆"),TEXT("拾"),TEXT("佰"),TEXT("仟"),TEXT("京")};

	//转换数值
	TCHAR szSwitchScore[21]=TEXT("");
	_sntprintf(szSwitchScore,CountArray(szSwitchScore),TEXT("%I64d"),lScore);

	//变量定义
	bool bNeedFill=false;
	bool bNeedZero=false;
	UINT uTargetIndex=0,uSourceIndex=0;

	//字符长度
	UINT uItemLength=lstrlen(pszNumber[0]);
	UINT uSwitchLength=lstrlen(szSwitchScore);

	//转换操作
	for (UINT i=0;i<uSwitchLength;i++)
	{
		//变量定义
		WORD wNumberIndex=szSwitchScore[i]-TEXT('0');

		//补零操作
		if ((bNeedZero==true)&&(wNumberIndex!=0L))
		{
			bNeedZero=false;
			_tcsncat(pszBuffer,pszNumber[0],wBufferSize-lstrlen(pszBuffer)-1);
		}

		//拷贝数字
		if (wNumberIndex!=0L)
		{
			_tcsncat(pszBuffer,pszNumber[wNumberIndex],wBufferSize-lstrlen(pszBuffer)-1);
		}

		//拷贝位名
		if ((wNumberIndex!=0L)&&((uSwitchLength-i)>=2))
		{
			bNeedZero=false;
			_tcsncat(pszBuffer,pszWeiName[uSwitchLength-i-2],wBufferSize-lstrlen(pszBuffer)-1);
		}

		//补零判断
		if ((bNeedZero==false)&&(wNumberIndex==0))
		{
			bNeedZero=true;
		}

		//补位判断
		if ((bNeedFill==false)&&(wNumberIndex!=0))
		{
			bNeedFill=true;
		}

		//填补位名
		if (((uSwitchLength-i)==5)||((uSwitchLength-i)==9))
		{
			//拷贝位名
			if ((bNeedFill==true)&&(wNumberIndex==0L))
			{
				_tcsncat(pszBuffer,pszWeiName[uSwitchLength-i-2],wBufferSize-lstrlen(pszBuffer)-1);
			}

			//设置变量
			bNeedZero=false;
			bNeedFill=false;
		}
	}

	return;
}

//转换字符
VOID CDlgInsureItem::SwitchScoreFormat(SCORE lScore, UINT uSpace, LPTSTR pszBuffer, WORD wBufferSize)
{
	//转换数值
	TCHAR szSwitchScore[21]=TEXT("");
	_sntprintf(szSwitchScore,CountArray(szSwitchScore),TEXT("%I64d"),lScore);

	//变量定义
	WORD wTargetIndex=0;
	WORD wSourceIndex=0;
	UINT uSwitchLength=lstrlen(szSwitchScore);

	//转换字符
	while (szSwitchScore[wSourceIndex]!=0)
	{
		//拷贝字符
		pszBuffer[wTargetIndex++]=szSwitchScore[wSourceIndex++];

		//插入逗号
		if ((uSwitchLength!=wSourceIndex)&&(((uSwitchLength-wSourceIndex)%uSpace)==0L))
		{
			pszBuffer[wTargetIndex++]=TEXT(',');
		}
	}

	//结束字符
	pszBuffer[wTargetIndex++]=0;

	return;
}
/////////////////////////////////////////////////////////////////////////
CDlgInsurePassword::CDlgInsurePassword() : CDialog(IDD_INSURE_PASSWORD)
{
//	m_pDlgInsureMain = NULL;
}

CDlgInsurePassword::~CDlgInsurePassword()
{

}

//控件绑定
VOID CDlgInsurePassword::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BT_OK, m_btOk);
	DDX_Control(pDX, IDC_BT_CANCEL, m_btCancel);
	DDX_Control(pDX, IDC_INSURE_PASSWORD, m_PasswordControl);
}

//消息解释
BOOL CDlgInsurePassword::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//创建函数
BOOL CDlgInsurePassword::OnInitDialog()
{
	__super::OnInitDialog();

	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	// 设置控件
	m_btOk.SetButtonImage(IDB_BT_OK, hResInstance, false, false);
	m_btCancel.SetButtonImage(IDB_BT_CANCEL, hResInstance, false, false);

	m_PasswordControl.SetDrawBorad(false);

	// 移动控件位置
	m_PasswordControl.MoveWindow(165, 31, 180, 18);

	CRect rcBtOk;
	m_btOk.GetClientRect(rcBtOk);
	m_btOk.MoveWindow(169, 230, rcBtOk.Width(), rcBtOk.Height());

	CRect rcBtCancel;
	m_btCancel.GetClientRect(rcBtCancel);
	m_btCancel.MoveWindow(269, 230, rcBtCancel.Width(), rcBtCancel.Height());

	return FALSE;
}

// 确定按钮
VOID CDlgInsurePassword::OnOK()
{
	return;
}

// 取消按钮
VOID CDlgInsurePassword::OnCancel()
{
	return;
}

// 确定密码按钮
VOID CDlgInsurePassword::OnBnClickedOk()
{
	// 提交银行密码

	// 获取密码
	TCHAR szPassword[LEN_PASSWORD] = {0};
	m_PasswordControl.GetUserPassword(szPassword);

	//6603加密密码
	TCHAR szInsurePass[LEN_MD5] = {0};
	CWHEncrypt::MD5Encrypt(szPassword,szInsurePass);

	if (szPassword[0]==0)
	{
		// 提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("银行密码不能为空，请重新输入银行密码！"),MB_ICONERROR,30);

		// 设置焦点
		m_PasswordControl.SetFocus();

		return;
	}

//	MessageBox(szInsurePass);
	if (m_pDlgInsureMain)
	{
		m_pDlgInsureMain->OnMissionStart();
		m_pDlgInsureMain->PerformInsurePassword(szInsurePass);
	}
}

// 取消按钮
VOID CDlgInsurePassword::OnBnClickedCancel()
{
	// 取消银行输入密码框
	if (GetParent())
	{
		GetParent()->PostMessage(WM_CLOSE);
	}
}

BOOL CDlgInsurePassword::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//创建 DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pDCBuffer=CDC::FromHandle(BufferDC);

	// 加载资源
	CPngImage ImageInsureSave;
	ImageInsureSave.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_PASSWORD"));

	//绘画背景
	pDCBuffer->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	// 载入图片，绘制背景
	ImageInsureSave.DrawImage(pDCBuffer, 0, 0, ImageInsureSave.GetWidth(), ImageInsureSave.GetHeight(), 0, 0);


	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pDCBuffer,0,0,SRCCOPY);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTransRecordList, CSkinListCtrl)
END_MESSAGE_MAP()

//绘画函数
VOID CTransRecordList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    //变量定义
	CRect rcItem=lpDrawItemStruct->rcItem;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	

	//获取属性
	INT nItemID=lpDrawItemStruct->itemID;
	INT nColumnCount=m_SkinHeaderCtrl.GetItemCount();

	//定制颜色
	COLORREF rcTextColor=RGB(10,10,10);
	COLORREF rcBackColor=RGB(229,249,255);
	GetItemColor(lpDrawItemStruct,rcTextColor,rcBackColor);

	//绘画区域
	CRect rcClipBox;
	pDC->GetClipBox(&rcClipBox);

	//设置环境
	pDC->SetBkColor(rcBackColor);
	pDC->SetTextColor(rcTextColor);

	

	//绘画子项
	for (INT i=0;i<nColumnCount;i++)
	{
		//获取位置
		CRect rcSubItem;
		GetSubItemRect(nItemID,i,LVIR_BOUNDS,rcSubItem);

		//绘画判断
		if (rcSubItem.left>rcClipBox.right) break;
		if (rcSubItem.right<rcClipBox.left) continue;

		//绘画背景
		pDC->FillSolidRect(&rcSubItem,rcBackColor);

		//绘画数据
		DrawCustomItem(pDC,lpDrawItemStruct,rcSubItem,i);
	}

	/*	//c 去掉焦点绘制
	//绘画焦点
	if (lpDrawItemStruct->itemState&ODS_FOCUS)
	{
		pDC->DrawFocusRect(&rcItem);

	}
	//绘画焦点
	if ((lpDrawItemStruct->itemState&ODS_FOCUS)==false)
	{
		POINT ptSrc,ptDest;
		ptSrc.x=lpDrawItemStruct->rcItem.left;
		ptSrc.y=lpDrawItemStruct->rcItem.bottom-2;
		ptDest.x=lpDrawItemStruct->rcItem.right;
		ptDest.y=lpDrawItemStruct->rcItem.bottom-2;
		pDC->MoveTo(ptSrc);
		pDC->LineTo(ptDest);
	} */  

	return;
}
//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgTransRecord, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()
//构造函数
CDlgTransRecord::CDlgTransRecord()
{
    m_bQueryRecord = true;
	m_cbCurPage=0;
	m_cbPageCell=20;
    m_cbPageCount=0;
}
//析构函数
CDlgTransRecord::~CDlgTransRecord()
{

}


//控件绑定
VOID CDlgTransRecord::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	//按钮控件
	DDX_Control(pDX, IDC_RECORD_LIST, m_ListView);
    DDX_Control(pDX, IDC_QUERY, m_btQuery);
    DDX_Control(pDX, IDC_FIRST, m_btFirst);
    DDX_Control(pDX, IDC_PRE, m_btPre);
    DDX_Control(pDX, IDC_NEXT, m_btNext);
    DDX_Control(pDX, IDC_TAIL, m_btLast);
}

//创建函数
BOOL CDlgTransRecord::OnInitDialog()
{
	__super::OnInitDialog();

//  m_ListView.InsertColumn(0,TEXT("编号"),LVCFMT_CENTER,40,0);
    m_ListView.InsertColumn(0,TEXT("赠送人"),LVCFMT_CENTER,70,0);
	m_ListView.InsertColumn(1,TEXT("接收人"),LVCFMT_CENTER,70,0);
	m_ListView.InsertColumn(2,TEXT("接收人ID"),LVCFMT_CENTER,70,0);
	m_ListView.InsertColumn(3,TEXT("赠送数量"),LVCFMT_CENTER,80,0);
//	m_ListView.InsertColumn(4,TEXT("税收"),LVCFMT_CENTER,80,0);
	m_ListView.InsertColumn(4,TEXT("赠送时间"),LVCFMT_CENTER,170,0);

	return FALSE;
}
void CDlgTransRecord::ShowTransRecord()
{
	m_ListView.DeleteAllItems();
	TCHAR sz[32];
	if (m_vecTransRecord.GetCount()==0)
	{
		m_btPre.EnableWindow(FALSE);
		m_btNext.EnableWindow(FALSE);
		m_btFirst.EnableWindow(FALSE);
		m_btLast.EnableWindow(FALSE);
		_sntprintf(sz,CountArray(sz),TEXT("第%d/%d页"),0,0);
		GetDlgItem(IDC_TIPS)->SetWindowText(sz);
		return;
	}
	
	//for (int index=(m_cbCurPage-1)*m_cbPageCell;index<min(m_vecTransRecord.GetCount(),m_cbPageCell*m_cbCurPage);index++)
	for (int index=min(m_vecTransRecord.GetCount(),m_cbPageCell*m_cbCurPage)-1;index>=(m_cbCurPage-1)*m_cbPageCell;index--)
	{
		RealTransRecord* pTransRecord = m_vecTransRecord.GetAt(index);
		
		//_sntprintf(sz,CountArray(sz),TEXT("%ld"),pTransRecord->dwGameID);
		int j=m_ListView.InsertItem(0,pTransRecord->szSourceAccount);
		//m_ListView.SetItemText(j,1,pTransRecord->szSourceAccount);
		m_ListView.SetItemText(j,1,pTransRecord->szTargetAccounts);
		_sntprintf(sz,CountArray(sz),TEXT("%d"),pTransRecord->dwTargetID);
		m_ListView.SetItemText(j,2,sz);
		_sntprintf(sz,CountArray(sz),TEXT("%d"),pTransRecord->lSwapScore);
		m_ListView.SetItemText(j,3,sz);
		m_ListView.SetItemText(j,4,pTransRecord->szTime);
	}
	m_btPre.EnableWindow(TRUE);
	m_btNext.EnableWindow(TRUE);
	m_btFirst.EnableWindow(TRUE);
	m_btLast.EnableWindow(TRUE);
	if (m_cbCurPage==1)
	{
		m_btPre.EnableWindow(FALSE);
	}
	else
	{
		m_btPre.EnableWindow(TRUE);
	}

	if (m_cbCurPage==m_cbPageCount)
	{
		m_btNext.EnableWindow(FALSE);
	}
	else
	{
		m_btNext.EnableWindow(TRUE);
	}

	_sntprintf(sz,CountArray(sz),TEXT("第%d/%d页"),m_cbCurPage,m_cbPageCount);
	GetDlgItem(IDC_TIPS)->SetWindowText(sz);
}
void CDlgTransRecord::SetRecordList(CMD_GR_RealTransRecord* pRealTransRecord)
{
	if (pRealTransRecord->bOver==false)
	{
		RealTransRecord* pRecord = new RealTransRecord;
		pRecord->dwGameID=pRealTransRecord->dwGameID;
		_tcscpy(pRecord->szSourceAccount,pRealTransRecord->szSourceAccount);
		_tcscpy(pRecord->szTargetAccounts,pRealTransRecord->szTargetAccounts);
		pRecord->lSwapScore=pRealTransRecord->lSwapScore;
		pRecord->dwSourceID=pRealTransRecord->dwSourceID;
		pRecord->dwTargetID=pRealTransRecord->dwTargetID;
		pRecord->lRevenue=pRealTransRecord->lRevenue;
		_tcscpy(pRecord->szTime,pRealTransRecord->szTime);
		m_vecTransRecord.Add(pRecord);
	}
	else
	{	
		if(m_vecTransRecord.GetCount()%m_cbPageCell==0)
		{
			m_cbPageCount=m_vecTransRecord.GetCount()/m_cbPageCell;
		}
		else
		{
			m_cbPageCount=m_vecTransRecord.GetCount()/m_cbPageCell+1;
		}
		m_cbCurPage=1;
		ShowTransRecord();
		GetDlgItem(IDC_STATUS_TIP)->SetWindowText(TEXT(""));
		m_btQuery.EnableWindow(TRUE);
		/*if (m_bQueryRecord)
		{
			m_bQueryRecord=false;
			SetTimer(100,3000,NULL);
		}*/
	}
	
}
void CDlgTransRecord::SetRecordList(CMD_GP_RealTransRecord* pRealTransRecord)
{
	if (pRealTransRecord->bOver==false)
	{
		RealTransRecord* pRecord = new RealTransRecord;
		pRecord->dwGameID=pRealTransRecord->dwGameID;
		_tcscpy(pRecord->szSourceAccount,pRealTransRecord->szSourceAccount);
		_tcscpy(pRecord->szTargetAccounts,pRealTransRecord->szTargetAccounts);
		pRecord->lSwapScore=pRealTransRecord->lSwapScore;
		pRecord->lRevenue=pRealTransRecord->lRevenue;
		pRecord->dwSourceID=pRealTransRecord->dwSourceID;
		pRecord->dwTargetID=pRealTransRecord->dwTargetID;
		_tcscpy(pRecord->szTime,pRealTransRecord->szTime);
		m_vecTransRecord.Add(pRecord);
	}
	else
	{	
		if(m_vecTransRecord.GetCount()%m_cbPageCell==0)
		{
			m_cbPageCount=m_vecTransRecord.GetCount()/m_cbPageCell;
		}
		else
		{
			m_cbPageCount=m_vecTransRecord.GetCount()/m_cbPageCell+1;
		}
		m_cbCurPage=1;
		ShowTransRecord();
		GetDlgItem(IDC_STATUS_TIP)->SetWindowText(TEXT(""));
		m_btQuery.EnableWindow(TRUE);
		if (m_bQueryRecord)
		{
			m_bQueryRecord=false;
			SetTimer(100,5000,NULL);
		}

	}
}
//时间消息
void CDlgTransRecord::OnTimer(UINT nIDEvent)
{
	TRACE("CDlgTransRecord::OnTimer\n");

	//时间消息
	switch(nIDEvent)
	{
	case 100:		//更新界面
		{
			KillTimer(100);
			//更新界面
			m_bQueryRecord=true;
			return;
		}
	}
	__super::OnTimer(nIDEvent);
}
//命令函数
BOOL CDlgTransRecord::OnCommand(WPARAM wParam, LPARAM lParam)
{
    //变量定义
    UINT nCommandID=LOWORD(wParam);

    //功能按钮
    switch (nCommandID)
    {
    case IDC_QUERY:        
        {
			if (m_bQueryRecord==false)
			{
				//AfxMessageBox(TEXT("查询频率太快，请稍候查询!"));
				CInformation Information(this);
				Information.ShowMessageBox(TEXT("查询频率太快，请离上次查询过去5秒后再查询!"),MB_ICONSTOP);
				return TRUE;
			}
			m_ListView.DeleteAllItems();
			for (int i=0;i<m_vecTransRecord.GetCount();i++)
			{
				RealTransRecord* pRecord = m_vecTransRecord.GetAt(i);
				delete pRecord;
				pRecord=NULL;
			}
			m_vecTransRecord.RemoveAll();
            //事件通知
        	m_pDlgInsureMain->OnMissionStart();
        	m_pDlgInsureMain->PerformQueryTransRecord();

			GetDlgItem(IDC_STATUS_TIP)->SetWindowText(TEXT("正在查询....."));
			m_btQuery.EnableWindow(FALSE);

			m_btPre.EnableWindow(FALSE);
			m_btNext.EnableWindow(FALSE);
			m_btFirst.EnableWindow(FALSE);
			m_btLast.EnableWindow(FALSE);
	
            return TRUE;
        }
    case IDC_FIRST:
        {
			m_cbCurPage=1;
			ShowTransRecord();
            return TRUE;
        }
    case IDC_PRE:
        {
			m_cbCurPage--;
			ShowTransRecord();
            return TRUE;
        }
    case IDC_NEXT:
        {
			m_cbCurPage++;
			ShowTransRecord();
            return TRUE;
        }
    case IDC_TAIL:
        {
			m_cbCurPage=m_cbPageCount;
			ShowTransRecord();
            return TRUE;
        }
    }

    return __super::OnCommand(wParam,lParam);
}

//消息函数
//绘画背景
BOOL CDlgTransRecord::OnEraseBkgnd(CDC * pDC)
{
    
    //获取位置
    CRect rcClient;
    GetClientRect(&rcClient);

    //建立缓冲
    CImage ImageBuffer;
    ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

    //创建 DC
    CImageDC BufferDC(ImageBuffer);
    CDC * pDCBuffer=CDC::FromHandle(BufferDC);

    //设置环境
    pDCBuffer->SetBkMode(TRANSPARENT);
    pDCBuffer->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

    //绘画背景
    pDCBuffer->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

    CRect rcListControl;
	m_ListView.GetWindowRect(&rcListControl);
    //转换位置
	ScreenToClient(&rcListControl);
	
    pDCBuffer->Draw3dRect(rcListControl.left-2,rcListControl.top-2,rcListControl.Width()+3,rcListControl.Height()+3,RGB(84,165,213),RGB(84,165,213));

    //绘画界面
    pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pDCBuffer,0,0,SRCCOPY);
    
    
	return TRUE;
}
//控件颜色
HBRUSH CDlgTransRecord::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
    switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_BTN:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlText);

			return CSkinDialog::m_SkinAttribute.m_brBackGround;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInsureSave::CDlgInsureSave() : CDialog(IDD_INSURE_SAVE)
{
	//设置变量
	m_bSwitchIng=false;

	return;
}

//析构函数
CDlgInsureSave::~CDlgInsureSave()
{
}

//控件绑定
VOID CDlgInsureSave::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//按钮控件
	DDX_Control(pDX, IDC_BT_TAKE_SCORE, m_btTakeScore);
	DDX_Control(pDX, IDC_BT_SAVE_SCORE, m_btSaveScore);

	//控件变量
	DDX_Control(pDX, IDC_SCORE, m_edScore);
//	DDX_Control(pDX, IDC_PASSWORD, m_PasswordControl);
//	DDX_Control(pDX, IDC_FORGET_INSURE, m_ForgetInsure);

	DDX_Control(pDX, IDC_HYPERLINK_1, m_hlOneMillion);
	DDX_Control(pDX, IDC_HYPERLINK_2, m_hlFiveMillion);
	DDX_Control(pDX, IDC_HYPERLINK_3, m_hlTenMillion);
	DDX_Control(pDX, IDC_HYPERLINK_4, m_hlFiftyMillion);
	DDX_Control(pDX, IDC_HYPERLINK_5, m_hlOneHundredMillion);
	DDX_Control(pDX, IDC_HYPERLINK_6, m_hlFiveHundredMillion);
	DDX_Control(pDX, IDC_HYPERLINK_7, m_hlAllScore);
	DDX_Control(pDX, IDC_HYPERLINK_8, m_hlAllInsureScore);
}

//创建函数
BOOL CDlgInsureSave::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件
	m_edScore.LimitText(16L);

	//设置按钮
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	m_btSaveScore.SetButtonImage(IDB_BT_SAVE_SCORE, hResInstance, false, false);
	m_btTakeScore.SetButtonImage(IDB_BT_TAKE_SCORE, hResInstance, false, false);

	// 控件位置移动
	m_edScore.MoveWindow(104, 77, 115, 18);

	m_hlOneMillion.MoveWindow(235, 70, 30, 16);
	m_hlFiveMillion.MoveWindow(235, 86, 30,16);
	m_hlTenMillion.MoveWindow(275, 70, 30, 16);
	m_hlFiftyMillion.MoveWindow(275, 86, 30, 16);
	m_hlOneHundredMillion.MoveWindow(315, 70, 18, 16);
	m_hlFiveHundredMillion.MoveWindow(315, 86, 18, 16);
	m_hlAllScore.MoveWindow(340, 70, 60, 16);
	m_hlAllInsureScore.MoveWindow(340, 86, 60, 16);

	m_hlOneMillion.SetUnderLine(true);
	m_hlFiveMillion.SetUnderLine(true);
	m_hlTenMillion.SetUnderLine(true);
	m_hlFiftyMillion.SetUnderLine(true);
	m_hlOneHundredMillion.SetUnderLine(true);
	m_hlFiveHundredMillion.SetUnderLine(true);
	m_hlAllScore.SetUnderLine(true);
	m_hlAllInsureScore.SetUnderLine(true);

	m_hlOneMillion.SetBackGroundColor(RGB(255,255,255));
	m_hlFiveMillion.SetBackGroundColor(RGB(255,255,255));
	m_hlTenMillion.SetBackGroundColor(RGB(255,255,255));
	m_hlFiftyMillion.SetBackGroundColor(RGB(255,255,255));
	m_hlOneHundredMillion.SetBackGroundColor(RGB(255,255,255));
	m_hlFiveHundredMillion.SetBackGroundColor(RGB(255,255,255));
	m_hlAllScore.SetBackGroundColor(RGB(255,255,255));
	m_hlAllInsureScore.SetBackGroundColor(RGB(255,255,255));


	GetDlgItem(IDC_SCORE_STRING)->MoveWindow(250, 77, 140, 22);

	return FALSE;
}

//确定按钮
VOID CDlgInsureSave::OnOK()
{
	return;
}

//取消按钮
VOID CDlgInsureSave::OnCancel()
{
	return;
}

//输入信息
VOID CDlgInsureSave::OnEnChangeScore()
{
	//状态变量
	if (m_bSwitchIng==false)
	{
		//设置变量
		m_bSwitchIng=true;

		//变量定义
		CWnd * pWndScore=GetDlgItem(IDC_SCORE);
		SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);
		SCORE lMaxScore=__max(m_pUserInsureInfo->lUserScore, m_pUserInsureInfo->lUserInsure);
		if((m_pUserInsureInfo->lUserScore==0) || (m_pUserInsureInfo->lUserInsure==0))
			lMaxScore=__max(m_pUserInsureInfo->lUserScore, m_pUserInsureInfo->lUserInsure);

		//输入限制
		if(lScore > lMaxScore) lScore = lMaxScore;

		//设置界面
		if (lScore>0L)
		{
			//转换数值
			TCHAR szString[128]=TEXT("");
			m_DlgInsureItem.SwitchScoreString(lScore,szString,CountArray(szString));

			//转换逗号
			TCHAR szControl[128]=TEXT("");
			m_DlgInsureItem.SwitchScoreFormat(lScore,3L,szControl,CountArray(szControl));

			//设置控件
			SetDlgItemText(IDC_SCORE,szControl);
			SetDlgItemText(IDC_SCORE_STRING,szString);

			//设置光标
			INT nScoreLength=m_edScore.GetWindowTextLength();
			m_edScore.SetSel(nScoreLength,nScoreLength,FALSE);
		}
		else
		{
			//设置控件
			SetDlgItemText(IDC_SCORE,TEXT(""));
			SetDlgItemText(IDC_SCORE_STRING,TEXT(""));
		}

		//设置变量
		m_bSwitchIng=false;
	}

	return;
}

//取款按钮
VOID CDlgInsureSave::OnBnClickedTakeScore()
{
	//获取变量
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lTakeScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	//获取密码
//	TCHAR szPassword[LEN_PASSWORD];
//	m_PasswordControl.GetUserPassword(szPassword);

	//加密密码
//	TCHAR szInsurePass[LEN_MD5];
//	CWHEncrypt::MD5Encrypt(szPassword,szInsurePass);

	//数据判断
	if (lTakeScore<=0L)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("取出的银子数量不能为空，请重新输入游戏币数量！"),MB_ICONERROR,30);

		//设置焦点
		m_edScore.SetFocus();

		return;
	}

	//数目判断
	if (lTakeScore>m_pUserInsureInfo->lUserInsure)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("您保险柜银子的数目余额不足，请重新输入游戏币数量！"),MB_ICONERROR,30);

		//设置焦点
		m_edScore.SetFocus();

		return;
	}

	//密码判断
//	if (szPassword[0]==0)
//	{
		//提示消息
//		CInformation Information(this);
//		Information.ShowMessageBox(TEXT("保险柜密码不能为空，请重新输入保险柜密码！"),MB_ICONERROR,30);

		//设置焦点
//		m_PasswordControl.SetFocus();

//		return;
//	}
	
	//事件通知
	m_pDlgInsureMain->OnMissionStart();
	m_pDlgInsureMain->PerformTakeScore(lTakeScore,m_pDlgInsureMain->m_szInsurePass);

	return;
}

//存款按钮
VOID CDlgInsureSave::OnBnClickedSaveScore()
{
	//变量定义
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lSaveScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	//数据判断
	if (lSaveScore<=0L)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("存入的银子数量不能为空，请重新输入游戏币数量！"),MB_ICONERROR,30);

		//设置焦点
		m_edScore.SetFocus();

		return;
	}

	//数目判断
	if (lSaveScore>m_pUserInsureInfo->lUserScore)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("您所携带银子的数目余额不足，请重新输入游戏币数量！"),MB_ICONERROR,30);

		//设置焦点
		m_edScore.SetFocus();

		return;
	}

	//执行存款
	m_pDlgInsureMain->OnMissionStart();
	m_pDlgInsureMain->PerformSaveScore(lSaveScore);

	return;
}

//忘记密码
VOID CDlgInsureSave::OnBnClickedForgetInsure()
{
	//构造地址
	TCHAR szLogonLink[256]=TEXT("");
	_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("%s/LogonLink4.aspx"),szPlatformLink);

	//打开页面
	ShellExecute(NULL,TEXT("OPEN"),szLogonLink,NULL,NULL,SW_NORMAL);

	return;
}

VOID CDlgInsureSave::OnBnClickedHyperLink1()
{
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	CString str;
	str.Format(TEXT("%I64d"), lScore+1000000);

	SetDlgItemText(IDC_SCORE,str);
}

VOID CDlgInsureSave::OnBnClickedHyperLink2()
{
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	CString str;
	str.Format(TEXT("%I64d"), lScore+5000000);
	SetDlgItemText(IDC_SCORE,str);
}

VOID CDlgInsureSave::OnBnClickedHyperLink3()
{
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	CString str;
	str.Format(TEXT("%I64d"), lScore+10000000);
	SetDlgItemText(IDC_SCORE,str);
}

VOID CDlgInsureSave::OnBnClickedHyperLink4()
{
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	CString str;
	str.Format(TEXT("%I64d"), lScore+50000000);
	SetDlgItemText(IDC_SCORE,str);
}

VOID CDlgInsureSave::OnBnClickedHyperLink5()
{
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	CString str;
	str.Format(TEXT("%I64d"), lScore+100000000);
	SetDlgItemText(IDC_SCORE,str);
}

VOID CDlgInsureSave::OnBnClickedHyperLink6()
{
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	CString str;
	str.Format(TEXT("%I64d"), lScore+500000000);
	SetDlgItemText(IDC_SCORE,str);
}

VOID CDlgInsureSave::OnBnClickedHyperLink7()
{
	CString cstr;
	cstr.Format(SCORE_STRING, m_pUserInsureInfo->lUserScore);
	SetDlgItemText(IDC_SCORE, cstr);
}

VOID CDlgInsureSave::OnBnClickedHyperLink8()
{
	CString cstr;
	cstr.Format(SCORE_STRING, m_pUserInsureInfo->lUserInsure);
	SetDlgItemText(IDC_SCORE, cstr);
}

//绘画背景
BOOL CDlgInsureSave::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//创建 DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pDCBuffer=CDC::FromHandle(BufferDC);

	// 加载资源
	CPngImage ImageInsureSave;
	ImageInsureSave.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_SAVE"));

	//设置环境
//	pDCBuffer->SetBkMode(TRANSPARENT);
//	pDCBuffer->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//绘画背景
	pDCBuffer->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	// 载入图片，绘制背景
	ImageInsureSave.DrawImage(pDCBuffer, 0, 0, ImageInsureSave.GetWidth(), ImageInsureSave.GetHeight(), 0, 0);

	//绘画数字
	if (m_pUserInsureInfo)
	{
		m_DlgInsureItem.DrawNumberString(pDCBuffer,m_pUserInsureInfo->lUserInsure, 99, 18L);
		m_DlgInsureItem.DrawNumberString(pDCBuffer,m_pUserInsureInfo->lUserScore, 99, 46L);
	}

	//获取位置
//	CRect rcScore;
//	m_edScore.GetWindowRect(&rcScore);

	//转换位置
//	ScreenToClient(&rcScore);

	//绘画输入
//	COLORREF crColorBorad=CSkinEdit::m_SkinAttribute.m_crEnableBorad;
//	pDCBuffer->Draw3dRect(rcScore.left-6,rcScore.top-5,rcScore.Width()+12,24L,crColorBorad,crColorBorad);

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pDCBuffer,0,0,SRCCOPY);

	return TRUE;
}

//控件颜色
HBRUSH CDlgInsureSave::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_BTN:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlText);
			return CSkinDialog::m_SkinAttribute.m_brBackGround;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInsureTransfer::CDlgInsureTransfer() : CDialog(IDD_INSURE_TRANSFER)
{
	//设置变量
	m_bSwitchIng=false;

	return;
}

//析构函数
CDlgInsureTransfer::~CDlgInsureTransfer()
{
}

// 设置赠送玩家昵称
VOID CDlgInsureTransfer::SetUserNickName(LPCTSTR lpszNickName)
{
	SetDlgItemText(IDC_TRANSFER_NICK_NAME, lpszNickName);
}

//控件绑定
VOID CDlgInsureTransfer::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//控件变量
	DDX_Control(pDX, IDC_SCORE, m_edScore);
	DDX_Control(pDX, IDC_NICKNAME, m_edNickName);
//	DDX_Control(pDX, IDC_PASSWORD, m_PasswordControl);
//	DDX_Control(pDX, IDC_FORGET_INSURE, m_ForgetInsure);
	DDX_Control(pDX, IDC_BT_TRANSFER_SCORE, m_btTransferScore);

	DDX_Control(pDX, IDC_HYPERLINK_1, m_hlOneMillion);
	DDX_Control(pDX, IDC_HYPERLINK_2, m_hlFiveMillion);
	DDX_Control(pDX, IDC_HYPERLINK_3, m_hlTenMillion);
	DDX_Control(pDX, IDC_HYPERLINK_4, m_hlFiftyMillion);
	DDX_Control(pDX, IDC_HYPERLINK_5, m_hlOneHundredMillion);
	DDX_Control(pDX, IDC_HYPERLINK_6, m_hlFiveHundredMillion);
}

//创建函数
BOOL CDlgInsureTransfer::OnInitDialog()
{
	__super::OnInitDialog();

	//设置控件
	m_edScore.LimitText(16L);
	m_edNickName.LimitText(LEN_NICKNAME-1);

	// 移动控件位置
	m_edNickName.MoveWindow(107,49,180,18);
	m_edScore.MoveWindow(107,105,180,18);

	GetDlgItem(IDC_TRANSFER_NICK_NAME)->MoveWindow(107, 78, 180, 18);
	GetDlgItem(IDC_SCORE_STRING)->MoveWindow(107, 134, 180, 18);

	m_hlOneMillion.MoveWindow(300, 98, 40, 16);
	m_hlFiveMillion.MoveWindow(300, 114, 40, 16);
	m_hlTenMillion.MoveWindow(340, 98, 40, 16);
	m_hlFiftyMillion.MoveWindow(340, 114, 40, 16);
	m_hlOneHundredMillion.MoveWindow(380, 98, 40, 16);
	m_hlFiveHundredMillion.MoveWindow(380, 114, 40, 16);

	// 设置控件颜色
//	m_edNickName.SetEnableColor( RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255) );
//	m_edScore.SetEnableColor( RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255) );

	//设置按钮
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	m_btTransferScore.SetButtonImage(IDB_BT_TRANSFER_SCORE, hResInstance, false, false);
//	((CButton *)GetDlgItem(IDC_BY_NAME))->SetCheck(1);

	CRect rcBtTransferScore;
	m_btTransferScore.GetClientRect(rcBtTransferScore);
	m_btTransferScore.MoveWindow(149, 168, rcBtTransferScore.Width(), rcBtTransferScore.Height());

	// CSkinControl代码修改生效
	m_hlOneMillion.SetUnderLine(true);
	m_hlFiveMillion.SetUnderLine(true);
	m_hlTenMillion.SetUnderLine(true);
	m_hlFiftyMillion.SetUnderLine(true);
	m_hlOneHundredMillion.SetUnderLine(true);
	m_hlFiveHundredMillion.SetUnderLine(true);

	return FALSE;
}

//确定按钮
VOID CDlgInsureTransfer::OnOK()
{
	//转账操作
	OnBnClickedTransferScore();

	return;
}

//取消按钮
VOID CDlgInsureTransfer::OnCancel()
{
	return;
}

//输入信息
VOID CDlgInsureTransfer::OnEnChangeScore()
{
	//状态变量
	if (m_bSwitchIng==false)
	{
		//设置变量
		m_bSwitchIng=true;

		//变量定义
		CWnd * pWndScore=GetDlgItem(IDC_SCORE);
		SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

		//输入限制
		if(lScore > m_pUserInsureInfo->lUserInsure) lScore = m_pUserInsureInfo->lUserInsure;

		//设置界面
		if (lScore>0L)
		{
			//转换数值
			TCHAR szString[128]=TEXT("");
			m_DlgInsureItem.SwitchScoreString(lScore,szString,CountArray(szString));

			//转换逗号
			TCHAR szControl[128]=TEXT("");
			m_DlgInsureItem.SwitchScoreFormat(lScore,3L,szControl,CountArray(szControl));

			//设置控件
			SetDlgItemText(IDC_SCORE,szControl);
			SetDlgItemText(IDC_SCORE_STRING,szString);

			//设置光标
			INT nScoreLength=m_edScore.GetWindowTextLength();
			m_edScore.SetSel(nScoreLength,nScoreLength,FALSE);
		}
		else
		{
			//设置控件
			SetDlgItemText(IDC_SCORE,TEXT(""));
			SetDlgItemText(IDC_SCORE_STRING,TEXT(""));
		}

		//设置变量
		m_bSwitchIng=false;
	}

	return;
}


//忘记密码
VOID CDlgInsureTransfer::OnBnClickedForgetInsure()
{
	//构造地址
	TCHAR szLogonLink[256]=TEXT("");
	_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("%s/LogonLink4.aspx"),szPlatformLink);

	//打开页面
	ShellExecute(NULL,TEXT("OPEN"),szLogonLink,NULL,NULL,SW_NORMAL);

	return;
}

//转账按钮
VOID CDlgInsureTransfer::OnBnClickedTransferScore()
{
	//条件判断
 	if (m_pUserInsureInfo->lUserInsure < m_pUserInsureInfo->lTransferPrerequisite)
 	{
 		//提示消息
 		TCHAR szTipInfo[128]={0};
 		_sntprintf(szTipInfo,CountArray(szTipInfo),TEXT("您保险柜银子的数目余额不足最低转账条件 %I64d，无法进行转账操作！"),m_pUserInsureInfo->lTransferPrerequisite);
 		CInformation Information(this);
 		Information.ShowMessageBox(szTipInfo,MB_ICONERROR,30);
 
 		return;
 	}
 
 	//获取变量
 	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
 	SCORE lTransferScore=m_DlgInsureItem.GetUserControlScore(pWndScore);
 
 	//获取密码
 	TCHAR szPassword[LEN_PASSWORD];
 //	m_PasswordControl.GetUserPassword(szPassword);
 
 	//加密密码
 	TCHAR szInsurePass[LEN_MD5];
 //	CWHEncrypt::MD5Encrypt(szPassword,szInsurePass);
 
 	//获取昵称
 	CString strNickName;
 	GetDlgItemText(IDC_NICKNAME,strNickName);
 
 	//格式昵称
 	strNickName.TrimLeft();
 	strNickName.TrimRight();
 
 	//昵称判断
 	if (strNickName.GetLength()==0)
 	{
 		//提示消息
 		CInformation Information(this);
 		if(((CButton *)GetDlgItem(IDC_BY_NAME))->GetCheck()==BST_CHECKED)
 			Information.ShowMessageBox(TEXT("请输入要赠送的玩家昵称！"),MB_ICONERROR,30);
 		else
 			Information.ShowMessageBox(TEXT("请输入要赠送的玩家ID！"),MB_ICONERROR,30);
 
 		//设置焦点
 		m_edNickName.SetFocus();
 
 		return;
 	}
 
 	//ID号判断
 	if(((CButton *)GetDlgItem(IDC_BY_ID))->GetCheck()==BST_CHECKED)
 	{
 		int nlen = strNickName.GetLength();
 		for(int i=0; i<nlen; i++)
 		{
 			if(strNickName[i] < '0' || strNickName[i] > '9')
 			{
 				//提示消息
 				CInformation Information(this);
 				Information.ShowMessageBox(TEXT("请输入正确的玩家ID！"),MB_ICONERROR,30);
 
 				//设置焦点
 				m_edNickName.SetFocus();
 				return;
 			}
 		}
 	}
 
 	//数据判断
 	if (lTransferScore<=0L)
 	{
 		//提示消息
 		CInformation Information(this);
 		Information.ShowMessageBox(TEXT("赠送的银子数量不能为空，请重新输入游戏币数量！"),MB_ICONERROR,30);
 
 		//设置焦点
 		m_edScore.SetFocus();
 
 		return;
 	}
 
 	//数目判断
 	if (lTransferScore>m_pUserInsureInfo->lUserInsure)
 	{
 		//提示消息
 		CInformation Information(this);
 		Information.ShowMessageBox(TEXT("您保险柜银子的数目余额不足，请重新输入游戏币数量！"),MB_ICONERROR,30);
 
 		//设置焦点
 		m_edScore.SetFocus();
 
 		return;
 	}
 
 	//数目判断
 	if (lTransferScore<m_pUserInsureInfo->lTransferPrerequisite)
 	{
 		//提示消息
 		TCHAR szTipInfo[128]={0};
 		_sntprintf(szTipInfo,CountArray(szTipInfo),TEXT("从保险柜转账的银子数目不能少于 %I64d，无法进行转账操作！"),m_pUserInsureInfo->lTransferPrerequisite);
 		CInformation Information(this);
 		Information.ShowMessageBox(szTipInfo,MB_ICONERROR,30);
 
 		//设置焦点
 		m_edScore.SetFocus();
 
 		return;
 	}
 
 	//密码判断
//  	if (szPassword[0]==0)
//  	{
//  		//提示消息
//  		CInformation Information(this);
//  		Information.ShowMessageBox(TEXT("保险柜密码不能为空，请重新输入保险柜密码！"),MB_ICONERROR,30);
//  
//  		//设置焦点
//  //		m_PasswordControl.SetFocus();
//  
//  		return;
//  	}
 	
 	//事件通知
 	m_pDlgInsureMain->OnMissionStart();
 	m_pDlgInsureMain->PerformTransferScore(NULL, strNickName,lTransferScore,szInsurePass);
 
 	//设置焦点
 //	m_PasswordControl.SetFocus();
 //	m_PasswordControl.SetUserPassword(TEXT(""));
 
 	return;
}

// GameID输入窗口失去焦点
VOID CDlgInsureTransfer::OnKillFocusNickName()
{
 	// 发起查询
 	// 判断ID是否有值
 	CString strNickName;
 	GetDlgItemText(IDC_NICKNAME,strNickName);
 
 	//格式昵称
 	strNickName.TrimLeft();
 	strNickName.TrimRight();
 
 	//昵称判断
 	if (strNickName.GetLength()==0)
 	{
 		return;
 	}
 
 	// 不为GameID
 	int nlen = strNickName.GetLength();
 	for(int i=0; i<nlen; i++)
 	{
 		if(strNickName[i] < '0' || strNickName[i] > '9')
 		{
 			return;
 		}
 	}

	// 设置定时器，延迟1秒后查询
	SetDlgItemText(IDC_TRANSFER_NICK_NAME, _T("正在查询玩家昵称..."));
	SetTimer(EVENT_QUERYNICKNAME, 1000, NULL);

}

// 各种金额超链接
VOID CDlgInsureTransfer::OnBnClickedHyperLink1()
{
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	CString str;
	str.Format(TEXT("%I64d"), lScore+1000000);

	SetDlgItemText(IDC_SCORE,str);
}

VOID CDlgInsureTransfer::OnBnClickedHyperLink2()
{
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	CString str;
	str.Format(TEXT("%I64d"), lScore+5000000);

	SetDlgItemText(IDC_SCORE,str);
}

VOID CDlgInsureTransfer::OnBnClickedHyperLink3()
{
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	CString str;
	str.Format(TEXT("%I64d"), lScore+10000000);

	SetDlgItemText(IDC_SCORE,str);
}

VOID CDlgInsureTransfer::OnBnClickedHyperLink4()
{
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	CString str;
	str.Format(TEXT("%I64d"), lScore+50000000);

	SetDlgItemText(IDC_SCORE,str);
}

VOID CDlgInsureTransfer::OnBnClickedHyperLink5()
{
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	CString str;
	str.Format(TEXT("%I64d"), lScore+100000000);

	SetDlgItemText(IDC_SCORE,str);
}

VOID CDlgInsureTransfer::OnBnClickedHyperLink6()
{
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	CString str;
	str.Format(TEXT("%I64d"), lScore+500000000);

	SetDlgItemText(IDC_SCORE,str);
}

//绘画背景
BOOL CDlgInsureTransfer::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//创建 DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pDCBuffer=CDC::FromHandle(BufferDC);

	// 底图图片
	CPngImage ImageInsureTransfer;
	ImageInsureTransfer.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_TRANSFER"));
	
	//设置环境
	pDCBuffer->SetBkMode(TRANSPARENT);
	pDCBuffer->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//绘画背景
	pDCBuffer->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	// 载入图片，绘制背景
	ImageInsureTransfer.DrawImage(pDCBuffer, 0, 0, ImageInsureTransfer.GetWidth(), ImageInsureTransfer.GetHeight(), 0, 0);

	//获取位置
	CRect rcScore;
	CRect rcNickName;
	m_edScore.GetWindowRect(&rcScore);
	m_edNickName.GetWindowRect(&rcNickName);

	//转换位置
	ScreenToClient(&rcScore);
	ScreenToClient(&rcNickName);

	if (m_pUserInsureInfo)
	{
		m_DlgInsureItem.DrawNumberString(pDCBuffer,m_pUserInsureInfo->lUserInsure, 107, 20L);
	}

	//绘画输入
//	COLORREF crColorBorad=CSkinEdit::m_SkinAttribute.m_crEnableBorad;
//	pDCBuffer->Draw3dRect(rcScore.left-6,rcScore.top-5,rcScore.Width()+12,24L,crColorBorad,crColorBorad);
//	pDCBuffer->Draw3dRect(rcNickName.left-6,rcNickName.top-5,rcNickName.Width()+12,24L,crColorBorad,crColorBorad);

	//绘画界面
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pDCBuffer,0,0,SRCCOPY);

	return TRUE;
}

//控件颜色
HBRUSH CDlgInsureTransfer::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_BTN:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlText);
			return CSkinDialog::m_SkinAttribute.m_brBackGround;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//时间消息
VOID CDlgInsureTransfer::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case EVENT_QUERYNICKNAME:
		{
			CString strNickName;
			GetDlgItemText(IDC_NICKNAME,strNickName);

			DWORD dwGameID = NULL; 
			#ifdef _UNICODE
				dwGameID = _ttoi(strNickName);
			#else
				dwGameID = atoi(strNickName);
			#endif
			
			m_pDlgInsureMain->OnMissionStart();
			m_pDlgInsureMain->PerformQueryUserNickName(dwGameID);

			KillTimer(EVENT_QUERYNICKNAME);
		}
		break;
	}

	__super::OnTimer(nIDEvent);
}

void CDlgInsureTransfer::OnBnClickedById()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_TRANSFER_TYPE_TIP)->SetWindowText(TEXT("赠送玩家 I D："));
	m_edNickName.SetFocus();
	m_edNickName.SetWindowText(TEXT(""));
}

void CDlgInsureTransfer::OnBnClickedByName()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_TRANSFER_TYPE_TIP)->SetWindowText(TEXT("赠送玩家昵称："));
	m_edNickName.SetFocus();
	m_edNickName.SetWindowText(TEXT(""));
}
//////////////////////////////////////////////////////////////////////////////////
CDlgInsureModifyPassword::CDlgInsureModifyPassword()
{
	m_pDlgInsureMain = NULL;
}

CDlgInsureModifyPassword::~CDlgInsureModifyPassword()
{

}

//控件绑定
VOID CDlgInsureModifyPassword::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_INSURE_PASSWORD1, m_edInsurePassword1);
	DDX_Control(pDX, IDC_INSURE_PASSWORD2, m_edInsurePassword2);
	DDX_Control(pDX, IDC_INSURE_PASSWORD3, m_edInsurePassword3);

	DDX_Control(pDX, IDC_MODIFY_INSURE_PASSWORD, m_btModify);
}

//消息解释
BOOL CDlgInsureModifyPassword::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//创建函数
BOOL CDlgInsureModifyPassword::OnInitDialog()
{
	__super::OnInitDialog();

	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	// 设置控件
	m_edInsurePassword1.SetDrawBorad(false);
	m_edInsurePassword2.SetDrawBorad(false);
	m_edInsurePassword3.SetDrawBorad(false);

	m_btModify.SetButtonImage(IDB_BT_MODIFY, hResInstance, false, false);

	// 移动控件
	m_edInsurePassword1.MoveWindow(145, 18, 230, 18);
	m_edInsurePassword2.MoveWindow(145, 47, 230, 18);
	m_edInsurePassword3.MoveWindow(145, 76, 230, 18);

	CRect rcBtModify;
	m_btModify.GetClientRect(rcBtModify);
	m_btModify.MoveWindow(182, 152, rcBtModify.Width(), rcBtModify.Height());

	return FALSE;
}

// 确定按钮
VOID CDlgInsureModifyPassword::OnOK()
{
	return;
}

// 取消按钮
VOID CDlgInsureModifyPassword::OnCancel()
{
	return;
}

// 修改密码按钮
VOID CDlgInsureModifyPassword::OnBnClickedModifyPassword()
{
	// 密码本地检测
	TCHAR szSrcPassword[LEN_PASSWORD] = {0};
	TCHAR szDesPassword[2][LEN_PASSWORD];

	//获取信息
	m_edInsurePassword1.GetUserPassword(szSrcPassword);
	m_edInsurePassword2.GetUserPassword(szDesPassword[0]);
	m_edInsurePassword3.GetUserPassword(szDesPassword[1]);

	//密码判断
	if (szSrcPassword[0]==0)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("原银行密码不能为空，请重新输入！"),MB_ICONERROR,0);

		m_edInsurePassword1.SetFocus();

		return;
	}

	//密码判断
	if (lstrcmp(szDesPassword[0],szDesPassword[1])!=0)
	{
		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("两次输入的新银行密码不一致，请重新输入！"),MB_ICONERROR,0);

		//设置焦点
		m_edInsurePassword2.SetFocus();

		return;
	}

	//变量定义
	TCHAR szDescribe[128]=TEXT("");
	CUserItemElement* pUserItemElement=CUserItemElement::GetInstance();

	//密码判断
	if (pUserItemElement->EfficacyPassword(szDesPassword[0],szDescribe,CountArray(szDescribe))==false)
	{
		//提示消息
		CInformation Information(this);
		_sntprintf(szDescribe, CountArray(szDescribe),TEXT("密码必须大于 %ld 位以上，请重新输入！"),LEN_LESS_PASSWORD);
		Information.ShowMessageBox(szDescribe, MB_ICONERROR, 0);

		//设置焦点
		m_edInsurePassword2.SetFocus();

		return;
	}

	//一致判断
	if (lstrcmp(szSrcPassword,szDesPassword[0])==0)
	{
		//提示信息
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("您所输入的新密码与原密码一致，请重新输入新密码！"),MB_ICONERROR,0);

		//设置焦点
		m_edInsurePassword2.SetFocus();

		//设置密码
		m_edInsurePassword2.SetUserPassword(TEXT(""));
		m_edInsurePassword3.SetUserPassword(TEXT(""));

		return;
	}


	if (m_pDlgInsureMain)
	{
		m_pDlgInsureMain->OnMissionStart();

		m_pDlgInsureMain->PerformModiyInsurePassword(szSrcPassword, szDesPassword[0]);
	}
}

//绘画背景
BOOL CDlgInsureModifyPassword::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//建立缓冲
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//创建 DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pDCBuffer=CDC::FromHandle(BufferDC);

	// 加载资源
	CPngImage ImageInsureModifyPassword;
	ImageInsureModifyPassword.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_MODIFY_PASSWORD"));

	//绘画背景
	pDCBuffer->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	// 载入图片，绘制背景
	ImageInsureModifyPassword.DrawImage(pDCBuffer, 0, 0, ImageInsureModifyPassword.GetWidth(), ImageInsureModifyPassword.GetHeight(), 0, 0);

	//绘画界面
	pDC->BitBlt(0, 0, rcClient.Width(), rcClient.Height(), pDCBuffer, 0, 0, SRCCOPY);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////

TCHAR	CDlgInsureMain::m_szInsurePass[LEN_MD5] = {0};
bool	CDlgInsureMain::m_bAutoLogOff = true;

//构造函数
CDlgInsureMain::CDlgInsureMain() : CSkinDialog(IDD_INSURE_MAIN)
{
	//服务管理
	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=false;
    m_bTransRecordMission=false;

	//设置变量
	m_bInitInfo=false;
	m_cbInsureMode=INSURE_PASSWORD;
	ZeroMemory(&m_UserInsureInfo,sizeof(m_UserInsureInfo));

	m_hBrush = CreateSolidBrush(RGB(194,218,230));

	m_SkinAttribute.m_crBackGround = RGB(255, 255, 255);

	return;
}

//析构函数
CDlgInsureMain::~CDlgInsureMain()
{
	::DeleteObject(m_hBrush);
}

//控件绑定
VOID CDlgInsureMain::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//控件变量
	DDX_Control(pDX, IDC_TAB_CONTROL, m_TabControl);
//	DDX_Control(pDX, IDC_BT_QUERY_INFO, m_btQueryInfo);
}

//消息解释
BOOL CDlgInsureMain::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//创建函数
BOOL CDlgInsureMain::OnInitDialog()
{
	__super::OnInitDialog();

	CRect rcClient;
	GetClientRect(&rcClient);

	INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
	INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
	INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
	INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;

	//设置标题
	SetWindowText(TEXT("保险柜"));

	//设置资源
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//设置控件
	m_TabControl.SetItemSize(CSize(100,25));

	//存取控件
	m_DlgInsureSave.m_pDlgInsureMain=this;
	m_DlgInsureSave.m_pUserInsureInfo=&m_UserInsureInfo;

	//转账控件
	m_DlgInsureTransfer.m_pDlgInsureMain=this;
	m_DlgInsureTransfer.m_pUserInsureInfo=&m_UserInsureInfo;

	// 转账记录控件
	m_DlgTransRecord.m_pDlgInsureMain=this;
	m_DlgTransRecord.m_pUserInsureInfo=&m_UserInsureInfo;

	// 银行密码控件
	m_DlgInsurePassword.m_pDlgInsureMain = this;

	// 修改银行密码页签
	m_DlgInsureModifyPassword.m_pDlgInsureMain = this;

	//加入标签
	m_TabControl.InsertItem(INSURE_SAVE,TEXT(" 存款与取款 "));
	m_TabControl.InsertItem(INSURE_TRANSFER,TEXT(" 赠送银子 "));
	m_TabControl.InsertItem(TRANS_RECORD,TEXT(" 赠送日志 "));
	m_TabControl.InsertItem(INSURE_MODIFY_PASSWORD,TEXT(" 修改口令 "));

	m_TabControl.MoveWindow( nLBorder-4, 36, rcClient.Width() - nLBorder - nRBorder + 8, rcClient.Height()- nTBorder - nTBorder + 36);

	if (m_szInsurePass[0]==0)
	{
		// 没有设置过银行密码，需要输入银行密码
		m_TabControl.ShowWindow(SW_HIDE);

		// 输入密码模式
		SwitchInsureModeView(INSURE_PASSWORD);
	}
	else
	{
		m_TabControl.ShowWindow(SW_SHOW);
		//查询信息
		QueryUserInsureInfo();

		//存取模式
		SwitchInsureModeView(INSURE_SAVE);

		GetDlgItem(IDC_CHECK_AUTO_LOGOFF)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_AUTO_LOGOFF)->SetWindowPos(NULL, 340, 42, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	}
	((CButton*)GetDlgItem(IDC_CHECK_AUTO_LOGOFF))->SetCheck(m_bAutoLogOff);

	return FALSE;
}

//确定按钮
VOID CDlgInsureMain::OnOK()
{
	//默认动作
	switch (m_cbInsureMode)
	{
	case INSURE_SAVE:		//存取方式
		{
			m_DlgInsureSave.OnOK();
			break;
		}
	case INSURE_TRANSFER:	//转账方式
		{
			m_DlgInsureTransfer.OnOK();
			break;
		}
    case TRANS_RECORD:
        {
            break;
        }
	}

	return;
}

// 销毁窗口
// VOID CDlgInsureMain::OnDestory()
// {
// 	MessageBox(_T("CDlgInsureMain::OnDestory"));
// 	m_bAutoLogOff = ( (CButton*)GetDlgItem(IDC_CHECK_AUTO_LOGOFF) )->GetCheck();
// 	if (m_bAutoLogOff == true)
// 	{
// 		ZeroMemory(m_szInsurePass, sizeof(m_szInsurePass));
// 	}
// }

void CDlgInsureMain::OnClose()
{
 	m_bAutoLogOff = ( (CButton*)GetDlgItem(IDC_CHECK_AUTO_LOGOFF) )->GetCheck();
 	if (m_bAutoLogOff == true)
 	{
 		ZeroMemory(m_szInsurePass, sizeof(m_szInsurePass));
 	}

	__super::OnClose();
}

//绘画消息
VOID CDlgInsureMain::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight)
{
	// 获取当前显示的标签，进行刷新
 	if (m_cbInsureMode == INSURE_SAVE)
 	{
		if (m_DlgInsureSave.GetSafeHwnd() != NULL)
 			m_DlgInsureSave.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
 	}
	else if (m_cbInsureMode == INSURE_TRANSFER)
	{
		if (m_DlgInsureTransfer.GetSafeHwnd() != NULL)
			m_DlgInsureTransfer.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}
	else if (m_cbInsureMode == INSURE_MODIFY_PASSWORD)
	{
		if (m_DlgInsureModifyPassword.GetSafeHwnd() != NULL)
			m_DlgInsureModifyPassword.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}
	else if (m_cbInsureMode == TRANS_RECORD)
	{
		if (m_DlgTransRecord.GetSafeHwnd() != NULL)
			m_DlgTransRecord.RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ERASENOW);
	}
	return;
#if 0
	//获取位置
	CRect rcTabControl;
	m_TabControl.GetWindowRect(&rcTabControl);

	//转换位置
	ScreenToClient(&rcTabControl);

	//绘画线条
	INT nLBorder=m_SkinAttribute.m_EncircleInfoView.nLBorder;
	INT nRBorder=m_SkinAttribute.m_EncircleInfoView.nRBorder;
	pDC->FillSolidRect(nLBorder,rcTabControl.bottom-1L,nWidth-nLBorder-nRBorder,1,RGB(145,168,183));

	//加载资源
//	CPngImage ImageInsureInfo;
//	ImageInsureInfo.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INFO_FRAME"));

	//绘画信息
//	INT nYPosInfo=75L;
//	INT nXPosInfo=(nWidth-ImageInsureInfo.GetWidth())/2L;
//	ImageInsureInfo.DrawImage(pDC,nXPosInfo,nYPosInfo,ImageInsureInfo.GetWidth(),ImageInsureInfo.GetHeight(),0,0);

	//银行信息
	if (m_bInitInfo==true)
	{
		//加载资源
		CPngImage ImageInsureIcon;
		ImageInsureIcon.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_ICON"));

		//绘画标志
		ImageInsureIcon.DrawImage(pDC,nXPosInfo+10,nYPosInfo+17);

		//绘画数字
		m_DlgInsureItem.DrawNumberString(pDC,m_UserInsureInfo.lUserScore,nXPosInfo+136,nYPosInfo+18L);
		m_DlgInsureItem.DrawNumberString(pDC,m_UserInsureInfo.lUserInsure,nXPosInfo+136,nYPosInfo+53L);

		//绘画提示
		LPCTSTR pszScoreString=TEXT("银子：");
		LPCTSTR pszInsureString=TEXT("保险柜：");
		pDC->TextOut(nXPosInfo+83,nYPosInfo+20L,pszScoreString,lstrlen(pszScoreString));
		pDC->TextOut(nXPosInfo+83,nYPosInfo+55L,pszInsureString,lstrlen(pszInsureString));

    		//构造提示
    /*		TCHAR szString[128]=TEXT("");
    		_sntprintf(szString,CountArray(szString),TEXT("温馨提示：存入游戏币免手续费，取出将扣除%ld‰,赠送将扣除%ld‰的手续费"),m_UserInsureInfo.wRevenueTake,m_UserInsureInfo.wRevenueTransfer);

    		//温馨提示
    		pDC->SetTextColor(RGB(50,50,50));
    		pDC->TextOut(18,nHeight-28,szString,lstrlen(szString));*/
	}
	else
	{
	}
#endif
	return;
}

//开始任务
VOID CDlgInsureMain::OnMissionStart()
{
	return;
}

//终止任务
VOID CDlgInsureMain::OnMissionConclude()
{
	return;
}

//查询资料
VOID CDlgInsureMain::QueryUserInsureInfo()
{
	//设置变量
	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=true;
	m_bTransferMission=false;
	m_bTransRecordMission = false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = false; 

	//事件处理
	OnMissionStart();

	//执行查询
	PerformQueryInfo();

	return;
}

//设置模式
VOID CDlgInsureMain::SwitchInsureModeView(BYTE cbInsureMode)
{
	switch (cbInsureMode)
	{
	case INSURE_SAVE:				//存取方式
		{
			//设置变量
			m_cbInsureMode=INSURE_SAVE;

			//设置控件
			if (m_DlgInsureSave.m_hWnd==NULL)
			{
				//获取位置
				CRect rcClient;
				GetClientRect(&rcClient);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//创建控件
				m_DlgInsureSave.Create(IDD_INSURE_SAVE, &m_TabControl);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(NULL));

				//移动位置
				INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
				INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
				INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
				INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
				m_DlgInsureSave.SetWindowPos(NULL,nLBorder,nTBorder, 439, 229, SWP_NOZORDER|SWP_SHOWWINDOW);
			}
			else
			{
				//显示窗口
				m_DlgInsureSave.ShowWindow(SW_SHOW);
			}

			//设置控件
			if (m_DlgInsureTransfer.m_hWnd!=NULL) m_DlgInsureTransfer.ShowWindow(SW_HIDE);
			if (m_DlgTransRecord.m_hWnd!=NULL) m_DlgTransRecord.ShowWindow(SW_HIDE);
			if (m_DlgInsurePassword.GetSafeHwnd()!=NULL) m_DlgInsurePassword.ShowWindow(SW_HIDE);
			if (m_DlgInsureModifyPassword.GetSafeHwnd() != NULL) m_DlgInsureModifyPassword.ShowWindow(SW_HIDE);
			//设置焦点
			m_DlgInsureSave.SetFocus();

			//设置选择
			m_TabControl.SetCurSel(m_cbInsureMode);

			break;
		}
	case INSURE_TRANSFER:			//转账方式
		{
			//设置变量
			m_cbInsureMode=INSURE_TRANSFER;

			//设置控件
			if (m_DlgInsureTransfer.m_hWnd==NULL)
			{
				//获取位置
				CRect rcClient;
				GetClientRect(&rcClient);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//创建控件
				m_DlgInsureTransfer.Create(IDD_INSURE_TRANSFER, &m_TabControl);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(NULL));

				//移动位置
				INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
				INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
				INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
				INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
				m_DlgInsureTransfer.SetWindowPos(NULL,nLBorder,nTBorder, 439, 229, SWP_NOZORDER|SWP_SHOWWINDOW);
			}
			else
			{
				//显示窗口
				m_DlgInsureTransfer.ShowWindow(SW_SHOW);
			}

			//设置控件
			if (m_DlgInsureSave.m_hWnd!=NULL) m_DlgInsureSave.ShowWindow(SW_HIDE);
			if (m_DlgTransRecord.m_hWnd!=NULL) m_DlgTransRecord.ShowWindow(SW_HIDE);
			if (m_DlgInsurePassword.GetSafeHwnd()!=NULL) m_DlgInsurePassword.ShowWindow(SW_HIDE);
			if (m_DlgInsureModifyPassword.GetSafeHwnd() != NULL) m_DlgInsureModifyPassword.ShowWindow(SW_HIDE);
			//设置焦点
			m_DlgInsureTransfer.SetFocus();

			//设置选择
			m_TabControl.SetCurSel(m_cbInsureMode);

			break;
		}
	case TRANS_RECORD:				// 转账历史记录
		{
			m_cbInsureMode=TRANS_RECORD;
			//设置控件
			if (m_DlgTransRecord.m_hWnd==NULL)
			{
				//获取位置
				CRect rcClient;
				GetClientRect(&rcClient);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//创建控件
				m_DlgTransRecord.Create(IDD_TRNASRECORD_DLG,&m_TabControl);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(NULL));

				//移动位置
				INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
				INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
				INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
				INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
				m_DlgTransRecord.SetWindowPos(NULL, 1, nTBorder, 476, 300, SWP_NOZORDER|SWP_SHOWWINDOW);
			}
			else
			{
				//显示窗口
				m_DlgTransRecord.ShowWindow(SW_SHOW);
			}

			//设置控件
			if (m_DlgInsureSave.m_hWnd!=NULL) m_DlgInsureSave.ShowWindow(SW_HIDE);
			if (m_DlgInsureTransfer.m_hWnd!=NULL) m_DlgInsureTransfer.ShowWindow(SW_HIDE);
			if (m_DlgInsurePassword.GetSafeHwnd()!=NULL) m_DlgInsurePassword.ShowWindow(SW_HIDE);
			if (m_DlgInsureModifyPassword.GetSafeHwnd() != NULL) m_DlgInsureModifyPassword.ShowWindow(SW_HIDE);
           
			//设置焦点
			m_DlgTransRecord.SetFocus();

			//设置选择
			m_TabControl.SetCurSel(m_cbInsureMode);
			break;
		}	
	case INSURE_PASSWORD:			// 验证银行密码
		{
			//设置控件
			if (m_DlgInsurePassword.GetSafeHwnd()==NULL)
			{
				//获取位置
				CRect rcClient;
				GetClientRect(&rcClient);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//创建控件
				m_DlgInsurePassword.Create(IDD_INSURE_PASSWORD, this);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(NULL));

				//移动位置
				INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
				INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
				INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
				INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
				m_DlgInsurePassword.SetWindowPos(NULL, nLBorder, nTBorder, 439, 260, SWP_NOZORDER|SWP_SHOWWINDOW);
			}
			else
			{
				//显示窗口
				m_DlgInsurePassword.ShowWindow(SW_SHOW);
			}

			//设置控件
			if (m_DlgInsureSave.m_hWnd!=NULL) m_DlgInsureSave.ShowWindow(SW_HIDE);
			if (m_DlgInsureTransfer.m_hWnd!=NULL) m_DlgInsureTransfer.ShowWindow(SW_HIDE);
			if (m_DlgTransRecord.GetSafeHwnd()!=NULL) m_DlgTransRecord.ShowWindow(SW_HIDE);
			if (m_DlgInsureModifyPassword.GetSafeHwnd() != NULL) m_DlgInsureModifyPassword.ShowWindow(SW_HIDE);

			m_DlgInsurePassword.SetFocus();
			if (m_DlgInsurePassword.m_PasswordControl.m_hWnd != NULL)
			{
				m_DlgInsurePassword.m_PasswordControl.SetFocus();
				m_DlgInsurePassword.m_PasswordControl.SetUserPassword(TEXT(""));
			}
		}
		break;
	case INSURE_MODIFY_PASSWORD:	//	修改银行密码
		{
			//设置变量
			m_cbInsureMode=INSURE_MODIFY_PASSWORD;

			//设置控件
			if (m_DlgInsureModifyPassword.m_hWnd==NULL)
			{
				//获取位置
				CRect rcClient;
				GetClientRect(&rcClient);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//创建控件
				m_DlgInsureModifyPassword.Create(IDD_INSURE_MODIFY_PASSWORD, &m_TabControl);

				//设置资源
				AfxSetResourceHandle(GetModuleHandle(NULL));

				//移动位置
				INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
				INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
				INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
				INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
				m_DlgInsureModifyPassword.SetWindowPos(NULL,nLBorder,nTBorder, 439, 229, SWP_NOZORDER|SWP_SHOWWINDOW);
			}
			else
			{
				//显示窗口
				m_DlgInsureModifyPassword.ShowWindow(SW_SHOW);
			}

			if (m_DlgInsureSave.GetSafeHwnd() != NULL)	m_DlgInsureSave.ShowWindow(SW_HIDE);
			if (m_DlgInsureTransfer.GetSafeHwnd() != NULL) m_DlgInsureTransfer.ShowWindow(SW_HIDE);
			if (m_DlgTransRecord.GetSafeHwnd() != NULL) m_DlgTransRecord.ShowWindow(SW_HIDE);
			if (m_DlgInsurePassword.GetSafeHwnd() != NULL) m_DlgInsurePassword.ShowWindow(SW_HIDE);

			m_DlgInsureModifyPassword.SetFocus();
			m_TabControl.SetCurSel(m_cbInsureMode);
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
	}

	return;
}

//查询按钮
VOID CDlgInsureMain::OnBnClickedQueryInfo()
{
	//查询信息
	QueryUserInsureInfo();

	//设置焦点
	switch (m_cbInsureMode)
	{
	case INSURE_SAVE:		//存取方式
		{
			m_DlgInsureSave.SetFocus();
			break;
		}
	case INSURE_TRANSFER:	//转账方式
		{
			m_DlgInsureTransfer.SetFocus();
			break;
		}
    case TRANS_RECORD:
        {
            m_DlgTransRecord.SetFocus();
            break;
        }
	}

	return;
}

//类型改变
VOID CDlgInsureMain::OnTcnSelchangeInsureMode(NMHDR * pNMHDR, LRESULT * pResult)
{
	switch (m_TabControl.GetCurSel())
	{
	case INSURE_SAVE:				//	存取方式
		{
			SwitchInsureModeView(INSURE_SAVE);
			break;
		}
	case INSURE_TRANSFER:			//	转账方式
		{
			SwitchInsureModeView(INSURE_TRANSFER);
			break;
		}
	case TRANS_RECORD:				//	转账历史记录
		{
			SwitchInsureModeView(TRANS_RECORD);
			break;
		}
	case INSURE_MODIFY_PASSWORD:	//	改变密码
		{
			SwitchInsureModeView(INSURE_MODIFY_PASSWORD);
			break;
		}
	default:
		{
			ASSERT(FALSE);
		}
	}

	return;
}

// 改变控件颜色
HBRUSH CDlgInsureMain::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_BTN:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			return m_hBrush;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}


//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInsurePlaza::CDlgInsurePlaza()
{
	//设置任务
	m_MissionManager.InsertMissionItem(this);

	//设置变量
	m_lScore=0L;
	ZeroMemory(m_szNickName,sizeof(m_szNickName));

	m_dwGameID = NULL;

	return;
}

//析构函数
CDlgInsurePlaza::~CDlgInsurePlaza()
{
}

//查询事件
VOID CDlgInsurePlaza::PerformQueryInfo()
{
	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}

//存入事件
VOID CDlgInsurePlaza::PerformSaveScore(SCORE lSaveScore)
{
	//设置变量
	m_bSaveMission=true;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
    m_bTransRecordMission=false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = false;

	//设置变量
	m_lScore=lSaveScore;

	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}

//取出事件
VOID CDlgInsurePlaza::PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass)
{
	//设置变量
	m_bTakeMission=true;
	m_bSaveMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
    m_bTransRecordMission=false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = false;

	//设置变量
	m_lScore=lTakeScore;
//	lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));

	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}

//转账事件
VOID CDlgInsurePlaza::PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass)
{
	//设置变量
	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bTransferMission=true;
    m_bTransRecordMission=false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = false;

	//设置变量
	m_cbByNickName=cbByNickName;
	m_lScore=lTransferScore;
	lstrcpyn(m_szNickName,pszNickName,CountArray(m_szNickName));
//	lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));

	//发起连接
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}
//转账记录
VOID CDlgInsurePlaza::PerformQueryTransRecord()
{
    m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
    m_bTransRecordMission=true;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = false;

    if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}
}

// 根据玩家GameID查询昵称
VOID CDlgInsurePlaza::PerformQueryUserNickName(DWORD dwGameID)
{
	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
	m_bTransRecordMission=false;
	m_bQueryNickNameMission=true;
	m_bVerifyInsurePassword = false;

	m_dwGameID = dwGameID;

	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}
}

// 处理银行密码
VOID CDlgInsurePlaza::PerformInsurePassword(LPCTSTR pszInsurePass)
{
	//设置变量
	m_bTakeMission=false;
	m_bSaveMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
	m_bTransRecordMission=false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = true;

	//设置变量
	lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}
}

// 修改银行密码
VOID CDlgInsurePlaza::PerformModiyInsurePassword(LPCTSTR pszSrcPassword, LPCTSTR pszDesPassword)
{
	m_bSaveMission = false;						//存款任务
	m_bTakeMission = false;						//取款任务
	m_bQueryMission = false;					//查询任务
	m_bTransferMission = false;					//转账任务
	m_bTransRecordMission = false;              //转账记录
	m_bQueryNickNameMission = false;			// 查询昵称
	m_bVerifyInsurePassword = false;			// 输入银行密码，进行检测
	m_bModifyInsurePassword = true;				// 修改银行密码

	lstrcpyn(m_szSrcInsurePass, pszSrcPassword, CountArray(m_szSrcInsurePass));
	lstrcpyn(m_szDesInsurePass, pszDesPassword, CountArray(m_szDesInsurePass));

	//设置变量
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}
}


//连接事件
bool CDlgInsurePlaza::OnEventMissionLink(INT nErrorCode)
{
	//连接结果
	if (nErrorCode!=0)
	{
		//重试任务
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//事件处理
		OnMissionConclude();

		//错误提示
		if (m_bSaveMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，银子存入失败，请稍后再次重试！"),MB_OK|MB_ICONSTOP,30);
		}

		//错误提示
		if (m_bTakeMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，银子取出失败，请稍后再次重试！"),MB_OK|MB_ICONSTOP,30);
		}

		//错误提示
		if (m_bQueryMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，保险柜资料获取失败！"),MB_OK|MB_ICONSTOP,30);
		}

		//错误提示
		if (m_bTransferMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，银子赠送失败，请稍后再次重试！"),MB_OK|MB_ICONSTOP,30);
		}

		// 银行密码
		if (m_bVerifyInsurePassword == true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，验证银行密码失败，请稍后再次重试！"),MB_OK|MB_ICONSTOP,30);
		}

		// 修改银行密码
		if (m_bModifyInsurePassword == true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("连接服务器超时，修改银行密码失败，请稍后再次重试！"),MB_OK|MB_ICONSTOP,30);
		}
		
		return true;
	}

	//存入任务
	if (m_bSaveMission==true)
	{
		//变量定义
		CMD_GP_UserSaveScore UserSaveScore;
		ZeroMemory(&UserSaveScore,sizeof(UserSaveScore));

		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//设置变量
		UserSaveScore.lSaveScore=m_lScore;
		UserSaveScore.dwUserID=pGlobalUserData->dwUserID;
		CWHService::GetMachineIDEx(UserSaveScore.szMachineID);

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_USER_SAVE_SCORE,&UserSaveScore,sizeof(UserSaveScore));

		return true;
	}

	//取出任务
	if (m_bTakeMission==true)
	{
		//变量定义
		CMD_GP_UserTakeScore UserTakeScore;
		ZeroMemory(&UserTakeScore,sizeof(UserTakeScore));

		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//设置变量
		UserTakeScore.lTakeScore=m_lScore;
		UserTakeScore.dwUserID=pGlobalUserData->dwUserID;
		CWHService::GetMachineIDEx(UserTakeScore.szMachineID);
		lstrcpyn(UserTakeScore.szPassword,m_szInsurePass,CountArray(UserTakeScore.szPassword));

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_USER_TAKE_SCORE,&UserTakeScore,sizeof(UserTakeScore));

		return true;
	}

	//转账任务
	if (m_bTransferMission==true)
	{
		//变量定义
		CMD_GP_QueryUserInfoRequest QueryUserInfoRequest;
		ZeroMemory(&QueryUserInfoRequest,sizeof(QueryUserInfoRequest));

		//设置变量
		QueryUserInfoRequest.cbByNickName=m_cbByNickName;
		lstrcpyn(QueryUserInfoRequest.szNickName,m_szNickName,CountArray(QueryUserInfoRequest.szNickName));

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_USER_INFO_REQUEST,&QueryUserInfoRequest,sizeof(QueryUserInfoRequest));
	}

	//查询任务
	if (m_bQueryMission==true)
	{
		//变量定义
		CMD_GP_QueryInsureInfo QueryInsureInfo;
		ZeroMemory(&QueryInsureInfo,sizeof(QueryInsureInfo));

		//设置变量
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		QueryInsureInfo.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_INSURE_INFO,&QueryInsureInfo,sizeof(QueryInsureInfo));
	}

    //转账记录
    if(m_bTransRecordMission==true)
    {
        CMD_GP_QueryInsureInfo QueryTransRecord;
        ZeroMemory(&QueryTransRecord,sizeof(QueryTransRecord));
        
        CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
        QueryTransRecord.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
        //发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_TRANSRECORD,&QueryTransRecord,sizeof(QueryTransRecord));
        
    }

	// 查询转账用户昵称
	if (m_bQueryNickNameMission == true)
	{
		CMD_GP_QueryNickNameByGameID QueryNickNameByGameID;
		ZeroMemory(&QueryNickNameByGameID,sizeof(QueryNickNameByGameID));

		QueryNickNameByGameID.dwGameID=m_dwGameID;
		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_NICKNAME_BY_GAMEID,&QueryNickNameByGameID,sizeof(QueryNickNameByGameID));
	}

	// 验证银行密码
	if (m_bVerifyInsurePassword == true)
	{
		// 发送银行密码消息
 		CMD_GP_VerifyUserInsurePassword VerifyUserInsurePassword;
 		ZeroMemory(&VerifyUserInsurePassword,sizeof(VerifyUserInsurePassword));
 
 		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
 		VerifyUserInsurePassword.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
 		lstrcpyn(VerifyUserInsurePassword.szInsurePassword,m_szInsurePass,CountArray(VerifyUserInsurePassword.szInsurePassword));
 
 		//发送数据
 		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_VERIFY_INSURE_PASSWORD,&VerifyUserInsurePassword,sizeof(VerifyUserInsurePassword));
	}

	// 修改银行密码
	if (m_bModifyInsurePassword == true)
	{
		//变量定义
		CMD_GP_ModifyInsurePass ModifyInsurePass;
		ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

		//加密密码
		TCHAR szSrcPassword[LEN_PASSWORD]=TEXT("");
		TCHAR szDesPassword[LEN_PASSWORD]=TEXT("");
		CWHEncrypt::MD5Encrypt(m_szSrcInsurePass, szSrcPassword);
		CWHEncrypt::MD5Encrypt(m_szDesInsurePass, szDesPassword);

		//构造数据
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		ModifyInsurePass.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
		lstrcpyn(ModifyInsurePass.szScrPassword,szSrcPassword,CountArray(ModifyInsurePass.szScrPassword));
		lstrcpyn(ModifyInsurePass.szDesPassword,szDesPassword,CountArray(ModifyInsurePass.szDesPassword));

		//发送数据
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_MODIFY_INSURE_PASS,&ModifyInsurePass,sizeof(ModifyInsurePass));
	}
	return true;
}

//关闭事件
bool CDlgInsurePlaza::OnEventMissionShut(BYTE cbShutReason)
{
	//提示消息
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//重试任务
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//存入任务
		if (m_bSaveMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("与服务器的连接异常断开了，银子存入失败！"),MB_ICONERROR,30);
		}

		//取出任务
		if (m_bTakeMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("与服务器的连接异常断开了，银子取出失败！"),MB_ICONERROR,30);
		}

		//查询任务
		if (m_bQueryMission==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("与服务器的连接异常断开了，保险柜资料获取失败！"),MB_ICONERROR,30);
		}

		// 验证银行密码
		if (m_bVerifyInsurePassword==true)
		{
			//提示消息
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("与服务器的连接异常断开了，验证银行密码失败！"),MB_ICONERROR,30);
		}
	}

	//事件处理
	OnMissionConclude();

	return true;
}

//读取事件
bool CDlgInsurePlaza::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//命令处理
	if (Command.wMainCmdID==MDM_GP_USER_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
        case SUB_GP_TRANS_RECORD:
            {
                ASSERT(sizeof(CMD_GP_RealTransRecord) == wDataSize);
                if(sizeof(CMD_GP_RealTransRecord) != wDataSize) return false;
                CMD_GP_RealTransRecord*  pRealTransRecord = (CMD_GP_RealTransRecord*)pData;
			    m_DlgTransRecord.SetRecordList(pRealTransRecord);
                
                return true;
            }
		case SUB_GP_USER_INSURE_INFO:		//银行资料
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GP_UserInsureInfo));
				if (wDataSize<sizeof(CMD_GP_UserInsureInfo)) return false;

				//变量定义
				CMD_GP_UserInsureInfo * pUserInsureInfo=(CMD_GP_UserInsureInfo *)pData;

				//设置变量
				m_bInitInfo=true;
				m_UserInsureInfo.wRevenueTake=pUserInsureInfo->wRevenueTake;
				m_UserInsureInfo.wRevenueTransfer=pUserInsureInfo->wRevenueTransfer;
				m_UserInsureInfo.wServerID=pUserInsureInfo->wServerID;
				m_UserInsureInfo.lUserScore=pUserInsureInfo->lUserScore;
				m_UserInsureInfo.lUserInsure=pUserInsureInfo->lUserInsure;
				m_UserInsureInfo.lTransferPrerequisite=pUserInsureInfo->lTransferPrerequisite;

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				//更新大厅
				tagInsurePlazaUpdata Updata;
				Updata.lUserScore=pUserInsureInfo->lUserScore;
				Updata.lUserInsure=pUserInsureInfo->lUserInsure;
				AfxGetMainWnd()->SendMessage(WM_INSUREPLAZA_UPDATA,(WPARAM)&Updata,0);

				//更新界面
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				return true;
			}
		case SUB_GP_USER_INSURE_SUCCESS:	//银行各种操作成功(存入，取出)
			{
				//效验参数
				CMD_GP_UserInsureSuccess * pUserInsureSuccess=(CMD_GP_UserInsureSuccess *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString))) return false;

				//设置变量
				m_UserInsureInfo.lUserScore=pUserInsureSuccess->lUserScore;
				m_UserInsureInfo.lUserInsure=pUserInsureSuccess->lUserInsure;

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				//输入控件
//				if(m_DlgInsureSave.m_PasswordControl.m_hWnd != NULL)
//				{
//					m_DlgInsureSave.m_PasswordControl.SetFocus();
//					m_DlgInsureSave.m_PasswordControl.SetUserPassword(TEXT(""));
//				}
// 				if(m_DlgInsureTransfer.m_PasswordControl.m_hWnd != NULL)
// 				{
// 					m_DlgInsureTransfer.m_PasswordControl.SetFocus();
// 					m_DlgInsureTransfer.m_PasswordControl.SetUserPassword(TEXT(""));
// 				}

				//更新大厅
				tagInsurePlazaUpdata Updata;
				Updata.lUserScore=pUserInsureSuccess->lUserScore;
				Updata.lUserInsure=pUserInsureSuccess->lUserInsure;
				AfxGetMainWnd()->SendMessage(WM_INSUREPLAZA_UPDATA,(WPARAM)&Updata,0);

				//更新界面
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				//显示消息
				if (pUserInsureSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}

				if (m_bVerifyInsurePassword)
				{
					// 验证银行密码阶段
					m_TabControl.ShowWindow(SW_SHOW);

					SwitchInsureModeView(INSURE_SAVE);

					GetDlgItem(IDC_CHECK_AUTO_LOGOFF)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_CHECK_AUTO_LOGOFF)->SetWindowPos(NULL, 340, 42, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
				}

				return true;
			}
		case SUB_GP_USER_INSURE_FAILURE:	//银行失败
			{
				//效验参数
				CMD_GP_UserInsureFailure * pUserInsureFailure=(CMD_GP_UserInsureFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString))) return false;

				//失败处理
				OnMissionConclude();

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				//输入控件
//				if(m_DlgInsureSave.m_PasswordControl.m_hWnd != NULL)
//				{
//					m_DlgInsureSave.m_PasswordControl.SetFocus();
//					m_DlgInsureSave.m_PasswordControl.SetUserPassword(TEXT(""));
//				}
// 				if(m_DlgInsureTransfer.m_PasswordControl.m_hWnd != NULL)
// 				{
// 					m_DlgInsureTransfer.m_PasswordControl.SetFocus();
// 					m_DlgInsureTransfer.m_PasswordControl.SetUserPassword(TEXT(""));
// 				}

				if (m_DlgInsurePassword.m_PasswordControl.m_hWnd != NULL)
				{
					m_DlgInsurePassword.m_PasswordControl.SetFocus();
					m_DlgInsurePassword.m_PasswordControl.SetUserPassword(TEXT(""));
				}

				//显示消息
				if (pUserInsureFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureFailure->szDescribeString,MB_ICONERROR,60);
				}

				return true;
			}
		case SUB_GP_QUERY_USER_INFO_RESULT:	//用户信息
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GP_UserTransferUserInfo));
				if (wDataSize<sizeof(CMD_GP_UserTransferUserInfo)) return false;

				//变量定义
				CMD_GP_UserTransferUserInfo * pTransferUserInfo=(CMD_GP_UserTransferUserInfo *)pData;

				if (m_bTransferMission == true)
				{
					//构造消息
					TCHAR szMessage[128]={0};
					_sntprintf(szMessage, CountArray(szMessage), TEXT("您确定要给[%s], ID:%lu 赠送%I64d 银子吗?"), pTransferUserInfo->szNickName, pTransferUserInfo->dwTargetGameID, m_lScore);

					//提示消息
					CInformation Information(this);
					if (Information.ShowMessageBox(szMessage,MB_ICONQUESTION|MB_YESNO,0)==IDYES)
					{
						//变量定义
						CMD_GP_UserTransferScore UserTransferScore;
						ZeroMemory(&UserTransferScore,sizeof(UserTransferScore));

						//变量定义
						CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
						tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

						//设置变量
						UserTransferScore.cbByNickName=m_cbByNickName;
						UserTransferScore.lTransferScore=m_lScore;
						UserTransferScore.dwUserID=pGlobalUserData->dwUserID;
						CWHService::GetMachineIDEx(UserTransferScore.szMachineID);
						lstrcpyn(UserTransferScore.szNickName,m_szNickName,CountArray(UserTransferScore.szNickName));
						lstrcpyn(UserTransferScore.szPassword,m_szInsurePass,CountArray(UserTransferScore.szPassword));

//						MessageBox(m_szInsurePass);
					
						//发送数据
						m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_USER_TRANSFER_SCORE,&UserTransferScore,sizeof(UserTransferScore));
					}
				}

				// 查询昵称任务
				if (m_bQueryNickNameMission)
				{
					if (m_DlgInsureTransfer.GetSafeHwnd() != NULL)
					{
						m_DlgInsureTransfer.SetUserNickName(pTransferUserInfo->szNickName);
					}
				}

				return true;
			}
		case SUB_GP_OPERATE_FAILURE:		// 修改密码失败
			{
				//效验参数
				CMD_GP_OperateFailure * pOperateFailure=(CMD_GP_OperateFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString))) return false;

				//失败处理
				OnMissionConclude();

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				//显示消息
				if (pOperateFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pOperateFailure->szDescribeString,MB_ICONERROR,60);
				}

				//设置焦点
				if (m_bModifyInsurePassword == true && m_DlgInsureModifyPassword.GetSafeHwnd() != NULL)
				{
					m_DlgInsureModifyPassword.SetFocus();
				}
				return true;
			}
			break;
		case SUB_GP_OPERATE_SUCCESS:		// 修改密码成功(关闭对话框)
			{
				//变量定义
				CMD_GP_OperateSuccess * pOperateSuccess=(CMD_GP_OperateSuccess *)pData;

				//效验数据
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString))) return false;

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				if (pOperateSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pOperateSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}

				ZeroMemory(m_szInsurePass, sizeof(m_szInsurePass));

				PostMessage(WM_CLOSE, NULL, NULL);
				return true;
			}
			break;
		case SUB_GP_USER_TRANSFER_RECEIPT:		// 银行转账回执
			{
				// 收到游戏回执
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GR_UserTransferReceipt));
				if (wDataSize<sizeof(CMD_GR_UserTransferReceipt)) return false;

				CMD_GR_UserTransferReceipt * pUserTransferReceipt=(CMD_GR_UserTransferReceipt *)pData;
				m_UserInsureInfo.lUserScore=pUserTransferReceipt->lSourceUserScore;
				m_UserInsureInfo.lUserInsure=pUserTransferReceipt->lSourceUserInsure;

				//关闭连接
				m_MissionManager.ConcludeMissionItem(this,false);

				// 更新全局数据
				//变量定义
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
				pGlobalUserData->dwLoveLiness = pUserTransferReceipt->dwLoveliness;

				//更新大厅
				tagInsurePlazaUpdata Updata;
				Updata.lUserScore=pUserTransferReceipt->lSourceUserScore;
				Updata.lUserInsure=pUserTransferReceipt->lSourceUserInsure;
				AfxGetMainWnd()->SendMessage(WM_INSUREPLAZA_UPDATA,(WPARAM)&Updata,0);

				//更新界面
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				// 弹出回执窗口
				CDlgInsureTransferReceipt InsureTransferReceipt(this);
				InsureTransferReceipt.ShowReceipt(pUserTransferReceipt);

				// 清除玩家信息
				m_DlgInsureTransfer.m_edScore.SetWindowText(_T(""));
				m_DlgInsureTransfer.m_edNickName.SetWindowText(_T(""));
				m_DlgInsureTransfer.SetUserNickName(_T(""));

				return true;
			}
			break;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgInsureServer::CDlgInsureServer()
{
	//设置变量
	m_pITCPSocket=NULL;
	m_pIMySelfUserItem=NULL;

	return;
}

//析构函数
CDlgInsureServer::~CDlgInsureServer()
{
}

//查询事件
VOID CDlgInsureServer::PerformQueryInfo()
{
	//效验参数
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	//变量定义
	CMD_GR_C_QueryInsureInfoRequest QueryInsureInfo;
	QueryInsureInfo.cbActivityGame=FALSE;

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE, SUB_GR_QUERY_INSURE_INFO, &QueryInsureInfo, sizeof(QueryInsureInfo));

	return;
}

VOID CDlgInsureServer::PerformQueryTransRecord()
{
    //效验参数
    ASSERT(m_pITCPSocket!=NULL);
    if (m_pITCPSocket==NULL) return;

    //变量定义
    CMD_GR_C_QueryInsureInfoRequest QueryInsureInfo;
    QueryInsureInfo.cbActivityGame=FALSE;

    //发送数据
    m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_QUERY_TRANSRECORD,&QueryInsureInfo,sizeof(QueryInsureInfo));
}

// 根据GameID查询玩家昵称
VOID CDlgInsureServer::PerformQueryUserNickName(DWORD dwGameID)
{
	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
	m_bTransRecordMission=false;
	m_bQueryNickNameMission=true;
	m_bVerifyInsurePassword = false;
	m_bModifyInsurePassword = false;

//	m_dwGameID = dwGameID;

	CMD_GP_QueryNickNameByGameID QueryNickNameByGameID;
	ZeroMemory(&QueryNickNameByGameID,sizeof(QueryNickNameByGameID));
	QueryNickNameByGameID.dwGameID=dwGameID;

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_QUERY_NICKNAME_BY_GAMEID,&QueryNickNameByGameID,sizeof(QueryNickNameByGameID));
}

// 处理银行密码
VOID CDlgInsureServer::PerformInsurePassword(LPCTSTR pszInsurePass)
{
	//设置变量
	m_bTakeMission=false;
	m_bSaveMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
	m_bTransRecordMission=false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = true;
	m_bModifyInsurePassword = false;

	//设置变量
	lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));

	// 发送银行密码消息
	CMD_GR_VerifyUserInsurePassword VerifyUserInsurePassword;
	ZeroMemory(&VerifyUserInsurePassword,sizeof(VerifyUserInsurePassword));

	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	VerifyUserInsurePassword.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
	lstrcpyn(VerifyUserInsurePassword.szInsurePassword,pszInsurePass,CountArray(VerifyUserInsurePassword.szInsurePassword));

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_VERIFY_INSURE_PASSWORD,&VerifyUserInsurePassword,sizeof(VerifyUserInsurePassword));
}
// 修改银行密码
VOID CDlgInsureServer::PerformModiyInsurePassword(LPCTSTR pszSrcPassword, LPCTSTR pszDesPassword)
{
	m_bSaveMission = false;						//存款任务
	m_bTakeMission = false;						//取款任务
	m_bQueryMission = false;					//查询任务
	m_bTransferMission = false;					//转账任务
	m_bTransRecordMission = false;              //转账记录
	m_bQueryNickNameMission = false;			// 查询昵称
	m_bVerifyInsurePassword = false;			// 输入银行密码，进行检测
	m_bModifyInsurePassword = true;				// 修改银行密码

//	lstrcpyn(m_szSrcInsurePass, pszSrcPassword, CountArray(m_szSrcInsurePass));
//	lstrcpyn(m_szDesInsurePass, pszDesPassword, CountArray(m_szDesInsurePass));



	//设置变量
	CMD_GP_ModifyInsurePass ModifyInsurePass;
	ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

	//加密密码
	TCHAR szSrcPassword[LEN_PASSWORD]=TEXT("");
	TCHAR szDesPassword[LEN_PASSWORD]=TEXT("");
	CWHEncrypt::MD5Encrypt(pszSrcPassword, szSrcPassword);
	CWHEncrypt::MD5Encrypt(pszDesPassword, szDesPassword);

	//构造数据
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	ModifyInsurePass.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
	lstrcpyn(ModifyInsurePass.szScrPassword,szSrcPassword,CountArray(ModifyInsurePass.szScrPassword));
	lstrcpyn(ModifyInsurePass.szDesPassword,szDesPassword,CountArray(ModifyInsurePass.szDesPassword));

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_MODIFY_INSURE_PASS,&ModifyInsurePass,sizeof(ModifyInsurePass));
}

//存入事件
VOID CDlgInsureServer::PerformSaveScore(SCORE lSaveScore)
{
	//效验参数
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	//变量定义
	CMD_GR_C_SaveScoreRequest SaveScoreRequest;
	ZeroMemory(&SaveScoreRequest,sizeof(SaveScoreRequest));

	//设置变量
	SaveScoreRequest.lSaveScore=lSaveScore;

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_SAVE_SCORE_REQUEST,&SaveScoreRequest,sizeof(SaveScoreRequest));

	return;
}

//取出事件
VOID CDlgInsureServer::PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass)
{
	//效验参数
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	//变量定义
	CMD_GR_C_TakeScoreRequest TakeScoreRequest;
	ZeroMemory(&TakeScoreRequest,sizeof(TakeScoreRequest));

	//设置变量
	TakeScoreRequest.lTakeScore=lTakeScore;
//	lstrcpyn(TakeScoreRequest.szInsurePass,pszInsurePass,CountArray(TakeScoreRequest.szInsurePass));
	lstrcpyn(TakeScoreRequest.szInsurePass,m_szInsurePass,CountArray(TakeScoreRequest.szInsurePass));

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_TAKE_SCORE_REQUEST,&TakeScoreRequest,sizeof(TakeScoreRequest));

	return;
}

//转账事件
VOID CDlgInsureServer::PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass)
{
	//效验参数
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bTransferMission=true;
	m_bTransRecordMission=false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = false;

	//设置变量
	m_cbByNickName=cbByNickName;
	m_lScore=lTransferScore;
	lstrcpyn(m_szNickName,pszNickName,CountArray(m_szNickName));
//	lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));

	//变量定义
	CMD_GR_C_QueryUserInfoRequest QueryUserInfoRequest;
	ZeroMemory(&QueryUserInfoRequest,sizeof(QueryUserInfoRequest));

	//设置变量
	QueryUserInfoRequest.cbByNickName=cbByNickName;
	lstrcpyn(QueryUserInfoRequest.szNickName,pszNickName,CountArray(QueryUserInfoRequest.szNickName));

	//发送数据
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_QUERY_USER_INFO_REQUEST,&QueryUserInfoRequest,sizeof(QueryUserInfoRequest));

	return;
}

//配置银行
VOID CDlgInsureServer::Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem)
{
	//设置变量
	m_pITCPSocket=pITCPSocket;
	m_pIMySelfUserItem=pIMySelfUserItem;

	return;
}

//银行消息
bool CDlgInsureServer::OnServerInsureMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//消息判断
	if (wMainCmdID==MDM_GR_INSURE)
	{
		switch (wSubCmdID)
		{
		case SUB_GR_USER_INSURE_INFO:		//银行资料
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GR_S_UserInsureInfo));
				if (wDataSize<sizeof(CMD_GR_S_UserInsureInfo)) return false;

				//变量定义
				CMD_GR_S_UserInsureInfo * pUserInsureInfo=(CMD_GR_S_UserInsureInfo *)pData;

				//设置变量
				m_bInitInfo=true;
				m_UserInsureInfo.wRevenueTake=pUserInsureInfo->wRevenueTake;
				m_UserInsureInfo.wRevenueTransfer=pUserInsureInfo->wRevenueTransfer;
				m_UserInsureInfo.wServerID=pUserInsureInfo->wServerID;
				m_UserInsureInfo.lUserScore=pUserInsureInfo->lUserScore;
				m_UserInsureInfo.lUserInsure=pUserInsureInfo->lUserInsure;
				m_UserInsureInfo.lTransferPrerequisite=pUserInsureInfo->lTransferPrerequisite;

				//更新界面
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				return true;
			}
        case SUB_GR_TRANS_RECORD:
            {
                ASSERT(sizeof(CMD_GR_RealTransRecord) == wDataSize);
                if(sizeof(CMD_GR_RealTransRecord) != wDataSize) return false;
                CMD_GR_RealTransRecord*  pRealTransRecord = (CMD_GR_RealTransRecord*)pData;
                m_DlgTransRecord.SetRecordList(pRealTransRecord);
              
                return true;
            }
		case SUB_GR_USER_INSURE_SUCCESS:	//银行成功
			{
				//效验参数
				CMD_GR_S_UserInsureSuccess * pUserInsureSuccess=(CMD_GR_S_UserInsureSuccess *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GR_S_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GR_S_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString))) return false;

				//设置变量
				m_UserInsureInfo.lUserScore=pUserInsureSuccess->lUserScore;
				m_UserInsureInfo.lUserInsure=pUserInsureSuccess->lUserInsure;

				//输入控件
//				if(m_DlgInsureSave.m_PasswordControl.m_hWnd != NULL)
//				{
//					m_DlgInsureSave.m_PasswordControl.SetFocus();
//					m_DlgInsureSave.m_PasswordControl.SetUserPassword(TEXT(""));
//				}
// 				if(m_DlgInsureTransfer.m_PasswordControl.m_hWnd != NULL)
// 				{
// 					m_DlgInsureTransfer.m_PasswordControl.SetFocus();
// 					m_DlgInsureTransfer.m_PasswordControl.SetUserPassword(TEXT(""));
// 				}

				//更新界面
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);


				//显示消息
				if (pUserInsureSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}

				if (m_bVerifyInsurePassword)
				{
					// 验证银行密码阶段
					m_TabControl.ShowWindow(SW_SHOW);

					SwitchInsureModeView(INSURE_SAVE);

					GetDlgItem(IDC_CHECK_AUTO_LOGOFF)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_CHECK_AUTO_LOGOFF)->SetWindowPos(NULL, 340, 42, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
				}

// 				if (m_bModifyInsurePassword)
// 				{
// 					// 修改密码成功
// 					if (pUserInsureSuccess->szDescribeString[0]!=0)
// 					{
// 						CInformation Information(this);
// 						Information.ShowMessageBox(pUserInsureSuccess->szDescribeString,MB_ICONINFORMATION,60);
// 					}
// 					ZeroMemory(m_szInsurePass, sizeof(m_szInsurePass));
// 					PostMessage(WM_CLOSE, NULL, NULL);
// 				}

				return true;
			}
		case SUB_GR_USER_INSURE_FAILURE:	//银行失败
			{
				//效验参数
				CMD_GR_S_UserInsureFailure * pUserInsureFailure=(CMD_GR_S_UserInsureFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString))) return false;

				//失败处理
				OnMissionConclude();

				//输入控件
				if (m_bInitInfo==true)
				{
//					if(m_DlgInsureSave.m_PasswordControl.m_hWnd != NULL)
//					{
//						m_DlgInsureSave.m_PasswordControl.SetFocus();
//						m_DlgInsureSave.m_PasswordControl.SetUserPassword(TEXT(""));
//					}
// 					if(m_DlgInsureTransfer.m_PasswordControl.m_hWnd != NULL)
// 					{
// 						m_DlgInsureTransfer.m_PasswordControl.SetFocus();
// 						m_DlgInsureTransfer.m_PasswordControl.SetUserPassword(TEXT(""));
// 					}
				}

				//显示消息
				if (pUserInsureFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureFailure->szDescribeString,MB_ICONERROR,60);
				}

// 				if (m_bModifyInsurePassword)
// 				{
// 					//设置焦点
// 					if (m_bModifyInsurePassword == true && m_DlgInsureModifyPassword.GetSafeHwnd() != NULL)
// 					{
// 						m_DlgInsureModifyPassword.SetFocus();
// 					}
// 				}

				return true;
			}
		case SUB_GR_USER_TRANSFER_USER_INFO:  //用户信息
			{
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GR_S_UserTransferUserInfo));
				if (wDataSize<sizeof(CMD_GR_S_UserTransferUserInfo)) return false;

				//变量定义
				CMD_GR_S_UserTransferUserInfo * pTransferUserInfo=(CMD_GR_S_UserTransferUserInfo *)pData;


				if (m_bTransferMission == true)
				{
					//构造消息
					TCHAR szMessage[128]={0};
					_sntprintf(szMessage, CountArray(szMessage), TEXT("您确定要给[%s], ID:%lu 赠送%I64d 银子吗?"), pTransferUserInfo->szNickName, pTransferUserInfo->dwTargetGameID, m_lScore);

					//提示消息
					CInformation Information(this);
					if (Information.ShowMessageBox(szMessage,MB_ICONQUESTION|MB_YESNO,0)==IDYES)
					{
						//变量定义
						CMD_GP_C_TransferScoreRequest TransferScoreRequest;
						ZeroMemory(&TransferScoreRequest,sizeof(TransferScoreRequest));

						//设置变量
						TransferScoreRequest.cbByNickName=m_cbByNickName;
						TransferScoreRequest.lTransferScore=m_lScore;
						lstrcpyn(TransferScoreRequest.szNickName,m_szNickName,CountArray(TransferScoreRequest.szNickName));
						lstrcpyn(TransferScoreRequest.szInsurePass,m_szInsurePass,CountArray(TransferScoreRequest.szInsurePass));

						//发送数据
						m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_TRANSFER_SCORE_REQUEST,&TransferScoreRequest,sizeof(TransferScoreRequest));
					}
				}

				// 查询昵称任务
				if (m_bQueryNickNameMission)
				{
					if (m_DlgInsureTransfer.GetSafeHwnd() != NULL)
					{
						m_DlgInsureTransfer.SetUserNickName(pTransferUserInfo->szNickName);
					}
				}
				return true;
			}
		case SUB_GR_USER_TRANSFER_RECEIPT:		// 转账回执
			{
				// 收到游戏回执
				//效验参数
				ASSERT(wDataSize==sizeof(CMD_GR_UserTransferReceipt));
				if (wDataSize<sizeof(CMD_GR_UserTransferReceipt)) return false;

				CMD_GR_UserTransferReceipt * pUserTransferReceipt=(CMD_GR_UserTransferReceipt *)pData;

				m_UserInsureInfo.lUserScore=pUserTransferReceipt->lSourceUserScore;
				m_UserInsureInfo.lUserInsure=pUserTransferReceipt->lSourceUserInsure;

				// 更新全局数据
				//变量定义
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
				pGlobalUserData->dwLoveLiness = pUserTransferReceipt->dwLoveliness;

				//更新界面
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				//关闭连接
				OnMissionConclude();

				// 弹出回执窗口
				CDlgInsureTransferReceipt InsureTransferReceipt(this);
				InsureTransferReceipt.ShowReceipt(pUserTransferReceipt);

				// 清除玩家信息
				m_DlgInsureTransfer.m_edScore.SetWindowText(_T(""));
				m_DlgInsureTransfer.m_edNickName.SetWindowText(_T(""));
				m_DlgInsureTransfer.SetUserNickName(_T(""));


				return true;
			}
		case SUB_GP_OPERATE_SUCCESS:
			{
				
				//效验参数
				CMD_GP_OperateSuccess * pUserInsureSuccess=(CMD_GP_OperateSuccess *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateSuccess)-sizeof(pUserInsureSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateSuccess)-sizeof(pUserInsureSuccess->szDescribeString))) return false;


				//更新界面
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				if (m_bModifyInsurePassword)
				{
					// 修改密码成功
					if (pUserInsureSuccess->szDescribeString[0]!=0)
					{
						CInformation Information(this);
						Information.ShowMessageBox(pUserInsureSuccess->szDescribeString,MB_ICONINFORMATION,60);
					}
					ZeroMemory(m_szInsurePass, sizeof(m_szInsurePass));
					PostMessage(WM_CLOSE, NULL, NULL);
				}
				return true;
			}
		case SUB_GP_OPERATE_FAILURE:
			{
				//效验参数
				CMD_GP_OperateFailure * pUserInsureFailure=(CMD_GP_OperateFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateFailure)-sizeof(pUserInsureFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateFailure)-sizeof(pUserInsureFailure->szDescribeString))) return false;

				//失败处理
				OnMissionConclude();

				//显示消息
				if (pUserInsureFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureFailure->szDescribeString,MB_ICONERROR,60);
				}

				if (m_bModifyInsurePassword)
				{
					//设置焦点
					if (m_bModifyInsurePassword == true && m_DlgInsureModifyPassword.GetSafeHwnd() != NULL)
					{
						m_DlgInsureModifyPassword.SetFocus();
					}
				}
				return true;
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgInsureTransferReceipt, CSkinDialog)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BT_OK, OnBnClickedOk)
END_MESSAGE_MAP()

// 转账成功回执窗口
CDlgInsureTransferReceipt::CDlgInsureTransferReceipt(CWnd * pParentWnd) : CSkinDialog(IDD_INSURE_TRANSFER_RECEIPT, pParentWnd)
{
	m_pUserTransferReceipt = NULL;

	m_SkinAttribute.m_crBackGround = RGB(255, 255, 255);
}

CDlgInsureTransferReceipt::~CDlgInsureTransferReceipt()
{

}

//控件绑定
VOID CDlgInsureTransferReceipt::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BT_OK, m_btOk);
}


INT CDlgInsureTransferReceipt::ShowReceipt(CMD_GR_UserTransferReceipt * pUserTransferReceipt)
{
	if (pUserTransferReceipt == NULL)
	{
		return NULL;
	}

	m_pUserTransferReceipt = pUserTransferReceipt;

	//设置资源
//	m_hResInstance=AfxGetResourceHandle();
	AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

	//显示窗口
	INT nResult=(INT)DoModal();

	return nResult;
}

//初始化消息
BOOL CDlgInsureTransferReceipt::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("赠送凭证"));

	AfxSetResourceHandle(GetModuleHandle(NULL));

	// 设置控件
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	m_btOk.SetButtonImage(IDB_BT_OK, hResInstance, false, false);

	CRect rcBtOk;
	m_btOk.GetClientRect(rcBtOk);
	m_btOk.MoveWindow(165, 215, rcBtOk.Width(), rcBtOk.Height());


	// 移动控件
	int space = 20;
	GetDlgItem(IDC_SOURCE_NICKNAME)->MoveWindow(162, 53, 140, 18);
	GetDlgItem(IDC_TARGET_NICKNAME)->MoveWindow(162, 53+space, 140, 18);
//	GetDlgItem(IDC_SOURCE_GAMEID)->MoveWindow(162, 53+2*space, 140, 18);
	GetDlgItem(IDC_TARGET_GAMEID)->MoveWindow(162, 53+2*space, 140, 18);
	GetDlgItem(IDC_TRANSFER_SCORE)->MoveWindow(162, 53+3*space, 140, 18);
	GetDlgItem(IDC_TRANSFER_SCORE_CHN)->MoveWindow(162, 53+4*space, 140, 18);
	GetDlgItem(IDC_TRANSFER_FEE)->MoveWindow(162, 53+5*space, 140, 18);
	GetDlgItem(IDC_TRANSFER_FACT)->MoveWindow(162, 53+6*space, 140, 18);
	GetDlgItem(IDC_TRANSFER_DATE)->MoveWindow(162, 53+7*space, 140 ,18);

	GetDlgItem(IDC_TRANSFER_RECEIPTID)->MoveWindow(340, 43+10, 110, 16);

	GetDlgItem(IDC_TIP)->MoveWindow(50, 258, 330, 18);

	// 设置文字
	SetDlgItemText(IDC_SOURCE_NICKNAME, m_pUserTransferReceipt->szSourceNickName);
	SetDlgItemText(IDC_TARGET_NICKNAME, m_pUserTransferReceipt->szTargetNickName);

	TCHAR chReceiptID[128] = TEXT("");
	_sntprintf(chReceiptID, CountArray(chReceiptID), _T("%lu"), m_pUserTransferReceipt->dwRecordID);
	SetDlgItemText(IDC_TRANSFER_RECEIPTID, chReceiptID);

	TCHAR chSourceGameID[32] = {0};
	_sntprintf(chSourceGameID, CountArray(chSourceGameID), _T("%lu"), m_pUserTransferReceipt->dwSourceGameID);
	SetDlgItemText(IDC_SOURCE_GAMEID, chSourceGameID);

	TCHAR chTargetGameID[32] = {0};
	_sntprintf(chTargetGameID, CountArray(chTargetGameID), _T("%lu"), m_pUserTransferReceipt->dwTargetGameID);
	SetDlgItemText(IDC_TARGET_GAMEID, chTargetGameID);

	TCHAR chTransferScore[128] = TEXT("");
	m_DlgInsureItem.SwitchScoreFormat(m_pUserTransferReceipt->lTransferScore, 3L, chTransferScore, CountArray(chTransferScore) );
	SetDlgItemText(IDC_TRANSFER_SCORE, chTransferScore);

	TCHAR chTranferFee[128] = TEXT("");
	m_DlgInsureItem.SwitchScoreFormat(m_pUserTransferReceipt->lRevenueScore, 3L, chTranferFee, CountArray(chTranferFee) );
	SetDlgItemText(IDC_TRANSFER_FEE, chTranferFee);

	TCHAR chTranferFact[128] = TEXT("");
	m_DlgInsureItem.SwitchScoreFormat(m_pUserTransferReceipt->lReceviceScore, 3L, chTranferFact, CountArray(chTranferFact) );
	SetDlgItemText(IDC_TRANSFER_FACT, chTranferFact);

	TCHAR chTransferScoreCHN[128] = TEXT("");
	m_DlgInsureItem.SwitchScoreString(m_pUserTransferReceipt->lTransferScore, chTransferScoreCHN, CountArray(chTransferScoreCHN) );
	SetDlgItemText(IDC_TRANSFER_SCORE_CHN, chTransferScoreCHN);

	CString cstr;
	cstr.Format(_T("%d-%02d-%02d %02d:%02d:%02d"), m_pUserTransferReceipt->stTransferTime.wYear, m_pUserTransferReceipt->stTransferTime.wMonth, \
		m_pUserTransferReceipt->stTransferTime.wDay, m_pUserTransferReceipt->stTransferTime.wHour, m_pUserTransferReceipt->stTransferTime.wMinute, \
		m_pUserTransferReceipt->stTransferTime.wSecond);
	SetDlgItemText(IDC_TRANSFER_DATE, cstr);

	return TRUE;
}

// VOID CDlgInsureTransferReceipt::OnPaint()
// {
// 	
// }

//绘画消息
VOID CDlgInsureTransferReceipt::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight)
{
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);

	CRect rcClient;
	GetClientRect(rcClient);

	CImage ImageTransferSuccess;
	ImageTransferSuccess.LoadFromResource(hResInstance, IDB_INSURE_TRANSFER_SUCCESS);

	CPngImage ImageTransferReceipt;
	ImageTransferReceipt.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_TRANSFER_RECEIPT"));

	// 获取边框大小
	INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
	INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
	INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
	INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;

	ImageTransferReceipt.DrawImage(pDC, nLBorder, nTBorder, ImageTransferReceipt.GetWidth(), ImageTransferReceipt.GetHeight(), 0, 0);
	ImageTransferSuccess.BitBlt(pDC->GetSafeHdc(), rcClient.Width() - nRBorder - ImageTransferSuccess.GetWidth(), rcClient.Height()- nBBorder - ImageTransferSuccess.GetHeight());
}

HBRUSH CDlgInsureTransferReceipt::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
/*	switch (nCtlColor)
	{
 	case CTLCOLOR_DLG:
 	case CTLCOLOR_BTN:
 	case CTLCOLOR_STATIC:
 		{
 			pDC->SetBkMode(TRANSPARENT);
 			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlText);
 			return m_SkinAttribute.m_brBackGround;
 		}
 	}*/

	if (pWnd->GetDlgCtrlID() == IDC_TRANSFER_RECEIPTID)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(98, 98, 98));
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
 
 	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

// 确定按钮
VOID CDlgInsureTransferReceipt::OnBnClickedOk()
{
	// 方案1,但是截图有黑边
// 	RECT rect;
// 	::GetWindowRect(GetSafeHwnd(), &rect);
// 
// 	HBITMAP hBmp = NULL;
// 
// 	HDC hdcScreen = ::GetDC(NULL);
// 	HDC hdc = CreateCompatibleDC(hdcScreen);
// 	hBmp = CreateCompatibleBitmap(hdcScreen, rect.right - rect.left, rect.bottom - rect.top);   
// 	SelectObject(hdc, hBmp);
// 
// 	::PrintWindow(GetSafeHwnd(), hdc, PW_CLIENTONLY);
// 
// 	// 拷贝当前窗口截图
// 	if( OpenClipboard() )   
// 	{   
// 		EmptyClipboard(); 
// 		SetClipboardData(CF_BITMAP, hBmp);
// 		CloseClipboard();
// 	}
// 	else
// 	{
// 		// 提示要用户手动复制
// 		MessageBox(_T("打开剪切板失败"));
// 	}
// 	DeleteDC(hdc); 
// 	DeleteObject(hBmp); 
// 	::ReleaseDC(NULL, hdcScreen); 

	// 方案2
//	HBITMAP hBit = NULL;
	RECT rect;
	::GetWindowRect(GetSafeHwnd(), &rect);

	HWND	hWnd = ::GetDesktopWindow();//获得屏幕的HWND.   
	HDC		hScreenDC = ::GetDC(hWnd);       //获得屏幕的HDC.   
	HDC		MemDC = ::CreateCompatibleDC(hScreenDC);

	HBITMAP	hBitmap	= ::CreateCompatibleBitmap(hScreenDC,rect.right-rect.left,rect.bottom-rect.top);   
	HGDIOBJ	hOldBMP	= ::SelectObject(MemDC,hBitmap);   
	::BitBlt(MemDC, 0, 0, rect.right, rect.bottom, hScreenDC, rect.left, rect.top, SRCCOPY);   //截图

	hBitmap=(HBITMAP)::SelectObject(MemDC,hOldBMP); 

	bool bClose = false;
	if( OpenClipboard() )   
	{   
	 	EmptyClipboard(); 
	 	SetClipboardData(CF_BITMAP, hBitmap);
	 	CloseClipboard();

		bClose = true;
	}
	else
	{
	 	// 提示要用户手动复制
		//提示消息
		TCHAR szBuffer[256]=TEXT("");
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("打开剪切板错误,截屏失败,如有需要请用其它截图工具进行截图!"));

		//提示消息
		CInformation Information(this);
		Information.ShowMessageBox(szBuffer,MB_OK|MB_ICONWARNING);

		bClose = false;
	}

	::DeleteObject(MemDC);   
	::ReleaseDC(hWnd,hScreenDC);

	if (bClose == false)
	{
		return;
	}

	PostMessage(WM_CLOSE, NULL, NULL);
}

void CDlgInsureTransferReceipt::OnClose()
{
//	MessageBox(_T("CDlgInsureTransferReceipt::OnClose"));
	// 拷贝当前窗口截图
// 	if( OpenClipboard() )   
// 	{   
// 		EmptyClipboard();   
// 		//create   some   data   
// 		CBitmap   *   junk   =   new   CBitmap();   
// 		CClientDC   cdc(this);   
// 		CDC   dc;   
// 		dc.CreateCompatibleDC(&cdc);   
// 		CRect   client(0,0,200,200);   
// 		junk->CreateCompatibleBitmap(&cdc,client.Width(),client.Height());   
// 		dc.SelectObject(junk);   
// 
// 		//call   draw   routine   here   that   makes   GDI   calls   
// 		DrawImage(&dc,CString("Bitmap"));   
// 
// 		//put   the   data   on   the   clipboard   
// 		SetClipboardData(CF_BITMAP,junk->m_hObject);   
// 		CloseClipboard();   
// 
// 		//copy   has   been   made   on   clipboard   so   we   can   delete   
// 		delete   junk;   
// 	}   

	__super::OnClose();
}

