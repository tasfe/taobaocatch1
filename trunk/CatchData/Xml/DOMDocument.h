/*************************************
	����:mycro
	����:mycrolee@163.com
	����ʱ��:2007-03-27
*************************************/
#if !defined(_DOMDOCUMENT_H_)
#define _DOMDOCUMENT_H_

#include "DOMElement.h"

#define TRIM_LEFT(str)\
{\
	while (0 < str.size() &&\
		   (str[0] == '\r' ||\
			str[0] == '\n' ||\
			str[0] == '\t' ||\
			str[0] == ' '))\
	{\
		str.erase(0, 1);\
	}\
}


#define TRIM_RIGHT(str)\
{\
	while (0 < str.size() &&\
		   (str[str.size() - 1] == '\r' ||\
		    str[str.size() - 1] == '\n' ||\
		    str[str.size() - 1] == '\t' ||\
		    str[str.size() - 1] == ' '))\
	{\
		str.erase(str.size() - 1, 1);\
	}\
}


class DOMDocument  
{
public:
	DOMElement* getDocumentElement();
	DOMDocument();
	virtual ~DOMDocument();

	// ��һ���������ڴ���װ��XML��������(Parse)��XML Tree
	int LoadXML(const char *buffer, unsigned long bufferSize);

	// ��ָ�����ļ���װ��XML��������(Parse)��XML Tree
	int LoadXML(MLPCSTR pathXML);

	// ����ǰ��XML Treeת����XML�ĵ�
	int BuildXML(DOMElement* pElement);

	// ���XML��version��Ϣ
	string get_version() const {return m_version;}

	// ���XML�ĵ� (std::string����)
	string get_xml() const {return m_xml;}

	// ���XML��Tree�ĸ��ڵ�(root node)
	DOMElement* get_root(){return &m_root;}

	// ָ��һ���ڵ�Ϊ���ڵ㣬����Ҫ���ͷ�֮ǰ��XML Tree�����������ڴ�й¶
	//void set_root(DOMElement* root){ m_root = *root;}
	
	// �������нڵ㣬����ÿһ���ڵ����pFunc����ʵ���Լ���һ���������ñ�����Ϊ���Դ�����һ���ڵ㿪ʼ��
	int EnumerateElements(LPENUM_CALLBACK_FUNC pFunc,DOMElement* pElectment,void* pRef);

	// �ͷ�XML Tree

	// ��ձ����XML�ĵ�����
	
protected:
	int m_pos;

	string m_xml;
	string m_version;
	string m_encode;

	DOMElement m_root;

	int ParseXmlInfo();
	
	int ParseRoot();
	int ParseElement(DOMElement* element);
	int ParseAttribute(MLPCSTR tagString, string &tagName, AttributeMap &attribute);

	int findNextTag(string &tagName, string &beforeTag);

	int BuildXmlInfo();
};

#endif // !defined(_DOMDOCUMENT_H_)
