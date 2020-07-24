#pragma once
#include"DATA.h"

// Cdllinjectdlg 对话框

class Cdllinjectdlg : public CDialog
{
	DECLARE_DYNAMIC(Cdllinjectdlg)

public:
	Cdllinjectdlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Cdllinjectdlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Cdllinjectdlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	dllinjectinf inf;
	CEdit edit_dllpath;
	SOCKET sclient;
	afx_msg void OnBnClickedButton1();
};
