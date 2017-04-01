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

#define WM_INSUREPLAZA_UPDATA      (WM_USER+1680)             //���и�����Ϣ

struct tagInsurePlazaUpdata
{
	SCORE  lUserScore;
	SCORE  lUserInsure;
};

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgInsureItem::CDlgInsureItem()
{
}

//��������
CDlgInsureItem::~CDlgInsureItem()
{
}

//��ȡ��Ϸ��
SCORE CDlgInsureItem::GetUserControlScore(CWnd * pWndScore)
{
	//��������
	WORD wIndex=0;
	SCORE lScore=0L;

	//��ȡ��ֵ
	TCHAR szBuffer[32]=TEXT("");
	pWndScore->GetWindowText(szBuffer,CountArray(szBuffer));

	//������ֵ
	while (szBuffer[wIndex]!=0)
	{
		//��������
		WORD wCurrentIndex=wIndex++;

		//������ֵ
		if ((szBuffer[wCurrentIndex]>=TEXT('0'))&&(szBuffer[wCurrentIndex]<=TEXT('9')))
		{
			lScore=lScore*10L+(szBuffer[wCurrentIndex]-TEXT('0'));
		}
	}

	return lScore;
}

//�滭����
VOID CDlgInsureItem::DrawNumberString(CDC * pDC, SCORE lScore, INT nXPos, INT nYPos)
{
	//ת������
	TCHAR szControl[128]=TEXT("");
	SwitchScoreFormat(lScore,3L,szControl,CountArray(szControl));

	//��������
	INT nXDrawPos=nXPos;
	INT nScoreLength=lstrlen(szControl);

	//�滭�ж�
	if (nScoreLength>0L)
	{
		//������Դ
		CPngImage ImageNumber;
		ImageNumber.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("SCORE_NUMBER"));

		//��ȡ��С
		CSize SizeNumber;
		SizeNumber.SetSize(ImageNumber.GetWidth()/12L,ImageNumber.GetHeight());

		//�滭����
		for (INT i=0;i<nScoreLength;i++)
		{
			//�滭����
			if (szControl[i]==TEXT(','))
			{
				ImageNumber.DrawImage(pDC,nXDrawPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*10L,0L);
			}

			//�滭���
			if (szControl[i]==TEXT('.'))
			{
				ImageNumber.DrawImage(pDC,nXDrawPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*11L,0L);
			}

			//�滭����
			if (szControl[i]>=TEXT('0')&&szControl[i]<=TEXT('9'))
			{
				ImageNumber.DrawImage(pDC,nXDrawPos,nYPos,SizeNumber.cx,SizeNumber.cy,SizeNumber.cx*(szControl[i]-TEXT('0')),0L);
			}

			//����λ��
			nXDrawPos+=SizeNumber.cx;
		}
	}

	return;
}

//ת���ַ�
VOID CDlgInsureItem::SwitchScoreString(SCORE lScore, LPTSTR pszBuffer, WORD wBufferSize)
{
	//��������
	LPCTSTR pszNumber[]={TEXT("��"),TEXT("Ҽ"),TEXT("��"),TEXT("��"),TEXT("��"),TEXT("��"),TEXT("½"),TEXT("��"),TEXT("��"),TEXT("��")};
	LPCTSTR pszWeiName[]={TEXT("ʰ"),TEXT("��"),TEXT("Ǫ"),TEXT("��"),TEXT("ʰ"),TEXT("��"),TEXT("Ǫ"),TEXT("��"),TEXT("ʰ"),TEXT("��"),TEXT("Ǫ"),TEXT("��"),TEXT("ʰ"),TEXT("��"),TEXT("Ǫ"),TEXT("��")};

	//ת����ֵ
	TCHAR szSwitchScore[21]=TEXT("");
	_sntprintf(szSwitchScore,CountArray(szSwitchScore),TEXT("%I64d"),lScore);

	//��������
	bool bNeedFill=false;
	bool bNeedZero=false;
	UINT uTargetIndex=0,uSourceIndex=0;

	//�ַ�����
	UINT uItemLength=lstrlen(pszNumber[0]);
	UINT uSwitchLength=lstrlen(szSwitchScore);

	//ת������
	for (UINT i=0;i<uSwitchLength;i++)
	{
		//��������
		WORD wNumberIndex=szSwitchScore[i]-TEXT('0');

		//�������
		if ((bNeedZero==true)&&(wNumberIndex!=0L))
		{
			bNeedZero=false;
			_tcsncat(pszBuffer,pszNumber[0],wBufferSize-lstrlen(pszBuffer)-1);
		}

		//��������
		if (wNumberIndex!=0L)
		{
			_tcsncat(pszBuffer,pszNumber[wNumberIndex],wBufferSize-lstrlen(pszBuffer)-1);
		}

		//����λ��
		if ((wNumberIndex!=0L)&&((uSwitchLength-i)>=2))
		{
			bNeedZero=false;
			_tcsncat(pszBuffer,pszWeiName[uSwitchLength-i-2],wBufferSize-lstrlen(pszBuffer)-1);
		}

		//�����ж�
		if ((bNeedZero==false)&&(wNumberIndex==0))
		{
			bNeedZero=true;
		}

		//��λ�ж�
		if ((bNeedFill==false)&&(wNumberIndex!=0))
		{
			bNeedFill=true;
		}

		//�λ��
		if (((uSwitchLength-i)==5)||((uSwitchLength-i)==9))
		{
			//����λ��
			if ((bNeedFill==true)&&(wNumberIndex==0L))
			{
				_tcsncat(pszBuffer,pszWeiName[uSwitchLength-i-2],wBufferSize-lstrlen(pszBuffer)-1);
			}

			//���ñ���
			bNeedZero=false;
			bNeedFill=false;
		}
	}

	return;
}

//ת���ַ�
VOID CDlgInsureItem::SwitchScoreFormat(SCORE lScore, UINT uSpace, LPTSTR pszBuffer, WORD wBufferSize)
{
	//ת����ֵ
	TCHAR szSwitchScore[21]=TEXT("");
	_sntprintf(szSwitchScore,CountArray(szSwitchScore),TEXT("%I64d"),lScore);

	//��������
	WORD wTargetIndex=0;
	WORD wSourceIndex=0;
	UINT uSwitchLength=lstrlen(szSwitchScore);

	//ת���ַ�
	while (szSwitchScore[wSourceIndex]!=0)
	{
		//�����ַ�
		pszBuffer[wTargetIndex++]=szSwitchScore[wSourceIndex++];

		//���붺��
		if ((uSwitchLength!=wSourceIndex)&&(((uSwitchLength-wSourceIndex)%uSpace)==0L))
		{
			pszBuffer[wTargetIndex++]=TEXT(',');
		}
	}

	//�����ַ�
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

//�ؼ���
VOID CDlgInsurePassword::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BT_OK, m_btOk);
	DDX_Control(pDX, IDC_BT_CANCEL, m_btCancel);
	DDX_Control(pDX, IDC_INSURE_PASSWORD, m_PasswordControl);
}

//��Ϣ����
BOOL CDlgInsurePassword::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//��������
BOOL CDlgInsurePassword::OnInitDialog()
{
	__super::OnInitDialog();

	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	// ���ÿؼ�
	m_btOk.SetButtonImage(IDB_BT_OK, hResInstance, false, false);
	m_btCancel.SetButtonImage(IDB_BT_CANCEL, hResInstance, false, false);

	m_PasswordControl.SetDrawBorad(false);

	// �ƶ��ؼ�λ��
	m_PasswordControl.MoveWindow(165, 31, 180, 18);

	CRect rcBtOk;
	m_btOk.GetClientRect(rcBtOk);
	m_btOk.MoveWindow(169, 230, rcBtOk.Width(), rcBtOk.Height());

	CRect rcBtCancel;
	m_btCancel.GetClientRect(rcBtCancel);
	m_btCancel.MoveWindow(269, 230, rcBtCancel.Width(), rcBtCancel.Height());

	return FALSE;
}

// ȷ����ť
VOID CDlgInsurePassword::OnOK()
{
	return;
}

// ȡ����ť
VOID CDlgInsurePassword::OnCancel()
{
	return;
}

