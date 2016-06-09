template <class T>
class CSingletonT
{
	static T* s_pObject;

public:
	CSingletonT()
	{
		_ASSERT(s_pObject == NULL);
		int offset = (int)(T*)1 - (int)(CSingletonT<T>*)(T*)1;
		s_pObject = (T*)((int)this + offset);
	}
	~CSingletonT()
	{
		s_pObject = 0;
	}

#pragma warning (disable : 6011)
	static T& GetSingleton()
	{
		_ASSERT(s_pObject);
		return (*s_pObject);
	}
#pragma warning (default : 6011)

	static T* GetSingletonPtr()
	{
		return s_pObject;
	}
};


#pragma once
