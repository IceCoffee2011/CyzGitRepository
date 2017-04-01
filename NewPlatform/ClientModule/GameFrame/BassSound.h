#ifndef BASS_SOUND_HEAD_FILE
#define BASS_SOUND_HEAD_FILE

#include "bass.h"
#include "GameFrameHead.h"

class CBassSound
{
	//静态变量
public:
	static CBassSound *				m_pBassSound;						//对象指针

	//函数定义
public:
	//构造函数
	CBassSound();
	//析构函数
	virtual ~CBassSound();

	//功能函数
public:
	//创建环境
	bool CreateBassSound(HWND hWnd);

	//声音播放
public:
	//停止声音
	bool StopSound();
	//播放声音
	bool PlaySound(LPCTSTR pszFileName, bool bRepetition);
	//播放声音
	bool PlaySound(HINSTANCE hInstance, LPCTSTR pszResource, LPCTSTR pszTypeName, bool bRepetition);

	//内部函数
// private:
// 	//获取对象
// 	bool CreateSoundBuffer(CD3DSoundBuffer * & pD3DSoundBuffer);
// 	//删除对象
// 	bool DeleteSoundBuffer(CD3DSoundBuffer * & pD3DSoundBuffer);

	//静态函数
public:
	//获取对象
	static CBassSound * GetInstance() { return m_pBassSound; }
};

#endif
