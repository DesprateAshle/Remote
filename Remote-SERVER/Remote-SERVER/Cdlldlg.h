#pragma once
#include<vector>
#include<iostream>
// Cdlldlg 对话框

class Cdlldlg : public CDialog
{
	DECLARE_DYNAMIC(Cdlldlg)

public:
	Cdlldlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Cdlldlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Cdlldlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl list_dll;
public:
	std::vector<std::string> dllpath;
	virtual void OnCancel();
};
