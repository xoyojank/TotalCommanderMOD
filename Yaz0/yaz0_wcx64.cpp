#include "wcxhead.h"
#include "yaz0.h"
#include <shlwapi.h>

typedef struct
{
	CHAR ArchiveName[MAX_PATH];
	WCHAR ArchiveNameW[MAX_PATH];
	HANDLE ArchiveHandle;
	BYTE* ArchiveData;
	UINT PackSize;
	UINT UnpackSize;
	tChangeVolProc ChangeVolProc;
	tProcessDataProc ProcessDataProc;
	tChangeVolProcW ChangeVolProcW;
	tProcessDataProcW ProcessDataProcW;
} Yaz0Archive;
//------------------------------------------------------------------------------
HANDLE __stdcall OpenArchive(tOpenArchiveData* ArchiveData)
{
	HANDLE hArcFile = CreateFile(ArchiveData->ArcName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hArcFile)
	{
		ArchiveData->OpenResult = E_EOPEN;
		return 0;
	}
	DWORD bytesRead = 0;

	UINT32 magic = 0;
	if (!ReadFile(hArcFile, &magic, sizeof(magic), &bytesRead, NULL))
	{
		ArchiveData->OpenResult = E_EREAD;
		return 0;
	}
	if (magic != '0zaY')
	{
		ArchiveData->OpenResult = E_UNKNOWN_FORMAT;
		return 0;
	}

	Yaz0Archive* yaz0Archive = new Yaz0Archive();
	ZeroMemory(yaz0Archive, sizeof(Yaz0Archive));
	strcpy_s(yaz0Archive->ArchiveName, MAX_PATH, ArchiveData->ArcName);
	yaz0Archive->ArchiveHandle = hArcFile;
	return yaz0Archive;
}
//------------------------------------------------------------------------------
HANDLE __stdcall OpenArchiveW(tOpenArchiveDataW* ArchiveData)
{
	HANDLE hArcFile = CreateFileW(ArchiveData->ArcName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hArcFile)
	{
		ArchiveData->OpenResult = E_EOPEN;
		return 0;
	}
	DWORD bytesRead = 0;

	UINT32 magic = 0;
	if (!ReadFile(hArcFile, &magic, sizeof(magic), &bytesRead, NULL))
	{
		ArchiveData->OpenResult = E_EREAD;
		return 0;
	}
	if (magic != '0zaY')
	{
		ArchiveData->OpenResult = E_UNKNOWN_FORMAT;
		return 0;
	}

	Yaz0Archive* yaz0Archive = new Yaz0Archive();
	ZeroMemory(yaz0Archive, sizeof(Yaz0Archive));
	wcscpy_s(yaz0Archive->ArchiveNameW, MAX_PATH, ArchiveData->ArcName);
	yaz0Archive->ArchiveHandle = hArcFile;
	return yaz0Archive;
}
//------------------------------------------------------------------------------
int __stdcall CloseArchive(HANDLE hArcData)
{
	if (!hArcData)
		return E_BAD_DATA;
	Yaz0Archive* yaz0Archive = (Yaz0Archive*)hArcData;

	if (yaz0Archive->ArchiveData)
		delete[] yaz0Archive->ArchiveData;
	delete yaz0Archive;

	return 0;
}
//------------------------------------------------------------------------------
int __stdcall ReadHeader(HANDLE hArcData, tHeaderData* HeaderData)
{
	if (nullptr == hArcData || nullptr == HeaderData)
		return E_BAD_DATA;

	ZeroMemory(HeaderData, sizeof(tHeaderData));
	Yaz0Archive* yaz0Archive = (Yaz0Archive*)hArcData;

	if (yaz0Archive->ArchiveData)
		return E_END_ARCHIVE;

	yaz0Archive->PackSize = GetFileSize(yaz0Archive->ArchiveHandle, NULL);
	SetFilePointer(yaz0Archive->ArchiveHandle, 0, NULL, FILE_BEGIN);
	yaz0Archive->ArchiveData = new BYTE[yaz0Archive->PackSize];
	DWORD bytesRead = 0;
	if (!ReadFile(yaz0Archive->ArchiveHandle, yaz0Archive->ArchiveData, yaz0Archive->PackSize, &bytesRead, NULL))
	{
		return E_EREAD;
	}
	yaz0Archive->UnpackSize = yaz0_get_size(yaz0Archive->ArchiveData);
	// get file time
	INT archiveTime = 0;
	FILETIME time;
	GetFileTime(yaz0Archive->ArchiveHandle, &time, &time, &time);
	FileTimeToDosDateTime(&time, ((WORD*)&archiveTime) + 1, (WORD*)&archiveTime);

	strcpy_s(HeaderData->ArcName, MAX_PATH, yaz0Archive->ArchiveName);
	strcpy_s(HeaderData->FileName, MAX_PATH, PathFindFileNameA(yaz0Archive->ArchiveName));
	for (size_t i = strlen(HeaderData->FileName); i > 0; --i)
	{
		if (HeaderData->FileName[i] == '.')
		{
			HeaderData->FileName[i] = '\0';
			break;
		}
	}
	HeaderData->FileTime = archiveTime;
	HeaderData->PackSize = yaz0Archive->PackSize;
	HeaderData->UnpSize = yaz0Archive->UnpackSize;
	HeaderData->FileAttr = FA_ARCHIVE;

	return 0;
}
//------------------------------------------------------------------------------
int __stdcall ReadHeaderExW(HANDLE hArcData, tHeaderDataExW* HeaderDataExW)
{
	if(nullptr == hArcData || nullptr == HeaderDataExW)
		return E_BAD_DATA;

	ZeroMemory(HeaderDataExW, sizeof(tHeaderDataExW));
	Yaz0Archive* yaz0Archive = (Yaz0Archive*)hArcData;

	if (yaz0Archive->ArchiveData)
		return E_END_ARCHIVE;

	yaz0Archive->PackSize = GetFileSize(yaz0Archive->ArchiveHandle, NULL);
	SetFilePointer(yaz0Archive->ArchiveHandle, 0, NULL, FILE_BEGIN);
	yaz0Archive->ArchiveData = new BYTE[yaz0Archive->PackSize];
	DWORD bytesRead = 0;
	if (!ReadFile(yaz0Archive->ArchiveHandle, yaz0Archive->ArchiveData, yaz0Archive->PackSize, &bytesRead, NULL))
	{
		return E_EREAD;
	}
	yaz0Archive->UnpackSize = yaz0_get_size(yaz0Archive->ArchiveData);
	// get file time
	INT archiveTime = 0;
	FILETIME time;
	GetFileTime(yaz0Archive->ArchiveHandle, &time, &time, &time);
	FileTimeToDosDateTime(&time, ((WORD*)&archiveTime) + 1, (WORD*)&archiveTime);

	wcscpy_s(HeaderDataExW->ArcName, MAX_PATH, yaz0Archive->ArchiveNameW);
	wcscpy_s(HeaderDataExW->FileName, MAX_PATH, PathFindFileNameW(yaz0Archive->ArchiveNameW));
	for (size_t i = wcslen(HeaderDataExW->FileName); i > 0; --i)
	{
		if (HeaderDataExW->FileName[i] == L'.')
		{
			HeaderDataExW->FileName[i] = L'\0';
			break;
		}
	}
	HeaderDataExW->FileTime = archiveTime;
	HeaderDataExW->PackSize = yaz0Archive->PackSize;
	HeaderDataExW->UnpSize = yaz0Archive->UnpackSize;
	HeaderDataExW->FileAttr = FA_ARCHIVE;

	return 0;
}
//------------------------------------------------------------------------------
int __stdcall ProcessFile(HANDLE hArcData, int Operation, char* DestPath, char* DestName)
{
	if (!hArcData)
		return E_BAD_DATA;
	Yaz0Archive* yaz0Archive = (Yaz0Archive*)hArcData;

	if (Operation == PK_EXTRACT)
	{
		CHAR path[MAX_PATH * 2];
		path[0] = 0;
		if (DestPath)
			strcpy_s(path, MAX_PATH * 2, DestPath);
		if (DestName)
		{
			strcat_s(path, MAX_PATH * 2, path[0] ? "\\" : "");
			strcat_s(path, MAX_PATH * 2, DestName);
		}

		HANDLE hFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
			return E_ECREATE;
		BYTE* buffer = new BYTE[yaz0Archive->UnpackSize];
		yaz0_decode(yaz0Archive->ArchiveData, buffer);
		DWORD bytesWrite = 0;
		if (!WriteFile(hFile, buffer, yaz0Archive->UnpackSize, &bytesWrite, NULL))
			return E_EWRITE;
	}
	return 0;
}
//------------------------------------------------------------------------------
int __stdcall ProcessFileW(HANDLE hArcData, int Operation, WCHAR* DestPath, WCHAR* DestName)
{
	if(!hArcData)
		return E_BAD_DATA;
	Yaz0Archive* yaz0Archive = (Yaz0Archive*)hArcData;

	if(Operation == PK_EXTRACT)
	{
		WCHAR path[MAX_PATH * 2];
		path[0] = 0;
		if(DestPath)
			wcscpy_s(path, MAX_PATH * 2, DestPath);
		if (DestName)
		{
			wcscat_s(path, MAX_PATH * 2, path[0] ? L"\\" : L"");
			wcscat_s(path, MAX_PATH * 2, DestName);
		}

		HANDLE hFile = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
			return E_ECREATE;
		BYTE* buffer = new BYTE[yaz0Archive->UnpackSize];
		yaz0_decode(yaz0Archive->ArchiveData, buffer);
		DWORD bytesWrite = 0;
		if (!WriteFile(hFile, buffer, yaz0Archive->UnpackSize, &bytesWrite, NULL))
			return E_EWRITE;
	}
	return 0;
}

