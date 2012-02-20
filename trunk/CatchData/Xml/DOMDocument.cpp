/*************************************
	����:mycro
	����:mycrolee@163.com
	����ʱ��:2007-03-27
*************************************/
#include <stdafx.h>
#include "DOMDocument.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define TEMP_BUF_LEN 1024

DOMDocument::DOMDocument() :
	m_pos(0),
	m_root(NULL)
{
	m_version = "1.0";
	m_encode = "GB2312";
}

DOMDocument::~DOMDocument()
{

}

int DOMDocument::LoadXML(const char* buffer, unsigned long bufferSize)
{
	assert(buffer);
	if (!buffer)
		return -1;

	m_pos = 0;

	m_xml = buffer;

	int result = ParseXmlInfo();
	
	return ParseRoot();
}

int DOMDocument::LoadXML(MLPCSTR pathXML)
{
	int ret = 0;
	char *xmlBuffer = NULL;
	FILE *pf = fopen(pathXML, "rb");
	if (!pf)
	{
		//printf("ERROR: Can't open %s\n", pathXML);
		return -1;
	}

	fpos_t pos; 
	if (fseek(pf, 0, SEEK_END))
	{
		fclose(pf);
		return -1;
	}

	if (fgetpos(pf, &pos))
	{
		fclose(pf);		
		return -1;
	}

	if (pos <= 0)
	{
		fclose(pf);
		return -1;
	}

	xmlBuffer = new char[pos];
	assert(xmlBuffer);
	if (!xmlBuffer)
	{
		fclose(pf);
		delete[] xmlBuffer;
		return -1;
	}
	
	rewind(pf);
	fread(xmlBuffer, pos, 1, pf);
	fclose(pf);
	
	ret = LoadXML(xmlBuffer, pos);
	delete[] xmlBuffer;
	return ret;
}

int DOMDocument::ParseXmlInfo()
{
	//Try to find the start of the version tag.
	string m_ShortStr;

	//��ȡ�汾
	int version_start = 0;
	int version_finish = 0;
	
	m_pos = m_xml.find("version");
	if (m_pos == string::npos)
		return -1;
	version_start = m_xml.find("\"",m_pos) + 1;
	version_finish = m_xml.find("\"",version_start+1);

	if(version_start != string::npos && version_finish != string::npos)
	{
		m_version = m_xml.substr(version_start, version_finish - version_start);
	}
	else
	{
		version_start = m_xml.find("\'",m_pos) + 1;
		version_finish = m_xml.find("\'",version_start+1);
		if(version_start != string::npos && version_finish != string::npos)
		{
			m_version = m_xml.substr(version_start, version_finish - version_start);
		}
		else
		{
			m_version = "1.0";
		}
	}
	
	
	//��ȡ����
	version_start = 0;
	version_start = m_xml.find("encoding");

	if (version_start == string::npos)
		return -1;

	version_start = m_xml.find("\"",version_start) + 1;
	version_finish = m_xml.find("\"",version_start+1);
	if(version_start != string::npos && version_finish != string::npos)
	{
		m_encode = m_xml.substr(version_start, version_finish - version_start);
	}
	else
	{
		version_start = m_xml.find("\'",m_pos) + 1;
		version_finish = m_xml.find("\'",version_start+1);
		if(version_start != string::npos && version_finish != string::npos)
		{
			m_encode = m_xml.substr(version_start, version_finish - version_start);
		}
		else
		{
			m_encode = "GB2312";
		}
	}	
	
	return 0;
}


int DOMDocument::BuildXmlInfo()
{
	string version_str = "<?xml ";
	version_str += "version=\"";
	version_str += m_version;
	version_str += "\" encoding=\"";
	version_str += m_encode;
	
	version_str += "\"?>\r\n";
	m_xml = version_str;

	return 0;
}


int DOMDocument::BuildXML(DOMElement* pElement)
{
	//�������
	m_xml = "";
	m_pos = 0;

	int ret = 0;
	if ( (ret = BuildXmlInfo()) != 0)
		return ret;

	if(!pElement)
	{		
		return m_root.BuildElement(m_xml);
	}
	else
	{
	    return pElement->BuildElement(m_xml);	
	}
}



int DOMDocument::findNextTag(string &tagName, string &beforeTag)
{
	tagName = "";	
	beforeTag = "";

	int beforeTagStart = m_xml.find_first_not_of("\t\r\n ", m_pos);
	if (beforeTagStart == string::npos)
		return -1;

	int beforeTagFinish = m_xml.find("<", beforeTagStart);
	if (beforeTagFinish == string::npos)
		return -1;
	
	if (beforeTagFinish - beforeTagStart > 0)
	{
		beforeTag = m_xml.substr(beforeTagStart, beforeTagFinish - beforeTagStart);

		// trim left tempString
		TRIM_LEFT(beforeTag);
	}
	
	// Find the tag finish position.
	int tag_start = beforeTagFinish;
	int tag_finish = m_xml.find(">", tag_start);
	if (tag_finish == string::npos)
		return -1;
	
	tagName = m_xml.substr(tag_start + 1, tag_finish - tag_start - 1);
	m_pos = tag_finish + 1;

	return 0;
}

