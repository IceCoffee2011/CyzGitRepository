// DlgTipUserInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgTipUserInfo.h"
#include ".\dlgtipuserinfo.h"

TCHAR *szUserOrder[]={_T("普通玩家"),_T("蓝钻玩家"),_T("黄钻玩家"),_T("白钻玩家"),_T("红钻玩家"),_T("VIP会员")};

BEGIN_MESSAGE_MAP(CDlgTipUserInfo, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CDlgTipUserInfo 对话框

IMPLEMENT_DYNAMIC(CDlgTipUserInfo, CDialog)
CDlgTipUserInfo::CDlgTipUserInfo(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DLG_TIP_USERINFOR, pParent)
{
}

CDlgTipUserInfo::~CDlgTipUserInfo()
{
}

void CDlgTipUserInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

//构造信息
VOID CDlgTipUserInfo::ConstructUserInfo(IClientUserItem *pUserItem,IGameLevelParser *pIGameLevelParser)
{
	//参数判断
	if(pUserItem==NULL||pIGameLevelParser==NULL) return;

	//建立缓冲
	if(m_ImageBuf.IsNull()) m_ImageBuf.Create(TIP_USER_INFO_W,TIP_USER_INFO_H,32);

	//创建 DC
	CImageDC BufDC(m_ImageBuf);
	CDC * pDCBuf=CDC::FromHandle(BufDC);

	//设置环境
	pDCBuf->SetBkMode(TRANSPARENT);
	pDCBuf->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());
	pDCBuf->FillSolidRect(0,0,TIP_USER_INFO_W,TIP_USER_INFO_H,RGB(255,255,255));

	//创建画笔
	COLORREF crLine=RGB(117,130,161);
	COLORREF crText=RGB(67,80,158);
	CPen PenDot(PS_DOT,1,crLine);
	CPen * pOldPen=pDCBuf->SelectObject(&PenDot);

	//变量定义
	TCHAR szString[128]=TEXT("");
	int nFaceW=112;

	//变量定义
	tagCustomFaceInfo * pCustomFaceInfo=pUserItem->GetCustomFaceInfo();
	WORD wGameLevel=pIGameLevelParser->GetGameLevel(pUserItem->GetUserScore());
	BYTE cbGender=pUserItem->GetGender();

	//位置定义
	CRect rcNickName(5,2,TIP_USER_INFO_W-5,2+12);
	int nYoff=16;
	int nTextOffX=52;

	//用户信息
	pDCBuf->SetTextColor(crText);
	_sntprintf(szString, sizeof(szString), TEXT("昵  称："));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(0,0,0));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%s"),pUserItem->GetNickName());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//绘画虚线
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	pDCBuf->MoveTo(rcNickName.left,rcNickName.top);
	pDCBuf->LineTo(rcNickName.right,rcNickName.top);

	//头像框
	CRect rcFace(rcNickName.left, rcNickName.top+5, rcNickName.left+106, rcNickName.top+5+106);
	pDCBuf->Draw3dRect(&rcFace, crLine, crLine);

	//头像位置
	int nXPos = rcFace.left+(rcFace.Width()-FACE_CX)/2;
	int nYPos = rcFace.top+(rcFace.Height()-FACE_CY)/2;

	//绘画头像
	if ((pCustomFaceInfo->dwDataSize!=0L)&&(pUserItem->GetCustomID()!=0L))
	{
		IFaceItemControl * pIFaceItemControl=CFaceItemControl::GetInstance();
		pIFaceItemControl->DrawFaceNormal(pDCBuf,nXPos,nYPos,pCustomFaceInfo->dwCustomFace);
	}
	else
	{
		//IFaceItemControl * pIFaceItemControl=CFaceItemControl::GetInstance();
		//pIFaceItemControl->DrawFaceNormal(pDCBuf,nXPos,nYPos,pUserItem->GetFaceID());
		pDCBuf->SetTextColor(crText);
		_sntprintf(szString, sizeof(szString), TEXT("头像未上传"));
		pDCBuf->DrawText(szString, lstrlen(szString), &rcFace,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.left+=nFaceW; rcNickName.top+=5; rcNickName.bottom+=5;
	_sntprintf(szString, sizeof(szString), TEXT("魅力值："));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(0,0,0));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%ld"),pUserItem->GetUserLoveLiness());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("I   D ："));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(255,0,255));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%ld"),pUserItem->GetGameID());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("成  绩："));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(255,0,255));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%I64d"),pUserItem->GetUserScore());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("等  级："));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(255,0,255));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%s"),pIGameLevelParser->GetLevelDescribe(pUserItem->GetUserScore()));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//用户信息
	BYTE cbMemberOrder=pUserItem->GetMemberOrder()>5?0:pUserItem->GetMemberOrder();
	pDCBuf->SetTextColor(crText);
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("会  员："));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(255,0,255));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%s"),szUserOrder[cbMemberOrder]);
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("逃跑率："));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(0,0,0));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%4.2f%%"),pUserItem->GetUserFleeRate());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("胜  率："));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(0,0,0));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%4.2f%%"),pUserItem->GetUserWinRate());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//用户信息
	/*pDCBuf->SetTextColor(crText);
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("成  绩："));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(0,0,0));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%I64d"),pUserItem->GetUserGrade());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;*/

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("总局数："));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(0,0,0));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%ld"),pUserItem->GetUserPlayCount());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("胜  局："));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(0,0,0));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%ld"),pUserItem->GetUserWinCount());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("输  局："),pUserItem->GetUserLostCount());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(0,0,0));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%ld"),pUserItem->GetUserLostCount());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("和  局："),pUserItem->GetUserLostCount());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(0,0,0));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%ld"),pUserItem->GetUserDrawCount());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("逃  局："),pUserItem->GetUserLostCount());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(0,0,0));
	rcNickName.left += nTextOffX;
	_sntprintf(szString, sizeof(szString), TEXT("%ld"),pUserItem->GetUserFleeCount());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= nTextOffX;

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.left-=nFaceW; rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	_sntprintf(szString, sizeof(szString), TEXT("Lv."));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(255,0,255));
	rcNickName.left += 26;
	_sntprintf(szString, sizeof(szString), TEXT("%d"),wGameLevel);
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

	//用户信息
	pDCBuf->SetTextColor(crText);
	rcNickName.left += 36;
	_sntprintf(szString, sizeof(szString), TEXT("Exp."));
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	pDCBuf->SetTextColor(RGB(0,0,0));
	rcNickName.left += 26;
	_sntprintf(szString, sizeof(szString), TEXT("%ld"), pUserItem->GetUserExperience());
	pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	rcNickName.left -= (26+36+26);

	//绘画虚线
	rcNickName.top+=nYoff; rcNickName.bottom+=nYoff;
	pDCBuf->MoveTo(rcNickName.left,rcNickName.top);
	pDCBuf->LineTo(rcNickName.right,rcNickName.top);

	//用户信息
	rcNickName.top+=5; rcNickName.bottom+=(nYoff*2+5);
	if(lstrlen(pUserItem->GetUnderWrite())==0)
	{
		pDCBuf->SetTextColor(crText);
		_sntprintf(szString, sizeof(szString), TEXT("---签名档---"));
		pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}
	else
	{
		pDCBuf->SetTextColor(RGB(0,0,0));
		_sntprintf(szString, sizeof(szString), TEXT("%s"),pUserItem->GetUnderWrite());
		pDCBuf->DrawText(szString, lstrlen(szString), &rcNickName,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}

	//绘画虚线
	rcNickName.top+=(nYoff*2+10); rcNickName.bottom+=nYoff;
	pDCBuf->MoveTo(rcNickName.left,rcNickName.top);
	pDCBuf->LineTo(rcNickName.right,rcNickName.top);

	//
	pDCBuf->SelectObject(pOldPen);

	//更新界面
	RedrawWindow();

	return;
}

BOOL CDlgTipUserInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	//建立缓冲
	m_ImageBuf.Create(TIP_USER_INFO_W,TIP_USER_INFO_H,32);

	LONG rtn = GetWindowLong(m_hWnd,GWL_EXSTYLE);
	rtn |= WS_EX_LAYERED;
	SetWindowLong(m_hWnd,GWL_EXSTYLE,rtn);

	//设置窗口
	SetLayeredWindowAttributes(RGB(0,0,0),160,LWA_ALPHA);

	return TRUE;
}

void CDlgTipUserInfo::OnPaint()
{
	CPaintDC dc(this);

	m_ImageBuf.BitBlt(dc.GetSafeHdc(),0,0);

	return;
}
