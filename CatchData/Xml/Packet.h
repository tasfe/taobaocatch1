/*************************************
	����:mycro
	����:mycrolee@163.com
	����ʱ��:2007-03-27
*************************************/

#ifndef __PACKET__H__
#define __PACKET__H__ 


#include "DOMElement.h"
#include "DOMDocument.h"

#define ERRORMSGSIZE    512

typedef struct _REF_INPUT
{
	string sTag;
	string sAttr;
	string sValue;
	DOMElement* sFoundElement;
}REF_INPUT;


/*************************************
	��������SearchElementCell
	����: ����Ԫ������
	���룺
	 	pElement
		szinBuf
	���:
	       TRUE:  �ɹ�
	       FALSE: ʧ��
*************************************/
int SECellByName(DOMElement* pElement,void *pRef);
int SECellByAttr(DOMElement* pElement,void *pRef);

/***********************************
  ������CPacket
  ����������ͨѶ������,�����ΪһDom��
************************************/

class CPacket
{
public:
	CPacket();
	~CPacket();
	
	/*************************************
	 ��������BuiltTree
	 ����: ����XMl�ִ�/�ļ�����DOM��
	 ���룺�ļ�·��/�ִ�,����
	 ���:
	        0:  �ɹ�
	        -1: ʧ��
	 *************************************/	
	int BuiltTree(MLPCSTR pXmlPath);
	int BuiltTree(const char* pBuf,int nBufSize);

	/*************************************
	 ��������SearchElement
	 ����: ���ҽڵ�/���ݽڵ���,���Բ��ҽڵ�
	 ���룺�ڵ������,����
	 ���:
	        �ڵ�:  �ɹ�
	        NULL: ʧ��
	 *************************************/	
	DOMElement * SearchElement(char * szNodeName);
	DOMElement * SearchElement(char * szNodeName,char * szProper,char * szId);
	
	/*************************************
	 ��������SearchNextElement
	 ����: ���ҵ�ǰ�ڵ����һ���ڵ�
	 ע��: �޲�������Ҫ��SetCurrentElement�������ʹ��
	 ���룺�ڵ������,·��,����
	        pMoveNext: �Ƿ񽫵�ǰ�ڵ����ó���һ���ڵ�
	 ���:
	        �ڵ�:  �ɹ�
	        NULL: ʧ��
	 *************************************/	
	//���ҵ�ǰ�ڵ����һ���ڵ�
	DOMElement * SearchNextElement(bool pMoveNext = TRUE);
	DOMElement * SearchNextElement(char *szNodeName,char *szProper = NULL,char *szId = NULL);
	
	/*************************************
	 ��������SetCurrentNode
	 ����: ���õ�ǰ�ڵ�
	 ���룺�ڵ��ȫ·��
	 ���:
	        TRUE:  �ɹ�
	        FALSE: ʧ��
	 *************************************/
	bool SetCurrentElement(char szNodeName[]);	
	void SetCurrentElement(DOMElement * elem) { m_CurrentElement = elem; };
	
	/*************************************
	 ��������GetNodeValue
	 ����: ��ýڵ�
	 ���룺
	 	szNodeName����������Ե�ǰ�ڵ�·����ȫ·��
	 		    ".\"��ʾ��ǰ�ڵ�
	 	szOutBuf  : ����ַ���ֵ
	 ���:
	        TRUE:  �ɹ�
	        FALSE: ʧ��
	 *************************************/
	bool GetElementValue(char * szNodeName, char * szOutBuf);
	bool GetElementValue(char * szNodeName, char * szOutBuf, char * szProper, char * szId);
	
	
	/*************************************
	 ��������SetNodeValue
	 ����: ��ýڵ�
	 ���룺
	 	szNodeName ����������Ե�ǰ�ڵ�·����ȫ·��,
	 	szinBuf    : ������ַ���ֵ
	 ���:
	        TRUE:  �ɹ�
	        FALSE: ʧ��
	 *************************************/
	bool SetElementValue(char * szNodeName, char * szinBuf);	
	bool SetElementValue(DOMElement * pelem, char * szinBuf);
	
	
	/*************************************
	 ��������GetElementAttr
	 ����: ��ȡԪ������
	 ���룺
	 	szNodeName ����������Ե�ǰ�ڵ�·����ȫ·��,
	 	szinBuf    : ������ַ���ֵ
	 ���:
	        TRUE:       �ɹ�
	        FALSE:      ʧ��
	        szAttrValue:���Ե�ֵ
	 *************************************/
	bool GetElementAttr(char * szNodeName, char * szAttrName, string& szAttrValue);	
	bool GetElementAttr(DOMElement * pelem, char * szAttrName, string& szAttrValue);
	
	
	/*************************************
	 ��������SetElementAttr
	 ����: ����Ԫ������
	 ���룺
	 	szNodeName ����������Ե�ǰ�ڵ�·����ȫ·��,
	 	szAttrName : ��������
	 	szAttrValue: ����ֵ
	 ���:
	        TRUE:  �ɹ�
	        FALSE: ʧ��
	 *************************************/
	bool SetElementAttr(char * szNodeName, char * szAttrName, char * szAttrValue);	
	bool SetElementAttr(DOMElement * pelem, char * szAttrName, char * szAttrValue);


public:

	//�����ĵ��Ķ���
	DOMDocument * GetDOMDocument() { return &m_DomTree; };
	
	//����XML���ı���
	string GetXml(DOMElement* pElement);
	
	//���ø��ڵ�ı�ǩ
	int SetRootTag(string pTag);
	
	//�����ڵ�,���������·���;���·��
	DOMElement * CreateElement(char szNodeName[]);
	
	//���õ�ǰ�ڵ㵽���ڵ�
	void ResetCurrentNode();


private:

	//�з��ַ���������һ��������(�ڲ�����)
	int split_bychar(vector<string> & vec, const string& str, const char seperate='/');
	
	/*************************************
	 ��������Initialize
	 ����: �������ɰ��ĳ�ʼ��
	 ���룺n/N
	 ���:
	        TRUE:  �ɹ�
	        FALSE: ʧ��
	*************************************/
	bool Initialize();

	
private:
	/*dom���洢�˰��ṹ*/
	DOMDocument m_DomTree;
	DOMElement  * m_CurrentElement;	
	

	int m_LastError;
	bool m_bInitFlag;
	
	// ��ǰtag��
	string m_CurrentTag;
	string m_CrrentAttr;
	string m_CrrentValue;
	DOMElement* m_FoundElement;
};

#endif
