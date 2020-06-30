#pragma once

//template <typename _CLASS >
//class CLSingleton
//{
//protected :
//	CLSingleton() {}
//
//public :
//	_CLASS& GetInstance(void)
//	{
//		static _CLASS	m_instance;
//		return m_instance;
//	}
//};

//template <typename T>
//class SingletonBase
//{
//protected:
//	static T* singleton;
//
//	SingletonBase() {};
//	~SingletonBase() {};
//
//public:
//	//ΩÃ±€≈Ê ¡¢±Ÿ¿⁄
//	static T* getSingleton(void);
//
//	//ΩÃ±€≈Ê «ÿ¡¶
//	void releaseSingleton(void);
//};
//
////ΩÃ±€≈Ê √ ±‚»≠
//template <typename T>
//T* SingletonBase<T>::singleton = nullptr;
//
////ΩÃ±€≈Ê ª˝º∫ «‘ºˆ
//template <typename T>
//T* SingletonBase<T>::getSingleton()
//{
//	//ΩÃ±€≈Ê¿Ã µ«æÓ¿÷¡ˆæ ¥Ÿ∏È
//	if (!singleton) singleton = new T;
//
//	return singleton;
//}
//
//template <typename T>
//void SingletonBase<T>::releaseSingleton()
//{
//	if (singleton)
//	{
//		delete singleton;
//
//		singleton = nullptr;
//	}
//}

template <typename _Class>
class SingletonBase
{
protected:
	SingletonBase() {};
	~SingletonBase() {};

public:
	//ΩÃ±€≈Ê ¡¢±Ÿ¿⁄
	static _Class* getSingleton()
	{	
		static _Class singleton;

		return &singleton;
	}
};