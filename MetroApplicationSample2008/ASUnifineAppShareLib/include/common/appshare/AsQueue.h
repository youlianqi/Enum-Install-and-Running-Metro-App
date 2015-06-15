#ifndef	Utility_Queue_Definition_h
#define	Utility_Queue_Definition_h

#include <windows.h>

#define	QUEUE_DEFAULT_GROW_SIZE		30

template <class QueueType, int size> 
class CQueue
{
public:
	BOOL	IsFull();
	BOOL	IsEmpty();
	int		GetCount();
	BOOL	In(QueueType data);
	BOOL	Out(QueueType* pdata);
	BOOL	Peek(QueueType* pdata);
	BOOL	Peek(QueueType* pdata, int index);

public:
	CQueue();
	~CQueue();

protected:
	BOOL	SafeGrow(int nGrowSize);

private:
	QueueType*	m_pHead;
	int			m_nAlloc;
	int			m_nUsed;
	int			m_nSafeGrowSize;
};

typedef CQueue<int,0>	CIntQueue;

template <class ItemType, int GrowSize> class CDvArrayBase;
template <class T> class CDvDynamicBuffer;
template <class ItemType, int GrowSize> class CDvStack;

typedef CDvArrayBase<LPVOID, 16> CDvArray;
typedef CDvStack<LPVOID, 10>	CDvPtrStack;
typedef CDvArrayBase<DWORD, 16>		CDvDWordArray;

/////////////////////////////////////////////////////////////////////////////
// CDvArrayBase
//
template<class ItemType, int GrowSize>
class CDvArrayBase
{
public :
    CDvArrayBase()
	{
		m_ppHead = NULL;
		m_nAlloc = 0;
		m_nUsed  = 0;
	}

    CDvArrayBase(const CDvArray& other)
	{
		m_ppHead = NULL;
		m_nAlloc = 0;
		m_nUsed  = 0;

		*this = other;
	}

    virtual ~CDvArrayBase()
	{
	    RemoveAll();
	}

public :
    int GetCount()
	{
	    return m_nUsed;
	}

    BOOL Add(ItemType item, BOOL bCheckDuplicate = FALSE)
	{
		if(Expand())
		{
			if(bCheckDuplicate && GetIndex(item) >= 0)
				return FALSE;

			m_ppHead[m_nUsed++] = item;
			return TRUE;
		}
		return FALSE;
	}

    BOOL InsertAt(int nIndex, ItemType item)
	{
//		_ASSERTE(nIndex >= 0 && nIndex <= m_nUsed);

		if(nIndex >= 0 && nIndex <= m_nUsed)
		{
			if(!Expand())
				return FALSE;

			if(nIndex < m_nUsed)
				memmove(&m_ppHead[nIndex + 1], &m_ppHead[nIndex],
					sizeof(ItemType)*(m_nUsed - nIndex));

			m_ppHead[nIndex] = item;
			m_nUsed++;

			return TRUE;
		}
		return FALSE;
	}

    BOOL Remove(ItemType item)
	{
		int i=0;
		for( i = 0; i < m_nUsed; i++)
		{
			if( Compare(m_ppHead+i, &item) )
			{
				break;
			}
		}

		if (i < m_nUsed)
		{
			// found
			if(i < m_nUsed - 1)
				memcpy(&m_ppHead[i], &m_ppHead[i+1], 
					sizeof(ItemType)*(m_nUsed - i - 1));
			m_nUsed--;
			return  TRUE;
		}

		return FALSE;
	}

    BOOL RemoveAt(int i)
	{
		if(i < m_nUsed)
		{
			if(i < m_nUsed - 1)
				memcpy(&m_ppHead[i], &m_ppHead[i+1], 
					sizeof(ItemType)*(m_nUsed - i - 1));
			m_nUsed--;
			return  TRUE;
		}
		return FALSE;
	}

    void RemoveAll()
	{
		if(m_ppHead)
			delete []m_ppHead;
		m_ppHead = NULL;
		m_nAlloc = 0;
		m_nUsed = 0;
	}

    CDvArrayBase& operator = (const CDvArrayBase& other)
	{
		RemoveAll();

		if(other.m_nUsed > 0)
		{
			m_ppHead = new ItemType[other.m_nUsed];
			if(m_ppHead)
			{
				memcpy(m_ppHead, other.m_ppHead, sizeof(ItemType)*other.m_nUsed);
				m_nAlloc = m_nUsed = other.m_nUsed;
			}
		}

		return *this;
	}

    ItemType operator [](int i)
	{
//		_ASSERTE(m_ppHead != NULL);
		ItemType retVal = NULL;

		if(i >= 0 && i < m_nUsed)
		{
			retVal = m_ppHead[i];
		}
		else
		{
	//		WBXERROR(TEXT("Fail to get data from array: %d"), i);
		}

		return retVal;
	}

    BOOL SetAt(int nIndex, ItemType item)
	{
//		_ASSERTE(nIndex >= 0 && nIndex < m_nUsed);

		if (nIndex >= 0 && nIndex < m_nUsed)
		{
			m_ppHead[nIndex] = item;

			return TRUE;
		}

		return FALSE;
	}