// ȷ�����밴ť
VOID CDlgInsurePassword::OnBnClickedOk()
{
	// �ύ��������

	// ��ȡ����
	TCHAR szPassword[LEN_PASSWORD] = {0};
	m_PasswordControl.GetUserPassword(szPassword);

	//6603��������
	TCHAR szInsurePass[LEN_MD5] = {0};
	CWHEncrypt::MD5Encrypt(szPassword,szInsurePass);

	if (szPassword[0]==0)
	{
		// ��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("�������벻��Ϊ�գ������������������룡"),MB_ICONERROR,30);

		// ���ý���
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

// ȡ����ť
VOID CDlgInsurePassword::OnBnClickedCancel()
{
	// ȡ���������������
	if (GetParent())
	{
		GetParent()->PostMessage(WM_CLOSE);
	}
}

BOOL CDlgInsurePassword::OnEraseBkgnd(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//���� DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pDCBuffer=CDC::FromHandle(BufferDC);

	// ������Դ
	CPngImage ImageInsureSave;
	ImageInsureSave.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_PASSWORD"));

	//�滭����
	pDCBuffer->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	// ����ͼƬ�����Ʊ���
	ImageInsureSave.DrawImage(pDCBuffer, 0, 0, ImageInsureSave.GetWidth(), ImageInsureSave.GetHeight(), 0, 0);


	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pDCBuffer,0,0,SRCCOPY);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTransRecordList, CSkinListCtrl)
END_MESSAGE_MAP()

//�滭����
VOID CTransRecordList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    //��������
	CRect rcItem=lpDrawItemStruct->rcItem;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	

	//��ȡ����
	INT nItemID=lpDrawItemStruct->itemID;
	INT nColumnCount=m_SkinHeaderCtrl.GetItemCount();

	//������ɫ
	COLORREF rcTextColor=RGB(10,10,10);
	COLORREF rcBackColor=RGB(229,249,255);
	GetItemColor(lpDrawItemStruct,rcTextColor,rcBackColor);

	//�滭����
	CRect rcClipBox;
	pDC->GetClipBox(&rcClipBox);

	//���û���
	pDC->SetBkColor(rcBackColor);
	pDC->SetTextColor(rcTextColor);

	

	//�滭����
	for (INT i=0;i<nColumnCount;i++)
	{
		//��ȡλ��
		CRect rcSubItem;
		GetSubItemRect(nItemID,i,LVIR_BOUNDS,rcSubItem);

		//�滭�ж�
		if (rcSubItem.left>rcClipBox.right) break;
		if (rcSubItem.right<rcClipBox.left) continue;

		//�滭����
		pDC->FillSolidRect(&rcSubItem,rcBackColor);

		//�滭����
		DrawCustomItem(pDC,lpDrawItemStruct,rcSubItem,i);
	}

	/*	//c ȥ���������
	//�滭����
	if (lpDrawItemStruct->itemState&ODS_FOCUS)
	{
		pDC->DrawFocusRect(&rcItem);

	}
	//�滭����
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
//���캯��
CDlgTransRecord::CDlgTransRecord()
{
    m_bQueryRecord = true;
	m_cbCurPage=0;
	m_cbPageCell=20;
    m_cbPageCount=0;
}
//��������
CDlgTransRecord::~CDlgTransRecord()
{

}


//�ؼ���
VOID CDlgTransRecord::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	//��ť�ؼ�
	DDX_Control(pDX, IDC_RECORD_LIST, m_ListView);
    DDX_Control(pDX, IDC_QUERY, m_btQuery);
    DDX_Control(pDX, IDC_FIRST, m_btFirst);
    DDX_Control(pDX, IDC_PRE, m_btPre);
    DDX_Control(pDX, IDC_NEXT, m_btNext);
    DDX_Control(pDX, IDC_TAIL, m_btLast);
}

//��������
BOOL CDlgTransRecord::OnInitDialog()
{
	__super::OnInitDialog();

//  m_ListView.InsertColumn(0,TEXT("���"),LVCFMT_CENTER,40,0);
    m_ListView.InsertColumn(0,TEXT("������"),LVCFMT_CENTER,70,0);
	m_ListView.InsertColumn(1,TEXT("������"),LVCFMT_CENTER,70,0);
	m_ListView.InsertColumn(2,TEXT("������ID"),LVCFMT_CENTER,70,0);
	m_ListView.InsertColumn(3,TEXT("��������"),LVCFMT_CENTER,80,0);
//	m_ListView.InsertColumn(4,TEXT("˰��"),LVCFMT_CENTER,80,0);
	m_ListView.InsertColumn(4,TEXT("����ʱ��"),LVCFMT_CENTER,170,0);

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
		_sntprintf(sz,CountArray(sz),TEXT("��%d/%dҳ"),0,0);
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

	_sntprintf(sz,CountArray(sz),TEXT("��%d/%dҳ"),m_cbCurPage,m_cbPageCount);
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
//ʱ����Ϣ
void CDlgTransRecord::OnTimer(UINT nIDEvent)
{
	TRACE("CDlgTransRecord::OnTimer\n");

	//ʱ����Ϣ
	switch(nIDEvent)
	{
	case 100:		//���½���
		{
			KillTimer(100);
			//���½���
			m_bQueryRecord=true;
			return;
		}
	}
	__super::OnTimer(nIDEvent);
}
//�����
BOOL CDlgTransRecord::OnCommand(WPARAM wParam, LPARAM lParam)
{
    //��������
    UINT nCommandID=LOWORD(wParam);

    //���ܰ�ť
    switch (nCommandID)
    {
    case IDC_QUERY:        
        {
			if (m_bQueryRecord==false)
			{
				//AfxMessageBox(TEXT("��ѯƵ��̫�죬���Ժ��ѯ!"));
				CInformation Information(this);
				Information.ShowMessageBox(TEXT("��ѯƵ��̫�죬�����ϴβ�ѯ��ȥ5����ٲ�ѯ!"),MB_ICONSTOP);
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
            //�¼�֪ͨ
        	m_pDlgInsureMain->OnMissionStart();
        	m_pDlgInsureMain->PerformQueryTransRecord();

			GetDlgItem(IDC_STATUS_TIP)->SetWindowText(TEXT("���ڲ�ѯ....."));
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

//��Ϣ����
//�滭����
BOOL CDlgTransRecord::OnEraseBkgnd(CDC * pDC)
{
    
    //��ȡλ��
    CRect rcClient;
    GetClientRect(&rcClient);

    //��������
    CImage ImageBuffer;
    ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

    //���� DC
    CImageDC BufferDC(ImageBuffer);
    CDC * pDCBuffer=CDC::FromHandle(BufferDC);

    //���û���
    pDCBuffer->SetBkMode(TRANSPARENT);
    pDCBuffer->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

    //�滭����
    pDCBuffer->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

    CRect rcListControl;
	m_ListView.GetWindowRect(&rcListControl);
    //ת��λ��
	ScreenToClient(&rcListControl);
	
    pDCBuffer->Draw3dRect(rcListControl.left-2,rcListControl.top-2,rcListControl.Width()+3,rcListControl.Height()+3,RGB(84,165,213),RGB(84,165,213));

    //�滭����
    pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pDCBuffer,0,0,SRCCOPY);
    
    
	return TRUE;
}
//�ؼ���ɫ
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

//���캯��
CDlgInsureSave::CDlgInsureSave() : CDialog(IDD_INSURE_SAVE)
{
	//���ñ���
	m_bSwitchIng=false;

	return;
}

//��������
CDlgInsureSave::~CDlgInsureSave()
{
}

//�ؼ���
VOID CDlgInsureSave::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//��ť�ؼ�
	DDX_Control(pDX, IDC_BT_TAKE_SCORE, m_btTakeScore);
	DDX_Control(pDX, IDC_BT_SAVE_SCORE, m_btSaveScore);

	//�ؼ�����
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

//��������
BOOL CDlgInsureSave::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�
	m_edScore.LimitText(16L);

	//���ð�ť
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	m_btSaveScore.SetButtonImage(IDB_BT_SAVE_SCORE, hResInstance, false, false);
	m_btTakeScore.SetButtonImage(IDB_BT_TAKE_SCORE, hResInstance, false, false);

	// �ؼ�λ���ƶ�
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

//ȷ����ť
VOID CDlgInsureSave::OnOK()
{
	return;
}

//ȡ����ť
VOID CDlgInsureSave::OnCancel()
{
	return;
}

//������Ϣ
VOID CDlgInsureSave::OnEnChangeScore()
{
	//״̬����
	if (m_bSwitchIng==false)
	{
		//���ñ���
		m_bSwitchIng=true;

		//��������
		CWnd * pWndScore=GetDlgItem(IDC_SCORE);
		SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);
		SCORE lMaxScore=__max(m_pUserInsureInfo->lUserScore, m_pUserInsureInfo->lUserInsure);
		if((m_pUserInsureInfo->lUserScore==0) || (m_pUserInsureInfo->lUserInsure==0))
			lMaxScore=__max(m_pUserInsureInfo->lUserScore, m_pUserInsureInfo->lUserInsure);

		//��������
		if(lScore > lMaxScore) lScore = lMaxScore;

		//���ý���
		if (lScore>0L)
		{
			//ת����ֵ
			TCHAR szString[128]=TEXT("");
			m_DlgInsureItem.SwitchScoreString(lScore,szString,CountArray(szString));

			//ת������
			TCHAR szControl[128]=TEXT("");
			m_DlgInsureItem.SwitchScoreFormat(lScore,3L,szControl,CountArray(szControl));

			//���ÿؼ�
			SetDlgItemText(IDC_SCORE,szControl);
			SetDlgItemText(IDC_SCORE_STRING,szString);

			//���ù��
			INT nScoreLength=m_edScore.GetWindowTextLength();
			m_edScore.SetSel(nScoreLength,nScoreLength,FALSE);
		}
		else
		{
			//���ÿؼ�
			SetDlgItemText(IDC_SCORE,TEXT(""));
			SetDlgItemText(IDC_SCORE_STRING,TEXT(""));
		}

		//���ñ���
		m_bSwitchIng=false;
	}

	return;
}

//ȡ�ť
VOID CDlgInsureSave::OnBnClickedTakeScore()
{
	//��ȡ����
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lTakeScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	//��ȡ����
//	TCHAR szPassword[LEN_PASSWORD];
//	m_PasswordControl.GetUserPassword(szPassword);

	//��������
//	TCHAR szInsurePass[LEN_MD5];
//	CWHEncrypt::MD5Encrypt(szPassword,szInsurePass);

	//�����ж�
	if (lTakeScore<=0L)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("ȡ����������������Ϊ�գ�������������Ϸ��������"),MB_ICONERROR,30);

		//���ý���
		m_edScore.SetFocus();

		return;
	}

	//��Ŀ�ж�
	if (lTakeScore>m_pUserInsureInfo->lUserInsure)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("�����չ����ӵ���Ŀ���㣬������������Ϸ��������"),MB_ICONERROR,30);

		//���ý���
		m_edScore.SetFocus();

		return;
	}

	//�����ж�
//	if (szPassword[0]==0)
//	{
		//��ʾ��Ϣ
//		CInformation Information(this);
//		Information.ShowMessageBox(TEXT("���չ����벻��Ϊ�գ����������뱣�չ����룡"),MB_ICONERROR,30);

		//���ý���
//		m_PasswordControl.SetFocus();

//		return;
//	}
	
	//�¼�֪ͨ
	m_pDlgInsureMain->OnMissionStart();
	m_pDlgInsureMain->PerformTakeScore(lTakeScore,m_pDlgInsureMain->m_szInsurePass);

	return;
}

//��ť
VOID CDlgInsureSave::OnBnClickedSaveScore()
{
	//��������
	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
	SCORE lSaveScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

	//�����ж�
	if (lSaveScore<=0L)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("�����������������Ϊ�գ�������������Ϸ��������"),MB_ICONERROR,30);

		//���ý���
		m_edScore.SetFocus();

		return;
	}

	//��Ŀ�ж�
	if (lSaveScore>m_pUserInsureInfo->lUserScore)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("����Я�����ӵ���Ŀ���㣬������������Ϸ��������"),MB_ICONERROR,30);

		//���ý���
		m_edScore.SetFocus();

		return;
	}

	//ִ�д��
	m_pDlgInsureMain->OnMissionStart();
	m_pDlgInsureMain->PerformSaveScore(lSaveScore);

	return;
}

//��������
VOID CDlgInsureSave::OnBnClickedForgetInsure()
{
	//�����ַ
	TCHAR szLogonLink[256]=TEXT("");
	_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("%s/LogonLink4.aspx"),szPlatformLink);

	//��ҳ��
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

//�滭����
BOOL CDlgInsureSave::OnEraseBkgnd(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//���� DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pDCBuffer=CDC::FromHandle(BufferDC);

	// ������Դ
	CPngImage ImageInsureSave;
	ImageInsureSave.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_SAVE"));

	//���û���
//	pDCBuffer->SetBkMode(TRANSPARENT);
//	pDCBuffer->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//�滭����
	pDCBuffer->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	// ����ͼƬ�����Ʊ���
	ImageInsureSave.DrawImage(pDCBuffer, 0, 0, ImageInsureSave.GetWidth(), ImageInsureSave.GetHeight(), 0, 0);

	//�滭����
	if (m_pUserInsureInfo)
	{
		m_DlgInsureItem.DrawNumberString(pDCBuffer,m_pUserInsureInfo->lUserInsure, 99, 18L);
		m_DlgInsureItem.DrawNumberString(pDCBuffer,m_pUserInsureInfo->lUserScore, 99, 46L);
	}

	//��ȡλ��
//	CRect rcScore;
//	m_edScore.GetWindowRect(&rcScore);

	//ת��λ��
//	ScreenToClient(&rcScore);

	//�滭����
//	COLORREF crColorBorad=CSkinEdit::m_SkinAttribute.m_crEnableBorad;
//	pDCBuffer->Draw3dRect(rcScore.left-6,rcScore.top-5,rcScore.Width()+12,24L,crColorBorad,crColorBorad);

	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pDCBuffer,0,0,SRCCOPY);

	return TRUE;
}

//�ؼ���ɫ
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

//���캯��
CDlgInsureTransfer::CDlgInsureTransfer() : CDialog(IDD_INSURE_TRANSFER)
{
	//���ñ���
	m_bSwitchIng=false;

	return;
}

//��������
CDlgInsureTransfer::~CDlgInsureTransfer()
{
}

// ������������ǳ�
VOID CDlgInsureTransfer::SetUserNickName(LPCTSTR lpszNickName)
{
	SetDlgItemText(IDC_TRANSFER_NICK_NAME, lpszNickName);
}

//�ؼ���
VOID CDlgInsureTransfer::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�ؼ�����
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

//��������
BOOL CDlgInsureTransfer::OnInitDialog()
{
	__super::OnInitDialog();

	//���ÿؼ�
	m_edScore.LimitText(16L);
	m_edNickName.LimitText(LEN_NICKNAME-1);

	// �ƶ��ؼ�λ��
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

	// ���ÿؼ���ɫ
//	m_edNickName.SetEnableColor( RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255) );
//	m_edScore.SetEnableColor( RGB(10, 10, 10), RGB(255,255,255), RGB(255,255,255) );

	//���ð�ť
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	m_btTransferScore.SetButtonImage(IDB_BT_TRANSFER_SCORE, hResInstance, false, false);
//	((CButton *)GetDlgItem(IDC_BY_NAME))->SetCheck(1);

	CRect rcBtTransferScore;
	m_btTransferScore.GetClientRect(rcBtTransferScore);
	m_btTransferScore.MoveWindow(149, 168, rcBtTransferScore.Width(), rcBtTransferScore.Height());

	// CSkinControl�����޸���Ч
	m_hlOneMillion.SetUnderLine(true);
	m_hlFiveMillion.SetUnderLine(true);
	m_hlTenMillion.SetUnderLine(true);
	m_hlFiftyMillion.SetUnderLine(true);
	m_hlOneHundredMillion.SetUnderLine(true);
	m_hlFiveHundredMillion.SetUnderLine(true);

	return FALSE;
}

//ȷ����ť
VOID CDlgInsureTransfer::OnOK()
{
	//ת�˲���
	OnBnClickedTransferScore();

	return;
}

//ȡ����ť
VOID CDlgInsureTransfer::OnCancel()
{
	return;
}

//������Ϣ
VOID CDlgInsureTransfer::OnEnChangeScore()
{
	//״̬����
	if (m_bSwitchIng==false)
	{
		//���ñ���
		m_bSwitchIng=true;

		//��������
		CWnd * pWndScore=GetDlgItem(IDC_SCORE);
		SCORE lScore=m_DlgInsureItem.GetUserControlScore(pWndScore);

		//��������
		if(lScore > m_pUserInsureInfo->lUserInsure) lScore = m_pUserInsureInfo->lUserInsure;

		//���ý���
		if (lScore>0L)
		{
			//ת����ֵ
			TCHAR szString[128]=TEXT("");
			m_DlgInsureItem.SwitchScoreString(lScore,szString,CountArray(szString));

			//ת������
			TCHAR szControl[128]=TEXT("");
			m_DlgInsureItem.SwitchScoreFormat(lScore,3L,szControl,CountArray(szControl));

			//���ÿؼ�
			SetDlgItemText(IDC_SCORE,szControl);
			SetDlgItemText(IDC_SCORE_STRING,szString);

			//���ù��
			INT nScoreLength=m_edScore.GetWindowTextLength();
			m_edScore.SetSel(nScoreLength,nScoreLength,FALSE);
		}
		else
		{
			//���ÿؼ�
			SetDlgItemText(IDC_SCORE,TEXT(""));
			SetDlgItemText(IDC_SCORE_STRING,TEXT(""));
		}

		//���ñ���
		m_bSwitchIng=false;
	}

	return;
}


//��������
VOID CDlgInsureTransfer::OnBnClickedForgetInsure()
{
	//�����ַ
	TCHAR szLogonLink[256]=TEXT("");
	_sntprintf(szLogonLink,CountArray(szLogonLink),TEXT("%s/LogonLink4.aspx"),szPlatformLink);

	//��ҳ��
	ShellExecute(NULL,TEXT("OPEN"),szLogonLink,NULL,NULL,SW_NORMAL);

	return;
}

//ת�˰�ť
VOID CDlgInsureTransfer::OnBnClickedTransferScore()
{
	//�����ж�
 	if (m_pUserInsureInfo->lUserInsure < m_pUserInsureInfo->lTransferPrerequisite)
 	{
 		//��ʾ��Ϣ
 		TCHAR szTipInfo[128]={0};
 		_sntprintf(szTipInfo,CountArray(szTipInfo),TEXT("�����չ����ӵ���Ŀ�������ת������ %I64d���޷�����ת�˲�����"),m_pUserInsureInfo->lTransferPrerequisite);
 		CInformation Information(this);
 		Information.ShowMessageBox(szTipInfo,MB_ICONERROR,30);
 
 		return;
 	}
 
 	//��ȡ����
 	CWnd * pWndScore=GetDlgItem(IDC_SCORE);
 	SCORE lTransferScore=m_DlgInsureItem.GetUserControlScore(pWndScore);
 
 	//��ȡ����
 	TCHAR szPassword[LEN_PASSWORD];
 //	m_PasswordControl.GetUserPassword(szPassword);
 
 	//��������
 	TCHAR szInsurePass[LEN_MD5];
 //	CWHEncrypt::MD5Encrypt(szPassword,szInsurePass);
 
 	//��ȡ�ǳ�
 	CString strNickName;
 	GetDlgItemText(IDC_NICKNAME,strNickName);
 
 	//��ʽ�ǳ�
 	strNickName.TrimLeft();
 	strNickName.TrimRight();
 
 	//�ǳ��ж�
 	if (strNickName.GetLength()==0)
 	{
 		//��ʾ��Ϣ
 		CInformation Information(this);
 		if(((CButton *)GetDlgItem(IDC_BY_NAME))->GetCheck()==BST_CHECKED)
 			Information.ShowMessageBox(TEXT("������Ҫ���͵�����ǳƣ�"),MB_ICONERROR,30);
 		else
 			Information.ShowMessageBox(TEXT("������Ҫ���͵����ID��"),MB_ICONERROR,30);
 
 		//���ý���
 		m_edNickName.SetFocus();
 
 		return;
 	}
 
 	//ID���ж�
 	if(((CButton *)GetDlgItem(IDC_BY_ID))->GetCheck()==BST_CHECKED)
 	{
 		int nlen = strNickName.GetLength();
 		for(int i=0; i<nlen; i++)
 		{
 			if(strNickName[i] < '0' || strNickName[i] > '9')
 			{
 				//��ʾ��Ϣ
 				CInformation Information(this);
 				Information.ShowMessageBox(TEXT("��������ȷ�����ID��"),MB_ICONERROR,30);
 
 				//���ý���
 				m_edNickName.SetFocus();
 				return;
 			}
 		}
 	}
 
 	//�����ж�
 	if (lTransferScore<=0L)
 	{
 		//��ʾ��Ϣ
 		CInformation Information(this);
 		Information.ShowMessageBox(TEXT("���͵�������������Ϊ�գ�������������Ϸ��������"),MB_ICONERROR,30);
 
 		//���ý���
 		m_edScore.SetFocus();
 
 		return;
 	}
 
 	//��Ŀ�ж�
 	if (lTransferScore>m_pUserInsureInfo->lUserInsure)
 	{
 		//��ʾ��Ϣ
 		CInformation Information(this);
 		Information.ShowMessageBox(TEXT("�����չ����ӵ���Ŀ���㣬������������Ϸ��������"),MB_ICONERROR,30);
 
 		//���ý���
 		m_edScore.SetFocus();
 
 		return;
 	}
 
 	//��Ŀ�ж�
 	if (lTransferScore<m_pUserInsureInfo->lTransferPrerequisite)
 	{
 		//��ʾ��Ϣ
 		TCHAR szTipInfo[128]={0};
 		_sntprintf(szTipInfo,CountArray(szTipInfo),TEXT("�ӱ��չ�ת�˵�������Ŀ�������� %I64d���޷�����ת�˲�����"),m_pUserInsureInfo->lTransferPrerequisite);
 		CInformation Information(this);
 		Information.ShowMessageBox(szTipInfo,MB_ICONERROR,30);
 
 		//���ý���
 		m_edScore.SetFocus();
 
 		return;
 	}
 
 	//�����ж�
//  	if (szPassword[0]==0)
//  	{
//  		//��ʾ��Ϣ
//  		CInformation Information(this);
//  		Information.ShowMessageBox(TEXT("���չ����벻��Ϊ�գ����������뱣�չ����룡"),MB_ICONERROR,30);
//  
//  		//���ý���
//  //		m_PasswordControl.SetFocus();
//  
//  		return;
//  	}
 	
 	//�¼�֪ͨ
 	m_pDlgInsureMain->OnMissionStart();
 	m_pDlgInsureMain->PerformTransferScore(NULL, strNickName,lTransferScore,szInsurePass);
 
 	//���ý���
 //	m_PasswordControl.SetFocus();
 //	m_PasswordControl.SetUserPassword(TEXT(""));
 
 	return;
}

// GameID���봰��ʧȥ����
VOID CDlgInsureTransfer::OnKillFocusNickName()
{
 	// �����ѯ
 	// �ж�ID�Ƿ���ֵ
 	CString strNickName;
 	GetDlgItemText(IDC_NICKNAME,strNickName);
 
 	//��ʽ�ǳ�
 	strNickName.TrimLeft();
 	strNickName.TrimRight();
 
 	//�ǳ��ж�
 	if (strNickName.GetLength()==0)
 	{
 		return;
 	}
 
 	// ��ΪGameID
 	int nlen = strNickName.GetLength();
 	for(int i=0; i<nlen; i++)
 	{
 		if(strNickName[i] < '0' || strNickName[i] > '9')
 		{
 			return;
 		}
 	}

	// ���ö�ʱ�����ӳ�1����ѯ
	SetDlgItemText(IDC_TRANSFER_NICK_NAME, _T("���ڲ�ѯ����ǳ�..."));
	SetTimer(EVENT_QUERYNICKNAME, 1000, NULL);

}

// ���ֽ�����
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

//�滭����
BOOL CDlgInsureTransfer::OnEraseBkgnd(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//���� DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pDCBuffer=CDC::FromHandle(BufferDC);

	// ��ͼͼƬ
	CPngImage ImageInsureTransfer;
	ImageInsureTransfer.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_TRANSFER"));
	
	//���û���
	pDCBuffer->SetBkMode(TRANSPARENT);
	pDCBuffer->SelectObject(CSkinResourceManager::GetInstance()->GetDefaultFont());

	//�滭����
	pDCBuffer->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	// ����ͼƬ�����Ʊ���
	ImageInsureTransfer.DrawImage(pDCBuffer, 0, 0, ImageInsureTransfer.GetWidth(), ImageInsureTransfer.GetHeight(), 0, 0);

	//��ȡλ��
	CRect rcScore;
	CRect rcNickName;
	m_edScore.GetWindowRect(&rcScore);
	m_edNickName.GetWindowRect(&rcNickName);

	//ת��λ��
	ScreenToClient(&rcScore);
	ScreenToClient(&rcNickName);

	if (m_pUserInsureInfo)
	{
		m_DlgInsureItem.DrawNumberString(pDCBuffer,m_pUserInsureInfo->lUserInsure, 107, 20L);
	}

	//�滭����
//	COLORREF crColorBorad=CSkinEdit::m_SkinAttribute.m_crEnableBorad;
//	pDCBuffer->Draw3dRect(rcScore.left-6,rcScore.top-5,rcScore.Width()+12,24L,crColorBorad,crColorBorad);
//	pDCBuffer->Draw3dRect(rcNickName.left-6,rcNickName.top-5,rcNickName.Width()+12,24L,crColorBorad,crColorBorad);

	//�滭����
	pDC->BitBlt(0,0,rcClient.Width(),rcClient.Height(),pDCBuffer,0,0,SRCCOPY);

	return TRUE;
}

//�ؼ���ɫ
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

//ʱ����Ϣ
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
	GetDlgItem(IDC_TRANSFER_TYPE_TIP)->SetWindowText(TEXT("������� I D��"));
	m_edNickName.SetFocus();
	m_edNickName.SetWindowText(TEXT(""));
}

void CDlgInsureTransfer::OnBnClickedByName()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_TRANSFER_TYPE_TIP)->SetWindowText(TEXT("��������ǳƣ�"));
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

//�ؼ���
VOID CDlgInsureModifyPassword::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_INSURE_PASSWORD1, m_edInsurePassword1);
	DDX_Control(pDX, IDC_INSURE_PASSWORD2, m_edInsurePassword2);
	DDX_Control(pDX, IDC_INSURE_PASSWORD3, m_edInsurePassword3);

	DDX_Control(pDX, IDC_MODIFY_INSURE_PASSWORD, m_btModify);
}

//��Ϣ����
BOOL CDlgInsureModifyPassword::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//��������
BOOL CDlgInsureModifyPassword::OnInitDialog()
{
	__super::OnInitDialog();

	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	// ���ÿؼ�
	m_edInsurePassword1.SetDrawBorad(false);
	m_edInsurePassword2.SetDrawBorad(false);
	m_edInsurePassword3.SetDrawBorad(false);

	m_btModify.SetButtonImage(IDB_BT_MODIFY, hResInstance, false, false);

	// �ƶ��ؼ�
	m_edInsurePassword1.MoveWindow(145, 18, 230, 18);
	m_edInsurePassword2.MoveWindow(145, 47, 230, 18);
	m_edInsurePassword3.MoveWindow(145, 76, 230, 18);

	CRect rcBtModify;
	m_btModify.GetClientRect(rcBtModify);
	m_btModify.MoveWindow(182, 152, rcBtModify.Width(), rcBtModify.Height());

	return FALSE;
}

// ȷ����ť
VOID CDlgInsureModifyPassword::OnOK()
{
	return;
}

// ȡ����ť
VOID CDlgInsureModifyPassword::OnCancel()
{
	return;
}

// �޸����밴ť
VOID CDlgInsureModifyPassword::OnBnClickedModifyPassword()
{
	// ���뱾�ؼ��
	TCHAR szSrcPassword[LEN_PASSWORD] = {0};
	TCHAR szDesPassword[2][LEN_PASSWORD];

	//��ȡ��Ϣ
	m_edInsurePassword1.GetUserPassword(szSrcPassword);
	m_edInsurePassword2.GetUserPassword(szDesPassword[0]);
	m_edInsurePassword3.GetUserPassword(szDesPassword[1]);

	//�����ж�
	if (szSrcPassword[0]==0)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("ԭ�������벻��Ϊ�գ����������룡"),MB_ICONERROR,0);

		m_edInsurePassword1.SetFocus();

		return;
	}

	//�����ж�
	if (lstrcmp(szDesPassword[0],szDesPassword[1])!=0)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("������������������벻һ�£����������룡"),MB_ICONERROR,0);

		//���ý���
		m_edInsurePassword2.SetFocus();

		return;
	}

	//��������
	TCHAR szDescribe[128]=TEXT("");
	CUserItemElement* pUserItemElement=CUserItemElement::GetInstance();

	//�����ж�
	if (pUserItemElement->EfficacyPassword(szDesPassword[0],szDescribe,CountArray(szDescribe))==false)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		_sntprintf(szDescribe, CountArray(szDescribe),TEXT("���������� %ld λ���ϣ����������룡"),LEN_LESS_PASSWORD);
		Information.ShowMessageBox(szDescribe, MB_ICONERROR, 0);

		//���ý���
		m_edInsurePassword2.SetFocus();

		return;
	}

	//һ���ж�
	if (lstrcmp(szSrcPassword,szDesPassword[0])==0)
	{
		//��ʾ��Ϣ
		CInformation Information(this);
		Information.ShowMessageBox(TEXT("�����������������ԭ����һ�£����������������룡"),MB_ICONERROR,0);

		//���ý���
		m_edInsurePassword2.SetFocus();

		//��������
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

//�滭����
BOOL CDlgInsureModifyPassword::OnEraseBkgnd(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//���� DC
	CImageDC BufferDC(ImageBuffer);
	CDC * pDCBuffer=CDC::FromHandle(BufferDC);

	// ������Դ
	CPngImage ImageInsureModifyPassword;
	ImageInsureModifyPassword.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_MODIFY_PASSWORD"));

	//�滭����
	pDCBuffer->FillSolidRect(&rcClient,CSkinDialog::m_SkinAttribute.m_crBackGround);

	// ����ͼƬ�����Ʊ���
	ImageInsureModifyPassword.DrawImage(pDCBuffer, 0, 0, ImageInsureModifyPassword.GetWidth(), ImageInsureModifyPassword.GetHeight(), 0, 0);

	//�滭����
	pDC->BitBlt(0, 0, rcClient.Width(), rcClient.Height(), pDCBuffer, 0, 0, SRCCOPY);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////

TCHAR	CDlgInsureMain::m_szInsurePass[LEN_MD5] = {0};
bool	CDlgInsureMain::m_bAutoLogOff = true;

//���캯��
CDlgInsureMain::CDlgInsureMain() : CSkinDialog(IDD_INSURE_MAIN)
{
	//�������
	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=false;
    m_bTransRecordMission=false;

	//���ñ���
	m_bInitInfo=false;
	m_cbInsureMode=INSURE_PASSWORD;
	ZeroMemory(&m_UserInsureInfo,sizeof(m_UserInsureInfo));

	m_hBrush = CreateSolidBrush(RGB(194,218,230));

	m_SkinAttribute.m_crBackGround = RGB(255, 255, 255);

	return;
}

//��������
CDlgInsureMain::~CDlgInsureMain()
{
	::DeleteObject(m_hBrush);
}

//�ؼ���
VOID CDlgInsureMain::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�ؼ�����
	DDX_Control(pDX, IDC_TAB_CONTROL, m_TabControl);
//	DDX_Control(pDX, IDC_BT_QUERY_INFO, m_btQueryInfo);
}

//��Ϣ����
BOOL CDlgInsureMain::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//��������
BOOL CDlgInsureMain::OnInitDialog()
{
	__super::OnInitDialog();

	CRect rcClient;
	GetClientRect(&rcClient);

	INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
	INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
	INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
	INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;

	//���ñ���
	SetWindowText(TEXT("���չ�"));

	//������Դ
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//���ÿؼ�
	m_TabControl.SetItemSize(CSize(100,25));

	//��ȡ�ؼ�
	m_DlgInsureSave.m_pDlgInsureMain=this;
	m_DlgInsureSave.m_pUserInsureInfo=&m_UserInsureInfo;

	//ת�˿ؼ�
	m_DlgInsureTransfer.m_pDlgInsureMain=this;
	m_DlgInsureTransfer.m_pUserInsureInfo=&m_UserInsureInfo;

	// ת�˼�¼�ؼ�
	m_DlgTransRecord.m_pDlgInsureMain=this;
	m_DlgTransRecord.m_pUserInsureInfo=&m_UserInsureInfo;

	// ��������ؼ�
	m_DlgInsurePassword.m_pDlgInsureMain = this;

	// �޸���������ҳǩ
	m_DlgInsureModifyPassword.m_pDlgInsureMain = this;

	//�����ǩ
	m_TabControl.InsertItem(INSURE_SAVE,TEXT(" �����ȡ�� "));
	m_TabControl.InsertItem(INSURE_TRANSFER,TEXT(" �������� "));
	m_TabControl.InsertItem(TRANS_RECORD,TEXT(" ������־ "));
	m_TabControl.InsertItem(INSURE_MODIFY_PASSWORD,TEXT(" �޸Ŀ��� "));

	m_TabControl.MoveWindow( nLBorder-4, 36, rcClient.Width() - nLBorder - nRBorder + 8, rcClient.Height()- nTBorder - nTBorder + 36);

	if (m_szInsurePass[0]==0)
	{
		// û�����ù��������룬��Ҫ������������
		m_TabControl.ShowWindow(SW_HIDE);

		// ��������ģʽ
		SwitchInsureModeView(INSURE_PASSWORD);
	}
	else
	{
		m_TabControl.ShowWindow(SW_SHOW);
		//��ѯ��Ϣ
		QueryUserInsureInfo();

		//��ȡģʽ
		SwitchInsureModeView(INSURE_SAVE);

		GetDlgItem(IDC_CHECK_AUTO_LOGOFF)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_AUTO_LOGOFF)->SetWindowPos(NULL, 340, 42, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	}
	((CButton*)GetDlgItem(IDC_CHECK_AUTO_LOGOFF))->SetCheck(m_bAutoLogOff);

	return FALSE;
}

//ȷ����ť
VOID CDlgInsureMain::OnOK()
{
	//Ĭ�϶���
	switch (m_cbInsureMode)
	{
	case INSURE_SAVE:		//��ȡ��ʽ
		{
			m_DlgInsureSave.OnOK();
			break;
		}
	case INSURE_TRANSFER:	//ת�˷�ʽ
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

// ���ٴ���
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

//�滭��Ϣ
VOID CDlgInsureMain::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight)
{
	// ��ȡ��ǰ��ʾ�ı�ǩ������ˢ��
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
	//��ȡλ��
	CRect rcTabControl;
	m_TabControl.GetWindowRect(&rcTabControl);

	//ת��λ��
	ScreenToClient(&rcTabControl);

	//�滭����
	INT nLBorder=m_SkinAttribute.m_EncircleInfoView.nLBorder;
	INT nRBorder=m_SkinAttribute.m_EncircleInfoView.nRBorder;
	pDC->FillSolidRect(nLBorder,rcTabControl.bottom-1L,nWidth-nLBorder-nRBorder,1,RGB(145,168,183));

	//������Դ
//	CPngImage ImageInsureInfo;
//	ImageInsureInfo.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INFO_FRAME"));

	//�滭��Ϣ
//	INT nYPosInfo=75L;
//	INT nXPosInfo=(nWidth-ImageInsureInfo.GetWidth())/2L;
//	ImageInsureInfo.DrawImage(pDC,nXPosInfo,nYPosInfo,ImageInsureInfo.GetWidth(),ImageInsureInfo.GetHeight(),0,0);

	//������Ϣ
	if (m_bInitInfo==true)
	{
		//������Դ
		CPngImage ImageInsureIcon;
		ImageInsureIcon.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_ICON"));

		//�滭��־
		ImageInsureIcon.DrawImage(pDC,nXPosInfo+10,nYPosInfo+17);

		//�滭����
		m_DlgInsureItem.DrawNumberString(pDC,m_UserInsureInfo.lUserScore,nXPosInfo+136,nYPosInfo+18L);
		m_DlgInsureItem.DrawNumberString(pDC,m_UserInsureInfo.lUserInsure,nXPosInfo+136,nYPosInfo+53L);

		//�滭��ʾ
		LPCTSTR pszScoreString=TEXT("���ӣ�");
		LPCTSTR pszInsureString=TEXT("���չ�");
		pDC->TextOut(nXPosInfo+83,nYPosInfo+20L,pszScoreString,lstrlen(pszScoreString));
		pDC->TextOut(nXPosInfo+83,nYPosInfo+55L,pszInsureString,lstrlen(pszInsureString));

    		//������ʾ
    /*		TCHAR szString[128]=TEXT("");
    		_sntprintf(szString,CountArray(szString),TEXT("��ܰ��ʾ��������Ϸ���������ѣ�ȡ�����۳�%ld��,���ͽ��۳�%ld���������"),m_UserInsureInfo.wRevenueTake,m_UserInsureInfo.wRevenueTransfer);

    		//��ܰ��ʾ
    		pDC->SetTextColor(RGB(50,50,50));
    		pDC->TextOut(18,nHeight-28,szString,lstrlen(szString));*/
	}
	else
	{
	}
