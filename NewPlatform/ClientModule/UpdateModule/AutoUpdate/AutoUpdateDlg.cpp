// AutoUpdateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoUpdate.h"
#include "AutoUpdateDlg.h"
#include "TransparentHelper.h"
#include "MD5Checksum/MD5Checksum.h"
#include "../../../GlobalDefine/Platform.h"

// 用来产生随机地址，解决缓存的问题
#include "Random.h"

#import <msxml3.dll>
using namespace MSXML2;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define HTTP_PROTOCOL	_T("http://")
#define TIMER_EVENT_START	1000
// CAutoUpdateDlg dialog
#define ID_EXIT WM_USER + 100

#define WM_UPDATEFAIL		WM_USER+200

CAutoUpdateDlg::CAutoUpdateDlg(CWnd* pParent): CDialog(IDD_AUTOUPDATE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_ulTotalLength = 0L;
	m_ulCurentLenght = 0L;
	m_pMemDC = NULL;
	m_pOldBitmap = NULL;
	m_pBitmapMem = NULL;
	m_bmBack.LoadBitmap(IDB_DIALOG_BACK);

	m_strInfo[0] = _T("读取更新信息");
	m_strInfo[1] = _T("读取更新信息成功");
	m_strInfo[2] = _T("读取本地文件信息");
	m_strInfo[3] = _T("读取本地文件信息成功");
	m_strInfo[4] = _T("正在下载需要更新的文件");
	m_strInfo[5] = _T("下载完毕");
	m_strInfo[6] = _T("安装更新文件");
	m_strInfo[7] = _T("安装成功");
	m_strInfo[8] = _T("更新完毕,开始游戏");
	m_strInfo[9] = _T("此版本为最新版本,开始游戏");
}

CAutoUpdateDlg::~CAutoUpdateDlg()
{
	if (m_pMemDC!=NULL)
	{
		m_pMemDC->SelectObject(m_pOldBitmap);
		delete m_pMemDC;
		delete m_pBitmapMem;
	}
}

void CAutoUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CURRENT_PROGRESS, m_Progress1);
	DDX_Control(pDX, IDC_TOTAL_PROGRESS, m_Progress2);
	DDX_Control(pDX, IDC_STATUS_TEXT, m_StatusText);
	DDX_Control(pDX, IDC_COUNT_TEXT, m_CountText);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
}

BEGIN_MESSAGE_MAP(CAutoUpdateDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_MESSAGE( WM_TRANSPARENT_BK, &CAutoUpdateDlg::OnTransaprentBk)
	ON_BN_CLICKED(IDCANCEL, &CAutoUpdateDlg::OnBnClickedCancel)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()

	ON_BN_CLICKED(ID_EXIT, OnClickMenuExit)
	ON_MESSAGE(WM_UPDATEFAIL, OnUpdateFail)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




BOOL CAutoUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// 设置标题栏名称
	SetWindowText(_T("浙东山庄更新程序"));

	// 读取下载地址配置
	TCHAR szModulePath[MAX_PATH]=TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(),szModulePath,CountArray(szModulePath));
	//分析文件
	for (INT i=lstrlen(szModulePath);i>=0;i--)
	{
		if (szModulePath[i]==TEXT('\\'))
		{
			szModulePath[i]=0;
			break;
		}
	}
	//构造路径
	TCHAR szServerInfoPath[MAX_PATH]=TEXT("");
	_sntprintf(szServerInfoPath,CountArray(szServerInfoPath),TEXT("%s\\Local.dat"),szModulePath);

	//读取数据