    int GetIndex(ItemType item)
	{
		for(int i = 0; i < m_nUsed; i++)
		{
			if( Compare(m_ppHead+i, &item) )
				return	i;
		}

		return -1;
	}

    ItemType* GetHead()
	{
	    return m_ppHead;
	}

protected :
	BOOL Compare(ItemType* item1, ItemType* item2)
	{
		if( memcmp(item1, item2, sizeof(ItemType))==0 )
			return	TRUE;

		return	FALSE;
	}

    BOOL Expand()
	{
		if(m_nUsed >= m_nAlloc)
		{
			ItemType* ppNew = new ItemType[m_nAlloc + GrowSize];
			if(ppNew)
			{
				if(m_ppHead)
				{
					memcpy(ppNew, m_ppHead, sizeof(ItemType)*m_nAlloc);
					delete []m_ppHead;
				}
				m_ppHead = ppNew;
				m_nAlloc += GrowSize;
				return TRUE;
			}
			return FALSE;
		}
		return TRUE;
	}

protected :
    ItemType* m_ppHead;
    int     m_nAlloc;
    int     m_nUsed;
};

/////////////////////////////////////////////////////////////////////////////
// CDvDynamicBuffer<T>
//
template <class T>
class CDvDynamicBuffer
{
public :
    CDvDynamicBuffer(DWORD nSize = 256)
    {
        m_pHead = NULL;
        m_nSize = 0;
        SetSize(nSize);
    }

    ~CDvDynamicBuffer()
    {
        Reset();
    }

    void Reset()
    {
        if(m_pHead)
            delete []m_pHead;
        m_pHead = NULL;
        m_nSize = 0;
    }

    int GetSize() 
    {
        return m_nSize; 
    }

    BOOL SetSize(DWORD nSize)
    {
        if(nSize > m_nSize)
        {
            T* pNewHead = new T[nSize];
            if(pNewHead)
            {
                memset(pNewHead, 0, sizeof(T)*nSize);
                if(m_pHead)
                {
//                  _ASSERTE(m_nSize > 0);
                    memcpy(pNewHead, m_pHead, sizeof(T)*m_nSize);
                    delete []m_pHead;
                }

                m_pHead = pNewHead;
                m_nSize = nSize;
                return TRUE;
            }
            return FALSE;
        }
        return TRUE;
    }

    T* GetHead()   { return m_pHead; }
    operator T* () { return m_pHead; }

protected :
    T* m_pHead;
    DWORD m_nSize;
};

/////////////////////////////////////////////////////////////////////////////
// CDvStack
//
template <class ItemType, int GrowSize>
class CDvStack
{
public:
	CDvStack()
	{
		m_pStack = NULL;
		m_nUsed  = 0;
		m_nAlloc = 0;
	}
	~CDvStack()
	{
		RemoveAll();
	}

public:
	int GetCount()
	{ 
		return	m_nUsed;
	}

	BOOL Grow()
	{
		int			i;
		BOOL		bRet = TRUE;
		ItemType*	pNew;

		bRet = TRUE;
		if( m_nAlloc==m_nUsed )
		{
			pNew = new ItemType[m_nAlloc+GrowSize];
			if( pNew )
			{
				m_nAlloc += GrowSize;
				if( m_pStack )
				{
					for(i=0;i<m_nUsed;i++)
						pNew[i] = m_pStack[i];
					delete m_pStack;
				}
				m_pStack = pNew;
			}
			else
				bRet = FALSE;
		}

		return	bRet;
	}

	BOOL Push(ItemType Item)
	{
		BOOL	bRet = TRUE;

		if( Grow() )
			m_pStack[m_nUsed++] = Item;
		else
			bRet = FALSE;

		return	bRet;
	}

	BOOL Pop(ItemType* pItem)
	{
		BOOL	bRet = TRUE;

		if( GetCount()>0 )
			*pItem = m_pStack[--m_nUsed];
		else
			bRet = FALSE;

		return	bRet;
	}

	BOOL ReadTop(ItemType* pItem)
	{
		BOOL	bRet = TRUE;

		if( GetCount()>0 )
			*pItem = m_pStack[m_nUsed-1];
		else
			bRet = FALSE;

		return	bRet;
	}

	BOOL ReadAt(ItemType* pItem, int idx)
	{
		BOOL	bRet = TRUE;

		if( idx>=0 && idx<m_nUsed )
			*pItem = m_pStack[idx];
		else
			bRet = FALSE;

		return	bRet;
	}

	BOOL RemoveAt(int idx)
	{
		if( idx>=0 && idx<m_nUsed )
		{
			for(int i=idx; i<m_nUsed-1; i++)
				m_pStack[i] = m_pStack[i+1];
			m_nUsed--;
			return	TRUE;
		}

		return	FALSE;
	}

	void RemoveAll()
	{
		if( m_pStack )
			delete m_pStack;
		m_pStack = NULL;
		m_nUsed  = 0;
		m_nAlloc = 0;
	}

protected:
	ItemType*	m_pStack;
	int			m_nAlloc;
	int			m_nUsed;
};

#endif