//------------------------------------------------------------------------------
void __stdcall SetChangeVolProc(HANDLE hArcData, tChangeVolProc pChangeVolProc)
{
	Yaz0Archive* yaz0Archive = (Yaz0Archive*)hArcData;
	yaz0Archive->ChangeVolProc = pChangeVolProc;
}

//------------------------------------------------------------------------------
void __stdcall SetProcessDataProc(HANDLE hArcData, tProcessDataProc pProcessDataProc)
{
	Yaz0Archive* yaz0Archive = (Yaz0Archive*)hArcData;
	yaz0Archive->ProcessDataProc = pProcessDataProc;
}

//------------------------------------------------------------------------------
void __stdcall SetChangeVolProcW(HANDLE hArcData, tChangeVolProcW pChangeVolProc)
{
	Yaz0Archive* yaz0Archive = (Yaz0Archive*)hArcData;
	yaz0Archive->ChangeVolProcW = pChangeVolProc;
}

//------------------------------------------------------------------------------
void __stdcall SetProcessDataProcW(HANDLE hArcData, tProcessDataProcW pProcessDataProc)
{
	Yaz0Archive* yaz0Archive = (Yaz0Archive*)hArcData;
	yaz0Archive->ProcessDataProcW = pProcessDataProc;
}

//------------------------------------------------------------------------------
int __stdcall GetPackerCaps()
{
	return PK_CAPS_SEARCHTEXT;
}



