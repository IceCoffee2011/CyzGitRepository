// Random.cpp: implementation of the CRandom class.
//
//////////////////////////////////////////////////////////////////////
#include "Random.h"
#ifdef _MSC_VER  // windows
#include <Windows.h>
#endif
#include <time.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRandom::CRandom()
{
	Randomize();
}

CRandom::~CRandom()
{

}

//---------------------------------------------------------------------------
//	��ʼ���������
//---------------------------------------------------------------------------
#ifdef _MSC_VER  // windows

void CRandom::Randomize(void)
{
	int dwCPUTickCount;
	__asm
	{
		// Save registers.
			push	eax
			push	ebx
			push	edx

			// Get the tick count.
			rdtsc
			lea		ebx,dwCPUTickCount
			mov		[ebx],eax
			//mov		[ebx+4],edx

			// Restore registers.
			pop		edx
			pop		ebx
			pop		eax
	}
	
	int dwSeed = int(time(NULL) * GetTickCount() * dwCPUTickCount);
	this->srand(dwSeed);		//�����������ʼ��
}
#else // linux

static __inline__ int TimeStampCounter(void) {
	int eax, edx;
	asm __volatile__ (
			"rdtsc" "\n\t"
			: "=a" (eax), "=d" (edx)
			:
			);
	return eax;
}
//---------------------------------------------------------------------------
////	��ʼ���������
////---------------------------------------------------------------------------
void CRandom::Randomize(void)
{
	int dwSeed = time(NULL) * TimeStampCounter();
	this->srand(dwSeed);		//�����������ʼ��
}

#endif // _MSC_VER

//---------------------------------------------------------------------------
//	���һ��0-n-1֮������ź���
//---------------------------------------------------------------------------
int CRandom::Random(int n)
{
	return rand() % n;
}

void CRandom::srand(unsigned int seed)
{
	holdrand=seed;
}

int CRandom::rand()
{
	//VC++ rand
	return (((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
	//BC++ rand
	//return (((holdrand = holdrand * 0x015a4e35L + 1L) >> 16) & 0x7fff);
}

