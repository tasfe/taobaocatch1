
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

	CString strUrl,strContent;

	strUrl= _T("http://gw.api.taobao.com/router/rest?sign=3DDB776F4B44EE2CBA6FC3A4CB988CD6&timestamp=2012-02-17+14%3A56%3A33&v=2.0&app_key=12129701&method=taobao.items.search&partner_id=top-apitools&format=xml&q=%E6%89%8B%E6%9C%BA&fields=num_iid,title,nick,pic_url,cid,price,type,delist_time,post_fee,score,volume");
	if(HttpGet(strUrl,strContent) != 0)
	{
		MessageBox(strContent);
	}
	

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

DWORD CCatchDataDlg::HttpGet(CString strUrl , CString &strContent)
{
	if(strUrl.IsEmpty())
		return 0;

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
		return 0;
	}

	const int iLen = strlen(ReadBuf);
	WCHAR *pstrWide = new WCHAR[iLen];
	MultiByteToWideChar(CP_UTF8,0,ReadBuf,-1,pstrWide,iLen);
	strContent = pstrWide;


	CPacket inPacket;
	DOMElement* TransNode = NULL;
	DOMElement* AccNode = NULL;
	inPacket.BuiltTree(ReadBuf,iLen);
	AccNode = inPacket.SearchElement("/items_search_response/item_search/item_categories/item_category");
	inPacket.SetCurrentElement(AccNode);
	//while((AccNode = inPacket.SearchNextElement()) != NULL)
	CStringA str;
	CStringA strMsg;
	while(AccNode)
	{
		
		ElementList::iterator it;
		for(it = AccNode->m_children.begin();it != AccNode->m_children.end();it++)
		{
			str += (*it)->get_tag().c_str();
			str += (*it)->getTextContent();
		}
		
		strMsg+=str;
		AccNode = inPacket.SearchNextElement(true);
	}

	int i = strMsg.GetLength();
	MessageBoxA(NULL,strMsg.GetBuffer(0),NULL,MB_OK);
	

	
	delete [] pstrWide;
	delete [] ReadBuf;	

	return 1;
}
