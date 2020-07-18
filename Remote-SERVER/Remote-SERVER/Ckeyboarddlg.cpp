// Ckeyboarddlg.cpp: 实现文件
//

#include "pch.h"
#include "Remote-SERVER.h"
#include "Ckeyboarddlg.h"
#include "afxdialogex.h"


// Ckeyboarddlg 对话框

IMPLEMENT_DYNAMIC(Ckeyboarddlg, CDialog)

Ckeyboarddlg::Ckeyboarddlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_Ckeyboarddlg, pParent)
{

}

Ckeyboarddlg::~Ckeyboarddlg()
{
}

void Ckeyboarddlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, edit_keyboard_record);
}


BEGIN_MESSAGE_MAP(Ckeyboarddlg, CDialog)
END_MESSAGE_MAP()


// Ckeyboarddlg 消息处理程序
