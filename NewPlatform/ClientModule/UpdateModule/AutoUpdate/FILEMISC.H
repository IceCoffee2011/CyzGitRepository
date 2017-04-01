#ifndef _MISCFILE_FUNCTIONS_H_
#define _MISCFILE_FUNCTIONS_H_

#include	<time.h>
#include	<wtypes.h>

namespace FileMisc{

void	TerminatePath(CString& sPath);
void	UnterminatePath(CString& sPath);
void	TerminateNetPath(CString& sPath);
void	UnterminateNetPath(CString& sPath);

time_t	GetLastModified(const char* szPath);
bool	GetLastModified(const char* szPath, SYSTEMTIME& sysTime, bool bLocalTime = true);  
bool	ResetLastModified(const char* szPath); 
double	GetFileSize(const char* szPath);

CString TranslateToNetPath( const CString &strPath );
CString TranslateToLocalPath( const  CString &strPath );

bool RemoveFolder(const char* szFolder, HANDLE hTerminate = NULL, 
				  BOOL bProcessMsgLoop = TRUE);

bool DeleteFolderContents(const char* szFolder, 
						  BOOL bIncludeSubFolders, 
						  const char* szFileMask, 
						  HANDLE hTerminate = NULL, 
						  BOOL bProcessMsgLoop = TRUE);

double GetFolderSize(const char* szFolder, 
					 BOOL bIncludeSubFolders = TRUE, 
					 const char* szFileMask = NULL, 
					 HANDLE hTerminate = NULL, 
					 BOOL bProcessMsgLoop = TRUE);

bool CreateFolder(const char* szFolder);

bool MoveFolder(const char* szSrcFolder, 
				const char* szDestFolder, 
				HANDLE hTerminate = NULL, 
				BOOL bProcessMsgLoop = TRUE);

bool CopyFolder(const char* szSrcFolder, 
				const char* szDestFolder, 
				HANDLE hTerminate = NULL, 
				BOOL bProcessMsgLoop = TRUE);

bool MoveFolder(const char* szSrcFolder, 
				const char* szDestFolder, 
				const char* szFileMask, 
				HANDLE hTerminate = NULL, 
				BOOL bProcessMsgLoop = TRUE);

bool CopyFolder( const char* szSrcFolder, 
				 const char* szDestFolder, 
				 BOOL bIncludeSubFolders, 
				 const char* szFileMask, 
				 HANDLE hTerminate = NULL, 
				 BOOL bProcessMsgLoop = TRUE );

void Rename( const CString & oldname, const CString &newname );
CString GetPathFrom(const CString& realativePath, const CString& dir);


CString		GetModulePath( HMODULE hIns );
CString		GetFullPath( LPCSTR lpszPath );

BOOL		FileExist( const CString & fName);
bool		CheckFolderExist(CString strPath);
CString		GetFilePath(const CString& sFile);
CString		GetFileName(const CString& sFile);
CString		GetTempPath();

void		ProcessMsgLoop();

}
#endif