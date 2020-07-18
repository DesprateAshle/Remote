// Ccmddlg.cpp: 实现文件
//

#include "pch.h"
#include "Remote-SERVER.h"
#include "Ccmddlg.h"
#include "afxdialogex.h"
#include"DATA.h"

// Ccmddlg 对话框

IMPLEMENT_DYNAMIC(Ccmddlg, CDialog)

Ccmddlg::Ccmddlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_Ccmddlg, pParent)
{

}

Ccmddlg::~Ccmddlg()
{
}

void Ccmddlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, edit_input);
	DDX_Control(pDX, IDC_EDIT2, edit_output);
}


BEGIN_MESSAGE_MAP(Ccmddlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &Ccmddlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// Ccmddlg 消息处理程序


void Ccmddlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString getcommand;
	char wbuf[256] = { 0 };
	edit_input.GetWindowText(getcommand);
	int length = getcommand.GetLength();
	for (int i = 0; i < length; i++)
	{
		wbuf[i] = getcommand[i];
	}

	senddata(sclient, SEVER_CMD_COMMAND, wbuf, length);
}
