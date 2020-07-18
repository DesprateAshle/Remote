#pragma once


// Ckeyboarddlg 对话框

class Ckeyboarddlg : public CDialog
{
	DECLARE_DYNAMIC(Ckeyboarddlg)

public:
	Ckeyboarddlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~Ckeyboarddlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Ckeyboarddlg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit edit_keyboard_record;
};