#endif
	return;
}

//��ʼ����
VOID CDlgInsureMain::OnMissionStart()
{
	return;
}

//��ֹ����
VOID CDlgInsureMain::OnMissionConclude()
{
	return;
}

//��ѯ����
VOID CDlgInsureMain::QueryUserInsureInfo()
{
	//���ñ���
	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=true;
	m_bTransferMission=false;
	m_bTransRecordMission = false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = false; 

	//�¼�����
	OnMissionStart();

	//ִ�в�ѯ
	PerformQueryInfo();

	return;
}

//����ģʽ
VOID CDlgInsureMain::SwitchInsureModeView(BYTE cbInsureMode)
{
	switch (cbInsureMode)
	{
	case INSURE_SAVE:				//��ȡ��ʽ
		{
			//���ñ���
			m_cbInsureMode=INSURE_SAVE;

			//���ÿؼ�
			if (m_DlgInsureSave.m_hWnd==NULL)
			{
				//��ȡλ��
				CRect rcClient;
				GetClientRect(&rcClient);

				//������Դ
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//�����ؼ�
				m_DlgInsureSave.Create(IDD_INSURE_SAVE, &m_TabControl);

				//������Դ
				AfxSetResourceHandle(GetModuleHandle(NULL));

				//�ƶ�λ��
				INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
				INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
				INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
				INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
				m_DlgInsureSave.SetWindowPos(NULL,nLBorder,nTBorder, 439, 229, SWP_NOZORDER|SWP_SHOWWINDOW);
			}
			else
			{
				//��ʾ����
				m_DlgInsureSave.ShowWindow(SW_SHOW);
			}

			//���ÿؼ�
			if (m_DlgInsureTransfer.m_hWnd!=NULL) m_DlgInsureTransfer.ShowWindow(SW_HIDE);
			if (m_DlgTransRecord.m_hWnd!=NULL) m_DlgTransRecord.ShowWindow(SW_HIDE);
			if (m_DlgInsurePassword.GetSafeHwnd()!=NULL) m_DlgInsurePassword.ShowWindow(SW_HIDE);
			if (m_DlgInsureModifyPassword.GetSafeHwnd() != NULL) m_DlgInsureModifyPassword.ShowWindow(SW_HIDE);
			//���ý���
			m_DlgInsureSave.SetFocus();

			//����ѡ��
			m_TabControl.SetCurSel(m_cbInsureMode);

			break;
		}
	case INSURE_TRANSFER:			//ת�˷�ʽ
		{
			//���ñ���
			m_cbInsureMode=INSURE_TRANSFER;

			//���ÿؼ�
			if (m_DlgInsureTransfer.m_hWnd==NULL)
			{
				//��ȡλ��
				CRect rcClient;
				GetClientRect(&rcClient);

				//������Դ
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//�����ؼ�
				m_DlgInsureTransfer.Create(IDD_INSURE_TRANSFER, &m_TabControl);

				//������Դ
				AfxSetResourceHandle(GetModuleHandle(NULL));

				//�ƶ�λ��
				INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
				INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
				INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
				INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
				m_DlgInsureTransfer.SetWindowPos(NULL,nLBorder,nTBorder, 439, 229, SWP_NOZORDER|SWP_SHOWWINDOW);
			}
			else
			{
				//��ʾ����
				m_DlgInsureTransfer.ShowWindow(SW_SHOW);
			}

			//���ÿؼ�
			if (m_DlgInsureSave.m_hWnd!=NULL) m_DlgInsureSave.ShowWindow(SW_HIDE);
			if (m_DlgTransRecord.m_hWnd!=NULL) m_DlgTransRecord.ShowWindow(SW_HIDE);
			if (m_DlgInsurePassword.GetSafeHwnd()!=NULL) m_DlgInsurePassword.ShowWindow(SW_HIDE);
			if (m_DlgInsureModifyPassword.GetSafeHwnd() != NULL) m_DlgInsureModifyPassword.ShowWindow(SW_HIDE);
			//���ý���
			m_DlgInsureTransfer.SetFocus();

			//����ѡ��
			m_TabControl.SetCurSel(m_cbInsureMode);

			break;
		}
	case TRANS_RECORD:				// ת����ʷ��¼
		{
			m_cbInsureMode=TRANS_RECORD;
			//���ÿؼ�
			if (m_DlgTransRecord.m_hWnd==NULL)
			{
				//��ȡλ��
				CRect rcClient;
				GetClientRect(&rcClient);

				//������Դ
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//�����ؼ�
				m_DlgTransRecord.Create(IDD_TRNASRECORD_DLG,&m_TabControl);

				//������Դ
				AfxSetResourceHandle(GetModuleHandle(NULL));

				//�ƶ�λ��
				INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
				INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
				INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
				INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
				m_DlgTransRecord.SetWindowPos(NULL, 1, nTBorder, 476, 300, SWP_NOZORDER|SWP_SHOWWINDOW);
			}
			else
			{
				//��ʾ����
				m_DlgTransRecord.ShowWindow(SW_SHOW);
			}

			//���ÿؼ�
			if (m_DlgInsureSave.m_hWnd!=NULL) m_DlgInsureSave.ShowWindow(SW_HIDE);
			if (m_DlgInsureTransfer.m_hWnd!=NULL) m_DlgInsureTransfer.ShowWindow(SW_HIDE);
			if (m_DlgInsurePassword.GetSafeHwnd()!=NULL) m_DlgInsurePassword.ShowWindow(SW_HIDE);
			if (m_DlgInsureModifyPassword.GetSafeHwnd() != NULL) m_DlgInsureModifyPassword.ShowWindow(SW_HIDE);
           
			//���ý���
			m_DlgTransRecord.SetFocus();

			//����ѡ��
			m_TabControl.SetCurSel(m_cbInsureMode);
			break;
		}	
	case INSURE_PASSWORD:			// ��֤��������
		{
			//���ÿؼ�
			if (m_DlgInsurePassword.GetSafeHwnd()==NULL)
			{
				//��ȡλ��
				CRect rcClient;
				GetClientRect(&rcClient);

				//������Դ
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//�����ؼ�
				m_DlgInsurePassword.Create(IDD_INSURE_PASSWORD, this);

				//������Դ
				AfxSetResourceHandle(GetModuleHandle(NULL));

				//�ƶ�λ��
				INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
				INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
				INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
				INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
				m_DlgInsurePassword.SetWindowPos(NULL, nLBorder, nTBorder, 439, 260, SWP_NOZORDER|SWP_SHOWWINDOW);
			}
			else
			{
				//��ʾ����
				m_DlgInsurePassword.ShowWindow(SW_SHOW);
			}

			//���ÿؼ�
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
	case INSURE_MODIFY_PASSWORD:	//	�޸���������
		{
			//���ñ���
			m_cbInsureMode=INSURE_MODIFY_PASSWORD;

			//���ÿؼ�
			if (m_DlgInsureModifyPassword.m_hWnd==NULL)
			{
				//��ȡλ��
				CRect rcClient;
				GetClientRect(&rcClient);

				//������Դ
				AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

				//�����ؼ�
				m_DlgInsureModifyPassword.Create(IDD_INSURE_MODIFY_PASSWORD, &m_TabControl);

				//������Դ
				AfxSetResourceHandle(GetModuleHandle(NULL));

				//�ƶ�λ��
				INT nLBorder=m_SkinAttribute.m_EncircleInfoFrame.nLBorder;
				INT nRBorder=m_SkinAttribute.m_EncircleInfoFrame.nRBorder;
				INT nTBorder=m_SkinAttribute.m_EncircleInfoFrame.nTBorder;
				INT nBBorder=m_SkinAttribute.m_EncircleInfoFrame.nBBorder;
				m_DlgInsureModifyPassword.SetWindowPos(NULL,nLBorder,nTBorder, 439, 229, SWP_NOZORDER|SWP_SHOWWINDOW);
			}
			else
			{
				//��ʾ����
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

//��ѯ��ť
VOID CDlgInsureMain::OnBnClickedQueryInfo()
{
	//��ѯ��Ϣ
	QueryUserInsureInfo();

	//���ý���
	switch (m_cbInsureMode)
	{
	case INSURE_SAVE:		//��ȡ��ʽ
		{
			m_DlgInsureSave.SetFocus();
			break;
		}
	case INSURE_TRANSFER:	//ת�˷�ʽ
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

//���͸ı�
VOID CDlgInsureMain::OnTcnSelchangeInsureMode(NMHDR * pNMHDR, LRESULT * pResult)
{
	switch (m_TabControl.GetCurSel())
	{
	case INSURE_SAVE:				//	��ȡ��ʽ
		{
			SwitchInsureModeView(INSURE_SAVE);
			break;
		}
	case INSURE_TRANSFER:			//	ת�˷�ʽ
		{
			SwitchInsureModeView(INSURE_TRANSFER);
			break;
		}
	case TRANS_RECORD:				//	ת����ʷ��¼
		{
			SwitchInsureModeView(TRANS_RECORD);
			break;
		}
	case INSURE_MODIFY_PASSWORD:	//	�ı�����
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

// �ı�ؼ���ɫ
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

//���캯��
CDlgInsurePlaza::CDlgInsurePlaza()
{
	//��������
	m_MissionManager.InsertMissionItem(this);

	//���ñ���
	m_lScore=0L;
	ZeroMemory(m_szNickName,sizeof(m_szNickName));

	m_dwGameID = NULL;

	return;
}

//��������
CDlgInsurePlaza::~CDlgInsurePlaza()
{
}

//��ѯ�¼�
VOID CDlgInsurePlaza::PerformQueryInfo()
{
	//��������
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}

//�����¼�
VOID CDlgInsurePlaza::PerformSaveScore(SCORE lSaveScore)
{
	//���ñ���
	m_bSaveMission=true;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
    m_bTransRecordMission=false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = false;

	//���ñ���
	m_lScore=lSaveScore;

	//��������
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}

//ȡ���¼�
VOID CDlgInsurePlaza::PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass)
{
	//���ñ���
	m_bTakeMission=true;
	m_bSaveMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
    m_bTransRecordMission=false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = false;

	//���ñ���
	m_lScore=lTakeScore;
//	lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));

	//��������
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}

//ת���¼�
VOID CDlgInsurePlaza::PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass)
{
	//���ñ���
	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bTransferMission=true;
    m_bTransRecordMission=false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = false;

	//���ñ���
	m_cbByNickName=cbByNickName;
	m_lScore=lTransferScore;
	lstrcpyn(m_szNickName,pszNickName,CountArray(m_szNickName));
//	lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));

	//��������
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}

	return;
}
//ת�˼�¼
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

// �������GameID��ѯ�ǳ�
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

// ������������
VOID CDlgInsurePlaza::PerformInsurePassword(LPCTSTR pszInsurePass)
{
	//���ñ���
	m_bTakeMission=false;
	m_bSaveMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
	m_bTransRecordMission=false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = true;

	//���ñ���
	lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}
}

// �޸���������
VOID CDlgInsurePlaza::PerformModiyInsurePassword(LPCTSTR pszSrcPassword, LPCTSTR pszDesPassword)
{
	m_bSaveMission = false;						//�������
	m_bTakeMission = false;						//ȡ������
	m_bQueryMission = false;					//��ѯ����
	m_bTransferMission = false;					//ת������
	m_bTransRecordMission = false;              //ת�˼�¼
	m_bQueryNickNameMission = false;			// ��ѯ�ǳ�
	m_bVerifyInsurePassword = false;			// �����������룬���м��
	m_bModifyInsurePassword = true;				// �޸���������

	lstrcpyn(m_szSrcInsurePass, pszSrcPassword, CountArray(m_szSrcInsurePass));
	lstrcpyn(m_szDesInsurePass, pszDesPassword, CountArray(m_szDesInsurePass));

	//���ñ���
	if (m_MissionManager.AvtiveMissionItem(this,false)==false)  
	{
		OnMissionConclude();
		return;
	}
}


//�����¼�
bool CDlgInsurePlaza::OnEventMissionLink(INT nErrorCode)
{
	//���ӽ��
	if (nErrorCode!=0)
	{
		//��������
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//�¼�����
		OnMissionConclude();

		//������ʾ
		if (m_bSaveMission==true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("���ӷ�������ʱ�����Ӵ���ʧ�ܣ����Ժ��ٴ����ԣ�"),MB_OK|MB_ICONSTOP,30);
		}

		//������ʾ
		if (m_bTakeMission==true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("���ӷ�������ʱ������ȡ��ʧ�ܣ����Ժ��ٴ����ԣ�"),MB_OK|MB_ICONSTOP,30);
		}

		//������ʾ
		if (m_bQueryMission==true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("���ӷ�������ʱ�����չ����ϻ�ȡʧ�ܣ�"),MB_OK|MB_ICONSTOP,30);
		}

		//������ʾ
		if (m_bTransferMission==true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("���ӷ�������ʱ����������ʧ�ܣ����Ժ��ٴ����ԣ�"),MB_OK|MB_ICONSTOP,30);
		}

		// ��������
		if (m_bVerifyInsurePassword == true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("���ӷ�������ʱ����֤��������ʧ�ܣ����Ժ��ٴ����ԣ�"),MB_OK|MB_ICONSTOP,30);
		}

		// �޸���������
		if (m_bModifyInsurePassword == true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("���ӷ�������ʱ���޸���������ʧ�ܣ����Ժ��ٴ����ԣ�"),MB_OK|MB_ICONSTOP,30);
		}
		
		return true;
	}

	//��������
	if (m_bSaveMission==true)
	{
		//��������
		CMD_GP_UserSaveScore UserSaveScore;
		ZeroMemory(&UserSaveScore,sizeof(UserSaveScore));

		//��������
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//���ñ���
		UserSaveScore.lSaveScore=m_lScore;
		UserSaveScore.dwUserID=pGlobalUserData->dwUserID;
		CWHService::GetMachineIDEx(UserSaveScore.szMachineID);

		//��������
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_USER_SAVE_SCORE,&UserSaveScore,sizeof(UserSaveScore));

		return true;
	}

	//ȡ������
	if (m_bTakeMission==true)
	{
		//��������
		CMD_GP_UserTakeScore UserTakeScore;
		ZeroMemory(&UserTakeScore,sizeof(UserTakeScore));

		//��������
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//���ñ���
		UserTakeScore.lTakeScore=m_lScore;
		UserTakeScore.dwUserID=pGlobalUserData->dwUserID;
		CWHService::GetMachineIDEx(UserTakeScore.szMachineID);
		lstrcpyn(UserTakeScore.szPassword,m_szInsurePass,CountArray(UserTakeScore.szPassword));

		//��������
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_USER_TAKE_SCORE,&UserTakeScore,sizeof(UserTakeScore));

		return true;
	}

	//ת������
	if (m_bTransferMission==true)
	{
		//��������
		CMD_GP_QueryUserInfoRequest QueryUserInfoRequest;
		ZeroMemory(&QueryUserInfoRequest,sizeof(QueryUserInfoRequest));

		//���ñ���
		QueryUserInfoRequest.cbByNickName=m_cbByNickName;
		lstrcpyn(QueryUserInfoRequest.szNickName,m_szNickName,CountArray(QueryUserInfoRequest.szNickName));

		//��������
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_USER_INFO_REQUEST,&QueryUserInfoRequest,sizeof(QueryUserInfoRequest));
	}

	//��ѯ����
	if (m_bQueryMission==true)
	{
		//��������
		CMD_GP_QueryInsureInfo QueryInsureInfo;
		ZeroMemory(&QueryInsureInfo,sizeof(QueryInsureInfo));

		//���ñ���
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		QueryInsureInfo.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;

		//��������
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_INSURE_INFO,&QueryInsureInfo,sizeof(QueryInsureInfo));
	}

    //ת�˼�¼
    if(m_bTransRecordMission==true)
    {
        CMD_GP_QueryInsureInfo QueryTransRecord;
        ZeroMemory(&QueryTransRecord,sizeof(QueryTransRecord));
        
        CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
        QueryTransRecord.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
        //��������
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_TRANSRECORD,&QueryTransRecord,sizeof(QueryTransRecord));
        
    }

	// ��ѯת���û��ǳ�
	if (m_bQueryNickNameMission == true)
	{
		CMD_GP_QueryNickNameByGameID QueryNickNameByGameID;
		ZeroMemory(&QueryNickNameByGameID,sizeof(QueryNickNameByGameID));

		QueryNickNameByGameID.dwGameID=m_dwGameID;
		//��������
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_QUERY_NICKNAME_BY_GAMEID,&QueryNickNameByGameID,sizeof(QueryNickNameByGameID));
	}

	// ��֤��������
	if (m_bVerifyInsurePassword == true)
	{
		// ��������������Ϣ
 		CMD_GP_VerifyUserInsurePassword VerifyUserInsurePassword;
 		ZeroMemory(&VerifyUserInsurePassword,sizeof(VerifyUserInsurePassword));
 
 		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
 		VerifyUserInsurePassword.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
 		lstrcpyn(VerifyUserInsurePassword.szInsurePassword,m_szInsurePass,CountArray(VerifyUserInsurePassword.szInsurePassword));
 
 		//��������
 		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_VERIFY_INSURE_PASSWORD,&VerifyUserInsurePassword,sizeof(VerifyUserInsurePassword));
	}

	// �޸���������
	if (m_bModifyInsurePassword == true)
	{
		//��������
		CMD_GP_ModifyInsurePass ModifyInsurePass;
		ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

		//��������
		TCHAR szSrcPassword[LEN_PASSWORD]=TEXT("");
		TCHAR szDesPassword[LEN_PASSWORD]=TEXT("");
		CWHEncrypt::MD5Encrypt(m_szSrcInsurePass, szSrcPassword);
		CWHEncrypt::MD5Encrypt(m_szDesInsurePass, szDesPassword);

		//��������
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		ModifyInsurePass.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
		lstrcpyn(ModifyInsurePass.szScrPassword,szSrcPassword,CountArray(ModifyInsurePass.szScrPassword));
		lstrcpyn(ModifyInsurePass.szDesPassword,szDesPassword,CountArray(ModifyInsurePass.szDesPassword));

		//��������
		m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_MODIFY_INSURE_PASS,&ModifyInsurePass,sizeof(ModifyInsurePass));
	}
	return true;
}

