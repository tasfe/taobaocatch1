
// CatchDataDlg.h : header file
//

#pragma once

// CCatchDataDlg dialog
class CCatchDataDlg : public CDialogEx
{
// Construction
public:
	CCatchDataDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CATCHDATA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	DWORD HttpGetData(CString strUrl , CList<ItemsData,ItemsData&>  &listItems);
public:
	CString m_StaticErr;	
	CListCtrl m_ListData;
};
