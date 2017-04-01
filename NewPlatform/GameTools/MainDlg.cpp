// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"

#include "EncryptResManager.h"

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	CButton radio = GetDlgItem(IDC_RADIO1);
	radio.SetCheck(BST_CHECKED);
	radio = GetDlgItem(IDC_RADIO3);
	radio.SetCheck(BST_CHECKED);
	radio = GetDlgItem(IDC_CHECK1);
	radio.SetCheck(BST_CHECKED);
	radio = GetDlgItem(IDC_CHECK2);
	radio.SetCheck(BST_CHECKED);

	return TRUE;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnBnClickedBrowse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!DoDataExchange(TRUE)) return 0;
	if (m_bSourceIsFile)
	{
		CFileDialog fileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST,
					  m_bDecrypt ? _T("fish files (*.zyj)\0*.zyj\0\0") : _T("All files (*.*)\0*.*\0\0"));
		int nRet = fileDlg.DoModal();
		if (nRet == IDOK)
		{
			m_strSource = fileDlg.m_szFileName;
		}
	}
	else
	{
		BROWSEINFO bInfo;
		LPITEMIDLIST pidl;
		CString strPath;
		ZeroMemory ((PVOID)&bInfo, sizeof(BROWSEINFO));

		bInfo.hwndOwner = m_hWnd;
		bInfo.pszDisplayName = strPath.GetBuffer(MAX_PATH);
		bInfo.lpszTitle = _T("Browse");
		bInfo.ulFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bInfo.lpfn = NULL;
		bInfo.lParam = (LPARAM)this;

		if ((pidl = ::SHBrowseForFolder(&bInfo) ) == NULL)
		{
			return 0;
		}

		if (::SHGetPathFromIDList(pidl, strPath.GetBuffer(MAX_PATH)) == FALSE)
		{
			return 0;
		}

		m_strSource = bInfo.pszDisplayName;
	}

	if (!DoDataExchange(FALSE)) return 0;

	return 0;
}

LRESULT CMainDlg::OnBnClickedStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!DoDataExchange(TRUE)) 
		return 0;

	if (m_strSource.IsEmpty())
	{
		if (!m_bDecrypt)
			MessageBox(_T("请选择要加密的源文件或文件夹！"), _T("温馨提示"), MB_OK);
		else
			MessageBox(_T("请选择要解密的源文件或文件夹！"), _T("温馨提示"), MB_OK);

		return 0;
	}

	if (!m_bDecrypt)
	{
		if (m_bSourceIsFile)
		{
			EncryptFile(m_strSource, m_bDeleteFile);
		}
		else
		{
			EncryptFileFoder(m_strSource, m_bSubFoder, m_bDeleteFile);
		}
	}
	else
	{
		if (m_bSourceIsFile)
		{
			DecryptFile(m_strSource, m_bDeleteFile);
		}
		else
		{
			DecryptFileFoder(m_strSource, m_bSubFoder, m_bDeleteFile);
		}
	}

	MessageBox(_T("转化完成"), _T("温馨提示"), MB_OK);

	return 0;
}

bool CMainDlg::EncryptFile(const CString& strFile, BOOL bDeleteSourceFile)
{
	std::wstring _strFile = strFile;
	if (std::wstring::npos == _strFile.rfind(L".png", _strFile.length() - 1, 4))
		return false;

	FILE* pFile = NULL;
	_tfopen_s(&pFile, strFile, _T("rb"));
	if (pFile == NULL)
	{
		MessageBox(_T("文件打开失败"));
		return false;
	}

	int nPos = strFile.ReverseFind(_T('.'));
	if (nPos < 0)
	{
		MessageBox(_T("文件名错误"));
		return false;
	}
	CString strDstFile = strFile.Left(nPos);
	strDstFile += _T(".zyj");

	fseek(pFile, 0, SEEK_END);
	long lFileLength = ftell(pFile);

	fseek(pFile, 0, SEEK_SET);
	BYTE* pData = new BYTE[lFileLength];
	memset(pData, 0, lFileLength);
	fread(pData, lFileLength, 1, pFile);

	CEncryptResManager::GetSingleton().EncryptData(pData, lFileLength);

	FILE* wfile = NULL;
	_tfopen_s(&wfile, strDstFile, _T("wb"));
	fwrite(pData, lFileLength, 1, wfile);

	if (pData)
	{
		delete[] pData;
		pData = nullptr;
	}

	fclose(pFile);
	fclose(wfile);

	if (bDeleteSourceFile)
	{
		DeleteFile(strFile);
	}

	return true;
}

