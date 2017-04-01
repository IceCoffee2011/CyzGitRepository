#ifndef BASS_SOUND_HEAD_FILE
#define BASS_SOUND_HEAD_FILE

#include "bass.h"
#include "GameFrameHead.h"

class CBassSound
{
	//��̬����
public:
	static CBassSound *				m_pBassSound;						//����ָ��

	//��������
public:
	//���캯��
	CBassSound();
	//��������
	virtual ~CBassSound();

	//���ܺ���
public:
	//��������
	bool CreateBassSound(HWND hWnd);

	//��������
public:
	//ֹͣ����
	bool StopSound();
	//��������
	bool PlaySound(LPCTSTR pszFileName, bool bRepetition);
	//��������
	bool PlaySound(HINSTANCE hInstance, LPCTSTR pszResource, LPCTSTR pszTypeName, bool bRepetition);

	//�ڲ�����
// private:
// 	//��ȡ����
// 	bool CreateSoundBuffer(CD3DSoundBuffer * & pD3DSoundBuffer);
// 	//ɾ������
// 	bool DeleteSoundBuffer(CD3DSoundBuffer * & pD3DSoundBuffer);

	//��̬����
public:
	//��ȡ����
	static CBassSound * GetInstance() { return m_pBassSound; }
};

#endif