//	TCHAR szResultEncrypt[MAX_PATH];
	GetPrivateProfileString(_T("UpdateLink"), _T("UpdateLink"), TEXT(""),szUpdateLink,MAX_PATH,szServerInfoPath);

	LOGFONT	lf;
	GetDefaultFont(lf);
	m_defaultFont.CreateFontIndirect(&lf);

	ModifyStyleEx(0,WS_EX_LAYERED); 
	ImageFromIDResource(IDR_LOGO_PNG, _T("PNG"), (Image*&)m_bkImg);

	int iWidth = m_bkImg->GetWidth();
	int iHeight = m_bkImg->GetHeight();
	SetWindowPos(NULL, 0, 0, iWidth, iHeight, SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOMOVE|SWP_NOZORDER);

	LoadInfoImg();

	DrawUI(0);

	m_StatusText.SetTextColor(RGB(255,255,255));
	m_CountText.SetTextColor(RGB(255,255,255));

	//设置范围
	m_Progress2.SetRange32(0, m_ulTotalLength/1024);

	// Send WM_NCCALCSIZE messages to the window
	m_Progress1.SetWindowPos(NULL, 0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	m_Progress2.SetWindowPos(NULL, 0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	ShowWindow(SW_SHOW);
	BringWindowToTop();
	UpdateWindow();

	DWORD dwThreadId;
	m_hThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, &dwThreadId);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

DWORD WINAPI CAutoUpdateDlg::ThreadProc(LPVOID lpParam)
{
	CoInitialize(NULL);
	CAutoUpdateDlg* pAutoUpdate = reinterpret_cast<CAutoUpdateDlg*>(lpParam);

	HWND hNotifyWindow = NULL;
	TCHAR szModuleFile[MAX_PATH]={0};
	CString strSiteURL;
	CString strTempDir;

	//查找窗口

	hNotifyWindow = ::FindWindow(szPlazaClass, NULL);
	//#ifndef _DEBUG
	//	if (hNotifyWindow==NULL)
	//	{
	//		PostMessage(WM_CLOSE);
	//		return TRUE;
	//	}
	//#endif

	GetModuleFileName(AfxGetInstanceHandle(), szModuleFile, MAX_PATH);
	PathRemoveFileSpec(szModuleFile);
	PathAddBackslash(szModuleFile);
	pAutoUpdate->m_strCurrentDir = szModuleFile;

	//读取配置参数
	strSiteURL = szUpdateLink;
	strSiteURL += _T("/Download");
	strTempDir = _T("Update");

	//设置URL路径
	if( strSiteURL.Left(7) != HTTP_PROTOCOL )
		strSiteURL = HTTP_PROTOCOL + strSiteURL;

	strSiteURL += _T("/Update/");
	pAutoUpdate->m_DownloadMgr.Initialize((IDownloadSink*)pAutoUpdate, strSiteURL, pAutoUpdate->m_strCurrentDir+strTempDir, pAutoUpdate->m_strCurrentDir);

	Sleep(300);

	//检查版本
	int rl = 0;
	CRandom rand;
	rand.Randomize();
	CString cstrUpdateURL;
	cstrUpdateURL.Format("%s//Update.xml?%d", strSiteURL, rand.Random(1000));
	rl = pAutoUpdate->CheckVersionUpdate(cstrUpdateURL);
	if (rl == 0)
	{
		//	ASSERT(hNotifyWindow!=NULL);
		//	::SendMessage(hNotifyWindow, WM_USER+102, 0, 0);
		::MessageBox(NULL, _T("获取更新信息失败，请检查您的网络连接或稍后重新尝试"), _T("更新程序"), MB_OK);
//		pAutoUpdate->DrawUI(9);
//		Sleep(300);
//		pAutoUpdate->PostMessage(WM_CLOSE);
//		pAutoUpdate->DestroyWindow();
		pAutoUpdate->PostMessage(WM_UPDATEFAIL);
		return 0;
	}
	else if (rl == 2)
	{
		pAutoUpdate->DrawUI(9);
		Sleep(300);
		pAutoUpdate->PostMessage(WM_CLOSE);
		return 0;
	}
	//else
	//{
	//	::SendMessage(hNotifyWindow, WM_USER+102, 1, 0);
	//}

	pAutoUpdate->DrawUI(1);
	Sleep(300);

	pAutoUpdate->DrawUI(4);
	Sleep(300);

	//启动下载
	pAutoUpdate->m_DownloadMgr.Start();

	CoUninitialize();
	return 0;
}


void CAutoUpdateDlg::LoadInfoImg()
{
	for (int i = 0; i < sizeof(m_strInfo) / sizeof(char*); i++) {
		Image* img;
		ImageFromIDResource(IDR_INFO_PNG_0 + i, _T("PNG"), (Image*&)img);
		m_lstInfoImg.Add(img);
	}
}

void CAutoUpdateDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}

