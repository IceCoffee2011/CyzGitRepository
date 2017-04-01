#include "../StdAfx.h"
#include "../BassSound.h"

//////////////////////////////////////////////////////////////////////////////////
//静态变量

CBassSound* CBassSound::m_pBassSound=NULL;					// 播放声音组件

CBassSound::CBassSound()
{
	ASSERT(m_pBassSound==NULL);
	if (m_pBassSound==NULL)
	{
		m_pBassSound=this;
	}
}

CBassSound::~CBassSound()
{
	BASS_Free();
}

//创建环境
bool CBassSound::CreateBassSound(HWND hWnd)
{
	if (HIWORD(BASS_GetVersion())!=BASSVERSION)
	{
		ASSERT(FALSE);
		return false;
	}

	BOOL flag = FALSE;
	BASS_DEVICEINFO info;
	for (int a=0; BASS_GetDeviceInfo(a, &info); a++)
	{
		if (info.flags&BASS_DEVICE_ENABLED) // device is enabled
		{
			flag = BASS_Init(info.flags, 44100, BASS_DEVICE_3D, hWnd, NULL);

			if (!flag)
			{
				int errcode =  BASS_ErrorGetCode();				
				continue;
			}
			else
			{
				break;
			}
		}
	}

	if (flag == FALSE)
	{
		ASSERT(FALSE);
		return false;
	}
	BASS_SetConfig(BASS_CONFIG_ASYNCFILE_BUFFER, 4096*256*10);

	return true;
}

//停止声音
bool CBassSound::StopSound()
{
	BOOL flag = BASS_Stop();
	return flag==TRUE ? true : false;
}

//播放声音
bool CBassSound::PlaySound(LPCTSTR pszFileName, bool bRepetition)
{
//	ASSERT(FALSE);
	DWORD flags = BASS_STREAM_AUTOFREE;
 	if (bRepetition)
 	{
 		flags |= BASS_SAMPLE_LOOP;
 	}
 #ifdef _UNICODE
 	flags |= BASS_UNICODE;
 #endif
	HSTREAM hs = BASS_StreamCreateFile(FALSE, pszFileName, 0, 0, flags);
	if (hs == NULL)
	{
		int errcode = BASS_ErrorGetCode();
		ASSERT(FALSE);
		return false;
	}

	if (BASS_ChannelPlay(hs, TRUE) == FALSE)
	{
		int errcode = BASS_ErrorGetCode();
		if (errcode == BASS_ERROR_START)
		{
			BASS_Start();
		}
		if (BASS_ChannelPlay(hs, TRUE) == FALSE)
		{
			ASSERT(FALSE);
			return false;
		}
	}
	return true;
}

//播放声音
bool CBassSound::PlaySound(HINSTANCE hInstance, LPCTSTR pszResource, LPCTSTR pszTypeName, bool bRepetition)
{
//	ASSERT(FALSE);
	//加载资源
	HRSRC hResource=FindResource(hInstance, pszResource, pszTypeName);
	if (hResource==NULL) return false;

	DWORD nResSize = ::SizeofResource(NULL, hResource);

	//锁定资源
	HGLOBAL hGlobal=LoadResource(hInstance,hResource);
	if (hGlobal==NULL) return false;

	// 获取资源
	LPCTSTR pszMemory=(LPCTSTR)LockResource(hGlobal);

	DWORD flags = BASS_STREAM_AUTOFREE;
	if (bRepetition)
	{
		flags |= BASS_SAMPLE_LOOP;
	}
#ifdef _UNICODE
	flags |= BASS_UNICODE;
#endif
	
	HSTREAM hs = BASS_StreamCreateFile(TRUE, pszMemory, 0, nResSize, flags);
	if (hs == NULL)
	{
		int errcode = BASS_ErrorGetCode();
		ASSERT(FALSE);
		//清理资源
		UnlockResource(hGlobal);
		FreeResource(hGlobal);
		return false;
	}

	if (BASS_ChannelPlay(hs, TRUE) == FALSE)
	{
		int errcode = BASS_ErrorGetCode();
		if (errcode == BASS_ERROR_START)
		{
			BASS_Start();
		}
		if (BASS_ChannelPlay(hs, TRUE) == FALSE)
		{
			ASSERT(FALSE);
			//清理资源
			UnlockResource(hGlobal);
			FreeResource(hGlobal);
			return false;
		}
	}

	return true;
}
