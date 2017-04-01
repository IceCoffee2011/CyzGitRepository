#include "StdAfx.h"
#include ".\transcode.h"

void Transcode::Unicode_to_UTF8( const wchar_t* in, size_t len, std::string& out )
{
	size_t out_len = len * 3 + 1;
	char* lpBuf = new char[out_len];
	
	memset(lpBuf, 0, out_len);

	out_len = ::WideCharToMultiByte(CP_UTF8, 0, in, len, lpBuf, len * 3, NULL, NULL);
	out.assign(lpBuf, out_len);

	delete [] lpBuf;
}

void Transcode::UTF8_to_Unicode( const char* in, size_t len, std::wstring& out )
{
	wchar_t* lpBuf = new wchar_t[len + 1];
	size_t out_len = (len + 1) * sizeof(wchar_t);

	memset(lpBuf, 0, (len + 1) * sizeof(wchar_t));

	out_len = ::MultiByteToWideChar(CP_UTF8, 0, in, len, lpBuf, len * sizeof(wchar_t));
	out.assign(lpBuf, out_len);

	delete [] lpBuf;
}

void Transcode::ANSI_to_Unicode( const char* in, size_t len, std::wstring& out )
{
	int wbufferlen = (int)::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED, in, (int)len, NULL, 0);
	wchar_t* lpwbuffer = new wchar_t[wbufferlen+4];

	wbufferlen = (int)::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED, in, (int)len, lpwbuffer, wbufferlen+2);
	lpwbuffer[wbufferlen] = '\0';
	out.assign( lpwbuffer, wbufferlen );
	delete[] lpwbuffer;
}

void Transcode::Unicode_to_ANSI(const wchar_t* in, size_t len, std::string& out)
{
	int bufferlen = (int)::WideCharToMultiByte(CP_ACP,0,in,(int)len,NULL,0,NULL,NULL);
	char* pBuffer = new char[bufferlen + 4];
	if ( NULL == pBuffer )
	{
		return;
	}
	int out_len = ::WideCharToMultiByte(CP_ACP,0,in,(int)len,pBuffer,bufferlen+2,NULL,NULL);   
	pBuffer[bufferlen] = '\0';
	out.assign( pBuffer, out_len );
	delete[] pBuffer;
}

void Transcode::UTF8_to_ANSI(const char* in, size_t len, std::string& out)
{
	std::wstring wstr;
	UTF8_to_Unicode(in, len, wstr);
//	wstr += WCHAR('\n');
	Unicode_to_ANSI(wstr.c_str(), wstr.length(), out);
}

void Transcode::ANSI_to_UTF8(const char* in, size_t len, std::string& out)
{
	std::wstring wstr;
	ANSI_to_Unicode(in, len, wstr);
	Unicode_to_UTF8(wstr.c_str(), wstr.length(), out);
}