void CAutoUpdateDlg::DrawUI(int iInfoIndex)
{
	HDC hDC = ::GetDC(m_hWnd);
	HDC hMemDC=::CreateCompatibleDC(hDC);
	CDC dcMem;
	dcMem.Attach(hMemDC);
	RECT rc;
	::GetClientRect(m_hWnd,&rc);
	int w = rc.right - rc.left;
	int h = rc.bottom - rc.top;
	HBITMAP hbmp = CreateCompatibleBitmap(hDC,w,h);
	HBITMAP holdbmp = (HBITMAP)SelectObject(hMemDC,hbmp);
	Graphics g(hMemDC);
	g.DrawImage(m_bkImg,0,0,w,h); 

	g.DrawImage(m_lstInfoImg.GetAt(iInfoIndex), m_rtInfo.left, m_rtInfo.top,m_rtInfo.Width(),m_rtInfo.Height());
	//DrawString(hMemDC, m_strInfo[iInfoIndex]);

	CPoint DestPt(0,0);
	CSize psize(w,h);
	BLENDFUNCTION bn;
	bn.AlphaFormat = AC_SRC_ALPHA;
	bn.BlendFlags = 0;
	bn.BlendOp = AC_SRC_OVER;
	bn.SourceConstantAlpha = 255;
	::UpdateLayeredWindow(m_hWnd,hDC,NULL,&psize,hMemDC,&DestPt,0,&bn,ULW_ALPHA);

	::SelectObject (hMemDC,holdbmp);
	::DeleteObject(hbmp);
	::DeleteDC(hMemDC);
	::ReleaseDC(m_hWnd,hDC);

	dcMem.Detach();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAutoUpdateDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		return;

		CPaintDC dc(this);
		CRect rtClient;
		GetClientRect(&rtClient);

		if ( m_pMemDC )
		{
			dc.BitBlt( 0,0,rtClient.Width(), rtClient.Height(), m_pMemDC, 0,0, SRCCOPY);
		}
	}
}

BOOL CAutoUpdateDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CAutoUpdateDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (cx < 0 || cy < 0) {
		return;
	}

	GetClientRect(&m_rtClient);
	m_rtInfo.left = m_rtClient.left + 95;
	m_rtInfo.top = m_rtClient.bottom - 20;
	m_rtInfo.right = m_rtInfo.left + 195;
	m_rtInfo.bottom = m_rtClient.bottom;
}

void CAutoUpdateDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	CDialog::OnLButtonDown(nFlags, point);
}

void CAutoUpdateDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMenu popMenu;

	//建立菜单
	popMenu.CreatePopupMenu();
	popMenu.AppendMenu(0, ID_EXIT, _T("退出"));

	//显示菜单
	ClientToScreen(&point);
	popMenu.TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, this);
	popMenu.DestroyMenu();

	CDialog::OnRButtonDown(nFlags, point);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAutoUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAutoUpdateDlg::OnStatusCallback(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCTSTR pszStatusText)
{
	switch( ulStatusCode )
	{
	case DMS_SENDREQUEST:
		{
			break;
		}
	case DMS_BEGINDOWNLOAD:
		{
			m_ulCurentLenght = ulProgressMax;
			m_Progress1.SetRange32(0, ulProgressMax/1024);
			m_Progress1.SetPos(0);
			break;
		}
	case DMS_DOWNLOADDATA:
		{
			CString strText;
			CString strText2;
			strText.Format(TEXT("正在下载 %s "), pszStatusText);
			strText2.Format(TEXT("大小 %d KB  已下载 %d KB"), m_ulCurentLenght/1024, ulProgress/1024);
			m_StatusText.SetWindowText(strText);
			m_CountText.SetWindowText(strText2);

			m_Progress1.SetPos(ulProgress/1024);
			m_Progress2.SetPos(ulProgressMax/1024);
			break;
		}
	case DMS_ENDDOWNLOADDATA:
		{
			m_ulCurentLenght = 0;
			//m_Progress1.SetPos(0);
			break;
		}
	case DMS_FINISHDOWNLOAD:
		{
			DrawUI(5);
			Sleep(300);

			DrawUI(6);
			Sleep(300);

			m_btCancel.EnableWindow(FALSE);
			break;
		}
	case DMS_CANCELDOWNLOAD:
		{
			SendMessage(WM_CLOSE);
			break;
		}
	case DMS_ENDSETUP:
		{
			DrawUI(7);
			Sleep(300);

			DrawUI(8);
			Sleep(300);

			////启动程序
			//TCHAR szModuleName[MAX_PATH]={0};
			//GetModuleFileName(AfxGetInstanceHandle(), szModuleName, MAX_PATH);
			//PathRemoveFileSpec(szModuleName);
			//strcat(szModuleName, _T("\\GamePlaza.exe -noupdate"));
			//WinExec(szModuleName, SW_SHOWDEFAULT);

			SendMessage(WM_CLOSE);
			break;
		}
	case DMS_ERROR:
		{
			CString strError = AfxGetApp()->GetProfileString(_T("messages"), _T("ErrorMsg"), _T("更新失败，请到官网下载最新版本"));
			MessageBox(strError, _T("提示"), MB_OK|MB_ICONINFORMATION);
			SendMessage(WM_CLOSE);
			break;
		}
	}
}

