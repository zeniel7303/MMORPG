#pragma once

template <typename T>
class SingletonBase
{
protected:
	static T* singleton;

	SingletonBase() {};
	~SingletonBase() {};

public:
	//ΩÃ±€≈Ê ¡¢±Ÿ¿⁄
	static T* getSingleton(void);

	//ΩÃ±€≈Ê «ÿ¡¶
	void releaseSingleton(void);
};

//ΩÃ±€≈Ê √ ±‚»≠
template <typename T>
T* SingletonBase<T>::singleton = 0;

//ΩÃ±€≈Ê ª˝º∫ «‘ºˆ
template <typename T>
T* SingletonBase<T>::getSingleton()
{
	//ΩÃ±€≈Ê¿Ã µ«æÓ¿÷¡ˆæ ¥Ÿ∏È
	if (!singleton) singleton = new T;

	return singleton;
}

template <typename T>
void SingletonBase<T>::releaseSingleton()
{
	if (singleton)
	{
		delete singleton;

		singleton = 0;
	}
}