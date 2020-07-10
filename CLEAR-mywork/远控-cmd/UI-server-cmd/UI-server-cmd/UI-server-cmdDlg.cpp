
// UI-server-cmdDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "UI-server-cmd.h"
#include "UI-server-cmdDlg.h"
#include "afxdialogex.h"
#include<thread>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUIservercmdDlg 对话框



CUIservercmdDlg::CUIservercmdDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UISERVERCMD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUIservercmdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT3, edit_post);
	DDX_Control(pDX, IDC_EDIT1, edit_output);
	DDX_Control(pDX, IDC_BUTTON1, edit_command);
	DDX_Control(pDX, IDC_EDIT2, edit_order);
}

BEGIN_MESSAGE_MAP(CUIservercmdDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT3, &CUIservercmdDlg::OnEnChangeEdit3)
	ON_BN_CLICKED(IDC_BUTTON2, &CUIservercmdDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CUIservercmdDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CUIservercmdDlg 消息处理程序

BOOL CUIservercmdDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	::SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);



	
	CUIservercmdApp* app = (CUIservercmdApp*)AfxGetApp(); //生成指向应用程序类的指针
	app->wVersionRequested = MAKEWORD(2, 2);

	app->err = WSAStartup(app->wVersionRequested, &app->wsaData);
	if (app->err != 0) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return 0;
	}

	//1.socket
	app->s = socket(AF_INET, SOCK_STREAM, 0);


	//2.bind/listen
	//sockaddr addr;
	app->length = sizeof(sockaddr_in);
	app->addr.sin_family = AF_INET;
	app->addr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUIservercmdDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUIservercmdDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUIservercmdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUIservercmdDlg::OnEnChangeEdit3()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

class SR
{
public:
	CUIservercmdDlg* dlg;
	CUIservercmdApp* app;
};

DWORD WINAPI recvthread(LPVOID lpParam)
{
	CUIservercmdApp* app = (CUIservercmdApp*)AfxGetApp(); //生成指向应用程序类的指针
	CUIservercmdDlg* dlg = (CUIservercmdDlg*)lpParam;

	while (TRUE)
	{
		int datasize = 0;
		char rbuf[256] = { 0 };
		datasize = recv(app->sclient, rbuf, 256, 0);

		if (datasize > 0)
		{
			dlg->edit_output.SetSel(-1);
			dlg->edit_output.ReplaceSel((CString)rbuf);
		}
		else if (datasize == 0) break;
		else if (datasize < 0)
		{
			MessageBox(0, _T("客户端断开连接!!"),_T("警告"),0);
			break;
		}
	}
	return 0;

}

DWORD WINAPI acceptthread(LPVOID lpParam)
{
	CUIservercmdApp* app = (CUIservercmdApp*)AfxGetApp(); //生成指向应用程序类的指针

	app->clientaddr = { 0 };
	app->clientaddr.sin_family = AF_INET;
	int addrlen = sizeof(sockaddr_in);


	app->sclient = accept(app->s, (sockaddr*)&app->clientaddr, &addrlen);
	MessageBox(0,_T("端口绑定成功"), _T("成功"),0);
	
	CreateThread(0, 0, recvthread, lpParam, 0, 0);

	return 0;
}


void CUIservercmdDlg::OnBnClickedButton2()
{
	
	CUIservercmdApp* app = (CUIservercmdApp*)AfxGetApp(); //生成指向应用程序类的指针

	CString getpost;
	edit_post.GetWindowTextW(getpost);
	app->post = _ttoi(getpost);
	app->addr.sin_port = htons(app->post);
	bind(app->s, (sockaddr*)&app->addr, app->length);


	listen(app->s, 5);

	edit_post.EnableWindow(FALSE);

	CreateThread(NULL, 0, acceptthread, this, 0, 0);

	
	// TODO: 在此添加控件通知处理程序代码
}


void CUIservercmdDlg::OnBnClickedButton1()
{
	CUIservercmdApp* app = (CUIservercmdApp*)AfxGetApp(); //生成指向应用程序类的指针

	SR sr;
	sr.dlg = this;
	sr.app = app;
	Sleep(300);


	CString getcommand;
	char wbuf[256] = {0};
	edit_order.GetWindowText(getcommand);
	int length= getcommand.GetLength();
	for (int i = 0; i < length; i++)
	{
		wbuf[i] = getcommand[i];
	}

	int getdate = send(app->sclient, wbuf, sizeof(wbuf), 0);


	if (getdate <= 0)
	{
		MessageBox(_T("请输入命令!!!"), _T("send error!!!"));
	}


	
	// TODO: 在此添加控件通知处理程序代码
}


BOOL CUIservercmdDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			return TRUE;

		case VK_ESCAPE:
			return TRUE;

		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