//�ر��¼�
bool CDlgInsurePlaza::OnEventMissionShut(BYTE cbShutReason)
{
	//��ʾ��Ϣ
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//��������
		if (m_MissionManager.AvtiveMissionItem(this,true)==true)
		{
			return true;
		}

		//��������
		if (m_bSaveMission==true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("��������������쳣�Ͽ��ˣ����Ӵ���ʧ�ܣ�"),MB_ICONERROR,30);
		}

		//ȡ������
		if (m_bTakeMission==true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("��������������쳣�Ͽ��ˣ�����ȡ��ʧ�ܣ�"),MB_ICONERROR,30);
		}

		//��ѯ����
		if (m_bQueryMission==true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("��������������쳣�Ͽ��ˣ����չ����ϻ�ȡʧ�ܣ�"),MB_ICONERROR,30);
		}

		// ��֤��������
		if (m_bVerifyInsurePassword==true)
		{
			//��ʾ��Ϣ
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("��������������쳣�Ͽ��ˣ���֤��������ʧ�ܣ�"),MB_ICONERROR,30);
		}
	}

	//�¼�����
	OnMissionConclude();

	return true;
}

//��ȡ�¼�
bool CDlgInsurePlaza::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//�����
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
		case SUB_GP_USER_INSURE_INFO:		//��������
			{
				//Ч�����
				ASSERT(wDataSize==sizeof(CMD_GP_UserInsureInfo));
				if (wDataSize<sizeof(CMD_GP_UserInsureInfo)) return false;

				//��������
				CMD_GP_UserInsureInfo * pUserInsureInfo=(CMD_GP_UserInsureInfo *)pData;

				//���ñ���
				m_bInitInfo=true;
				m_UserInsureInfo.wRevenueTake=pUserInsureInfo->wRevenueTake;
				m_UserInsureInfo.wRevenueTransfer=pUserInsureInfo->wRevenueTransfer;
				m_UserInsureInfo.wServerID=pUserInsureInfo->wServerID;
				m_UserInsureInfo.lUserScore=pUserInsureInfo->lUserScore;
				m_UserInsureInfo.lUserInsure=pUserInsureInfo->lUserInsure;
				m_UserInsureInfo.lTransferPrerequisite=pUserInsureInfo->lTransferPrerequisite;

				//�ر�����
				m_MissionManager.ConcludeMissionItem(this,false);

				//���´���
				tagInsurePlazaUpdata Updata;
				Updata.lUserScore=pUserInsureInfo->lUserScore;
				Updata.lUserInsure=pUserInsureInfo->lUserInsure;
				AfxGetMainWnd()->SendMessage(WM_INSUREPLAZA_UPDATA,(WPARAM)&Updata,0);

				//���½���
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				return true;
			}
		case SUB_GP_USER_INSURE_SUCCESS:	//���и��ֲ����ɹ�(���룬ȡ��)
			{
				//Ч�����
				CMD_GP_UserInsureSuccess * pUserInsureSuccess=(CMD_GP_UserInsureSuccess *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString))) return false;

				//���ñ���
				m_UserInsureInfo.lUserScore=pUserInsureSuccess->lUserScore;
				m_UserInsureInfo.lUserInsure=pUserInsureSuccess->lUserInsure;

				//�ر�����
				m_MissionManager.ConcludeMissionItem(this,false);

				//����ؼ�
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

				//���´���
				tagInsurePlazaUpdata Updata;
				Updata.lUserScore=pUserInsureSuccess->lUserScore;
				Updata.lUserInsure=pUserInsureSuccess->lUserInsure;
				AfxGetMainWnd()->SendMessage(WM_INSUREPLAZA_UPDATA,(WPARAM)&Updata,0);

				//���½���
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				//��ʾ��Ϣ
				if (pUserInsureSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}

				if (m_bVerifyInsurePassword)
				{
					// ��֤��������׶�
					m_TabControl.ShowWindow(SW_SHOW);

					SwitchInsureModeView(INSURE_SAVE);

					GetDlgItem(IDC_CHECK_AUTO_LOGOFF)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_CHECK_AUTO_LOGOFF)->SetWindowPos(NULL, 340, 42, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
				}

				return true;
			}
		case SUB_GP_USER_INSURE_FAILURE:	//����ʧ��
			{
				//Ч�����
				CMD_GP_UserInsureFailure * pUserInsureFailure=(CMD_GP_UserInsureFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString))) return false;

				//ʧ�ܴ���
				OnMissionConclude();

				//�ر�����
				m_MissionManager.ConcludeMissionItem(this,false);

				//����ؼ�
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

				//��ʾ��Ϣ
				if (pUserInsureFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureFailure->szDescribeString,MB_ICONERROR,60);
				}

				return true;
			}
		case SUB_GP_QUERY_USER_INFO_RESULT:	//�û���Ϣ
			{
				//Ч�����
				ASSERT(wDataSize==sizeof(CMD_GP_UserTransferUserInfo));
				if (wDataSize<sizeof(CMD_GP_UserTransferUserInfo)) return false;

				//��������
				CMD_GP_UserTransferUserInfo * pTransferUserInfo=(CMD_GP_UserTransferUserInfo *)pData;

				if (m_bTransferMission == true)
				{
					//������Ϣ
					TCHAR szMessage[128]={0};
					_sntprintf(szMessage, CountArray(szMessage), TEXT("��ȷ��Ҫ��[%s], ID:%lu ����%I64d ������?"), pTransferUserInfo->szNickName, pTransferUserInfo->dwTargetGameID, m_lScore);

					//��ʾ��Ϣ
					CInformation Information(this);
					if (Information.ShowMessageBox(szMessage,MB_ICONQUESTION|MB_YESNO,0)==IDYES)
					{
						//��������
						CMD_GP_UserTransferScore UserTransferScore;
						ZeroMemory(&UserTransferScore,sizeof(UserTransferScore));

						//��������
						CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
						tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

						//���ñ���
						UserTransferScore.cbByNickName=m_cbByNickName;
						UserTransferScore.lTransferScore=m_lScore;
						UserTransferScore.dwUserID=pGlobalUserData->dwUserID;
						CWHService::GetMachineIDEx(UserTransferScore.szMachineID);
						lstrcpyn(UserTransferScore.szNickName,m_szNickName,CountArray(UserTransferScore.szNickName));
						lstrcpyn(UserTransferScore.szPassword,m_szInsurePass,CountArray(UserTransferScore.szPassword));

//						MessageBox(m_szInsurePass);
					
						//��������
						m_MissionManager.SendData(MDM_GP_USER_SERVICE,SUB_GP_USER_TRANSFER_SCORE,&UserTransferScore,sizeof(UserTransferScore));
					}
				}

				// ��ѯ�ǳ�����
				if (m_bQueryNickNameMission)
				{
					if (m_DlgInsureTransfer.GetSafeHwnd() != NULL)
					{
						m_DlgInsureTransfer.SetUserNickName(pTransferUserInfo->szNickName);
					}
				}

				return true;
			}
		case SUB_GP_OPERATE_FAILURE:		// �޸�����ʧ��
			{
				//Ч�����
				CMD_GP_OperateFailure * pOperateFailure=(CMD_GP_OperateFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateFailure)-sizeof(pOperateFailure->szDescribeString))) return false;

				//ʧ�ܴ���
				OnMissionConclude();

				//�ر�����
				m_MissionManager.ConcludeMissionItem(this,false);

				//��ʾ��Ϣ
				if (pOperateFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pOperateFailure->szDescribeString,MB_ICONERROR,60);
				}

				//���ý���
				if (m_bModifyInsurePassword == true && m_DlgInsureModifyPassword.GetSafeHwnd() != NULL)
				{
					m_DlgInsureModifyPassword.SetFocus();
				}
				return true;
			}
			break;
		case SUB_GP_OPERATE_SUCCESS:		// �޸�����ɹ�(�رնԻ���)
			{
				//��������
				CMD_GP_OperateSuccess * pOperateSuccess=(CMD_GP_OperateSuccess *)pData;

				//Ч������
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateSuccess)-sizeof(pOperateSuccess->szDescribeString))) return false;

				//�ر�����
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
		case SUB_GP_USER_TRANSFER_RECEIPT:		// ����ת�˻�ִ
			{
				// �յ���Ϸ��ִ
				//Ч�����
				ASSERT(wDataSize==sizeof(CMD_GR_UserTransferReceipt));
				if (wDataSize<sizeof(CMD_GR_UserTransferReceipt)) return false;

				CMD_GR_UserTransferReceipt * pUserTransferReceipt=(CMD_GR_UserTransferReceipt *)pData;
				m_UserInsureInfo.lUserScore=pUserTransferReceipt->lSourceUserScore;
				m_UserInsureInfo.lUserInsure=pUserTransferReceipt->lSourceUserInsure;

				//�ر�����
				m_MissionManager.ConcludeMissionItem(this,false);

				// ����ȫ������
				//��������
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
				pGlobalUserData->dwLoveLiness = pUserTransferReceipt->dwLoveliness;

				//���´���
				tagInsurePlazaUpdata Updata;
				Updata.lUserScore=pUserTransferReceipt->lSourceUserScore;
				Updata.lUserInsure=pUserTransferReceipt->lSourceUserInsure;
				AfxGetMainWnd()->SendMessage(WM_INSUREPLAZA_UPDATA,(WPARAM)&Updata,0);

				//���½���
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				// ������ִ����
				CDlgInsureTransferReceipt InsureTransferReceipt(this);
				InsureTransferReceipt.ShowReceipt(pUserTransferReceipt);

				// ��������Ϣ
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

//���캯��
CDlgInsureServer::CDlgInsureServer()
{
	//���ñ���
	m_pITCPSocket=NULL;
	m_pIMySelfUserItem=NULL;

	return;
}

//��������
CDlgInsureServer::~CDlgInsureServer()
{
}

//��ѯ�¼�
VOID CDlgInsureServer::PerformQueryInfo()
{
	//Ч�����
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	//��������
	CMD_GR_C_QueryInsureInfoRequest QueryInsureInfo;
	QueryInsureInfo.cbActivityGame=FALSE;

	//��������
	m_pITCPSocket->SendData(MDM_GR_INSURE, SUB_GR_QUERY_INSURE_INFO, &QueryInsureInfo, sizeof(QueryInsureInfo));

	return;
}

VOID CDlgInsureServer::PerformQueryTransRecord()
{
    //Ч�����
    ASSERT(m_pITCPSocket!=NULL);
    if (m_pITCPSocket==NULL) return;

    //��������
    CMD_GR_C_QueryInsureInfoRequest QueryInsureInfo;
    QueryInsureInfo.cbActivityGame=FALSE;

    //��������
    m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_QUERY_TRANSRECORD,&QueryInsureInfo,sizeof(QueryInsureInfo));
}

