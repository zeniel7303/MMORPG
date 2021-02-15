#pragma once
#define SAFE_DELETE(p)			{ if (p) { delete (p); (p) = nullptr; } }
#define	SAFE_DELETE_ARRAY(p)	{ if (p) { delete[] (p); (p) = nullptr;} }

template <typename T>
class SharedPointer
{
private:
	T*			pointer;

	long*		referenceCount;

	void AddRef()
	{
		if (referenceCount == nullptr)
		{
			referenceCount = new long(0);
		}

		InterlockedIncrement(referenceCount);
	}

	void Release()
	{
		if(InterlockedDecrement(referenceCount) == 0)
		{
			SAFE_DELETE(referenceCount);

			SAFE_DELETE_ARRAY(pointer);
		}
	}

public:
	SharedPointer()
		: pointer(nullptr)
		, referenceCount(nullptr) {}

	SharedPointer(T* object)
		: pointer(object)
		, referenceCount(nullptr)
	{
		AddRef();
	}

	SharedPointer(const SharedPointer& other)
		: pointer(other.pointer)
		, referenceCount(other.referenceCount)
	{
		AddRef();
	}

	~SharedPointer()
	{
		Release();
	}

	SharedPointer& operator = (const SharedPointer& other)
	{
		referenceCount = other.referenceCount;
		pointer = other.referenceCount;

		AddRef();
	}

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