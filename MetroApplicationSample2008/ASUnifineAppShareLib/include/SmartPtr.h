#pragma once //[public]

template<class class_dest>
class CSmartPtr
{
public:
	CSmartPtr(class_dest * pDest = NULL)
	{
		m_pDest = pDest;

		_AddRef();
	}
	CSmartPtr(const CSmartPtr<class_dest> & ptr)
	{
		m_pDest = ptr.m_pDest;

		_AddRef();
	}	
	class_dest * operator = (class_dest * pDest)
	{
		_Release();

		m_pDest = pDest;

		_AddRef();
		return m_pDest;
	}
	CSmartPtr<class_dest> & operator = (const CSmartPtr<class_dest> & ptr)
	{
		_Release();
		m_pDest = ptr.m_pDest;
		_AddRef();
		return *this;
	}
	class_dest * operator -> ()
	{
		return m_pDest;
	}
	operator class_dest*()
	{
		return m_pDest;
	}
	class_dest** operator & ()
	{
		return &m_pDest;
	}
	virtual ~CSmartPtr(void)
	{
		_Release();

	}

	class_dest * get() const
	{
		return m_pDest;
	}

protected:
	void _Release()
	{
		if ( m_pDest )
			m_pDest->Release();
	}

	void _AddRef()
	{
		if ( !m_pDest )
			return;

		m_pDest->AddRef();
	}

protected:
	class_dest * m_pDest;
};