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
	~SarcArchive();

	bool LoadContent();

	const char* GetArchiveName() const;
	int GetArchiveTime() const;
	const char* GetCurFileName() const;
	UINT GetCurFileSize() const;
	void NextFile();
	bool HasNextFile() const;
	bool ExtractFile(const char* destPath);
private:
	struct File
	{
		string path;
		UINT offset;
		UINT size;
	};
private:
	void ReadData(void* ptr, UINT size);
	DWORD Tell() const;
private:
	string archiveName;
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