// ����GameID��ѯ����ǳ�
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

	//��������
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_QUERY_NICKNAME_BY_GAMEID,&QueryNickNameByGameID,sizeof(QueryNickNameByGameID));
}

// ������������
VOID CDlgInsureServer::PerformInsurePassword(LPCTSTR pszInsurePass)
{
	//���ñ���
	m_bTakeMission=false;
	m_bSaveMission=false;
	m_bQueryMission=false;
	m_bTransferMission=false;
	m_bTransRecordMission=false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = true;
	m_bModifyInsurePassword = false;

	//���ñ���
	lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));

	// ��������������Ϣ
	CMD_GR_VerifyUserInsurePassword VerifyUserInsurePassword;
	ZeroMemory(&VerifyUserInsurePassword,sizeof(VerifyUserInsurePassword));

	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	VerifyUserInsurePassword.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
	lstrcpyn(VerifyUserInsurePassword.szInsurePassword,pszInsurePass,CountArray(VerifyUserInsurePassword.szInsurePassword));

	//��������
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_VERIFY_INSURE_PASSWORD,&VerifyUserInsurePassword,sizeof(VerifyUserInsurePassword));
}
// �޸���������
VOID CDlgInsureServer::PerformModiyInsurePassword(LPCTSTR pszSrcPassword, LPCTSTR pszDesPassword)
{
	m_bSaveMission = false;						//�������
	m_bTakeMission = false;						//ȡ������
	m_bQueryMission = false;					//��ѯ����
	m_bTransferMission = false;					//ת������
	m_bTransRecordMission = false;              //ת�˼�¼
	m_bQueryNickNameMission = false;			// ��ѯ�ǳ�
	m_bVerifyInsurePassword = false;			// �����������룬���м��
	m_bModifyInsurePassword = true;				// �޸���������

//	lstrcpyn(m_szSrcInsurePass, pszSrcPassword, CountArray(m_szSrcInsurePass));
//	lstrcpyn(m_szDesInsurePass, pszDesPassword, CountArray(m_szDesInsurePass));



	//���ñ���
	CMD_GP_ModifyInsurePass ModifyInsurePass;
	ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

	//��������
	TCHAR szSrcPassword[LEN_PASSWORD]=TEXT("");
	TCHAR szDesPassword[LEN_PASSWORD]=TEXT("");
	CWHEncrypt::MD5Encrypt(pszSrcPassword, szSrcPassword);
	CWHEncrypt::MD5Encrypt(pszDesPassword, szDesPassword);

	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	ModifyInsurePass.dwUserID=pGlobalUserInfo->GetGlobalUserData()->dwUserID;
	lstrcpyn(ModifyInsurePass.szScrPassword,szSrcPassword,CountArray(ModifyInsurePass.szScrPassword));
	lstrcpyn(ModifyInsurePass.szDesPassword,szDesPassword,CountArray(ModifyInsurePass.szDesPassword));

	//��������
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_MODIFY_INSURE_PASS,&ModifyInsurePass,sizeof(ModifyInsurePass));
}