INT CAutoUpdateDlg::CheckVersionUpdate(CString& strUpdateURL)
{
	OutputDebugString(strUpdateURL);
	BOOL bHasUpdate = FALSE;

	HRESULT hr = S_OK;
	IXMLHTTPRequestPtr pHttpRequest;
	IDispatchPtr pDispatch;
	MSXML2::IXMLDOMDocumentPtr pXmlDoc;
	MSXML2::IXMLDOMNodeListPtr pList;
	MSXML2::IXMLDOMElementPtr pChild;

//	UINT nFileSize;
	CString strFileName, strFileVer, strMD5String;
	LONG lElementCount = 0L;

	// !! 测试本地读取文件
 	try
 	{
 		hr = pHttpRequest.CreateInstance(TEXT("Msxml2.XMLHTTP.3.0"));
 		if( FAILED(hr) )
 			_com_issue_error(hr);
 
 		hr = pHttpRequest->open(TEXT("GET"), (_bstr_t)strUpdateURL, false);
 		if( FAILED(hr) )
 			_com_issue_error(hr);
 
 		hr = pHttpRequest->send();
 		if( FAILED(hr) )
 			_com_issue_error(hr);
 
 		if (pHttpRequest->Getstatus() != 200)
 			throw (0);
 
 		pDispatch = pHttpRequest->GetresponseXML();
 		hr = pDispatch->QueryInterface(pXmlDoc.GetIID(), (void**)&pXmlDoc);
 		if( FAILED(hr) )
 			_com_issue_error(hr);
 
 //		variant_t varXml(_T("D:\\Lobby.xml"));
 //		pXmlDoc->load(varXml);
 		
 		MSXML2::IXMLDOMElementPtr pRootElement= pXmlDoc->GetdocumentElement();
 //		CString strElemet = pElement->getAttribute("Name");
 
 		MSXML2::IXMLDOMElementPtr pNode = pRootElement->GetfirstChild();
 //		CString strNodeName = pNode->getAttribute("Name");
 		
 		MSXML2::IXMLDOMElementPtr pChildNode = pNode->GetfirstChild();
 //		strNodeName = pChildNode->getAttribute("Name");
 
 		CString strDir;
 		while(pChildNode != NULL)
 		{
 			if ( RecursionParse(pChildNode, strDir) == FALSE )
 			{
 				return 0;
 			}
 			pChildNode = pChildNode->GetnextSibling();
 		}
 
 		DrawUI(3);
 
 		if (m_DownloadMgr.GetTaskCount() == 0)
 		{
 			// 不需要更新
 			return 2;
 		}
 		else
 		{
 			// 需要更新
 			return 1;
 		}
 	}
#ifdef _DEBUG
	catch(_com_error& e)
	{
		CString strError;
		strError.Format(_T("Error code:%d\nMessage:%s\nDescrption:%s"), (int)e.WCode(), e.ErrorMessage(), (TCHAR*)e.Description());
		MessageBox(strError, _T("提示"), MB_OK|MB_ICONWARNING);
		return FALSE;
	}
#endif
 	catch(...)
 	{
		// 获取版本文件失败
		return FALSE;
 	}

// 	try
// 	{
// 		hr = pHttpRequest.CreateInstance(TEXT("Msxml2.XMLHTTP.3.0"));
// 		if( FAILED(hr) )
// 			_com_issue_error(hr);
// 
// 		hr = pHttpRequest->open(TEXT("GET"), (_bstr_t)strUpdateURL, false);
// 		if( FAILED(hr) )
// 			_com_issue_error(hr);
// 
// 		hr = pHttpRequest->send();
// 		if( FAILED(hr) )
// 			_com_issue_error(hr);
// 
// 		if (pHttpRequest->Getstatus() != 200)
// 			throw (0);
// 
// 		pDispatch = pHttpRequest->GetresponseXML();
// 		hr = pDispatch->QueryInterface(pXmlDoc.GetIID(), (void**)&pXmlDoc);
// 		if( FAILED(hr) )
// 			_com_issue_error(hr);
// 
// 		pList = pXmlDoc->selectNodes("/manifest/filelist/file");
// 		lElementCount = pList->Getlength();
// 
// 		DrawUI(2);
// 		for( LONG i = 0; i < lElementCount; i++ )
// 		{
// 			pChild = pList->Getitem(i);
// 			strFileName = pChild->getAttribute("filename");
// 			nFileSize = pChild->getAttribute("filesize");
// 			strFileVer = pChild->getAttribute("fileversion");
// 			strMD5String = pChild->getAttribute("md5");
// 
// 			//如果本地文件存在则效验文件，不存在就下载这个文件
// 			if (PathFileExists(m_strCurrentDir+strFileName))
// 			{
// 				//效验文件,如果MD5码不相同则重新下载
// 				if (CMD5Checksum::GetMD5(m_strCurrentDir+strFileName) != strMD5String)
// 				{
// 					m_ulTotalLength += nFileSize;
// 					m_DownloadMgr.AddTask(strFileName);
// 					bHasUpdate = TRUE;
// 				}	
// 			}
// 			else
// 			{
// 				m_ulTotalLength += nFileSize;
// 				m_DownloadMgr.AddTask(strFileName);
// 				bHasUpdate = TRUE;
// 			}
// 		}
// 
// 		DrawUI(3);
// //		return bHasUpdate;
// 		if (bHasUpdate == TRUE)
// 		{
// 			// 需要更新
// 			return 1;
// 		}
// 		else
// 		{
// 			// 不需要更新
// 			return 2;
// 		}
// 	}
// #ifdef _DEBUG
// 	catch(_com_error& e)
// 	{
// 		CString strError;
// 		strError.Format(_T("Error code:%d\nMessage:%s\nDescrption:%s"), (int)e.WCode(), e.ErrorMessage(), (TCHAR*)e.Description());
// 		MessageBox(strError, _T("提示"), MB_OK|MB_ICONWARNING);
// 		return FALSE;
// 	}
// #endif
// 	catch(...)
// 	{
// 		// 获取版本文件失败
// 		return FALSE;
// 	}
}

