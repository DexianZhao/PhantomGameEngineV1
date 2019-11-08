
// stdafx.cpp : 只包括标准包含文件的源文件
// phantom-gc.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"


base_reg::base_reg()
{
	m_hKey = 0;
}

base_reg::~base_reg()
{
	close();
}

bool	base_reg::openKey(HKEY hKey, const char* szSubKey)
{
	close();
	return (RegOpenKeyA(hKey, szSubKey, &m_hKey) == ERROR_SUCCESS);
}

bool	base_reg::createKey(HKEY hKey, const char* szSubKey)
{
	close();
	return (RegCreateKeyA(hKey, szSubKey, &m_hKey) == ERROR_SUCCESS);
}

void	base_reg::close()
{
	if(m_hKey)
		RegCloseKey(m_hKey);
	m_hKey = 0;
}

bool	base_reg::addTextValue(const char* szName, const char* szValue)
{
	if(m_hKey)
		return (::RegSetValueExA(m_hKey, szName, 0, REG_SZ, (BYTE*)szValue, (DWORD)strlen(szValue)) == ERROR_SUCCESS);
	return false;
}

int		base_reg::getBinaryValue(const char* szName, char* binary, int length)//读取二进制数据
{
	if(!m_hKey)return false;
	DWORD cbSize = length;
	DWORD dwT = REG_BINARY;
	return (::RegQueryValueExA(m_hKey, szName, NULL, &dwT, (LPBYTE)binary, &cbSize) == ERROR_SUCCESS);
}

int		base_reg::setBinaryValue(const char* szName, const char* binary, int length)//读取二进制数据
{
	if(!m_hKey)return false;
	DWORD cbSize = length;
	DWORD dwT = REG_BINARY;
	return (RegSetValueExA(m_hKey, szName, NULL, dwT, (LPBYTE)binary, cbSize) == ERROR_SUCCESS);
}

bool	base_reg::addIntValue(const char* szName, int nValue)
{
	if(m_hKey)
		return (::RegSetValueExA(m_hKey, szName, 0, REG_DWORD, (BYTE*)&nValue, sizeof(int)) == ERROR_SUCCESS);
	return false;
}


BOOL	IsKeyDown(int key)
{
	return HIBYTE(GetKeyState(key));
}
