#pragma once


// Cfiletransferdlg 对话框

class Cfiletransferdlg : public CDialog
{
	DECLARE_DYNAMIC(Cfiletransferdlg)

public:
	Cfiletransferdlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Cfiletransferdlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Cfiletransferdlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit edit_sourcefile;
	CEdit edit_desfile;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
public:
	SOCKET s=0;
};
