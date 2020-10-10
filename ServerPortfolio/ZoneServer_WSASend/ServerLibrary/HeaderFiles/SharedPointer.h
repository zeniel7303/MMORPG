#pragma once
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#include <cstdlib>
#include <crtdbg.h>
#include <atomic>

#define SAFE_DELETE(p)			{ if (p) { delete (p); (p) = nullptr; } }
#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = nullptr;} }

template <typename T>
class SharedPointer
{
private:
	T*			pointer;

	long*		referenceCount;

	bool		dontDelete;

	void AddRef()
	{
		//spinLock.Enter();

		if (referenceCount == nullptr)
		{
			referenceCount = new long(0);
			//printf("new addref %p new %p , ref count : %d \n", this, referenceCount, *referenceCount);
		}

		InterlockedIncrement(referenceCount);
		//printf("addref %p : add : %p,  ref count : %d \n", this, referenceCount, *referenceCount);
	
		//spinLock.Leave();
	}

	void Release()
	{
		//printf("release %p : %p,  ref count : %d \n", this, referenceCount, *referenceCount );

		//if((*referenceCount) == 0)
		if(InterlockedDecrement(referenceCount) == 0)
		{
			//printf("release delete %p : refcount : %p , pointer : %p \n", this, referenceCount, pointer);

			//printf("%d \n", tempInt);
			SAFE_DELETE(referenceCount);

			SAFE_DELETE_ARRAY(pointer);

			//printf("- ");
		}
	}

public:
	SharedPointer()
		: pointer(nullptr)
		, referenceCount(nullptr) {}

	SharedPointer(T* object, bool _bool)
		: pointer(object)
		, referenceCount(nullptr)
		, dontDelete(_bool)
	{
		AddRef();

		//printf("+ ");
	}

	SharedPointer(const SharedPointer& other)
		: pointer(other.pointer)
		, referenceCount(other.referenceCount)
		, dontDelete(other.dontDelete)
	{
		AddRef();

		//printf("= ");
	}

	SharedPointer(SharedPointer&& other)
		: pointer(std::move(other.pointer))
		, referenceCount(std::move(other.referenceCount))
		, dontDelete(std::move(other.dontDelete))
	{
		//printf("= ");
	}

	~SharedPointer()
	{
		Release();
	}

	SharedPointer& operator = (const SharedPointer& other)
	{
		referenceCount = other.referenceCount;
		pointer = other.referenceCount;
		dontDelete = other.dontDelete;

		AddRef();

		//printf("' ");
	}

	SharedPointer& operator = (const SharedPointer&& other)
	{
		*this = std::move(other);

		return *this;
		//printf("' ");
	}

	/*void Set(T* object)
	{
		//원래 있던 객체 해제
		if (referenceCount)
		{
			Release();

			referenceCount = nullptr;
			pointer = nullptr;
		}

		AddRef();
		pointer = object;
	}*/

	T* GetPointer()
	{
		return pointer;
	}

	void SetPointer(WORD _bytes)
	{
		pointer += _bytes;
	}
};

template<typename T, typename... Args>
SharedPointer<T> MakeShared(Args... args)
{
	return SharedPointer<T>(new T(args...));
}