//�����¼�
VOID CDlgInsureServer::PerformSaveScore(SCORE lSaveScore)
{
	//Ч�����
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	//��������
	CMD_GR_C_SaveScoreRequest SaveScoreRequest;
	ZeroMemory(&SaveScoreRequest,sizeof(SaveScoreRequest));

	//���ñ���
	SaveScoreRequest.lSaveScore=lSaveScore;

	//��������
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_SAVE_SCORE_REQUEST,&SaveScoreRequest,sizeof(SaveScoreRequest));

	return;
}

//ȡ���¼�
VOID CDlgInsureServer::PerformTakeScore(SCORE lTakeScore, LPCTSTR pszInsurePass)
{
	//Ч�����
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	//��������
	CMD_GR_C_TakeScoreRequest TakeScoreRequest;
	ZeroMemory(&TakeScoreRequest,sizeof(TakeScoreRequest));

	//���ñ���
	TakeScoreRequest.lTakeScore=lTakeScore;
//	lstrcpyn(TakeScoreRequest.szInsurePass,pszInsurePass,CountArray(TakeScoreRequest.szInsurePass));
	lstrcpyn(TakeScoreRequest.szInsurePass,m_szInsurePass,CountArray(TakeScoreRequest.szInsurePass));

	//��������
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_TAKE_SCORE_REQUEST,&TakeScoreRequest,sizeof(TakeScoreRequest));

	return;
}

