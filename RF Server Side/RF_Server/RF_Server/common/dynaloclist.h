#ifndef _dyn_aloc_list_h_
#define _dyn_aloc_list_h_

typedef DWORD		__CODE;
#define __NFN		0xFFFFFFFF

struct _DYN_ALOC_CODE_LIST
{
	struct __NODE{
		__CODE		ItemCode;
		__NODE*		pNext;
		__NODE*		pPrev;

		__NODE operator = (__NODE p_n)
		{
			memcpy(this, &p_n, sizeof(__NODE));
			return *this;
		}
	};

	__NODE	m_Head;
	__NODE	m_Tail;
	DWORD	m_dwListSize;

	_DYN_ALOC_CODE_LIST()
	{
		InitList();
	}

	~_DYN_ALOC_CODE_LIST()
	{
		ReleaseList();
	}

	void InitList()
	{
		m_dwListSize = 0;

		m_Head.pPrev = &m_Head;
		m_Head.pNext = &m_Tail;
		m_Tail.pPrev = &m_Head;
		m_Tail.pNext = &m_Tail;
	}

	void ReleaseList()
	{
		__NODE* pNode = m_Head.pNext;
		__NODE* pBufNode = NULL;

		while(pNode != &m_Tail)
		{
			pBufNode = pNode;
			pNode = pNode->pNext;			

			delete pBufNode;
			m_dwListSize--;

		}
	}

	void PushNode_Back(__CODE ItemCode)
	{
		__NODE* pNewNode = new __NODE;

		pNewNode->ItemCode = ItemCode;

		pNewNode->pNext = &m_Tail;
		pNewNode->pPrev = m_Tail.pPrev;
		m_Tail.pPrev->pNext = pNewNode;
		m_Tail.pPrev = pNewNode;
		m_dwListSize++;
	}

	BOOL PopNode_Front(__CODE* poutItemCode)
	{
		__NODE* pNode = m_Head.pNext;

		if(pNode == &m_Tail)
			return FALSE;

		m_Head.pNext = pNode->pNext;
		pNode->pNext->pPrev = &m_Head;
		m_dwListSize--;

		if(poutItemCode)
			*poutItemCode = pNode->ItemCode;

		delete pNode;

		return TRUE;
	}

	BOOL SearchNode(__CODE ItemCode)
	{
		__NODE* pNode = m_Head.pNext;

		while(pNode != &m_Tail)
		{
			if(pNode->ItemCode == ItemCode)
			{
				pNode->pPrev->pNext = pNode->pNext;
				pNode->pNext->pPrev = pNode->pPrev;
				m_dwListSize--;

				delete pNode;

				return TRUE;
			}
			else
				pNode = pNode->pNext;
		}

		return FALSE;
	}
};

#endif