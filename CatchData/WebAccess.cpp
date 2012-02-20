// WebAccess.cpp : implementation file
//

#include "stdafx.h"

#include "WebAccess.h"
#include "Registry.h"


// CWebAccess

CWebAccess::CWebAccess(LPCTSTR pstrAgent /*= NULL*/,
		DWORD dwContext /*= 1*/,
		DWORD dwAccessType /*= PRE_CONFIG_INTERNET_ACCESS*/,
		LPCTSTR pstrProxyName /*= NULL*/,
		LPCTSTR pstrProxyBypass /*= NULL*/,
		DWORD dwFlags /*= 0*/)
		: CInternetSession (pstrAgent, dwContext, dwAccessType, pstrProxyName, 
							pstrProxyBypass, dwFlags)
{
	CRegistry registry;
	registry.SetRootKey(HKEY_CURRENT_USER);
	registry.CreateKey(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings"));
	
	DWORD proxyEnabled;
	proxyEnabled = registry.ReadDword(_T("ProxyEnable"), 0);
	if( proxyEnabled )
	{
		m_bProxyEnable = TRUE;
		registry.ReadString(_T("ProxyOverride"), m_szProxyOverride);
		registry.ReadString(_T("ProxyServer"), m_szProxyServer);
	}
	else
	{
		m_bProxyEnable = FALSE;	

		m_szProxyOverride = "";
		m_szProxyServer = "";
	}
}

CWebAccess::~CWebAccess()
{
	this->Close();
}


// CWebAccess member functions
void CWebAccess::OnStatusCallback(DWORD  dwContext , DWORD dwInternetStatus,
	LPVOID  lpvStatusInformation , DWORD  dwStatusInformationLength )
{
    // Status callbacks need thread-state protection. 
    AFX_MANAGE_STATE(AfxGetAppModuleState( ));

    CString strStatus;

	switch (dwInternetStatus)
	{
	case INTERNET_STATUS_RESOLVING_NAME:
		strStatus.Format(_T("Resolving name %s"), lpvStatusInformation);
		break;

	case INTERNET_STATUS_NAME_RESOLVED:
		strStatus.Format(_T("Name resolved %s"), lpvStatusInformation);
		break;

	case INTERNET_STATUS_HANDLE_CREATED:
		strStatus.Format(_T("Handle created"));
		break;

	case INTERNET_STATUS_CONNECTING_TO_SERVER:
		strStatus.Format(_T("Connecting to socket address"));
		break;

	case INTERNET_STATUS_CONNECTED_TO_SERVER:
		strStatus.Format(_T("Connected to socket address"));
		break;

	case INTERNET_STATUS_SENDING_REQUEST:
		strStatus.Format(_T("Sending request"));
		break;

	case INTERNET_STATUS_REQUEST_SENT:
		strStatus.Format(_T("Request sent"));
		break;

	case INTERNET_STATUS_RECEIVING_RESPONSE:
        return;
		strStatus.Format(_T("Receiving response"));
		break;

	case INTERNET_STATUS_RESPONSE_RECEIVED:
		strStatus.Format(_T("Response received"));
		break;

	case INTERNET_STATUS_CLOSING_CONNECTION:
		strStatus.Format(_T("Closing the connection to the server"));
		break;

	case INTERNET_STATUS_CONNECTION_CLOSED:
		strStatus.Format(_T("Connection to the server closed"));
		break;

	case INTERNET_STATUS_HANDLE_CLOSING:
        return;
		strStatus.Format(_T("Handle closed"));
		break;
	
	//Check MSDN information about CInternetSession dwFlag INTERNET_FLAG_ASYNC
	//I have never been able to get this to work to my liking
	case INTERNET_STATUS_REQUEST_COMPLETE:
  		strStatus.Format(_T("Request complete"));
		break;

	case INTERNET_STATUS_REDIRECT:
		strStatus.Format(_T("Being redirected"));
		break;

	default:
		strStatus.Format(_T("Unknown status: %d"), dwInternetStatus);
		break;
	}  
}

//use this function to update any status, edit or what ever control
//that needs updating
void CWebAccess::ShowStatus(LPCTSTR strStatus)
{
	
}


//Lets get the file via http
DWORD CWebAccess::GetWebFile(LPCTSTR pstrAgent, LPCTSTR lpstrServer, int nPort, CString strFile)
{
	//Check what file types we will allow to be requested
	CString extension = strFile.Right(3);
		
	DWORD dwAccessType = PRE_CONFIG_INTERNET_ACCESS;
	DWORD dwHttpRequestFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE;

	
	return 0;
}

DWORD CWebAccess::Get(CString url, char **pResult)
{
	// http connection
	CHttpConnection *pHttpConnection;
	// http file pointer
	CHttpFile		*pHttpFile;
	// server url and object.
	CString			szServerUrl, szObject;


	if(*pResult != NULL)
		return ERROR_PORT_NOTNULL;

	// extract server and objects
	szServerUrl = url;
	ExtractObject(szServerUrl,szObject);

	try 
	{
		pHttpConnection=GetHttpConnection(szServerUrl);	

		if( NULL == pHttpConnection)
		{
			// no exception raised but there is an error. 
			return WEB_ACCESS_UNEXPECTED_ERROR;
		}
	}
	catch (CInternetException *pException)
	{
		TCHAR buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		
		*((TCHAR**)pResult) = new TCHAR[1024];
		_tcscpy((TCHAR*)*pResult,buffer);

		return pException->m_dwError;		
	}

	// open request
	try {
		pHttpFile = pHttpConnection->OpenRequest(NULL, szObject);

		if(NULL == pHttpFile)
		{
			return WEB_ACCESS_UNEXPECTED_ERROR;
		}
	}
	catch(CInternetException *pException)
	{
		TCHAR buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		
		*((TCHAR**)pResult) = new TCHAR[1024];
		_tcscpy((TCHAR*)*pResult,buffer);

		return pException->m_dwError;
    }

	// send the request
	try {
		BOOL ret = pHttpFile->SendRequest();

		if( FALSE == ret )
		{
			return WEB_ACCESS_UNEXPECTED_ERROR;
		}
	}
	catch(CInternetException *pException)
	{
		TCHAR buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		
		*((TCHAR**)pResult) = new TCHAR[1024];
		_tcscpy((TCHAR*)*pResult,buffer);

		return pException->m_dwError;
	}

	// query status code
	DWORD retCode;
	BOOL ret = pHttpFile->QueryInfoStatusCode(retCode);
	if( FALSE == ret )
	{
		return WEB_ACCESS_QUERY_INFO_ERROR;
	}
	else if( HTTP_STATUS_OK != retCode )
	{
		return retCode;
	}
		
	*pResult = new char[READ_HTTP_BUFFER];
	int iReadBytes = 0;
	while(iReadBytes < READ_HTTP_BUFFER)
	{
		if(pHttpFile->Read((*pResult+iReadBytes), 1)<= 0)
		{
			*(*pResult+iReadBytes+1) = 0;
			*(*pResult+iReadBytes+2) = 0;
			break;
		}
		iReadBytes++;
	}

	if(READ_HTTP_BUFFER == iReadBytes)
		return WEB_ACCESS_REQ_GREATER;

	return WEB_ACCESS_DONE;
}

DWORD CWebAccess::Post(CString url, CString szFormData, CString & resultString)
{
	// http connection
	CHttpConnection *pHttpConnection;
	// http file pointer
	CHttpFile		*pHttpFile;
	// server url and object.
	CString			szServerUrl, szObject;
	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");


	// extract server and objects
	szServerUrl = url;
	ExtractObject(szServerUrl,szObject);

	try 
	{
		pHttpConnection=GetHttpConnection(szServerUrl);	

		if( NULL == pHttpConnection)
		{
			// no exception raised but there is an error. 
			return WEB_ACCESS_UNEXPECTED_ERROR;
		}
	}
	catch (CInternetException *pException)
	{
		TCHAR buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		resultString = buffer;
		return pException->m_dwError;		
	}

	// open request
	try {
		pHttpFile = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, szObject);

		if(NULL == pHttpFile)
		{
			return WEB_ACCESS_UNEXPECTED_ERROR;
		}
	}
	catch(CInternetException *pException)
	{
		TCHAR buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		resultString = buffer;
		return pException->m_dwError;
    }

	// send the request
	try {

		BOOL ret = pHttpFile->SendRequest(strHeaders, (LPVOID)(LPCSTR)szFormData.GetBuffer(0), szFormData.GetLength());

		if( FALSE == ret )
		{
			return WEB_ACCESS_UNEXPECTED_ERROR;
		}
	}
	catch(CInternetException *pException)
	{
		TCHAR buffer[1023];
		pException->GetErrorMessage(buffer, 1023);
		resultString = buffer;
		return pException->m_dwError;
	}

	// query status code
	DWORD retCode;
	BOOL ret = pHttpFile->QueryInfoStatusCode(retCode);
	if( FALSE == ret )
	{
		return WEB_ACCESS_QUERY_INFO_ERROR;
	}
	else if( HTTP_STATUS_OK != retCode )
	{
		return retCode;
	}

	char buf[2];
	int bytesRead;
	resultString = "";
	
	while( (bytesRead = pHttpFile->Read(buf, 1)) > 0 )
	{
		resultString += buf[0];
	}
	return WEB_ACCESS_DONE;
}

void CWebAccess::AddPostArgument(CString name, CString value)
{
}

void CWebAccess::ClearPostArguments()
{
	postArguments.RemoveAll();
}

DWORD CWebAccess::DownloadFile(CString szCompleteUrl, CString szLocalFile)
{
	return 0;
}

// Extracts server and object from a complete url
// TODO:
// a. Check for malformed url. 
void CWebAccess::ExtractObject(CString & szUrl, CString &szObject)
{
	CString tempUrl = szUrl;

	// find "http" and remove it from the string if it's present.
	int index = tempUrl.Find(_T("http"), 0);
	if( index!= -1)
	{
		tempUrl.Delete(0, _tcslen(_T("http://")));
	}

	// extract server
	BOOL ret = AfxExtractSubString(szUrl, tempUrl, 0,_T( '/'));

	index = tempUrl.Find(_T("/"), 0);	
	if( index != -1 )
	{
		tempUrl.Delete(0, index+1);

		// temp url now contains object. 
		szObject = tempUrl;
	}
}