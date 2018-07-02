#include "SarcArchive.h"

//------------------------------------------------------------------------------
HANDLE __stdcall OpenArchive(tOpenArchiveData* ArchiveData)
{
	SarcArchive* sarcArchive = new SarcArchive(ArchiveData->ArcName);

	return sarcArchive;
}
//------------------------------------------------------------------------------
HANDLE __stdcall OpenArchiveW(tOpenArchiveDataW* ArchiveData)
{
	SarcArchive* sarcArchive = new SarcArchive(ArchiveData->ArcName);

	return sarcArchive;
}
//------------------------------------------------------------------------------
int __stdcall CloseArchive(HANDLE hArcData)
{
	SarcArchive* sarcArchive = (SarcArchive*)hArcData;
	delete sarcArchive;
	sarcArchive = NULL;

	return 0;
}
//------------------------------------------------------------------------------
int __stdcall ReadHeader(HANDLE hArcData, tHeaderData* HeaderData)
{
	if (NULL == hArcData || NULL == HeaderData)
		return E_BAD_DATA;

	ZeroMemory(HeaderData, sizeof(tHeaderData));
	SarcArchive* sarcArchive = (SarcArchive*)hArcData;

	if (!sarcArchive->HasNextFile())
		return E_END_ARCHIVE;

	lstrcpy(HeaderData->ArcName, sarcArchive->GetArchiveName());
	lstrcpy(HeaderData->FileName, sarcArchive->GetCurFileName());
	HeaderData->FileTime = sarcArchive->GetArchiveTime();
	HeaderData->PackSize = sarcArchive->GetCurFileSize();
	HeaderData->UnpSize = HeaderData->PackSize;
	HeaderData->FileAttr = FA_ARCHIVE;

	return 0;
}
//------------------------------------------------------------------------------
int __stdcall ReadHeaderExW(HANDLE hArcData, tHeaderDataExW* HeaderData)
{
	if (NULL == hArcData || NULL == HeaderData)
		return E_BAD_DATA;

	ZeroMemory(HeaderData, sizeof(tHeaderData));
	SarcArchive* sarcArchive = (SarcArchive*)hArcData;

	if (!sarcArchive->HasNextFile())
		return E_END_ARCHIVE;

	lstrcpy(HeaderData->ArcName, sarcArchive->GetArchiveName());
	lstrcpy(HeaderData->FileName, sarcArchive->GetCurFileName());
	HeaderData->FileTime = sarcArchive->GetArchiveTime();
	HeaderData->PackSize = sarcArchive->GetCurFileSize();
	HeaderData->UnpSize = HeaderData->PackSize;
	HeaderData->FileAttr = FA_ARCHIVE;

	return 0;
}
//------------------------------------------------------------------------------
int __stdcall ProcessFile(HANDLE hArcData, int Operation, char* DestPath, char* DestName)
{
	if (!hArcData)
		return E_BAD_DATA;

	SarcArchive* sarcArchive = (SarcArchive*)hArcData;

	if (Operation == PK_EXTRACT)
	{
		char path[MAX_PATH * 2];
		path[0] = 0;
		if (DestPath)
			lstrcpy(path, DestPath);
		if (DestName)
			lstrcat(lstrcat(path, path[0] ? "\\" : ""), DestName);

		sarcArchive->ExtractFile(path);
	}

	sarcArchive->NextFile();

	return 0;
}
//------------------------------------------------------------------------------
int __stdcall ProcessFileW(HANDLE hArcData, int Operation, WCHAR* DestPath, WCHAR* DestName)
{
	if (!hArcData)
		return E_BAD_DATA;

	SarcArchive* sarcArchive = (SarcArchive*)hArcData;

	if (Operation == PK_EXTRACT)
	{
		char path[MAX_PATH * 2];
		path[0] = 0;
		if (DestPath)
			lstrcpy(path, DestPath);
		if (DestName)
			lstrcat(lstrcat(path, path[0] ? "\\" : ""), DestName);

		sarcArchive->ExtractFile(path);
	}

	sarcArchive->NextFile();

	return 0;
}

//------------------------------------------------------------------------------
void __stdcall SetChangeVolProc(HANDLE hArcData, tChangeVolProc pChangeVolProc)
{
	SarcArchive* yaz0Archive = (SarcArchive*)hArcData;
	yaz0Archive->ChangeVolProc = pChangeVolProc;
}

//------------------------------------------------------------------------------
void __stdcall SetProcessDataProc(HANDLE hArcData, tProcessDataProc pProcessDataProc)
{
	SarcArchive* yaz0Archive = (SarcArchive*)hArcData;
	yaz0Archive->ProcessDataProc = pProcessDataProc;
}

//------------------------------------------------------------------------------
void __stdcall SetChangeVolProcW(HANDLE hArcData, tChangeVolProcW pChangeVolProc)
{
	SarcArchive* yaz0Archive = (SarcArchive*)hArcData;
	yaz0Archive->ChangeVolProcW = pChangeVolProc;
}

//------------------------------------------------------------------------------
void __stdcall SetProcessDataProcW(HANDLE hArcData, tProcessDataProcW pProcessDataProc)
{
	SarcArchive* yaz0Archive = (SarcArchive*)hArcData;
	yaz0Archive->ProcessDataProcW = pProcessDataProc;
}

//------------------------------------------------------------------------------
int __stdcall GetPackerCaps()
{
	return PK_CAPS_SEARCHTEXT;
}