//ת���¼�
VOID CDlgInsureServer::PerformTransferScore(BYTE cbByNickName, LPCTSTR pszNickName, SCORE lTransferScore, LPCTSTR pszInsurePass)
{
	//Ч�����
	ASSERT(m_pITCPSocket!=NULL);
	if (m_pITCPSocket==NULL) return;

	m_bSaveMission=false;
	m_bTakeMission=false;
	m_bQueryMission=false;
	m_bTransferMission=true;
	m_bTransRecordMission=false;
	m_bQueryNickNameMission = false;
	m_bVerifyInsurePassword = false;

	//���ñ���
	m_cbByNickName=cbByNickName;
	m_lScore=lTransferScore;
	lstrcpyn(m_szNickName,pszNickName,CountArray(m_szNickName));
//	lstrcpyn(m_szInsurePass,pszInsurePass,CountArray(m_szInsurePass));

	//��������
	CMD_GR_C_QueryUserInfoRequest QueryUserInfoRequest;
	ZeroMemory(&QueryUserInfoRequest,sizeof(QueryUserInfoRequest));

	//���ñ���
	QueryUserInfoRequest.cbByNickName=cbByNickName;
	lstrcpyn(QueryUserInfoRequest.szNickName,pszNickName,CountArray(QueryUserInfoRequest.szNickName));

	//��������
	m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_QUERY_USER_INFO_REQUEST,&QueryUserInfoRequest,sizeof(QueryUserInfoRequest));

	return;
}

//��������
VOID CDlgInsureServer::Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem)
{
	//���ñ���
	m_pITCPSocket=pITCPSocket;
	m_pIMySelfUserItem=pIMySelfUserItem;

	return;
}

