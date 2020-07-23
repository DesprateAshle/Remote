// Cprocessdlg.cpp: 实现文件
//

#include "pch.h"
#include "Remote-SERVER.h"
#include "Cprocessdlg.h"
#include "afxdialogex.h"
#include"DATA.h"
#include<vector>

// Cprocessdlg 对话框

IMPLEMENT_DYNAMIC(Cprocessdlg, CDialog)

Cprocessdlg::Cprocessdlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_Cprocessdlg, pParent)
{

}

Cprocessdlg::~Cprocessdlg()
{

}

void Cprocessdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, LIST_PROCESS);
}


BEGIN_MESSAGE_MAP(Cprocessdlg, CDialog)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &Cprocessdlg::OnNMRClickList1)
	ON_COMMAND(ID_32780, &Cprocessdlg::OnDLLview)
	ON_COMMAND(ID_32781, &Cprocessdlg::Onprocesskill)
END_MESSAGE_MAP()


// Cprocessdlg 消息处理程序


void Cprocessdlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	this->LIST_PROCESS.DeleteAllItems();
	CDialog::OnCancel();
}


void Cprocessdlg::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu menu;
	menu.LoadMenu(IDR_MENU2);
	CMenu* psubmenu = menu.GetSubMenu(0);


	//获取鼠标当前坐标
	CPoint cp;
	GetCursorPos(&cp);

	psubmenu->TrackPopupMenu(TPM_LEFTALIGN, cp.x, cp.y, this);


	*pResult = 0;
}


void Cprocessdlg::OnDLLview()
{
	// TODO: 在此添加命令处理程序代码

	int pos = this->LIST_PROCESS.GetSelectionMark();


	DWORD dwpid=LIST_PROCESS.GetItemData(pos);
	senddata(this->sclient, SERVER_DLLLIST_VIEW, (char*)&dwpid, sizeof(dwpid));

	if (this->pdlldlg == NULL)
	{
		this->pdlldlg = new Cdlldlg;
		this->pdlldlg->Create(IDD_Cdlldlg, this);

		this->pdlldlg->list_dll.InsertColumn(0, "DLL");

		this->pdlldlg->list_dll.SetColumnWidth(0, 1000);

		this->pdlldlg->list_dll.SetExtendedStyle(this->pdlldlg->list_dll.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);//设置风格
	}
	this->pdlldlg->ShowWindow(SW_SHOW);

}


void Cprocessdlg::Onprocesskill()
{
	// TODO: 在此添加命令处理程序代码
	int pos = this->LIST_PROCESS.GetSelectionMark();
	CString cstrprocess = this->LIST_PROCESS.GetItemText(pos, 0);
	cstrprocess = "taskkill -f -im " + cstrprocess;
	char process[128] = { 0 };
	for (int i = 0; i < cstrprocess.GetLength(); i++)
	{
		process[i] = cstrprocess[i];
	}
	senddata(sclient, SEVER_CMD_COMMAND, process, strlen(process));
}
