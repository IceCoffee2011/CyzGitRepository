// Random.h: interface for the CRandom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RANDOM_H__505F03DF_C311_4518_8389_A6447962E8B5__INCLUDED_)
#define AFX_RANDOM_H__505F03DF_C311_4518_8389_A6447962E8B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRandom  
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


#endif // !defined(AFX_RANDOM_H__505F03DF_C311_4518_8389_A6447962E8B5__INCLUDED_)
