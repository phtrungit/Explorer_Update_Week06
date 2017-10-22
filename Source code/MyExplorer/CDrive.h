#ifndef CDRIVE_H
#define CDRIVE_H

#include <windows.h>
#include <tchar.h>

class CDrive
{
private:
	TCHAR **m_pszDrive;
	TCHAR **m_pszVolName;
	TCHAR **m_pszDisplayName;
	int *m_nIconIndex;
	int m_nCount;

public:
	CDrive();
	~CDrive();

	TCHAR* GetDriveName(const int &i);
	TCHAR* GetVolName(const int &i);
	TCHAR* GetDisplayName(const int &i);
	int GetCount();
	int GetIconIndex(const int &i);

	void GetSystemDrives();

	LPWSTR GetType(const int &i);
};

#endif