#pragma once


#ifndef SERVICE_CORE_CLASS
#ifdef  SERVICE_CORE_DLL
#define SERVICE_CORE_CLASS _declspec(dllexport)
#else
#define SERVICE_CORE_CLASS _declspec(dllimport)
#endif
#endif

class SERVICE_CORE_CLASS CRandom
{
public:
	CRandom();
	virtual ~CRandom();
public:
	void Randomize();
	int Random(int n);
	void srand(unsigned int seed);
	int rand();
private:
	unsigned int holdrand;
};