//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com
	Design Writer :   赵德贤 Dexian Zhao
	Email: yuzhou_995@hotmail.com
	 
	*/
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PHANTOM_FILE_H__
#define __PHANTOM_FILE_H__
#include "PhantomBase.h"

namespace Phantom{
//内存读取器
class	MemoryFileStatic;
class	MemoryFile:public StreamBase
{
public:
	virtual	BOOL						readOnly(){return (m_bSelfCreate == false);}
	virtual	int64						write(const void* t,int64 tLength);
	virtual	int64						read(void* t,int64 tLength);
	virtual	int64						seek(int64 numSeek, int seekType = StreamBase::Seek_Set);
	virtual	int64						tell();
	virtual	int64						size(){return m_nFileSize;}
	virtual	BOOL						eof(){return (m_lPoint >= m_nFileSize);}
	void								close();
	virtual	char*						getMemoryPtr(){return m_pFile;}	//如果是内存流，返回内存地址
public:
	MemoryFile();
	~MemoryFile();
	//
	BOOL								open(MemoryFileStatic* mem);
	BOOL								open(char* origPtr, char* pFile, int dwFileSize,BOOL bAutoDelete);
	BOOL								create(int nFileSize);
public:
	MemoryFileStatic*					m_memFile;
	char*								m_pOrigPtr;
	char*								m_pFile;
	int64								m_nAllocSize;
	int64								m_nFileSize;
	int64								m_lPoint;
	BOOL								m_bSelfCreate;
};
class	MemoryFileStatic: public Base
{
public:
	MemoryFileStatic();
	virtual ~MemoryFileStatic();
	FileName							m_fileName;
	char*								m_pOrigPtr;
	int64								m_nAllocSize;
};
//内存读取器
class	IOFile:public StreamBase
{
public:
	virtual	BOOL						readOnly(){return m_bReadOnly;}
	virtual	int64						write(const void* t,int64 tLength);
	virtual	int64						read(void* t,int64 tLength);
	virtual	int64						seek(int64 numSeek, int seekType = StreamBase::Seek_Set);
	virtual	int64						tell();
	virtual	int64						size();
	virtual	BOOL						eof();
	void								close();
	virtual	char*						getMemoryPtr(){return 0;}	//如果是内存流，返回内存地址
public:
	IOFile();
	~IOFile();
public:
	BOOL								m_bReadOnly;
	FILE*								m_pFile;
};


struct ZipFileItem
{
	FileName							ext;
	FileName							fileName;
	FileName							zipFile;
	int									zipIndex;
};
class ZipFileFolder
{
public:
	ZipFileFolder();
	virtual ~ZipFileFolder();
	//
	const ZipFileItem*						find(const char* szFilePath);
	BOOL									add(const char* szFilePath, int zipIndex);
	ZipFileFolder*							findFolder(const char* szFolder);	//后面一定要带'\\'或者'/'
	//
	void									destroy();
	//
	PFileList*								enumFiles(const char* szPath, BOOL bsubFolder = true, std::string* szFindTypes = NULL,int numFindTypes = 0);
protected:
	const ZipFileItem*						find_loop(std::vector<std::string>& divs, std::string& fileName);
	ZipFileFolder*							findFolder_loop(std::vector<std::string>& divs);
	void									str_divs(const char* szFilePath, std::vector<std::string>& divsRet, std::string& retFileName);
	BOOL									add_loop(const ZipFileItem& f, std::vector<std::string>& divs, std::string& fileName);
	//
	void									enumFiles(PFileList* f, BOOL bsubFolder = true, std::string* szFindTypes = NULL,int numFindTypes = 0);
	friend class	ZipFilePackage;
public:
	ZipFileFolder*							parentDir;			//父文件夹名称
	char									directoryName[256];	//文件夹名称
	//
	std::vector<ZipFileFolder*>				directorys;
	std::vector<ZipFileItem>				files;
};

struct ZipFileBuffer
{
	char*			buffer;
	int				bufferLength;
	int				index;
};

enum ZipResult{
	ZipResult_RequestPassword	=	-2,	//需要密码
	ZipResult_Error		=	-1,	//未知错误
	ZipResult_OK		=	0,
};
class ZipFilePackage
{
public:
	ZipFilePackage();
	virtual ~ZipFilePackage();
	//
	ZipResult								openZip(const char* szFile, const char* szDir, const char* szPassword);
	ZipResult								openZip(const char* buffer, int bufferLength, const char* szDir, const char* szPassword);
	void									closeZip();
	//
	PFileList*								enumFiles(const char* path, BOOL bsubFolder = true, std::string* szFindTypes = NULL,int numFindTypes = 0);
	void									enumFiles(PFileList* f, const char* path, BOOL bsubFolder = true, std::string* szFindTypes = NULL,int numFindTypes = 0);
	//
	StreamBase*								open(const char* szFile);		//从map中打开文件
	BOOL									fexist(const char* _file);
	BOOL									fexist_local(const char* _file);
	//
	LFileName								m_zipFileName;
	static std::vector<ZipFilePackage*>		s_zipFiles;
	void									addDir(const char* szDir);
protected:
	//
	void*									m_hZipFile;
	std::vector<ZipFileBuffer>				m_fileBuffer;
	char*									m_memoryPtr;
	int										m_memoryLength;
	ZipFileFolder							m_zipFolder;
	ZipFileFolder*							m_curFolder;
	BOOL									m_bIsFile;
	CDynamicArray<Name>						m_initDirs;//m_zipInitDir;
};

typedef	std::vector<std::string>		TableHeader;
typedef	std::vector<TableHeader>		TableBody;

class	ITable: public Base
{
public:
	ITable(){}
	virtual ~ITable(){}
	ImplementComValue();
	virtual	int			GetLineCount() = 0;
	virtual int			GetIntVal(int row, int col) = 0;
	virtual double		GetDoubleVal(int row, int col) = 0;
	virtual const char*	GetTextVal(int row, int col) = 0;
	virtual int			SearchCol(const char* columnName) = 0;
	virtual int			GetIDCol() = 0;
	virtual int			GetNameCol() = 0;
	virtual	const char*	GetTableName() = 0;
	virtual	BOOL		IsCanDelete(){return false;}
	//
	LPCSTR				id_t/*表格数据_读文本根据ID*/(INT id/*ID值*/, LPCSTR szHeadName/*列标题*/);
	INT					id_i/*表格数据_读整数根据ID*/(INT id/*ID值*/, LPCSTR szHeadName/*列标题*/);
	float				id_f/*表格数据_读小数根据ID*/(INT id/*ID值*/, LPCSTR szHeadName/*列标题*/);
	LPCSTR				name_t/*表格数据_读文本根据Name*/(LPCSTR szName/*ID值*/, LPCSTR szHeadName/*列标题*/);
	INT					name_i/*表格数据_读整数根据Name*/(LPCSTR szName/*ID值*/, LPCSTR szHeadName/*列标题*/);
	float				name_f/*表格数据_读小数根据Name*/(LPCSTR szName/*ID值*/, LPCSTR szHeadName/*列标题*/);
	LPCSTR				search_t/*表格数据_读文本根据搜索*/(LPCSTR szFindHeader/*返回列名*/, LPCSTR szHeadName/*列标题*/, LPCSTR szLineValue/*行值*/);
	INT					search_i/*表格数据_读整数根据搜索*/(LPCSTR szFindHeader/*返回列名*/, LPCSTR szHeadName/*列标题*/, LPCSTR szLineValue/*行值*/);
	float				search_f/*表格数据_读小数根据搜索*/(LPCSTR szFindHeader/*返回列名*/, LPCSTR szHeadName/*列标题*/, LPCSTR szLineValue/*行值*/);
};

class	TableTxt: public ITable
{
public:
	TableTxt(void);
	~TableTxt(void);
	int						GetIntVal(int row, int col);
	double					GetDoubleVal(int row, int col);
	const char*				GetTextVal(int row, int col);
	int						SearchCol(const char* columnName);
	int						SearchRow(int id);
	int						SearchRow(const char* name);
	int						SearchRow(const char* columnName, const char* name);
	int						GetIDCol();
	int						GetNameCol();
	virtual	int			GetLineCount(){return this->m_vTable.size();}
	virtual	const char*	GetTableName(){return this->m_tableName;}
	virtual	BOOL		IsCanDelete(){return true;}
	//
	FileName			m_tableName;
	void				destroy();
public:
	char*				readLine(char* ptr, TableHeader& lineData, char chSkin='\t');
	//
	unsigned int		GetNumofLine(){return (unsigned int)m_vTable.size();}
	TableHeader*		GetLine(unsigned int index){if(index>=(unsigned int)m_vTable.size())return 0;return &m_vTable.at(index);}
	//
	BOOL				readFromMemory(const char* szTableData,int szTableDataLength,char chSkin = '\t');
	//
	std::string*		getLineItem(TableHeader& vLineIn,const char* szHeadName);
	//
	BOOL				read(int& ret, int lineIndex, const char* headName);
	BOOL				read(long& ret, int lineIndex, const char* headName);
	BOOL				read(double& ret, int lineIndex, const char* headName);
	BOOL				read(float& ret, int lineIndex, const char* headName);
	BOOL				read(char* ret, int lineIndex, const char* headName);
	const char*			read(int lineIndex, const char* headName);
public:
	BOOL				Load(const char* szFile,char chSymbol = '\t');//从文件读取表格信息
	//
	int					getNumLine();	//得到行个数
	//
	const char*			getLineItem(int lineIndex,const char* headName, const char* szDefault = 0);	//得到第lineIndex行的第headName列
	//
	BOOL				getLineItem(int lineIndex, const char* headName, float& f);
	BOOL				getLineItem(int lineIndex, const char* headName, int& i);
	BOOL				getLineItem(int lineIndex, const char* headName, char* buf, int numBuffer);
	//
	int					getHeadIndex(const char* headName);		//得到表头的列索引,headIndex
	const char*			getLineItemI(int lineIndex,int headIndex);	//得到第lineIndex行的第headIndex列
	//
	int					getNumHead();		//返回标头个数
	const char*			getHead(int headIndex);		//返回标头名称
	const char*			findLineText( const char* szFind, int headIndex ,int headRet);//得到指定数据的行
	TableHeader			m_vTableHead;//表格头
	TableBody			m_vTable;//表格体

};

#define	PAK_FILE_HEAD_LEN	8192
#pragma pack(push,1)
struct	PakFileHeader{
	unsigned int	head;//pak0
	unsigned int	chk;//
	int				filecount;
	int				infobegin;
};
struct	PakFileInfo{
	FileName		lFile;
	char			bPassword;//是否有密码
	char			bCompress;//是否压缩了
	char			bIsImage;//是否图片文件，如果是图片文件则不会判断扩展名
	unsigned int	length;
	unsigned char	filehead[PAK_FILE_HEAD_LEN];//文件开头
	unsigned int	compressLen;//压缩之后的大小
	unsigned int	fbegin;//数据开头位置
	unsigned int	nadd;//比对是否正确
};
#pragma pack(pop)

struct	PakFileData{
	BOOL						bCompress;
	BOOL						bIsImage;
	int							length;
	FileName					file;
	CDynamicArray<char>			buffers;
};

class	PakDirectory: public BaseDirectory{
public:
	PakDirectory();
	~PakDirectory();
	virtual	BOOL				fexist(const char* szFile);
	virtual	StreamBase*			open(const char* szFile);
	//
	static BOOL					IsPakFile(StreamBase* file);//是否压缩文件
	static char					ExportPak(const char* szFile, CDynamicArray<PakFileData*>& datas);//, const unsigned char* pass, int passlen);
	//
	char						LoadPakFile(CSafeFileHelperR& r, PakFileInfo& info, CDynamicArray<char>& rets);
	static BOOL					ResetPassword(CDynamicArray<char>& dest, CDynamicArray<char>& src, const unsigned char* oldpass, int oldpasslen);
	//
	StreamBase*					OpenPakFile(int index);//自动载入某个索引的文件
	VOID						ClosePak();
	BOOL						OpenPak(const char* szFile, CSafeFileHelperR& ret);
	BOOL						OpenPak(CSafeFileHelperR& r, CSafeFileHelperR& ret);
public:
	FileName					m_fileName;//文件名
	StreamBase*					m_streamPak;//数据流保存，自动载入流数据
	CDynamicArray<PakFileInfo>	m_pakfiles;
};
PakFileData*						LoadFileData(const char* szFile, BOOL bCompress);
PakFileData*						LoadTexturePak(const char* szTextureFile, const char* szTexturePath);

	BOOL				LoadUtf8(const char* szFile, CDynamicArray<char>& ret);
	VOID				SaveUtf8(const char* szFile, const char* text);
};

#endif