BOOL CAutoUpdateDlg::RecursionParse(MSXML2::IXMLDOMElementPtr pElement, CString strDir)
{
	if (_tcscmp(pElement->GetnodeName(), XML_DIRECTORY) == 0)
	{
		CString strDirName = pElement->getAttribute("Name");
		MSXML2::IXMLDOMNodePtr pNode = pElement->GetfirstChild();
 		if(pNode == NULL)
 		{
 			return TRUE;
 		}
		strDir.Append( strDirName );
		if( !strDir.IsEmpty())
		{
			strDir.Append( BLACK_SLASH );
		}

		MSXML2::IXMLDOMElementPtr pCurElem(pNode);
		while(pCurElem != NULL)
		{
			if( !RecursionParse( pCurElem, strDir ) )
			{
				return FALSE;
			}
			pCurElem = pCurElem->GetnextSibling();
		}
	}
	else if ( _tcscmp( pElement->GetnodeName(), XML_FILE) == 0 )
	{
		CString strFileName = pElement->getAttribute("Name");
		CString strMD5String = pElement->getAttribute("MD5");
		CString strZipMD5String = pElement->getAttribute("ZipMD5");
		UINT nFileSize = pElement->getAttribute("Size");
		UINT nZipFileSize = pElement->getAttribute("ZipSize");
		if (PathFileExists(m_strCurrentDir+strDir+strFileName))
		{
			// 本地文件存在，且MD5不一致才去下载
			if (CMD5Checksum::GetMD5(m_strCurrentDir+strDir+strFileName) != strMD5String)
			{
				m_ulTotalLength += nFileSize;
				m_DownloadMgr.AddTask(strDir+strFileName, strDir+strFileName);
//				bHasUpdate = TRUE;
			}	
		}
		else
		{
			// 判断临时目录下是否有这个文件，不存在临时文件或者临时文件md5不一致，都需要下载
			CString strTempPath = m_strCurrentDir+TEMP_DIR;
			CString strTempFile;
			strTempFile.Format("%s\\%s\\%s.dtm", strTempPath, strDir, strFileName);
			if ( PathFileExists(strTempFile) && CMD5Checksum::GetMD5(strTempFile) == strZipMD5String )
			{
				m_ulTotalLength += nFileSize;
				m_DownloadMgr.AddTask(strDir+strFileName, strDir+strFileName, TO_INSTALL);
			}
			else
			{
				m_ulTotalLength += nFileSize;
				m_DownloadMgr.AddTask(strDir+strFileName, strDir+strFileName);
			}
//			bHasUpdate = TRUE;
		}
	}
	return TRUE;
}

void CAutoUpdateDlg::OnBnClickedCancel()
{
	if ( m_DownloadMgr.GetStatus() == DMS_FINISHDOWNLOAD || m_DownloadMgr.GetStatus() == DMS_ERROR )
	{
		DestroyWindow();
	}
	else
	{
		m_DownloadMgr.CancelTask();
		m_btCancel.EnableWindow(FALSE);
	}
}

void CAutoUpdateDlg::OnClose()
{
	//启动程序
	TCHAR szModuleName[MAX_PATH]={0};
	GetModuleFileName(AfxGetInstanceHandle(), szModuleName, MAX_PATH);
	PathRemoveFileSpec(szModuleName);
	strcat(szModuleName, _T("\\Lobby.exe -noupdate"));
	WinExec(szModuleName, SW_SHOWDEFAULT);

	DestroyWindow();
}

LRESULT CAutoUpdateDlg::OnTransaprentBk( WPARAM wParam, LPARAM lParam )
{
	HDC hdc = ( HDC)wParam;
	HWND hwnd = ( HWND)lParam;
	CTransparentHelper::OnTranparentControl( m_pMemDC->GetSafeHdc(), (WPARAM)hdc, (LPARAM)hwnd);
	return TRUE;
}

LRESULT CAutoUpdateDlg::OnUpdateFail(WPARAM wParam, LPARAM lParam)
{
	DestroyWindow();
	return TRUE;
}

