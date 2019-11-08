//#ifdef ENGINE_LIB_MODE
//#include "windows.h"
//#else
#include "PhantomBase.h"
#ifdef WIN32
#include "io.h"
#include "stdio.h"
#include <vector>
#endif
#include "encrypt.h"

namespace Phantom{
	Phantom::GenuineGUID	g_tempAuthGUID;
	BOOL					g_bTempAuthGUID = false;
	BOOL	GetAuthCode(Phantom::GenuineGUID* r)
	{
		if(!g_bTempAuthGUID)return false;
		memcpy(r, &g_tempAuthGUID, sizeof(Phantom::GenuineGUID));
		return true;
	}
	VOID	SetAuthCode(unsigned int* r)
	{
		g_bTempAuthGUID = true;
		memcpy(&g_tempAuthGUID, r, sizeof(Phantom::GenuineGUID));
	}
	VOID	EncryptRandomVal(RandomVal& retSave, RandomVal& retEncrypt, const GenuineGUID& guid)
	{
		memset(&retSave, 0, sizeof(retSave));
		memset(&retEncrypt, 0, sizeof(retEncrypt));
		unsigned short* guid_xor = (unsigned short*)&guid;
		retSave.curr = 0;
		for(int i=0;i<RandomVal::Random_Count;i++)
		{
			retSave.rnd[i] = (rand()%65536);
			retSave.val.n1 += retSave.rnd[i];
			if((i%2)==0)
				retSave.val.n3 += retSave.rnd[i];
			else
				retSave.val.n3 -= retSave.rnd[i];
		}
		int guidlen = sizeof(GenuineGUID) / 2;
		for(int i=0;i<RandomVal::Random_Count;i++)
		{
			retEncrypt.rnd[i] = retSave.rnd[i] ^ guid_xor[i%guidlen];
			retSave.val.n2 += retEncrypt.rnd[i];
		}
		retSave.curr = rand();
		retEncrypt.curr = retSave.curr;
	}
	BOOL	DecryptRandomVal(RandomVal& ret, const GenuineGUID& guid)
	{
		unsigned short* guid_xor = (unsigned short*)&guid;
		int guidlen = sizeof(GenuineGUID) / 2;
		EncryptVal val;
		for(int i=0;i<RandomVal::Random_Count;i++)
		{
			val.n1 += ret.rnd[i];
			if((i%2)==0)
				val.n3 += ret.rnd[i];
			else
				val.n3 -= ret.rnd[i];
			ret.rnd[i] = ret.rnd[i] ^ guid_xor[i%guidlen];
			val.n2 += ret.rnd[i];
		}
		return (val == ret.val && (val.n1!=0 || val.n2!=0 || val.n3!=0));
	}

	VOID	EncryptData(char* data, int length, RandomVal& rnd, EncryptVal& ret)
	{
		unsigned short* ssptr = (unsigned short*)data;
		int sslen = (length/2);
		if(sslen < 32)
			return;
		ret.Reset();
		int lens = sizeof(EncryptVal);
		lens = 0;
		for(int i=0;i<sslen;i++)
		{
			ret.n1 += ssptr[i];
			if((i%2)==0)
				ret.n3 += ssptr[i];
			else
				ret.n3 -= ssptr[i];
			ssptr[i] ^= rnd.rnd[(rnd.curr++)%RandomVal::Random_Count];
			ret.n2 += ssptr[i];
		}
	}
	BOOL	DecryptData(char* data, int length, RandomVal& rnd, const EncryptVal& chk)
	{
		unsigned short* ssptr = (unsigned short*)data;
		int sslen = (length/2);
		if(sslen < 32)
			return true;
		EncryptVal val;
		int lens = sizeof(EncryptVal);
		for(int i=0;i<sslen;i++)
		{
			val.n2 += ssptr[i];
			ssptr[i] ^= rnd.rnd[(rnd.curr++)%RandomVal::Random_Count];
			if((i%2)==0)
				val.n3 += ssptr[i];
			else
				val.n3 -= ssptr[i];
			val.n1 += ssptr[i];
		}
		return (val == chk);
	}
};

