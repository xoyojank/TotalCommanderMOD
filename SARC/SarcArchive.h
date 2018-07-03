#pragma once
#include "wcxhead.h"
#include <vector>
#include <string>
using namespace std;
#include <windows.h>
#include <assert.h>

class SarcArchive
{
public:
	SarcArchive(const char* arcName);
	SarcArchive(const wchar_t* arcName);

	~SarcArchive();

	bool ReadFileHeader();

	const char* GetArchiveName() const;
	const wchar_t* GetArchiveNameW() const;
	int GetArchiveTime() const;
	const char* GetCurFileName() const;
	const wchar_t* GetCurFileNameW() const;
	UINT GetCurFileSize() const;
	void NextFile();
	bool HasNextFile() const;
	bool ExtractFile(const char* destPath);
	bool ExtractFileW(const wchar_t* destPath);
private:
	struct File
	{
		string path;
		wstring pathW;
		UINT offset;
		UINT size;
	};
private:
	void ReadData(void* ptr, size_t size);
	DWORD Tell() const;

	void ReadFileTime();
	void WriteFileData(HANDLE hFile);
private:
	string archiveName;
	wstring archiveNameW;
	int archiveTime;
	vector<File> files;
	UINT curIndex;
	HANDLE hArcFile;

public:
	tChangeVolProc   ChangeVolProc;
	tProcessDataProc ProcessDataProc;
	tChangeVolProcW   ChangeVolProcW;
	tProcessDataProcW ProcessDataProcW;
};

//------------------------------------------------------------------------------
inline const char*
SarcArchive::GetArchiveName() const
{
	assert(this->curIndex < this->files.size());
	return this->archiveName.c_str();
}

//------------------------------------------------------------------------------
inline const wchar_t*
SarcArchive::GetArchiveNameW() const
{
	assert(this->curIndex < this->files.size());
	return this->archiveNameW.c_str();
}

//------------------------------------------------------------------------------
inline int
SarcArchive::GetArchiveTime() const
{
	return this->archiveTime;
}

//------------------------------------------------------------------------------
inline const char*
SarcArchive::GetCurFileName() const
{
	assert(this->curIndex < this->files.size());
	return this->files[this->curIndex].path.c_str();
}

//------------------------------------------------------------------------------
inline const wchar_t*
SarcArchive::GetCurFileNameW() const
{
	assert(this->curIndex < this->files.size());
	return this->files[this->curIndex].pathW.c_str();
}

//------------------------------------------------------------------------------
inline UINT
SarcArchive::GetCurFileSize() const
{
	assert(this->curIndex < this->files.size());
	return this->files[this->curIndex].size;
}

//------------------------------------------------------------------------------
inline void
SarcArchive::NextFile()
{
	++this->curIndex;
}

//------------------------------------------------------------------------------
inline bool
SarcArchive::HasNextFile() const
{
	return (this->curIndex < this->files.size());
}
