// CscreenDlg.cpp: 实现文件
//

#include "pch.h"
#include "Remote-SERVER.h"
#include "CscreenDlg.h"
#include "afxdialogex.h"
#include"DATA.h"

// CscreenDlg 对话框

IMPLEMENT_DYNAMIC(CscreenDlg, CDialog)

CscreenDlg::CscreenDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CscreenDlg, pParent)
{

}

CscreenDlg::~CscreenDlg()
{
}

void CscreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CscreenDlg, CDialog)
	
END_MESSAGE_MAP()


// CscreenDlg 消息处理程序


bool CscreenDlg::showscreen(char* pbuf, int bufsize)
{
	//获取屏幕数据并且显示到屏幕监控的对话框

	screendata* pscreendata = (screendata*)pbuf;

	CDC serverdc;  //创建内存DC
	CBitmap serverbmp;    //创建兼容位图

	serverdc.CreateCompatibleDC(GetDC());
	serverbmp.CreateCompatibleBitmap(GetDC(), pscreendata->width, pscreendata->height);

	serverdc.SelectObject(serverbmp);
	serverbmp.SetBitmapBits(bufsize - 8, pscreendata->data);

	GetDC()->BitBlt(0, 0, pscreendata->width, pscreendata->height, &serverdc, 0, 0, SRCCOPY);

	UpdateWindow();

	return true;
}




void CscreenDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	isclose = false;
	CDialog::OnCancel();
}