bool CMainDlg::DecryptFile(const CString& strFile, BOOL bDeleteSourceFile)
{
	std::wstring _strFile = strFile;
	if (std::wstring::npos == _strFile.rfind(L".zyj", _strFile.length() - 1, 4))
		return false;

	FILE* pFile = NULL;
	_tfopen_s(&pFile, strFile, _T("rb"));
	if (pFile == NULL) 
	{
		MessageBox(_T("文件打开失败"));
		return false;
	}

	int nPos = strFile.ReverseFind(_T('.'));
	if (nPos < 0)
	{
		MessageBox(_T("文件名错误"));
		return false;
	}
	CString strDstFile = strFile.Left(nPos);

	fseek(pFile, 0, SEEK_END);
	long lFileLength = ftell(pFile);

	fseek(pFile, 0, SEEK_SET);
	BYTE* pData = new BYTE[lFileLength];
	memset(pData, 0, lFileLength);
	fread(pData, lFileLength, 1, pFile);

	CEncryptResManager::GetSingleton().DecryptData(pData, lFileLength);
	strDstFile += CEncryptResManager::GetSingleton().GetFileExtension(pData).c_str();

	FILE* wfile = NULL;
	_tfopen_s(&wfile, strDstFile, _T("wb"));
	fwrite(pData, lFileLength, 1, wfile);

	if (pData)
	{
		delete[] pData;
		pData = nullptr;
	}

	fclose(pFile);
	fclose(wfile);

	if (bDeleteSourceFile)
	{
		DeleteFile(strFile);
	}

	return true;
}

bool CMainDlg::EncryptFileFoder(const CString& strFileFoder, BOOL bSubFoder, BOOL bDeleteSourceFile)
{
	CString strSource = strFileFoder;
	if (strSource.Right(1) != _T("\\") && strSource.Right(1) != _T("/"))
		strSource += _T("\\");
	strSource += _T("*.*");

	CFindFile finder;
	if (finder.FindFile(strSource))
	{
		do
		{
			if (finder.IsDots()) continue;

			CString strFilePath = finder.GetFilePath();

			if (finder.IsDirectory())
			{
				if (bSubFoder)
					EncryptFileFoder(strFilePath, bSubFoder, bDeleteSourceFile);
			}
			else
			{
				EncryptFile(strFilePath, bDeleteSourceFile);
			}

		}
		while (finder.FindNextFile());

	}
	finder.Close();

	return true;
}

bool CMainDlg::DecryptFileFoder(const CString& strFileFoder, BOOL bSubFoder, BOOL bDeleteSourceFile)
{
	CString strSource = strFileFoder;
	if (strSource.Right(1) != _T("\\") && strSource.Right(1) != _T("/"))
		strSource += _T("\\");
	strSource += _T("*.*");

	CFindFile finder;
	if (finder.FindFile(strSource))
	{
		do
		{
			if (finder.IsDots()) continue;

			CString strFilePath = finder.GetFilePath();

			if (finder.IsDirectory())
			{
				if (bSubFoder)
					DecryptFileFoder(strFilePath, bSubFoder, bDeleteSourceFile);
			}
			else
			{
				DecryptFile(strFilePath, bDeleteSourceFile);
			}

		}
		while (finder.FindNextFile());

	}
	finder.Close();

	return true;
}
