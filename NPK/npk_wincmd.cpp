//------------------------------------------------------------------------------
//  npk_wincmd.cpp
//  (C) 2010 xoyojank
//------------------------------------------------------------------------------
#include "wcxhead.h"
#include "NpkArchive.h"

tChangeVolProc   ChangeVolProc;
tProcessDataProc ProcessDataProc;

NpkHeader npkHeader;

//------------------------------------------------------------------------------
BOOL APIENTRY DllMain( HANDLE hinstDLL, DWORD fdwReason, LPVOID )
{
    if( !hinstDLL )
        return FALSE;

    switch( fdwReason )
    {
        case DLL_PROCESS_ATTACH:
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

//------------------------------------------------------------------------------
HANDLE __stdcall OpenArchive( tOpenArchiveData* ArchiveData )
{
	NpkArchive* npkArchive = new NpkArchive(ArchiveData->ArcName);

	return npkArchive;
}

//------------------------------------------------------------------------------
int __stdcall CloseArchive( HANDLE hArcData )
{
	NpkArchive* npkArchive = (NpkArchive*)hArcData;
	delete npkArchive;
	npkArchive = NULL;

	return 0;
}

//------------------------------------------------------------------------------
int __stdcall ReadHeader( HANDLE hArcData, tHeaderData* HeaderData )
{
    if( NULL == hArcData || NULL == HeaderData )
        return E_BAD_DATA;

    ZeroMemory(HeaderData, sizeof(tHeaderData));
    NpkArchive* npkArchive = (NpkArchive*)hArcData;

	if (!npkArchive->HasNextFile())
		return E_END_ARCHIVE;

	lstrcpy(HeaderData->ArcName, npkArchive->GetArchiveName());
	lstrcpy(HeaderData->FileName, npkArchive->GetCurFileName());
	HeaderData->FileTime = npkArchive->GetArchiveTime();
	HeaderData->PackSize = npkArchive->GetCurFileSize();
	HeaderData->UnpSize = HeaderData->PackSize;
	HeaderData->FileAttr = FA_ARCHIVE;

    return 0;
}

//------------------------------------------------------------------------------
int __stdcall ProcessFile( HANDLE hArcData, int Operation, char* DestPath, char* DestName )
{
    //DebugString( "ProcessFile" );
    if( !hArcData )
        return E_BAD_DATA;

	NpkArchive* npkArchive = (NpkArchive*)hArcData;

    if( Operation == PK_EXTRACT )
    {
        char path[MAX_PATH * 2];
        path[0] = 0;
        if( DestPath )
            lstrcpy( path, DestPath );
        if( DestName )
            lstrcat( lstrcat( path, path[0] ? "\\" : "" ), DestName );

		npkArchive->ExtractFile(path);
    }

	npkArchive->NextFile();

    return 0;
}

//------------------------------------------------------------------------------
void __stdcall SetChangeVolProc( HANDLE /*hArcData*/, tChangeVolProc pChangeVolProc )
{
    ChangeVolProc = pChangeVolProc;
}

//------------------------------------------------------------------------------
void __stdcall SetProcessDataProc( HANDLE /*hArcData*/, tProcessDataProc pProcessDataProc )
{
    ProcessDataProc = pProcessDataProc;
}

//------------------------------------------------------------------------------
int __stdcall GetPackerCaps()
{
    return PK_CAPS_SEARCHTEXT;
}



