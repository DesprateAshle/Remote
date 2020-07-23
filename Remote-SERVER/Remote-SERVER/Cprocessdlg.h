#pragma once
#include<iostream>
#include<vector>
#include"Cdlldlg.h"
// Cprocessdlg 对话框

class Cprocessdlg : public CDialog
{
	DECLARE_DYNAMIC(Cprocessdlg)

public:
	Cprocessdlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Cprocessdlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Cprocessdlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl LIST_PROCESS;
	virtual void OnCancel();
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDLLview();


public:
	Cdlldlg* pdlldlg = NULL;
	SOCKET sclient;
	afx_msg void Onprocesskill();
};