void CAutoUpdateDlg::BuildBkDC()
{
	CDC* pDC;
	CDC MemDC;
	CBitmap* pOldBitmap;
	CRect rtClient;

	pDC = GetDC();
	MemDC.CreateCompatibleDC(pDC);
	pOldBitmap = MemDC.SelectObject( &m_bmBack );

	GetClientRect(&rtClient);

	if ( m_pMemDC )
	{
		m_pMemDC->SelectObject( m_pOldBitmap);
		delete m_pBitmapMem;
		m_pBitmapMem = NULL;
		delete m_pMemDC;
		m_pMemDC = NULL;
	}

	if ( m_pMemDC == NULL )
	{
		m_pMemDC = new CDC;
		m_pMemDC->CreateCompatibleDC(pDC);

		m_pBitmapMem = new CBitmap;
		m_pBitmapMem->CreateCompatibleBitmap(pDC, rtClient.Width(), rtClient.Height());
		m_pOldBitmap = (CBitmap*)m_pMemDC->SelectObject( m_pBitmapMem);
	}

	m_pMemDC->BitBlt( 0,0, rtClient.Width(), rtClient.Height(), &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject( pOldBitmap );
	ReleaseDC(pDC);

	//when the parent dialog's background is rebuild, notify the child which has an transparent tag.
	CTransparentHelper::NotifyTransparentChild( GetSafeHwnd());
}

BOOL CAutoUpdateDlg::ImageFromIDResource(UINT nID, LPCTSTR sTR, Image * & pImg)  
{  
	HINSTANCE hInst = AfxGetResourceHandle();  
	HRSRC hRsrc = ::FindResource (hInst,MAKEINTRESOURCE(nID),sTR); // type  
	if (!hRsrc)  
		return FALSE;  
	// load resource into memory  
	DWORD len = SizeofResource(hInst, hRsrc);  
	BYTE* lpRsrc = (BYTE*)LoadResource(hInst, hRsrc);  
	if (!lpRsrc)  
		return FALSE;  
	// Allocate global memory on which to create stream  
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);  
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);  
	memcpy(pmem,lpRsrc,len);  
	IStream* pstm;  
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);  
	// load from stream  
	pImg=Gdiplus::Image::FromStream(pstm);  
	// free/release stuff  
	GlobalUnlock(m_hMem);  
	pstm->Release();  
	FreeResource(lpRsrc);  
	return TRUE;  
} 

BOOL CAutoUpdateDlg::GetDefaultFont(LOGFONT& lf)
{
	memset(&lf, 0, sizeof(LOGFONT));
	::GetObject((HFONT)GetStockObject(DEFAULT_GUI_FONT), sizeof(lf), &lf);

	OSVERSIONINFO osVerInfo;
	ZeroMemory(&osVerInfo, sizeof(osVerInfo));
	osVerInfo.dwOSVersionInfoSize = sizeof(osVerInfo);
	GetVersionEx(&osVerInfo);

	if (osVerInfo.dwMajorVersion >= 6) {
		strcpy(lf.lfFaceName, _T("arial"));
		int nHeight;
		CALC_FONT_HEIGHT(nHeight, 10);
		if (abs(lf.lfHeight) < abs(nHeight))
			lf.lfHeight = nHeight;
	}

	return TRUE;
}

