#pragma once

#include<thread>
// Ccmddlg 对话框

class Ccmddlg : public CDialog
{
	DECLARE_DYNAMIC(Ccmddlg)

public:
	Ccmddlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Ccmddlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Ccmddlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit edit_input;
	CEdit edit_output;
	afx_msg void OnBnClickedButton1();

public:
	char *buf= NULL;
	SOCKET sclient;
};
