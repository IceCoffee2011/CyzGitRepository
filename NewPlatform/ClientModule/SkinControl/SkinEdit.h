#ifndef SKIN_EDIT_HEAD_FILE
#define SKIN_EDIT_HEAD_FILE

#pragma once

#include "SkinControlHead.h"
#include "SkinRenderManager.h"

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603�ؼ�����
class SKIN_CONTROL_CLASS CSkinEditAttribute
{
	//6603��ˢ����
public:
	CBrush							m_brEnable;							//6603��ˢ��Դ
	CBrush							m_brDisable;						//6603��ˢ��Դ

	//6603������ɫ
public:
	COLORREF						m_crEnableBK;						//6603������ɫ
	COLORREF						m_crDisableBK;						//6603������ɫ

	//6603������ɫ
public:
	COLORREF						m_crEnableText;						//6603������ɫ
	COLORREF						m_crDisableText;					//6603������ɫ

	//6603�߿���ɫ
public:
	COLORREF						m_crEnableBorad;					//6603�߿���ɫ
	COLORREF						m_crDisableBorad;					//6603�߿���ɫ

	//6603��������
public:
	//6603���캯��
	CSkinEditAttribute();
	//6603��������
	virtual ~CSkinEditAttribute();

	//6603������
public:
	//6603������Դ
	bool Initialization(CSkinRenderManager * pSkinRenderManager);
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603�༭��
class SKIN_CONTROL_CLASS CSkinEdit : public CEdit
{
	//6603��̬����
public:
	static CSkinEditAttribute		m_SkinAttribute;					//6603��������

	//6603��������
public:
	//6603���캯��
	CSkinEdit();
	//6603��������
	virtual ~CSkinEdit();

	//6603�ڲ�����
protected:
	//6603�滭�ؼ�
	VOID DrawControlView(CDC * pDC, COLORREF crColorBorad, COLORREF crColorBK);

	//6603��Ϣ���� 
protected:
	//6603�ػ���Ϣ
    VOID OnNcPaint();
	//6603�滭�ؼ�
	HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

//6603�༭��
class SKIN_CONTROL_CLASS CSkinEditEx : public CSkinEdit
{
	//6603��ˢ����
public:
	CBrush							m_brEnable;							//6603��ˢ��Դ
	CBrush							m_brDisable;						//6603��ˢ��Դ

	//6603������ɫ
public:
	COLORREF						m_crEnableBK;						//6603������ɫ
	COLORREF						m_crDisableBK;						//6603������ɫ

	//6603������ɫ
public:
	COLORREF						m_crEnableText;						//6603������ɫ
	COLORREF						m_crDisableText;					//6603������ɫ

	//6603�߿���ɫ
public:
	COLORREF						m_crEnableBorad;					//6603�߿���ɫ
	COLORREF						m_crDisableBorad;					//6603�߿���ɫ

	//6603��������
public:
	//6603���캯��
	CSkinEditEx();
	//6603��������
	virtual ~CSkinEditEx();

	//6603���ܺ���
public:
	//6603������ɫ
	VOID SetEnableColor(COLORREF crEnableText, COLORREF crEnableBK, COLORREF crEnableBorad);
	//6603������ɫ
	VOID SetDisableColor(COLORREF crDisableText, COLORREF crDisableBK, COLORREF crDisableBorad);

	//6603��Ϣ���� 
protected:
	//6603�ػ���Ϣ
    VOID OnNcPaint();
	//6603�滭�ؼ�
	HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603//6603

#endif