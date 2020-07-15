#pragma once


// CscreenDlg 对话框

class CscreenDlg : public CDialog
{
	DECLARE_DYNAMIC(CscreenDlg)

public:
	CscreenDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CscreenDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CscreenDlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	bool showscreen(char* pbuf, int bufsize);
	afx_msg void OnBnClickedButton1();
public:
	bool isclose;
	virtual void OnCancel();
};
