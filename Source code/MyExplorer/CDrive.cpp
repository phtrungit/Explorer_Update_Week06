#include"stdafx.h"
#include "CDrive.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#define BUFFER_LEN 105 
#define I_FLOPPY 3
#define I_USB 4
#define I_HDD 5
#define I_CD  6 

CDrive::CDrive()
{
	m_pszDrive = NULL;
	m_pszVolName = NULL;
	m_pszDisplayName = NULL;
	m_nIconIndex = NULL;
	m_nCount = 0;
}

CDrive::~CDrive()
{
	for (int i = 0; i < m_nCount; ++i)
	{
		delete[] m_pszDrive[i];
		delete[] m_pszVolName[i];
		delete[] m_pszDisplayName[i];
	}

	delete[] m_pszDrive;
	delete[] m_pszVolName;
	delete[] m_pszDisplayName;
	delete[] m_nIconIndex;
	m_nCount = 0;
}


TCHAR* CDrive::GetDriveName(const int &i)
{
	return m_pszDrive[i];
}

TCHAR* CDrive::GetVolName(const int &i)
{
	return m_pszVolName[i];
}

TCHAR* CDrive::GetDisplayName(const int &i)
{
	return m_pszDisplayName[i];
}

int CDrive::GetCount()
{
	return m_nCount;
}

int CDrive::GetIconIndex(const int &i)
{
	return m_nIconIndex[i];
}

void CDrive::GetSystemDrives()
{
	TCHAR buffer[BUFFER_LEN];
	int i;

	GetLogicalDriveStrings(BUFFER_LEN, buffer);


	for (i = 0; !((buffer[i] == 0) && (buffer[i + 1] == 0)); ++i)
		if (buffer[i] == 0)
			++m_nCount;
	++m_nCount;


	m_pszDrive = new TCHAR*[m_nCount];
	m_pszVolName = new TCHAR*[m_nCount];
	m_pszDisplayName = new TCHAR*[m_nCount];
	m_nIconIndex = new int[m_nCount];

	for (i = 0; i < m_nCount; ++i)
	{
		m_pszDrive[i] = new TCHAR[4];
		m_pszVolName[i] = new TCHAR[30];
		m_pszDisplayName[i] = new TCHAR[35];
	}

	int j, k;
	i = 0;

	for (j = 0; j< m_nCount; ++j)
	{
		k = 0;
		while (buffer[i] != 0)
			m_pszDrive[j][k++] = buffer[i++];
		m_pszDrive[j][k] = 0; 
		++i;
	}

	int nType;

	for (i = 0; i < m_nCount; ++i)
	{
		nType = GetDriveType(m_pszDrive[i]);
		StrCpy(buffer, _T(""));

		if ((nType == DRIVE_FIXED) || ((i > 1) && (nType == DRIVE_REMOVABLE)))
		{
			if (nType == DRIVE_FIXED)
				m_nIconIndex[i] = I_HDD;
			else
				m_nIconIndex[i] = I_USB;

			GetVolumeInformation(m_pszDrive[i], buffer, BUFFER_LEN, NULL, NULL, NULL, NULL, 0);
			StrCpy(m_pszVolName[i], buffer);
		}
		else
			if (nType == DRIVE_CDROM)
			{
				GetVolumeInformation(m_pszDrive[i], buffer, BUFFER_LEN, NULL, NULL, NULL, NULL, 0);
				if (wcslen(buffer) == 0)
					StrCpy(m_pszVolName[i], _T("CD Rom"));
				else
					StrCpy(m_pszVolName[i], buffer);
				m_nIconIndex[i] = I_CD;
			}
			else
				if (((i == 0) || (i == 1)) && (nType == DRIVE_REMOVABLE)) 
				{
					StrCpy(m_pszVolName[i], _T("3½ Floppy"));
					m_nIconIndex[i] = I_FLOPPY;
				}

		if (wcslen(m_pszVolName[i]) == 0) 
			StrCpy(m_pszDisplayName[i], _T("Local Disk"));
		else
			StrCpy(m_pszDisplayName[i], m_pszVolName[i]);

		StrCat(m_pszDisplayName[i], _T(" ("));
		StrNCat(m_pszDisplayName[i], m_pszDrive[i], 3);
		StrCat(m_pszDisplayName[i], _T(")"));
	}
}

LPWSTR CDrive::GetType(const int &i)
{
	switch (GetIconIndex(i))
	{
	case I_FLOPPY:
		return _T("3½ Floppy");
		break;
	case I_CD:
		return _T("CD Rom");
		break;
	case I_HDD:
		return _T("Local Disk");
		break;
	}

	return _T("Removable Disk");
}
