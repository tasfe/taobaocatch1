/*************************************
	����:mycro
	����:mycrolee@163.com
	����ʱ��:2007-03-27
*************************************/

#include <stdafx.h>
#include "DOMElement.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DOMElement::DOMElement() :
	m_parent(NULL)
{
	m_content = NULL;
	m_tag = "";
	m_level = 0;
	
	m_next = NULL;
	m_preced = NULL;
}

DOMElement::DOMElement(DOMElement* parent) :
	m_parent(NULL)
{
	m_next = NULL;
	m_preced = NULL;
	m_content = NULL;
		
	if (parent)
	{
		if(parent->m_children.size() > 0)
		{
			DOMElement* back = parent->m_children.back();
			back->m_next = this;
			this->m_preced = back;
		}
		
		parent->m_children.push_back(this);
		m_parent = parent;
		m_level = parent->m_level + 1;
	}
	else
	{
		m_level = 0;
	}
}

DOMElement::~DOMElement()
{
	FreeChilds();
}

void DOMElement::SetAttribute(MLPCSTR attribute, MLPCSTR value)
{
	assert(attribute && value);
	if (!attribute || !value)
		return ;

	string key = attribute;
	string val = value;
	
	_attribute.insert(StringPair(key, val));
	
}

void DOMElement::GetAttribute(MLPCSTR attribute, string &value)
{
	value = "";

	assert(attribute);
	if (!attribute)
		return ;
	
	string key = attribute;
	AttributeIterator it = _attribute.find(key);
	if (it == _attribute.end())
		return ;

	value = it->second;
}

void DOMElement::FreeChilds()
{
	ElementIterator it = m_children.begin();
	for(;it != m_children.end(); it++)
		delete (*it);
	m_children.clear();
}

int DOMElement::Enumerate(LPENUM_CALLBACK_FUNC pFunc,void* pRef)
{
	assert(pFunc);
	if (!pFunc)
		return 0;

	int ret = 0;
	if ( (ret = (*pFunc)(this,pRef)) != 0)
		return ret;

	ElementIterator it = m_children.begin();
	for(; it != m_children.end(); it++)
	{
		if ((ret = (*it)->Enumerate(pFunc,pRef)) != 0)
			return ret;
	}
	
	return 0;
}

int DOMElement::BuildElement(string& xml)
{
	int ret = 0;

	string indent;
	for(int i = 0; i < m_level; i++)
	{
		indent += "    ";
	}

	// ����ǰ�ߵı�ǩ
	string begin_tag = indent;
	begin_tag += "<";
	begin_tag += m_tag;

	// ������Դ�
	begin_tag += GetAttrStr();

	begin_tag += ">";
	xml += begin_tag;
	
	string finish_tag;
	if (m_content)
	{
		xml += m_content;		
	}
	else
	{
		xml += "\r\n";
		finish_tag = indent;
	}

	ElementIterator it = m_children.begin();
	for(; it != m_children.end(); it++)
	{
		DOMElement* child = (*it);
		assert(child != NULL);
		if (!child)
			return -1;

		if ( (ret = child->BuildElement(xml)) != 0)
			return ret;
	}

	finish_tag += "</";
	finish_tag += m_tag;
	finish_tag += ">\r\n";
	xml += finish_tag;

	return 0;
}

DOMElement* DOMElement::AddChild(MLPCSTR tag, MLPCSTR content)
{
	assert(tag);
	//if (!tag || !content)
	if (!tag)
		return NULL;

	DOMElement* child = new DOMElement(this);
	if (child == NULL)
		return NULL;

	child->set_tag(tag);
		
	if(content)
		child->setTextContent(content);

	return child;
}

int DOMElement::GetChildrenByTag(MLPCSTR tag, ElementList &list)
{
	assert(tag);
	if (!tag)
		return -1;

	list.clear();
	
	ElementIterator it = m_children.begin();
	for(; it != m_children.end(); it++)
	{
		DOMElement* child = (*it);
		assert(child);
		if (!child)
			return -1;

		// NOTIFY: don't release child out by this list.
		if (tag != NULL)
		{
			string childTag = child->get_tag();
			if (childTag.compare(tag) == 0)
				list.push_back(child);
		}
		else
			list.push_back(child);
	}

	return 0;
}

DOMElement* DOMElement::GetFirstChildByTag(MLPCSTR tag,int IsGrand)
{
	ElementIterator it = m_children.begin();
	for(; it != m_children.end(); it++)
	{
		DOMElement* child = (*it);
		assert(child);
		if (!child)
			return NULL;
		
		// NOTIFY: don't release child out by this list.
		if (tag != NULL)
		{
			string childTag = child->get_tag();
			if (childTag.compare(tag) == 0)
				return child;
			else if(IsGrand)
			{
				//Add by litz,2006-04-20
				DOMElement* tagChild = child->GetFirstChildByTag(tag,IsGrand); 
				if(tagChild)
					return tagChild;
				else
					continue;
			}
		}
		else
			return child;
	}
	
	return NULL;
}

string DOMElement::GetAttrStr()
{
	string m_rst;

	AttributeIterator it;
	for (it = _attribute.begin(); it != _attribute.end(); it++)
	{
		m_rst += " ";
		m_rst += it->first;
		m_rst += " =\"";
		m_rst += it->second;
		m_rst += "\"";
	}

	return m_rst;
}

DOMElement* DOMElement::GetNextElement()
{
	
	return m_next;
	
	//Ϊ�����Ч�ʣ�ȡ����ֱ�������ķ�ʽ,�޸�Ϊ����ʱ��¼
	///////////////////////////////////
	DOMElement* pFoundElement = NULL;
	DOMElement* pParent = NULL;
	bool  IsFound = FALSE;
	pParent = get_parent();
	if(pParent == NULL)
		return NULL;

	ElementIterator it = pParent->m_children.begin();

	int i =0;
	for(; i < pParent->m_children.size() && it != m_children.end(); it++,i++)
	{
		if(IsFound)
			break;
		if((*it) == this)
		{
			IsFound = TRUE;
		}
	}

	if(!IsFound)
		return NULL;
	if(i == pParent->m_children.size())
		return NULL;
	
	pFoundElement = (*it);
	
	return pFoundElement;	
}