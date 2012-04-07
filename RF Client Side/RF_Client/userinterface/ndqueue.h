#ifndef __NDQUEUE_H__
#define __NDQUEUE_H__

#include <assert.h>

/*//////////////////////////////////////////////////////////////////////////

[ CNDQueue ]

   - 제한된 갯수만큼 넣을 수 있는 Queue   

//////////////////////////////////////////////////////////////////////////*/
template < typename T > class CNDQueue
{
private:
	class CNDElement {
	public:
		bool	m_bUsed;
		T		m_Element;
	public:		
		CNDElement() { m_bUsed = false; }
		~CNDElement() {;}

		CNDElement& operator = ( CNDElement & pi_Element )
		{
			m_bUsed = pi_Element.m_bUsed;
			m_Element = pi_Element.m_Element;
			return *this;
		}

		bool operator == ( CNDElement & pi_Element )
		{
			if( m_Element == pi_Element.m_Element )
				return true;
			else
				return false;
		}

		bool operator != ( CNDElement & pi_Element )
		{
			if( *this != pi_Element )
				return true;
			else 
				return false;
		}
	};

	CNDElement	*	m_pElementList;

	int				m_nSize;			// queue 고정 크기
	
	int				m_nStartOffset;		// 맨앞에 값을 가리킨다.

	int				m_nCurNum;			// 현재 element 갯수

public:
	CNDQueue();
	~CNDQueue();	

	void	Create( int pi_nSize );		// Queue Size 지정
	void	Destroy( void );

	void	Push( T& pi_m_Element );
	void	Pop( void );				// 맨 앞에 것을 꺼낸다.

	void	Remove( int pi_nIndex );

	void	Clear( void );

	int		GetCurNum( void ) { return m_nCurNum; }

	T&		GetElement( int pi_nIndex );
	T&		operator[]( int pi_nIndex );	
};

template < typename T >
CNDQueue<T>::CNDQueue()
{	
	m_pElementList = NULL;
}

template < typename T >
CNDQueue<T>::~CNDQueue()
{
	Destroy();	
}


template < typename T >
void
CNDQueue<T>::Create( int pi_nSize )
{
	if( pi_nSize < 1 )
		return;	

	if( m_pElementList )
		Destroy();	

	m_nSize = pi_nSize;

	m_pElementList = new CNDElement[m_nSize];
	
	m_nStartOffset = 0;

	m_nCurNum = 0;
}

template < typename T >
void
CNDQueue<T>::Destroy( void )
{	
	if( m_pElementList != NULL )
	{
		delete[] m_pElementList;
		m_pElementList = NULL;
	}
	
	m_nStartOffset = 0;
	m_nCurNum = 0;	
}

template < typename T >
void
CNDQueue<T>::Push( T& pi_m_Element )
{
	if( !m_pElementList )
		return;
	
	if( m_nCurNum < m_nSize )
	{
		m_pElementList[(m_nStartOffset + m_nCurNum ) % m_nSize].m_bUsed = true;
		m_pElementList[(m_nStartOffset + m_nCurNum ) % m_nSize].m_Element = pi_m_Element;

		++m_nCurNum;
	}
	else
	{
		// 이전에 있는 데이터는 지워진다.
		m_pElementList[m_nStartOffset].m_bUsed = true;
		m_pElementList[m_nStartOffset].m_Element = pi_m_Element;

		// m_nStartOffset 증가
		m_nStartOffset = ( m_nStartOffset + 1 ) % m_nSize;
	}	
	
}

template < typename T >
void
CNDQueue<T>::Pop( void )
{
	if( m_nCurNum == 0 )
		return;	

	// 맨앞에 있는 것을 삭제한다.
	if( m_pElementList[m_nStartOffset].m_bUsed )
		m_pElementList[m_nStartOffset].m_bUsed = false;
	
	m_nStartOffset = ( m_nStartOffset + 1 ) % m_nSize;

	--m_nCurNum;
}

template < typename T >
void
CNDQueue<T>::Remove( int pi_nIndex )
{
	if( m_nCurNum == 0 ||
		m_nCurNum <= pi_nIndex )
		return;

	int l_nIndex = ( m_nStartOffset + pi_nIndex ) % m_nSize;

	if( !m_pElementList[l_nIndex].m_bUsed )
		return;	

	// 삭제
	m_pElementList[l_nIndex].m_bUsed = false;

	// shift	
	for( int i=0; i<m_nCurNum-pi_nIndex-1; ++i )
	{
		m_pElementList[l_nIndex + i] = m_pElementList[l_nIndex + i + 1];
	}	
	m_pElementList[l_nIndex + i].m_bUsed = false;	

	--m_nCurNum;
}

template < typename T >
void
CNDQueue<T>::Clear( void )
{
	for( int i=0; i<m_nSize; ++i )
	{
		m_pElementList[i].m_bUsed = false;
	}

	m_nStartOffset = 0;
	m_nCurNum = 0;
}

template < typename T >
T&
CNDQueue<T>::GetElement( int pi_nIndex )
{
	int l_nIndex = ( m_nStartOffset + pi_nIndex ) % m_nSize;

	assert( m_pElementList[l_nIndex].m_bUsed );

	return m_pElementList[l_nIndex].m_Element;
}

template < typename T >
T&
CNDQueue<T>::operator[]( int pi_nIndex )
{
	int l_nIndex = (m_nStartOffset + pi_nIndex)%m_nSize;	

	assert( m_pElementList[l_nIndex].m_bUsed );

	return m_pElementList[l_nIndex].m_Element;
}

#endif // __NDQUEUE_H__
