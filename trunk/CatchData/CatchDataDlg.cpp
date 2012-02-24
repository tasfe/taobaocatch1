
// CatchDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CatchData.h"
#include "CatchDataDlg.h"
#include "afxdialogex.h"

#include "WebAccess.h"
#include "Xml\Packet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCatchDataDlg dialog




CCatchDataDlg::CCatchDataDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCatchDataDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_StaticErr = _T("");
}

void CCatchDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_ERROR, m_StaticErr);
	DDX_Control(pDX, IDC_LIST_DATA, m_ListData);
}

BEGIN_MESSAGE_MAP(CCatchDataDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CCatchDataDlg message handlers

BOOL CCatchDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_ListData.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_ListData.InsertColumn(0,_T("标  题"),LVCFMT_LEFT,500);
	m_ListData.InsertColumn(1,_T("邮  费"),LVCFMT_LEFT,60);
	m_ListData.InsertColumn(2,_T("价  格"),LVCFMT_LEFT,60);
	m_ListData.InsertColumn(3,_T("店铺名"),LVCFMT_LEFT,100);
	
	/*
	m_ListData.InsertItem(0,_T("测试机"));
	m_ListData.SetItemText(0,1,_T("邮费10"));
	m_ListData.SetItemText(0,2,_T("价格10"));

	m_ListData.InsertItem(1,_T("测试机"));
	m_ListData.SetItemText(1,1,_T("邮费10"));
	m_ListData.SetItemText(1,2,_T("价格10"));

	m_ListData.InsertItem(2,_T("测试机"));
	m_ListData.SetItemText(2,1,_T("邮费10"));
	m_ListData.SetItemText(2,2,_T("价格10"));

	return TRUE;
	*/
	

	CString strUrl;
	CList<ItemsData,ItemsData&> list;
		
	//strUrl= _T("http://gw.api.taobao.com/router/rest?sign=D4F615F7606E0BFF6A4C71850F46C84C&timestamp=2012-02-21+10%3A09%3A26&v=2.0&app_key=12129701&method=taobao.items.search&partner_id=top-apitools&format=xml&q=%E5%86%85%E5%AD%98&fields=num_iid,title,nick,pic_url,cid,price,type,delist_time,post_fee,score,volume");
	
	char strUTF8[128]={0};
	WideCharToMultiByte(CP_UTF8,0,L"内存",-1,strUTF8,128,NULL,NULL);

	CString strSearch;
	for(int i=0;i<strlen(strUTF8);i++)
	{
		TCHAR strHex[8]={0};
		swprintf_s(strHex,8,_T("%%%02X"),(BYTE)strUTF8[i]);
		strSearch +=strHex;
	}

	for(int iPage = 0;iPage<1;iPage++)
	{
		strUrl.Format(_T("http://gw.api.taobao.com/router/rest?sign=5232075A217C9F02A0C942FB4D6C0EE8&")\
			_T("timestamp=2012-02-23+15%%3A01%%3A04&v=2.0&app_key=12129701&method=taobao.items.search&partner_id=top-apitools&")\
			_T("format=xml&q=%s&page_no=%d&fields=num_iid,title,nick,pic_url,cid,price,type,delist_time,post_fee,score,volume"),
			strSearch,iPage);			

		/*
		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);

		strUrl.Format(_T("http://gw.api.taobao.com/router/rest?sign=60EB76D1E6363B18EEC71F14E1E2FCB9&")\
		_T("timestamp=%d-%02d-%02d+15%%3A00%%3A46&v=2.0&app_key=12129701&method=taobao.items.search&partner_id=top-apitools&")\
		_T("format=xml&q=%s&page_no=%d&fields=num_iid,title,nick,pic_url,cid,price,type,delist_time,post_fee,score,volume"),
		sysTime.wYear,sysTime.wMonth,sysTime.wDay-1,strSearch,iPage);
		*/

		//MessageBox(strUrl);

		HttpGetData(strUrl,list);

		while( !list.IsEmpty() )
		{
			CString str;
			ItemsData item = list.GetHead();
			list.RemoveHead();

			const int iRow = m_ListData.GetItemCount();

			m_ListData.InsertItem(iRow,item.strTitle);
			str.Format(_T("%.2f"),item.fPost_fee);
			m_ListData.SetItemText(iRow,1,str);
			str.Format(_T("%.2f"),item.fPrice);
			m_ListData.SetItemText(iRow,2,str);
			m_ListData.SetItemText(iRow,3,item.strNick);

		}
	}
	
	CString str;
	str.Format(_T("%d 条记录"),m_ListData.GetItemCount());
	m_StaticErr = str;
	UpdateData(FALSE);
	/*
	const int iSize = list.GetSize();
	for(int i = 0;i<iSize;i++)
	{
		
		//ItemsData itme = list.GetAt(i);
	
	}

	*/
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCatchDataDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCatchDataDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCatchDataDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD CCatchDataDlg::HttpGetData(CString strUrl , CList<ItemsData,ItemsData&>  &listItems)
{
	BOOL bRet = TRUE;

	if(strUrl.IsEmpty())
		return FALSE;

	CWebAccess web;
	char *ReadBuf = NULL;

	DWORD dwRet = web.Get(strUrl,&ReadBuf);
	if( dwRet != WEB_ACCESS_DONE)
	{
		m_StaticErr.Format(_T("HttpGet is Error Code:%d"),dwRet);
		if(ReadBuf != NULL)
		{
			m_StaticErr += ReadBuf;
			delete [] ReadBuf;
		}

		return FALSE;
	}

	
	WCHAR strWide[512]={0};
	//MultiByteToWideChar(CP_UTF8,0,ReadBuf,-1,pstrWide,iLen);

	CPacket inPacket;
	DOMElement* TransNode = NULL;
	DOMElement* AccNode = NULL;
	inPacket.BuiltTree(ReadBuf,strlen(ReadBuf)+1);
	AccNode = inPacket.SearchElement("/items_search_response/item_search/items/item");
	inPacket.SetCurrentElement(AccNode);
	
	if(NULL == AccNode)
	{
	        bRet = FALSE;//错误代码返回
	}


	while(AccNode)
	{
		ElementList::iterator it;
		ItemsData items;
		int i = 0;
		for(it = AccNode->m_children.begin();it != AccNode->m_children.end();it++)
		{	
			i++;

			switch(i)			
			{
			case 1:    //cid
				{
					items.dwCid = atoi((*it)->getTextContent());
					break;				
				}
			case 2:    //delist_time
				{					
					break;				
				}
			case 3:    //nick
				{
					MultiByteToWideChar(CP_UTF8,0,(*it)->getTextContent(),-1,strWide,510);
					items.strNick = strWide;
					break;				
				}
			case 4:    //num_iid
				{
					items.dwNum_iid = 0;
					//测试取会值太大atoi函数溢出
					//items.dwNum_iid = (DWORD)atoi((*it)->getTextContent());
					break;				
				}
			case 5:    //pic_url
				{					
					break;
				}
			case 6:    //post_fee
				{
					items.fPost_fee = atof((*it)->getTextContent());
					break;
				}
			case 7:    //price
				{
					items.fPrice = atof((*it)->getTextContent());
					break;				
				}
			case 8:    //score
				{
					items.dwScore = atoi((*it)->getTextContent());
					break;
				}
			case 9:   //title
				{
					MultiByteToWideChar(CP_UTF8,0,(*it)->getTextContent(),-1,strWide,510);
					items.strTitle = strWide;
					//去掉<span> 标签
					items.strTitle.Replace(_T("&lt;span class=H&gt;"),_T("@"));
					items.strTitle.Replace(_T("&lt;/span&gt;"),_T("@"));

					break;
				}
			case 10:    //type
				{
					MultiByteToWideChar(CP_UTF8,0,(*it)->getTextContent(),-1,strWide,510);
					items.strType = strWide;
					break;
				}

			case 11:    //volume
				{
					items.dwVolume = atoi((*it)->getTextContent());
					break;
				}
			
			}
		}

		listItems.AddTail(items);
		
		AccNode = inPacket.SearchNextElement(true);
	}
	
	delete [] ReadBuf;	

	return bRet;
}