//������Ϣ
bool CDlgInsureServer::OnServerInsureMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��Ϣ�ж�
	if (wMainCmdID==MDM_GR_INSURE)
	{
		switch (wSubCmdID)
		{
		case SUB_GR_USER_INSURE_INFO:		//��������
			{
				//Ч�����
				ASSERT(wDataSize==sizeof(CMD_GR_S_UserInsureInfo));
				if (wDataSize<sizeof(CMD_GR_S_UserInsureInfo)) return false;

				//��������
				CMD_GR_S_UserInsureInfo * pUserInsureInfo=(CMD_GR_S_UserInsureInfo *)pData;

				//���ñ���
				m_bInitInfo=true;
				m_UserInsureInfo.wRevenueTake=pUserInsureInfo->wRevenueTake;
				m_UserInsureInfo.wRevenueTransfer=pUserInsureInfo->wRevenueTransfer;
				m_UserInsureInfo.wServerID=pUserInsureInfo->wServerID;
				m_UserInsureInfo.lUserScore=pUserInsureInfo->lUserScore;
				m_UserInsureInfo.lUserInsure=pUserInsureInfo->lUserInsure;
				m_UserInsureInfo.lTransferPrerequisite=pUserInsureInfo->lTransferPrerequisite;

				//���½���
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
		case SUB_GR_USER_INSURE_SUCCESS:	//���гɹ�
			{
				//Ч�����
				CMD_GR_S_UserInsureSuccess * pUserInsureSuccess=(CMD_GR_S_UserInsureSuccess *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GR_S_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GR_S_UserInsureSuccess)-sizeof(pUserInsureSuccess->szDescribeString))) return false;

				//���ñ���
				m_UserInsureInfo.lUserScore=pUserInsureSuccess->lUserScore;
				m_UserInsureInfo.lUserInsure=pUserInsureSuccess->lUserInsure;

				//����ؼ�
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

				//���½���
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);


				//��ʾ��Ϣ
				if (pUserInsureSuccess->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureSuccess->szDescribeString,MB_ICONINFORMATION,60);
				}

				if (m_bVerifyInsurePassword)
				{
					// ��֤��������׶�
					m_TabControl.ShowWindow(SW_SHOW);

					SwitchInsureModeView(INSURE_SAVE);

					GetDlgItem(IDC_CHECK_AUTO_LOGOFF)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_CHECK_AUTO_LOGOFF)->SetWindowPos(NULL, 340, 42, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
				}

// 				if (m_bModifyInsurePassword)
// 				{
// 					// �޸�����ɹ�
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
		case SUB_GR_USER_INSURE_FAILURE:	//����ʧ��
			{
				//Ч�����
				CMD_GR_S_UserInsureFailure * pUserInsureFailure=(CMD_GR_S_UserInsureFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_UserInsureFailure)-sizeof(pUserInsureFailure->szDescribeString))) return false;

				//ʧ�ܴ���
				OnMissionConclude();

				//����ؼ�
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

				//��ʾ��Ϣ
				if (pUserInsureFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureFailure->szDescribeString,MB_ICONERROR,60);
				}

// 				if (m_bModifyInsurePassword)
// 				{
// 					//���ý���
// 					if (m_bModifyInsurePassword == true && m_DlgInsureModifyPassword.GetSafeHwnd() != NULL)
// 					{
// 						m_DlgInsureModifyPassword.SetFocus();
// 					}
// 				}

				return true;
			}
		case SUB_GR_USER_TRANSFER_USER_INFO:  //�û���Ϣ
			{
				//Ч�����
				ASSERT(wDataSize==sizeof(CMD_GR_S_UserTransferUserInfo));
				if (wDataSize<sizeof(CMD_GR_S_UserTransferUserInfo)) return false;

				//��������
				CMD_GR_S_UserTransferUserInfo * pTransferUserInfo=(CMD_GR_S_UserTransferUserInfo *)pData;


				if (m_bTransferMission == true)
				{
					//������Ϣ
					TCHAR szMessage[128]={0};
					_sntprintf(szMessage, CountArray(szMessage), TEXT("��ȷ��Ҫ��[%s], ID:%lu ����%I64d ������?"), pTransferUserInfo->szNickName, pTransferUserInfo->dwTargetGameID, m_lScore);

					//��ʾ��Ϣ
					CInformation Information(this);
					if (Information.ShowMessageBox(szMessage,MB_ICONQUESTION|MB_YESNO,0)==IDYES)
					{
						//��������
						CMD_GP_C_TransferScoreRequest TransferScoreRequest;
						ZeroMemory(&TransferScoreRequest,sizeof(TransferScoreRequest));

						//���ñ���
						TransferScoreRequest.cbByNickName=m_cbByNickName;
						TransferScoreRequest.lTransferScore=m_lScore;
						lstrcpyn(TransferScoreRequest.szNickName,m_szNickName,CountArray(TransferScoreRequest.szNickName));
						lstrcpyn(TransferScoreRequest.szInsurePass,m_szInsurePass,CountArray(TransferScoreRequest.szInsurePass));

						//��������
						m_pITCPSocket->SendData(MDM_GR_INSURE,SUB_GR_TRANSFER_SCORE_REQUEST,&TransferScoreRequest,sizeof(TransferScoreRequest));
					}
				}

				// ��ѯ�ǳ�����
				if (m_bQueryNickNameMission)
				{
					if (m_DlgInsureTransfer.GetSafeHwnd() != NULL)
					{
						m_DlgInsureTransfer.SetUserNickName(pTransferUserInfo->szNickName);
					}
				}
				return true;
			}
		case SUB_GR_USER_TRANSFER_RECEIPT:		// ת�˻�ִ
			{
				// �յ���Ϸ��ִ
				//Ч�����
				ASSERT(wDataSize==sizeof(CMD_GR_UserTransferReceipt));
				if (wDataSize<sizeof(CMD_GR_UserTransferReceipt)) return false;

				CMD_GR_UserTransferReceipt * pUserTransferReceipt=(CMD_GR_UserTransferReceipt *)pData;

				m_UserInsureInfo.lUserScore=pUserTransferReceipt->lSourceUserScore;
				m_UserInsureInfo.lUserInsure=pUserTransferReceipt->lSourceUserInsure;

				// ����ȫ������
				//��������
				CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
				tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
				pGlobalUserData->dwLoveLiness = pUserTransferReceipt->dwLoveliness;

				//���½���
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				//�ر�����
				OnMissionConclude();

				// ������ִ����
				CDlgInsureTransferReceipt InsureTransferReceipt(this);
				InsureTransferReceipt.ShowReceipt(pUserTransferReceipt);

				// ��������Ϣ
				m_DlgInsureTransfer.m_edScore.SetWindowText(_T(""));
				m_DlgInsureTransfer.m_edNickName.SetWindowText(_T(""));
				m_DlgInsureTransfer.SetUserNickName(_T(""));


				return true;
			}
		case SUB_GP_OPERATE_SUCCESS:
			{
				
				//Ч�����
				CMD_GP_OperateSuccess * pUserInsureSuccess=(CMD_GP_OperateSuccess *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateSuccess)-sizeof(pUserInsureSuccess->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateSuccess)-sizeof(pUserInsureSuccess->szDescribeString))) return false;


				//���½���
				RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);

				if (m_bModifyInsurePassword)
				{
					// �޸�����ɹ�
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
				//Ч�����
				CMD_GP_OperateFailure * pUserInsureFailure=(CMD_GP_OperateFailure *)pData;
				ASSERT(wDataSize>=(sizeof(CMD_GP_OperateFailure)-sizeof(pUserInsureFailure->szDescribeString)));
				if (wDataSize<(sizeof(CMD_GP_OperateFailure)-sizeof(pUserInsureFailure->szDescribeString))) return false;

				//ʧ�ܴ���
				OnMissionConclude();

				//��ʾ��Ϣ
				if (pUserInsureFailure->szDescribeString[0]!=0)
				{
					CInformation Information(this);
					Information.ShowMessageBox(pUserInsureFailure->szDescribeString,MB_ICONERROR,60);
				}

				if (m_bModifyInsurePassword)
				{
					//���ý���
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

// ת�˳ɹ���ִ����
CDlgInsureTransferReceipt::CDlgInsureTransferReceipt(CWnd * pParentWnd) : CSkinDialog(IDD_INSURE_TRANSFER_RECEIPT, pParentWnd)
{
	m_pUserTransferReceipt = NULL;

	m_SkinAttribute.m_crBackGround = RGB(255, 255, 255);
}

CDlgInsureTransferReceipt::~CDlgInsureTransferReceipt()
{

}

//�ؼ���
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

	//������Դ
//	m_hResInstance=AfxGetResourceHandle();
	AfxSetResourceHandle(GetModuleHandle(SHARE_CONTROL_DLL_NAME));

	//��ʾ����
	INT nResult=(INT)DoModal();

	return nResult;
}

//��ʼ����Ϣ
BOOL CDlgInsureTransferReceipt::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("����ƾ֤"));

	AfxSetResourceHandle(GetModuleHandle(NULL));

	// ���ÿؼ�
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);
	m_btOk.SetButtonImage(IDB_BT_OK, hResInstance, false, false);

	CRect rcBtOk;
	m_btOk.GetClientRect(rcBtOk);
	m_btOk.MoveWindow(165, 215, rcBtOk.Width(), rcBtOk.Height());


	// �ƶ��ؼ�
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

	// ��������
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

//�滭��Ϣ
VOID CDlgInsureTransferReceipt::OnDrawClientArea(CDC * pDC, INT nWidth, INT nHeight)
{
	HINSTANCE hResInstance=GetModuleHandle(SHARE_CONTROL_DLL_NAME);

	CRect rcClient;
	GetClientRect(rcClient);

	CImage ImageTransferSuccess;
	ImageTransferSuccess.LoadFromResource(hResInstance, IDB_INSURE_TRANSFER_SUCCESS);

	CPngImage ImageTransferReceipt;
	ImageTransferReceipt.LoadImage(GetModuleHandle(SHARE_CONTROL_DLL_NAME),TEXT("INSURE_TRANSFER_RECEIPT"));

	// ��ȡ�߿��С
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

// ȷ����ť
VOID CDlgInsureTransferReceipt::OnBnClickedOk()
{
	// ����1,���ǽ�ͼ�кڱ�
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
// 	// ������ǰ���ڽ�ͼ
// 	if( OpenClipboard() )   
// 	{   
// 		EmptyClipboard(); 
// 		SetClipboardData(CF_BITMAP, hBmp);
// 		CloseClipboard();
// 	}
// 	else
// 	{
// 		// ��ʾҪ�û��ֶ�����
// 		MessageBox(_T("�򿪼��а�ʧ��"));
// 	}
// 	DeleteDC(hdc); 
// 	DeleteObject(hBmp); 
// 	::ReleaseDC(NULL, hdcScreen); 

	// ����2
//	HBITMAP hBit = NULL;
	RECT rect;
	::GetWindowRect(GetSafeHwnd(), &rect);

	HWND	hWnd = ::GetDesktopWindow();//�����Ļ��HWND.   
	HDC		hScreenDC = ::GetDC(hWnd);       //�����Ļ��HDC.   
	HDC		MemDC = ::CreateCompatibleDC(hScreenDC);

	HBITMAP	hBitmap	= ::CreateCompatibleBitmap(hScreenDC,rect.right-rect.left,rect.bottom-rect.top);   
	HGDIOBJ	hOldBMP	= ::SelectObject(MemDC,hBitmap);   
	::BitBlt(MemDC, 0, 0, rect.right, rect.bottom, hScreenDC, rect.left, rect.top, SRCCOPY);   //��ͼ

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
	 	// ��ʾҪ�û��ֶ�����
		//��ʾ��Ϣ
		TCHAR szBuffer[256]=TEXT("");
		_sntprintf(szBuffer,CountArray(szBuffer),TEXT("�򿪼��а����,����ʧ��,������Ҫ����������ͼ���߽��н�ͼ!"));

		//��ʾ��Ϣ
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
	// ������ǰ���ڽ�ͼ
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

