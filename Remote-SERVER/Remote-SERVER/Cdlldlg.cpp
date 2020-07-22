// Cdlldlg.cpp: 实现文件
//

#include "pch.h"
#include "Remote-SERVER.h"
#include "Cdlldlg.h"
#include "afxdialogex.h"

// Cdlldlg 对话框

IMPLEMENT_DYNAMIC(Cdlldlg, CDialog)

Cdlldlg::Cdlldlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_Cdlldlg, pParent)
{

}

Cdlldlg::~Cdlldlg()
{
}

void Cdlldlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, list_dll);
}


BEGIN_MESSAGE_MAP(Cdlldlg, CDialog)
END_MESSAGE_MAP()


// Cdlldlg 消息处理程序


void Cdlldlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	this->list_dll.DeleteAllItems();


	CDialog::OnCancel();
}