HBITMAP CAutoUpdateDlg::CreateAlphaTextBitmap(LPCSTR inText, HFONT inFont, COLORREF inColour)
{ 
	int TextLength = (int)strlen(inText); 
	if (TextLength <= 0) return NULL; 

	// Create DC and select font into it 
	HDC hTextDC = CreateCompatibleDC(NULL); 

	HFONT TempFont = CreateFont(15, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Arial\0"); 
	HFONT hOldFont = (HFONT)SelectObject(hTextDC, TempFont); 
	DeleteObject(TempFont); 
	HBITMAP hMyDIB = NULL; 

	// Get text area 
	RECT TextArea = {0, 0, 0, 0}; 
	DrawText(hTextDC, inText, TextLength, &TextArea, DT_CALCRECT); 
	if ((TextArea.right > TextArea.left) && (TextArea.bottom > TextArea.top))
	{ 
		BITMAPINFOHEADER BMIH; 
		memset(&BMIH, 0x0, sizeof(BITMAPINFOHEADER)); 
		void *pvBits = NULL; 

		// Specify DIB setup 
		BMIH.biSize = sizeof(BMIH); 
		BMIH.biWidth = TextArea.right - TextArea.left; 
		BMIH.biHeight = TextArea.bottom - TextArea.top; 
		BMIH.biPlanes = 1; 
		BMIH.biBitCount = 32; 
		BMIH.biCompression = BI_RGB; 

		// Create and select DIB into DC 
		hMyDIB = CreateDIBSection(hTextDC, (LPBITMAPINFO)&BMIH, 0, (LPVOID*)&pvBits, NULL, 0); 
		HBITMAP hOldBMP = (HBITMAP)SelectObject(hTextDC, hMyDIB); 
		if (hOldBMP != NULL)
		{ 
			// Set up DC properties 
			SetTextColor(hTextDC, 0x00FFFFFF); 
			SetBkColor(hTextDC, 0x00000000); 
			SetBkMode(hTextDC, OPAQUE); 

			// Draw text to buffer 
			DrawText(hTextDC, inText, TextLength, &TextArea, DT_NOCLIP); 
			BYTE* DataPtr = (BYTE*)pvBits; 
			BYTE FillR = GetRValue(inColour); 
			BYTE FillG = GetGValue(inColour); 
			BYTE FillB = GetBValue(inColour); 
			BYTE ThisA; 
			for (int LoopY = 0; LoopY < BMIH.biHeight; LoopY++) { 
				for (int LoopX = 0; LoopX < BMIH.biWidth; LoopX++) { 
					ThisA = *DataPtr; // Move alpha and pre-multiply with RGB 
					*DataPtr++ = (FillB * ThisA) >> 8; 
					*DataPtr++ = (FillG * ThisA) >> 8; 
					*DataPtr++ = (FillR * ThisA) >> 8; 
					*DataPtr++ = ThisA; // Set Alpha 
				} 
			} 

			// De-select bitmap 
			SelectObject(hTextDC, hOldBMP); 
		} 
	} 

	// De-select font and destroy temp DC 
	SelectObject(hTextDC, hOldFont); 
	DeleteDC(hTextDC); 

	// Return DIBSection 
	return hMyDIB; 
}

void CAutoUpdateDlg::DrawString(HDC inDC, char* szInfo)
{ 

	RECT TextArea = {0, 0, 0, 0}; 
	HFONT TempFont = CreateFont(15, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, _T("Segoe UI\0")); 
	HBITMAP MyBMP = CreateAlphaTextBitmap(szInfo, TempFont, RGB(53,158,205)); 
	DeleteObject(TempFont); 
	if (MyBMP)
	{
		// Create temporary DC and select new Bitmap into it 
		HDC hTempDC = CreateCompatibleDC(inDC); 
		HBITMAP hOldBMP = (HBITMAP)SelectObject(hTempDC, MyBMP); 
		if (hOldBMP)
		{
			// Get Bitmap image size
			BITMAP BMInf;
			GetObject(MyBMP, sizeof(BITMAP), &BMInf); 

			// Fill blend function and blend new text to window 
			BLENDFUNCTION bf; 
			bf.BlendOp = AC_SRC_OVER; 
			bf.BlendFlags = 0; 
			bf.SourceConstantAlpha = 128; 
			bf.AlphaFormat = AC_SRC_ALPHA; 
			AlphaBlend(inDC, m_rtInfo.left, m_rtInfo.top, BMInf.bmWidth, BMInf.bmHeight, hTempDC, 0, 0, BMInf.bmWidth, BMInf.bmHeight, bf); 

			// Clean up 
			SelectObject(hTempDC, hOldBMP); 
			DeleteObject(MyBMP); 
			DeleteDC(hTempDC); 
		} 
	} 
} 

void CAutoUpdateDlg::OnClickMenuExit()
{
	exit(0);
}