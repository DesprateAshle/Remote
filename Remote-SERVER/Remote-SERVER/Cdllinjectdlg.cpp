// Cdllinjectdlg.cpp: 实现文件
//

#include "pch.h"
#include "Remote-SERVER.h"
#include "Cdllinjectdlg.h"
#include "afxdialogex.h"


// Cdllinjectdlg 对话框

IMPLEMENT_DYNAMIC(Cdllinjectdlg, CDialog)

Cdllinjectdlg::Cdllinjectdlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_Cdllinjectdlg, pParent)
{

}

Cdllinjectdlg::~Cdllinjectdlg()
{
}

void Cdllinjectdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, edit_dllpath);
}


BEGIN_MESSAGE_MAP(Cdllinjectdlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &Cdllinjectdlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// Cdllinjectdlg 消息处理程序


void Cdllinjectdlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString dllpath;
	this->edit_dllpath.GetWindowTextA(dllpath);
	for (int i = 0; i < dllpath.GetLength(); i++)
	{
		(this->inf.dllpath)[i] = dllpath[i];
	}
	senddata(sclient, SERVER_DLLPATH_INJECT, (char*)&this->inf, sizeof(this->inf));
}
