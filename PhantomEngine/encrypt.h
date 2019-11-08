#ifndef ENCRYPT_H__
#define	ENCRYPT_H__

#define	PHANTOM_ENGINE_VERSION				2013	//幻影当前版本，每次发布完毕之后需要+1
#define	AUTH_INT_COUNT						128
namespace Phantom{
#pragma pack(push, 1)
	struct	GenuineGUID
	{
		//unsigned long				Data1;
		//unsigned char				year;
		//unsigned short				Data2;
		//unsigned short				day;
		//unsigned short				Data3;
		//unsigned char				month;
		//unsigned char				hour;
		//unsigned char				minute;
		//unsigned short				flags;
		////
		//enum GenuineFlags
		//{
		//	Genuine_Error			=	0x0000,	//没有授权
		//	Genuine_Professional	=	0x0001,	//个人版授权
		//	Genuine_Business		=	0x0002,	//企业版授权
		//	Genuine_Professional_1	=	0x0004,//一年期更新版本
		//};
		//unsigned char				Data4[ 8 ];
		//unsigned char				systemlock[128];//系统授权密码
		int							lock[AUTH_INT_COUNT];//系统授权密码
	};
	struct EncryptVal{
		int	version;
		long long n1,n2,n3;
		EncryptVal(){Reset();}
		VOID	Reset(){memset(this, 0, sizeof(EncryptVal));version = 1;}
		BOOL	operator !=(const EncryptVal& v){return ((n1==v.n1&&n2==v.n2&&n3==v.n3)==false);}
		BOOL	operator ==(const EncryptVal& v){return (n1==v.n1&&n2==v.n2&&n3==v.n3);}
		void	operator +=(const EncryptVal& v){
			n1 += v.n1;
			n2 += v.n2;
			n3 += v.n3;
		}
		long long reserved[3];//保留并待以后用
	};
	struct	RandomVal{
		enum{
			Random_Count = 512,
		};
		EncryptVal		val;
		unsigned short	rnd[Random_Count];
		int				curr;
	};
	struct	GameFileInfo{
		unsigned int	low,high;
		char			file[128];
	};
	struct	GameInfo{
		char	guid[32];//游戏guid，小写
		char	name[32];//游戏名字
		char	mainfile[128];
		char	auth[1025];//授权码,xor加密
		int		year,month,day,hour,second,minute;//创建时间
	};
#pragma pack(pop)
	VOID	EncryptRandomVal(RandomVal& retSave, RandomVal& retEncrypt, const GenuineGUID& guid);
	BOOL	DecryptRandomVal(RandomVal& inout, const GenuineGUID& guid);
	VOID	EncryptData(char* data, int length, RandomVal& rnd, EncryptVal& ret);
	BOOL	DecryptData(char* data, int length, RandomVal& rnd, const EncryptVal& chk);
	VOID	SetAuthCode(unsigned int* r);
};

#ifdef ENGINE_LIB_MODE
	BOOL	ReportLockFile(const char* szFile, char* ret);//导出锁定文件
	VOID	InitReport();
	VOID	SaveReport();
	extern	BOOL	GetGUID(Phantom::GenuineGUID& ret);
#endif

#endif
