
// Remote-SERVERDlg.h: 头文件
//

#pragma once
#include<map>
#include"Mysession.h"
#include<mutex>
#include"resource.h"
#include"CscreenDlg.h"
// CRemoteSERVERDlg 对话框
class CRemoteSERVERDlg : public CDialogEx
{
// 构造
public:
	CRemoteSERVERDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTESERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl list_client;

public:
	//自写变量

	SOCKET sserver; //服务端套接字
	HANDLE hclient;	//接受客户端的线程句柄
	std::map<SOCKET, Mysession*> map_session;
	std::mutex accept_mutex;

public:
	//自写函数

	void initialization();
	static DWORD WINAPI acceptthread(LPVOID lparam);

public:
	afx_msg void OnNMRClickList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnScreen();

	afx_msg void OnCmd();
	afx_msg void OnKeyboard();
};
