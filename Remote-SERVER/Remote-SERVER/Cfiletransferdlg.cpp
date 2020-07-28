// Cfiletransferdlg.cpp: 实现文件
//

#include "pch.h"
#include "Remote-SERVER.h"
#include "Cfiletransferdlg.h"
#include "afxdialogex.h"
#include<iostream>
#include"DATA.h"
using namespace std;
// Cfiletransferdlg 对话框

IMPLEMENT_DYNAMIC(Cfiletransferdlg, CDialog)

Cfiletransferdlg::Cfiletransferdlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_Cfiletransferdlg, pParent)
{

}

Cfiletransferdlg::~Cfiletransferdlg()
{
}

void Cfiletransferdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, edit_sourcefile);
	DDX_Control(pDX, IDC_EDIT2, edit_desfile);
}


BEGIN_MESSAGE_MAP(Cfiletransferdlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &Cfiletransferdlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &Cfiletransferdlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// Cfiletransferdlg 消息处理程序


void Cfiletransferdlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	if (s != 0)
	{
		CString sourcefilepath;
		CString desfilepath;
		edit_sourcefile.GetWindowText(sourcefilepath);
		edit_desfile.GetWindowText(desfilepath);

		/*char* ss = new char[128];
		memset(ss, 0, 128);
		for (int i = 0; i < 100; i++)
		{
			ss[i] = '5';
		}
		senddata(s, SERVER_FILE_UP, ss, 128);*/
		CFile sourcefile;
		BOOL bret=sourcefile.Open(sourcefilepath, CFile::modeRead, NULL);

		if (!bret)
		{
			MessageBox("源文件路径错误");
			return;
		}
		filedatastruct *fdatastruct = new filedatastruct;
		memset(fdatastruct, 0, sizeof(filedatastruct));
		int ret=0;
		for (int i = 0; i < desfilepath.GetLength(); i++)
		{
			fdatastruct->despath[i] = desfilepath[i];
		}
		while (true)
		{
			ret=sourcefile.Read(fdatastruct->filedata, TRANSFER_SIZE);
			fdatastruct->datasize = ret;
			bool bret = senddata(s, SERVER_FILE_UP, (char*)fdatastruct, 520+ret);
			if (ret < TRANSFER_SIZE) break;
		}
		delete fdatastruct;
		sourcefile.Close();
	}
}


void Cfiletransferdlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString sourcefilepath;
	CString desfilepath;
	edit_sourcefile.GetWindowText(sourcefilepath);
	edit_desfile.GetWindowText(desfilepath);

	filedatastruct *fdatastruct = new filedatastruct;
	for (int i = 0; i < sourcefilepath.GetLength(); i++)
	{
		fdatastruct->sourcepath[i] = sourcefilepath[i];
	}
	for (int i = 0; i < desfilepath.GetLength(); i++)
	{
		fdatastruct->despath[i] = desfilepath[i];
	}

	senddata(s, CLIENT_FILE_DOWN,(char*)fdatastruct , 512);
	delete fdatastruct;
	
}