int DOMDocument::ParseElement(DOMElement* element)
{
	assert(element != NULL);
	if (element == NULL)
		return -1;
	
	while(m_pos < m_xml.size())
	{
		string tagName;
		string beforeTag;
		if (findNextTag(tagName, beforeTag) == -1)
		{
			printf("Error: Without a finish tag for <%s>.\n", element->get_tag().c_str());
			return -1;
		}


		/*
		//Modiby litz,2006-08-23
		//Ϊʲô��Ҫ�ж����Ƿ�Ϊ��?
		if (element->getTextContent().empty() && !beforeTag.empty())
			element->setTextContent(beforeTag.c_str());
			*/
		if (!beforeTag.empty())
			element->setTextContent(beforeTag.c_str());
			
		
		// Is a end-tag?
		if (tagName[0] == '/')
		{
			// Remove the '/' character.
			string end_tagName;
			
			end_tagName = tagName;
			end_tagName.erase(0, 1);
			
			if (element->get_tag() != end_tagName)
			{
				printf("ERROR: Can't find a end tag for <%s>.\n", element->get_tag().c_str());
				return -1;
			}
			
			return 0;
		}

		DOMElement* sub_element = new DOMElement(element);
		if (!sub_element)
			return -1;
		
		// Is it a empty tag?
		if (tagName[tagName.size() - 1] == '/')
		{
			tagName.erase(tagName.size() - 1, 1);
			
			string actualTagName;
			
			int m_AttrNum = ParseAttribute(tagName.c_str(), actualTagName, sub_element->_attribute);
			if(m_AttrNum == -1)
				return -1;
			
			sub_element->set_tag(actualTagName.c_str());
			continue ;
		}

		string actualTagName;
		int m_AttrNum = ParseAttribute(tagName.c_str(), actualTagName, sub_element->_attribute);
		if(m_AttrNum == -1)
				return -1;

		sub_element->set_tag(actualTagName.c_str());
		if (ParseElement(sub_element) != 0)
			return -1;
	}

	printf("Error: without a finish tag for <%s>.\n", element->get_tag().c_str());
	return -1;
}

int DOMDocument::EnumerateElements(LPENUM_CALLBACK_FUNC pFunc,DOMElement* pElectment,void* pRef)
{
	assert(pFunc != NULL);
	if (!pFunc)
		return -1;

	return pElectment->Enumerate(pFunc,pRef);
}

int DOMDocument::ParseRoot()
{
	int tag_start = 0;

	int nPos = 0;
	while((nPos = m_xml.find("?>",nPos)) != string::npos)
	{
		nPos += 2;
		tag_start = nPos;
	}
	
	// ���Ҹ�����ǰ��ǩ
	tag_start = m_xml.find("<", tag_start);
			
	// Find the next tag start position (it is the content part finish also).
	if (tag_start == string::npos)
	{
		printf("ERROR: It is not XML Document.\n");
		return -1;
	}
	
	int tag_finish = m_xml.find('>', tag_start);
	if (tag_finish == string::npos || tag_finish - tag_start <= 1)
	{
		printf("ERROR: There is no tag name int the XML root.\n");
		return -1;
	}

	string tagName = m_xml.substr(tag_start + 1, tag_finish - tag_start - 1);
	//������������ԣ�ȥ��
	tag_start = tagName.find(" ", 0);
	if (tag_start != string::npos)
	{
		tagName = tagName.substr(0,tag_start);
	}

	m_root.set_tag(tagName.c_str());

	m_pos = tag_finish + 1;

	// �����Ƿ��н����ڵ�
	tag_start = m_xml.rfind( "/" + tagName);
	if (tag_start == string::npos)
	{
		printf("ERROR: Root tag nomatch.\n");
		return -1;
	}
	
	return ParseElement(&m_root);
}

int DOMDocument::ParseAttribute(MLPCSTR tagString, string &tagName, AttributeMap &attribute)
{
	assert(tagString);
	if (!tagString)
		return -1;

	attribute.clear();

	int m_AttrNum = 0;

	string tempString = tagString;

	TRIM_LEFT(tempString);
	TRIM_RIGHT(tempString);

	int firstSpacePos = tempString.find_first_of(" \t\r\n");
	if (firstSpacePos == string::npos)
	{
		tagName = tempString;
		return 0;
	}
	tagName = tempString.substr(0, firstSpacePos);

	int startPos = firstSpacePos + 1;
	int equalPos = tempString.find('=', startPos);
	while (equalPos != string::npos)
	{		
		m_AttrNum ++;

		string attributeValue = "";
		string attributeName = tempString.substr(startPos, equalPos - startPos);
		TRIM_LEFT(attributeName);	
		TRIM_RIGHT(attributeName);

		int leftInvertedCommasPos = tempString.find('\"', equalPos + 1);
		if (leftInvertedCommasPos == string::npos)
		{
			leftInvertedCommasPos = tempString.find('\'', equalPos + 1);
			
			if (leftInvertedCommasPos == string::npos)
			{
				return -1;
			}
		}

		int rightInvertedCommasPos = tempString.find('\"', leftInvertedCommasPos + 1);
		if (rightInvertedCommasPos == string::npos)
		{
			rightInvertedCommasPos = tempString.find('\'', leftInvertedCommasPos + 1);
			if (rightInvertedCommasPos == string::npos)
			{
				return -1;
			}
		}

		attributeValue = tempString.substr(leftInvertedCommasPos + 1, rightInvertedCommasPos - leftInvertedCommasPos - 1);	

		TRIM_LEFT(attributeValue);	
		TRIM_RIGHT(attributeValue);

		attribute.insert(StringPair(attributeName, attributeValue));

		startPos = rightInvertedCommasPos + 1;
		equalPos = tempString.find('=', startPos);
	}

	if (startPos <= tempString.size() - 1)
		return -1;

	return m_AttrNum;
}

DOMElement* DOMDocument::getDocumentElement()
{
	return &m_root;
}
