#ifndef DLG_PROFILE_HEAD_FILE
#define DLG_PROFILE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "WndValidateCode.h"
#include "SecurityQuestionsControl.h"

//////////////////////////////////////////////////////////////////////////////////

//注册窗口
class CDlgProfile : public CDialog
{
	//变量定义
public:
	WORD							m_wFaceID;									//头像标识
	BYTE							m_cbGender;									//用户性别
	TCHAR							m_szAccounts[LEN_ACCOUNTS];					//注册帐号
	TCHAR							m_szNickName[LEN_NICKNAME];					//注册昵称
	TCHAR							m_szMobilePhone[LEN_MOBILE_PHONE];			//移动电话
	TCHAR							m_szUnderWrite[LEN_UNDER_WRITE];			//个性签名
	TCHAR							m_szQQ[LEN_QQ];								//Q Q 号码
	TCHAR							m_szPassPortID[LEN_PASS_PORT_ID];			// 证件号码
	TCHAR							m_szCompellation[LEN_COMPELLATION];			// 真实姓名
	TCHAR							m_szSecurityQuestion[LEN_UNDER_WRITE];		// 密保问题
	TCHAR							m_szSecurityAnswer[LEN_UNDER_WRITE];		// 密保回答

	//IDC_EDIT_QQ

	//TCHAR							m_szSpreader[LEN_ACCOUNTS];			//推荐帐号
	//TCHAR							m_szLogonPass[LEN_PASSWORD];		//登录密码
	//TCHAR							m_szInsurePass[LEN_PASSWORD];		//银行密码
	//TCHAR							m_szPassPortID[LEN_PASS_PORT_ID];	//证件号码
	//TCHAR							m_szCompellation[LEN_COMPELLATION];	//真实名字
	BOOL							m_bCheckNickFlag;					//测试昵称 by chenj

	//配置变量
protected:
	BYTE							m_cbRemPassword;					//记住密码
	BYTE							m_cbLogonPassLevel;					//密码强度
	BYTE							m_cbInsurePassLevel;				//密码强度

	//控件变量
public:
	//CSkinEdit						m_edAccounts;						//注册帐号
	CSkinEdit						m_edNickName;						//注册昵称
	//CSkinEdit						m_edSpreader;						//推荐用户
	//CSkinEdit						m_edPassPortID;						//身份证号
	//CSkinEdit						m_edLogonPass1;						//登录密码
	//CSkinEdit						m_edLogonPass2;						//登录密码
	//CSkinEdit						m_edInsurePass1;					//银行密码
	//CSkinEdit						m_edInsurePass2;					//银行密码
	//CSkinEdit						m_edCompellation;					//真实名字
	//CSkinEdit						m_edValidateCode;					//验证数字
//	CSkinEdit						m_edMobilePhone;					//手机号码
	CSkinEdit						m_edQQ;								//qq
//	CSkinEdit						m_edUnderWrite;						//个性签名
	CSkinEdit						m_edPassPortID;						// 身份证号
	CSkinEdit						m_edCompellation;					// 真实姓名

	CSecurityQuestionsCtrl			m_cmSecurityQuestions;				// 密保问题
	CSkinEdit						m_edSecurityAnswer;					// 密保答案

	//控件变量
protected:
	CSkinButton						m_btQuit;							//取消按钮
	CSkinButton						m_btEnter;							//注册按钮
	CSkinButton						m_btTestNickname;					//检测Nickname
	CSkinButton						m_btCancel;							//关闭按钮
	CSkinButton						m_btSelectFace;						//选择按钮

	//控件变量
protected:
	CBrush							m_brBrush;							//背景画刷
	CFaceItemView					m_FaceItemView;						//头像视图
	CSkinHyperLink					m_ChangeValidate;					//更换验证

	//组件变量
protected:
	CWndValidateCode				m_WndValidateCode;					//验证控件
	CFaceSelectControlHelper		m_FaceSelectControl;				//头像选择

	//函数定义
public:
	//构造函数
	CDlgProfile();
	//析构函数
	virtual ~CDlgProfile();

	//重载函数
public:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//配置函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();
	//取消函数
	virtual VOID OnCancel();

	//功能函数
public:
	//构造数据
	WORD ConstructProfilePacket(BYTE cbBuffer[], WORD wBufferSize, WORD & wSubCmdID, BYTE cbValidateFlags);

	//辅助函数
private:
	//获取信息
	bool GetInformation();
	//获取字符
	VOID GetControlString(UINT uControlID, TCHAR szString[], WORD wMaxCount);

	//消息映射
public:
	//关闭按钮
	VOID OnBnClickedQuit();
	//密码输入
	VOID OnEnChangeLogonPass();
	//密码输入
	VOID OnEnChangeInsurePass();
	//头像选择
	VOID OnBnClickedSelectFace();
	//更换验证
	VOID OnBnClickedChangeValidate();
	//检测昵称
	VOID OnBnClickedCheckNick();

	//系统消息
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif