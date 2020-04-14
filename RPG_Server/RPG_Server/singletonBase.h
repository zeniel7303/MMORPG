#pragma once

template <typename T>
class singletonBase
{
protected:
	static T* singleton;

	singletonBase() {};
	~singletonBase() {};

public:
	//ΩÃ±€≈Ê ¡¢±Ÿ¿⁄
	static T* getSingleton(void);

	//ΩÃ±€≈Ê «ÿ¡¶
	void releaseSingleton(void);
};

//ΩÃ±€≈Ê √ ±‚»≠
template <typename T>
T* singletonBase<T>::singleton = 0;

//ΩÃ±€≈Ê ª˝º∫ «‘ºˆ
template <typename T>
T* singletonBase<T>::getSingleton()
{
	//ΩÃ±€≈Ê¿Ã µ«æÓ¿÷¡ˆæ ¥Ÿ∏È
	if (!singleton) singleton = new T;

	return singleton;
}

template <typename T>
void singletonBase<T>::releaseSingleton()
{
	if (singleton)
	{
		delete singleton;

		singleton = 0;
	}
}