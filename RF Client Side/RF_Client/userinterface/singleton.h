#ifndef __CSINGTON__
#define __CSINGTON__

#include <assert.h>

template <typename T> class CSingleton
{
	static T* m_pSingleton;
	
public:
	CSingleton(void)
	{
		assert(!m_pSingleton);
		int offset = (int )(T *)1 - (int)(CSingleton <T> *)( T*)1;
		m_pSingleton = (T*)((int)this + offset);
	}

	~CSingleton(void)
	{
		assert(m_pSingleton);
		m_pSingleton = 0;
	}

	static T& GetSingleton(void)
	{
		assert(m_pSingleton);
		return *m_pSingleton;
	}

	static T* GetSingletonPtr(void)
	{
		return m_pSingleton;
	}
};

template <typename T> T* CSingleton<T>::m_pSingleton = 0;

#endif // __CSINGTON__